// Fill out your copyright notice in the Description page of Project Settings.


#include "MapGenerator.h"
#include "Engine/Engine.h"

// Sets default values for this component's properties
UMapGenerator::UMapGenerator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	CurrentPlayerNode = nullptr;
	// ...
}


void UMapGenerator::GenerateMap()
{
	//기존 맵 정리 나중에 없앨수도
	ClearMap();

	int32 SeedToUse = GenerationSettings.RandomSeed;
	if(SeedToUse == -1)
	{
		SeedToUse = FMath::Rand();
	}
	RandomStream.Initialize(SeedToUse);

	//절차적 노드 개수 생성
	TArray<int32> ActualNodesPerDepth;

	//깊이 0: 시작 노드 고정
	ActualNodesPerDepth.Add(1);

	//깊이 1~4: 연결 제약 조건을 고려한 절차적 생성
	for(int32 Depth = 1; Depth <= 4; ++Depth)
	{
		int32 PreviousDepthNodeCount = ActualNodesPerDepth[Depth - 1];
		//최대 노드 수 = 이전 깊이 노드 수 * 3
		int32 MaxPossibleNodes = PreviousDepthNodeCount * 3;
		//헤더에서 최대 6으로 해뒀는데 위의 계산 값이 6을 넘으면 6으로 사용
		int32 ActualMaxNodes = FMath::Min(GenerationSettings.MaxNodesPerDepth, MaxPossibleNodes);
		//최소는 2개로 고정, 하지만 최대값보다 클 수 없음.
		int32 MinNodes = FMath::Min(2, ActualMaxNodes);

		int32 RandomNodeCount = RandomStream.RandRange(MinNodes, ActualMaxNodes);
		ActualNodesPerDepth.Add(RandomNodeCount);

		UE_LOG(LogTemp, Log, TEXT("Depth %d: PrevNodes = %d, MaxPossible = %d, Generated = %d"), Depth, PreviousDepthNodeCount, MaxPossibleNodes, RandomNodeCount);
	}
	ActualNodesPerDepth.Add(1); // 깊이 5: 정비 노드
	ActualNodesPerDepth.Add(1); // 깊이 6: 보스 노드

	//깊이별 노드 생성
	for(int32 Depth = 0; Depth < ActualNodesPerDepth.Num(); Depth++)
	{
		int32 NodeCount = ActualNodesPerDepth[Depth];
		CreateNodesAtDepth(Depth, NodeCount);
	}

	//특수 노드 타입 할당
	AssignSpecialNodeTypes();

	//노드 연결
	ConnectNodes();

	//모든 노드가 정비 노드에 도달 가능한지 검증
	//ValidateAllNodesReachable();

	//모든 노드에 접근 가능 막기
	UpdateNodeAccessibility();

	//시작 노드를 현재 위치로 설정
	SetPlayerPosition(GetStartNode());

	//맵 생성 완료 이벤트 호출
	OnMapGenerated.Broadcast(AllNodes.Num());

	UE_LOG(LogTemp, Log, TEXT("Map generation complete. Total nodes: %d"), AllNodes.Num());
}

void UMapGenerator::ClearMap()
{
	AllNodes.Empty();
	NodesByDepth.Empty();
	CurrentPlayerNode = nullptr;
}

void UMapGenerator::CreateNodesAtDepth(int32 Depth, int32 NodeCount)
{
	TArray<UMapNode*> NodesAtThisDepth;
	for(int32 Row = 0; Row < NodeCount; Row++)
	{
		UMapNode* NewNode = NewObject<UMapNode>(this);
		NewNode->Position = FNodePosition(Depth, Row);

		if(Depth == 0)
		{
			NewNode->NodeType = ENodeType::Start;
		}
		else if(Depth == 5)
		{
			NewNode->NodeType = ENodeType::Repair;
		}
		else if(Depth == 6)
		{
			NewNode->NodeType = ENodeType::Boss;
		}
		else
		{
			NewNode->NodeType = ENodeType::Normal; //일단은 노말로 세팅
		}

		NodesAtThisDepth.Add(NewNode);
		AllNodes.Add(NewNode);
	}
	NodesByDepth.Add(Depth, NodesAtThisDepth);
}

