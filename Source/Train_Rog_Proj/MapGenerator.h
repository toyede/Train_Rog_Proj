// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapNode.h"
#include "MapGenerator.generated.h"

USTRUCT(BlueprintType)
struct FMapGenerationSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Map Size", meta = (ClampMin = "1", ClampMax = "9"))
	int32 MaxNodesPerDepth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Nodes", meta = (ClampMin = "0"))
	int32 MinShopNodes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Nodes", meta = (ClampMin = "0"))
	int32 MaxShopNodes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Nodes", meta = (ClampMin = "0"))
	int32 MinRepairNodes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Nodes", meta = (ClampMin = "0"))
	int32 MaxRepairNodes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connections", meta = (ClampMin = "1", ClampMax = "3"))
	int32 MinConnectionsPerNode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connections", meta = (ClampMin = "1", ClampMax = "3"))
	int32 MaxConnectionsPerNode;
	// 추가 연결 생성 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connections")
	bool bAllowAdditionalConnections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	int32 RandomSeed;

	// 스테이지 개수 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stages", meta = (ClampMin = "1", ClampMax = "10"))
	int32 NumberOfStages;

	FMapGenerationSettings()
	{
		MaxNodesPerDepth = 6;

		MinShopNodes = 1;
		MaxShopNodes = 2;
		MinRepairNodes = 1;  // Special -> Repair
		MaxRepairNodes = 2;  // Special -> Repair
		MinConnectionsPerNode = 1;
		MaxConnectionsPerNode = 3;

		RandomSeed = -1; // -1이면 자동 시드
		NumberOfStages = 3; // 기본 3스테이지
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapGenerated, int32, NodeCount);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TRAIN_ROG_PROJ_API UMapGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMapGenerator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Generation")
	FMapGenerationSettings GenerationSettings;

	UPROPERTY(BlueprintReadOnly, Category = "Generated Map", meta = (AllowPrivateAccess = "true"))
	TArray<UMapNode*> AllNodes;

	// 깊이별로 정리된 노드들
	TMap<int32, TArray<UMapNode*>> NodesByDepth;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMapGenerated OnMapGenerated;

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	void GenerateMap();

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	void ClearMap();

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	UMapNode* GetStartNode() const;

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	UMapNode* GetRepairNode() const;

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	UMapNode* GetBossNode() const;

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	TArray<UMapNode*> GetNodesAtDepth(int32 Depth) const;

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	bool IsValidMapGenerated() const;

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	void SetPlayerPosition(UMapNode* NewCurrentNode);

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	TArray<UMapNode*> GetAvailableMovements() const;

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	TArray<UMapNode*> GetAllNodes() const;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	UMapNode* CurrentPlayerNode;

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	UMapNode* GetCurrentPlayerNode() const { return CurrentPlayerNode; }

	// 스테이지별 노드 조회
	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	UMapNode* GetStartNodeForStage(int32 StageNumber) const;

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	UMapNode* GetBossNodeForStage(int32 StageNumber) const;

protected:
	// 단일 스테이지 생성
	void GenerateSingleStage(int32 StageNumber);
	// 스테이지 간 연결
	void ConnectStageTransitions();

	void CreateNodesAtDepth(int32 Depth, int32 NodeCount);
	void AssignSpecialNodeTypes();
	void ConnectNodes();
	//void ValidateAllNodesReachable(); EnsureAllNodesReachable로 대체
	//void EnsureAllNodesReachable();

	// 새로운 오버랩 그룹핑 연결 시스템
	void ConnectWithNextNodeGrouping(const TArray<UMapNode*>& PreviousNodes, const TArray<UMapNode*>& NextNodes, int32 P, int32 N, int32& TotalConnections);
	void ConnectWithReversedNodeGrouping(const TArray<UMapNode*>& PreviousNodes, const TArray<UMapNode*>& NextNodes, int32 P, int32 N, int32& TotalConnections);

	void UpdateNodeAccessibility();

	// 위험도 배정 관련 함수
	void AssignDangerLevels(int32 StageNumber);
	void AssignDangerLevelForDepth(int32 ActualDepth, int32 LocalDepth, TArray<UMapNode*>& NodesAtDepth);
	int32 CalculateBaseDifficultyWeight(int32 LocalDepth) const;
	int32 GetDangerLevelBonus(EDangerLevel DangerLevel) const;
	float CalculateMonsterStatMultiplier(int32 LocalDepth, EDangerLevel DangerLevel) const;

	TArray<UMapNode*> GetNormalNodes() const;
	void ShuffleArray(TArray<UMapNode*>& Array) const;
	//bool CanNodeReachDepth(UMapNode* StartNode, int32 TargetDepth) const;
	//void DFS_CheckReachability(UMapNode* CurrentNode, int32 TargetDepth, TSet<UMapNode*>& VisitedNodes, bool& bCanReach) const;
	//void BackwardDFS(UMapNode* CurrentNode, TSet<UMapNode*>& ReachableNodes) const;
	//bool AddEmergencyConnection(UMapNode* IsolatedNode);
	UMapNode* SelectNodeWithFewestConnections(const TArray<UMapNode*>& Nodes) const;


private:
	FRandomStream RandomStream;
		
};
