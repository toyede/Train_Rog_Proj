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
	ValidateAllNodesReachable();

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
	// 각 깊이별로 다음 깊이와 연결
	for (int32 Depth = 0; Depth < 6; Depth++) // 깊이 6까지
	{
		TArray<UMapNode*>* CurrentDepthNodes = NodesByDepth.Find(Depth);
		TArray<UMapNode*>* NextDepthNodes = NodesByDepth.Find(Depth + 1);

		if (!CurrentDepthNodes || !NextDepthNodes)
		{
			continue;
		}

		// 현재 깊이의 각 노드에서 다음 깊이로 연결
		for (UMapNode* CurrentNode : *CurrentDepthNodes)
		{
			TArray<UMapNode*> PossibleConnections;

			// 연결 가능한 노드들 찾기
			for (UMapNode* NextNode : *NextDepthNodes)
			{
				if (CurrentNode->CanConnectTo(NextNode))
				{
					PossibleConnections.Add(NextNode);
				}
			}

			if (PossibleConnections.Num() == 0)
			{
				continue;
			}

			// 연결 개수 결정
			int32 ConnectionCount = RandomStream.RandRange(
				GenerationSettings.MinConnectionsPerNode,
				FMath::Min(GenerationSettings.MaxConnectionsPerNode, PossibleConnections.Num())
			);

			// 랜덤으로 연결할 노드들 선택
			ShuffleArray(PossibleConnections);

			for (int32 i = 0; i < ConnectionCount; i++)
			{
				CurrentNode->AddConnection(PossibleConnections[i]);
			}
		}
	}
}

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