void UMapGenerator::AssignSpecialNodeTypes()
{
	//일반 노드들 가져오기
	TArray<UMapNode*> NormalNodes = GetNormalNodes();

	if (NormalNodes.Num() == 0)
	{
		return;
	}

	ShuffleArray(NormalNodes);

	int32 NodeIndex = 0;

	//상점 노드 할당
	int32 ShopNodeCount = RandomStream.RandRange(GenerationSettings.MinShopNodes, GenerationSettings.MaxShopNodes);
	ShopNodeCount = FMath::Min(ShopNodeCount, NormalNodes.Num() - NodeIndex); // 남은 노드 수보다 많을 수 없음

	for (int32 i = 0; i < ShopNodeCount && NodeIndex < NormalNodes.Num(); i++)
	{
		NormalNodes[NodeIndex]->NodeType = ENodeType::Shop;
		NodeIndex++;
	}

	// 특수 이벤트 노드 할당
	int32 SpecialNodeCount = RandomStream.RandRange(GenerationSettings.MinSpecialNodes, GenerationSettings.MaxSpecialNodes);
	SpecialNodeCount = FMath::Min(SpecialNodeCount, NormalNodes.Num() - NodeIndex);

	for (int32 i = 0; i < SpecialNodeCount && NodeIndex < NormalNodes.Num(); i++)
	{
		NormalNodes[NodeIndex]->NodeType = ENodeType::Special;
		NodeIndex++;
	}

	UE_LOG(LogTemp, Log, TEXT("Special node assignment complete: Shop %d, Special %d"), ShopNodeCount, SpecialNodeCount);
}

void UMapGenerator::ConnectNodes()
{
	UE_LOG(LogTemp, Log, TEXT("Starting connection generation..."));

	// Phase 1: 모든 노드의 들어오는 연결 보장 (N+1 ← N)
	int32 GuaranteedConnections = 0;

	for (int32 Depth = 1; Depth <= 6; Depth++)
	{
		TArray<UMapNode*> CurrentDepthNodes = GetNodesAtDepth(Depth);
		TArray<UMapNode*> PreviousDepthNodes = GetNodesAtDepth(Depth - 1);

		if (PreviousDepthNodes.Num() == 0)
		{
			continue; // 이전 깊이에 노드가 없으면 건너뛰기
		}

		for (UMapNode* CurrentNode : CurrentDepthNodes)
		{
			// 모든 노드는 들어오는 연결이 최소 1개 필요
			if (CurrentNode->PreviousNodes.Num() == 0)
			{
				// 이전 깊이에서 연결 수가 가장 적은 노드를 찾아서 연결
				UMapNode* SelectedPreviousNode = SelectNodeWithFewestConnections(PreviousDepthNodes);

				if (SelectedPreviousNode && SelectedPreviousNode->CanConnectTo(CurrentNode))
				{
					SelectedPreviousNode->AddConnection(CurrentNode);
					GuaranteedConnections++;

					UE_LOG(LogTemp, Log, TEXT("Guaranteed connection: Node(%d,%d) -> Node(%d,%d) [Total outgoing: %d]"),
						SelectedPreviousNode->Position.Depth, SelectedPreviousNode->Position.Row,
						CurrentNode->Position.Depth, CurrentNode->Position.Row,
						SelectedPreviousNode->ConnectedNodes.Num());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to add guaranteed connection for node at depth %d, row %d"),
						CurrentNode->Position.Depth, CurrentNode->Position.Row);
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Phase 1 complete: Added %d guaranteed connections"), GuaranteedConnections);

	// Phase 2: 각 노드의 나가는 연결 보장 (N → N+1)
	int32 AdditionalConnections = 0;

	if (GenerationSettings.bAllowAdditionalConnections)
	{
		for (int32 Depth = 0; Depth < 6; Depth++) // 깊이 6은 보스 노드라서 제외
		{
			TArray<UMapNode*> CurrentDepthNodes = GetNodesAtDepth(Depth);
			TArray<UMapNode*> NextDepthNodes = GetNodesAtDepth(Depth + 1);

			if (NextDepthNodes.Num() == 0)
			{
				continue;
			}

			for (UMapNode* CurrentNode : CurrentDepthNodes)
			{
				int32 CurrentConnections = CurrentNode->ConnectedNodes.Num();

				// 최소 연결 수 체크
				if (CurrentConnections < GenerationSettings.MinConnectionsPerNode)
				{
					// 연결 가능한 노드들 찾기 (이미 연결된 노드 제외)
					TArray<UMapNode*> PossibleConnections;
					for (UMapNode* NextNode : NextDepthNodes)
					{
						if (CurrentNode->CanConnectTo(NextNode) && !CurrentNode->ConnectedNodes.Contains(NextNode))
						{
							PossibleConnections.Add(NextNode);
						}
					}

					if (PossibleConnections.Num() > 0)
					{
						// 최소 연결 수를 만족할 때까지 연결 추가
						int32 NeededConnections = GenerationSettings.MinConnectionsPerNode - CurrentConnections;
						int32 ActualConnections = FMath::Min(NeededConnections, PossibleConnections.Num());

						// 랜덤으로 연결할 노드들 선택
						ShuffleArray(PossibleConnections);

						for (int32 i = 0; i < ActualConnections; i++)
						{
							CurrentNode->AddConnection(PossibleConnections[i]);
							AdditionalConnections++;
						}

						UE_LOG(LogTemp, Log, TEXT("Minimum connections added: Node(%d,%d) added %d connections [Total: %d]"),
							CurrentNode->Position.Depth, CurrentNode->Position.Row, ActualConnections,
							CurrentNode->ConnectedNodes.Num());
					}
				}

				// 최대 연결 수 범위에서 추가 연결 (선택적)
				CurrentConnections = CurrentNode->ConnectedNodes.Num();
				if (CurrentConnections < GenerationSettings.MaxConnectionsPerNode)
				{
					// 연결 가능한 노드들 찾기 (이미 연결된 노드 제외)
					TArray<UMapNode*> PossibleConnections;
					for (UMapNode* NextNode : NextDepthNodes)
					{
						if (CurrentNode->CanConnectTo(NextNode) && !CurrentNode->ConnectedNodes.Contains(NextNode))
						{
							PossibleConnections.Add(NextNode);
						}
					}

					if (PossibleConnections.Num() > 0)
					{
						// 랜덤으로 추가 연결 개수 결정 (0 ~ 최대 가능 개수)
						int32 MaxAdditional = FMath::Min(
							GenerationSettings.MaxConnectionsPerNode - CurrentConnections,
							PossibleConnections.Num()
						);

						int32 AdditionalCount = RandomStream.RandRange(0, MaxAdditional);

						if (AdditionalCount > 0)
						{
							// 랜덤으로 추가 연결할 노드들 선택
							ShuffleArray(PossibleConnections);

							for (int32 i = 0; i < AdditionalCount; i++)
							{
								CurrentNode->AddConnection(PossibleConnections[i]);
								AdditionalConnections++;
							}

							UE_LOG(LogTemp, Log, TEXT("Optional connections added: Node(%d,%d) added %d connections [Total: %d]"),
								CurrentNode->Position.Depth, CurrentNode->Position.Row, AdditionalCount,
								CurrentNode->ConnectedNodes.Num());
						}
					}
				}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Phase 2 complete: Added %d additional connections"), AdditionalConnections);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Phase 2 skipped: Additional connections disabled"));
	}

	UE_LOG(LogTemp, Log, TEXT("Connection generation complete: %d guaranteed + %d additional = %d total connections"),
		GuaranteedConnections, AdditionalConnections, GuaranteedConnections + AdditionalConnections);
}

/*
void UMapGenerator::ValidateAllNodesReachable()
{
	UMapNode* StartNode = GetStartNode();
	if (!StartNode)
	{
		UE_LOG(LogTemp, Error, TEXT("Start node not found!"));
		return;
	}

	// 모든 노드가 정비 노드(깊이 5)에 도달 가능한지 확인
	for (UMapNode* Node : AllNodes)
	{
		if (Node->Position.Depth >= 5)
		{
			continue; // 정비 노드와 보스 노드는 검사하지 않음
		}

		TSet<UMapNode*> VisitedNodes;
		bool bCanReach = false;
		DFS_CheckReachability(Node, 5, VisitedNodes, bCanReach);

		if (!bCanReach)
		{
			UE_LOG(LogTemp, Warning, TEXT("Node (%d, %d) cannot reach repair node!"),
				Node->Position.Depth, Node->Position.Row);
		}
	}
}
*/
/*
void UMapGenerator::EnsureAllNodesReachable()
{
	UMapNode* RepairNode = GetRepairNode();
	if (!RepairNode)
	{
		UE_LOG(LogTemp, Error, TEXT("Repair node not found for reachability check!"));
		return;
	}

	// 1. 정비 노드에서 역방향 탐색으로 도달 가능한 노드들 찾기
	TSet<UMapNode*> ReachableFromRepair;
	BackwardDFS(RepairNode, ReachableFromRepair);

	// 2. 도달 불가능한 노드들 찾아서 연결 추가
	int32 FixedConnections = 0;
	bool bNeedRetry = true;

	while (bNeedRetry)
	{
		bNeedRetry = false;

		for (UMapNode* Node : AllNodes)
		{
			if (Node->Position.Depth >= 5) continue;  // 정비, 보스 노드는 제외

			if (!ReachableFromRepair.Contains(Node))
			{
				// 고립된 노드 발견 → 강제 연결 추가
				if (AddEmergencyConnection(Node))
				{
					FixedConnections++;
					UE_LOG(LogTemp, Warning, TEXT("Added emergency connection for isolated node at depth %d, row %d"),
						Node->Position.Depth, Node->Position.Row);

					// 연결 추가 후 다시 탐색
					ReachableFromRepair.Empty();
					BackwardDFS(RepairNode, ReachableFromRepair);
					bNeedRetry = true;
					break;  // 하나씩 처리
				}
			}
		}
	}

	if (FixedConnections > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fixed %d isolated nodes with emergency connections"), FixedConnections);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("All nodes are reachable from repair node"));
	}
}
*/
/*
void UMapGenerator::BackwardDFS(UMapNode* CurrentNode, TSet<UMapNode*>& ReachableNodes) const
{
	if (!CurrentNode || ReachableNodes.Contains(CurrentNode))
	{
		return;
	}

	ReachableNodes.Add(CurrentNode);

	// 이 노드로 들어오는 모든 연결을 따라 역방향 탐색
	for (UMapNode* PreviousNode : CurrentNode->PreviousNodes)
	{
		BackwardDFS(PreviousNode, ReachableNodes);
	}
}
*/

/*
bool UMapGenerator::AddEmergencyConnection(UMapNode* IsolatedNode)
{
	if (!IsolatedNode || IsolatedNode->Position.Depth == 0)
	{
		return false;  // 시작 노드는 처리하지 않음
	}

	// 이전 깊이의 노드들 찾기
	TArray<UMapNode*> PreviousDepthNodes = GetNodesAtDepth(IsolatedNode->Position.Depth - 1);

	if (PreviousDepthNodes.Num() == 0)
	{
		return false;
	}

	// 연결 수가 적은 노드 우선으로 정렬
	PreviousDepthNodes.Sort([](const UMapNode& A, const UMapNode& B) {
		return A.ConnectedNodes.Num() < B.ConnectedNodes.Num();
	});

	// 연결 수가 가장 적은 노드부터 시도
	for (UMapNode* PreviousNode : PreviousDepthNodes)
	{
		// 이미 최대 연결 수에 도달했으면 다음 노드 시도
		if (PreviousNode->ConnectedNodes.Num() >= GenerationSettings.MaxConnectionsPerNode)
		{
			continue;
		}

		// 연결 가능한지 확인
		if (PreviousNode->CanConnectTo(IsolatedNode))
		{
			// 응급 연결 추가
			PreviousNode->AddConnection(IsolatedNode);

			UE_LOG(LogTemp, Log, TEXT("Emergency connection: Node(%d,%d) -> Node(%d,%d)"),
				PreviousNode->Position.Depth, PreviousNode->Position.Row,
				IsolatedNode->Position.Depth, IsolatedNode->Position.Row);

			return true;
		}
	}

	// 모든 이전 노드가 최대 연결에 도달했거나 연결 불가능한 경우
	// 연결 수가 가장 적은 노드에 강제로 연결 (최대 연결 수 무시)
	if (PreviousDepthNodes.Num() > 0)
	{
		UMapNode* BestNode = PreviousDepthNodes[0];  // 이미 연결 수 순으로 정렬됨

		if (BestNode->CanConnectTo(IsolatedNode))
		{
			BestNode->AddConnection(IsolatedNode);

			UE_LOG(LogTemp, Warning, TEXT("Forced emergency connection (exceeded max): Node(%d,%d) -> Node(%d,%d)"),
				BestNode->Position.Depth, BestNode->Position.Row,
				IsolatedNode->Position.Depth, IsolatedNode->Position.Row);

			return true;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to create emergency connection for node at depth %d, row %d"),
		IsolatedNode->Position.Depth, IsolatedNode->Position.Row);

	return false;
}

*/

void UMapGenerator::UpdateNodeAccessibility()
{
	// 모든 노드를 접근 불가능으로 설정
	for (UMapNode* Node : AllNodes)
	{
		Node->bIsAccessible = false;
	}

	// 시작 노드만 접근 가능으로 설정
	UMapNode* StartNode = GetStartNode();
	if (StartNode)
	{
		StartNode->bIsAccessible = true;
	}
}

UMapNode* UMapGenerator::GetStartNode() const
{
	TArray<UMapNode*> const* StartDepthNodes = NodesByDepth.Find(0);
	if (StartDepthNodes && StartDepthNodes->Num() > 0)
	{
		return (*StartDepthNodes)[0];
	}
	return nullptr;
}

UMapNode* UMapGenerator::GetRepairNode() const
{
	TArray<UMapNode*> const* RepairDepthNodes = NodesByDepth.Find(5);
	if (RepairDepthNodes && RepairDepthNodes->Num() > 0)
	{
		return (*RepairDepthNodes)[0];
	}
	return nullptr;
}

UMapNode* UMapGenerator::GetBossNode() const
{
	TArray<UMapNode*> const* BossDepthNodes = NodesByDepth.Find(6);
	if (BossDepthNodes && BossDepthNodes->Num() > 0)
	{
		return (*BossDepthNodes)[0];
	}
	return nullptr;
}

TArray<UMapNode*> UMapGenerator::GetNodesAtDepth(int32 Depth) const
{
	TArray<UMapNode*> const* NodesAtDepth = NodesByDepth.Find(Depth);
	if (NodesAtDepth)
	{
		return *NodesAtDepth;
	}
	return TArray<UMapNode*>();
}

bool UMapGenerator::IsValidMapGenerated() const
{
	return AllNodes.Num() > 0 && GetStartNode() && GetRepairNode() && GetBossNode();
}

void UMapGenerator::SetPlayerPosition(UMapNode* NewCurrentNode)
{
	if (!NewCurrentNode)
	{
		return;
	}

	CurrentPlayerNode = NewCurrentNode;
	CurrentPlayerNode->bIsVisited = true;

	// 연결된 노드들을 접근 가능하게 설정
	for (UMapNode* ConnectedNode : CurrentPlayerNode->ConnectedNodes)
	{
		ConnectedNode->bIsAccessible = true;
	}
}

TArray<UMapNode*> UMapGenerator::GetAvailableMovements() const
{
	if (!CurrentPlayerNode)
	{
		return TArray<UMapNode*>();
	}

	return CurrentPlayerNode->GetAvailableNextNodes();
}

TArray<UMapNode*> UMapGenerator::GetAllNodes() const
{
	return AllNodes;
}

TArray<UMapNode*> UMapGenerator::GetNormalNodes() const
{
	TArray<UMapNode*> NormalNodes;

	for(UMapNode* Node : AllNodes)
	{
		if (Node->NodeType == ENodeType::Normal && Node->Position.Depth >= 1 && Node->Position.Depth <= 4)
		{
			NormalNodes.Add(Node);
		}
	}

	return NormalNodes;
}

void UMapGenerator::ShuffleArray(TArray<UMapNode*>& Array) const
{
	for (int32 i = Array.Num() - 1; i > 0; i--)
	{
		int32 j = RandomStream.RandRange(0, i);
		Array.Swap(i, j);
	}
}

UMapNode* UMapGenerator::SelectNodeWithFewestConnections(const TArray<UMapNode*>& Nodes) const
{
	if (Nodes.Num() == 0)
	{
		return nullptr;
	}

	// 연결 수별로 노드들을 그룹화
	TMap<int32, TArray<UMapNode*>> ConnectionGroups;

	for (UMapNode* Node : Nodes)
	{
		int32 ConnectionCount = Node->ConnectedNodes.Num();
		ConnectionGroups.FindOrAdd(ConnectionCount).Add(Node);
	}

	// 최소 연결 수 찾기
	int32 MinConnections = INT32_MAX;
	for (const auto& Group : ConnectionGroups)
	{
		if (Group.Key < MinConnections)
		{
			MinConnections = Group.Key;
		}
	}

	// 최소 연결 수를 가진 노드들 중에서 랜덤 선택
	TArray<UMapNode*>& MinConnectionNodes = ConnectionGroups[MinConnections];

	if (MinConnectionNodes.Num() == 1)
	{
		return MinConnectionNodes[0];
	}
	else
	{
		// 같은 연결 수를 가진 노드들 중 랜덤 선택
		int32 RandomIndex = RandomStream.RandRange(0, MinConnectionNodes.Num() - 1);
		return MinConnectionNodes[RandomIndex];
	}
}




/*
bool UMapGenerator::CanNodeReachDepth(UMapNode* StartNode, int32 TargetDepth) const
{
	if (!StartNode)
	{
		return false;
	}

	TSet<UMapNode*> VisitedNodes;
	bool bCanReach = false;
	DFS_CheckReachability(StartNode, TargetDepth, VisitedNodes, bCanReach);
	return bCanReach;
}


void UMapGenerator::DFS_CheckReachability(UMapNode* CurrentNode, int32 TargetDepth, TSet<UMapNode*>& VisitedNodes, bool& bCanReach) const
{
	if (!CurrentNode || bCanReach || VisitedNodes.Contains(CurrentNode))
	{
		return;
	}

	VisitedNodes.Add(CurrentNode);

	// 목표 깊이에 도달했는지 확인
	if (CurrentNode->Position.Depth == TargetDepth)
	{
		bCanReach = true;
		return;
	}

	// 연결된 노드들을 탐색
	for (UMapNode* ConnectedNode : CurrentNode->ConnectedNodes)
	{
		DFS_CheckReachability(ConnectedNode, TargetDepth, VisitedNodes, bCanReach);
		if (bCanReach)
		{
			return;
		}
	}
}

*/
