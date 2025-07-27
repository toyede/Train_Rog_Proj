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
	int32 MinSpecialNodes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Nodes", meta = (ClampMin = "0"))
	int32 MaxSpecialNodes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connections", meta = (ClampMin = "1", ClampMax = "3"))
	int32 MinConnectionsPerNode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connections", meta = (ClampMin = "1", ClampMax = "3"))
	int32 MaxConnectionsPerNode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	int32 RandomSeed;

	FMapGenerationSettings()
	{
		MaxNodesPerDepth = 6;

		MinShopNodes = 1;
		MaxShopNodes = 2;
		MinSpecialNodes = 1;
		MaxSpecialNodes = 2;
		MinConnectionsPerNode = 1;
		MaxConnectionsPerNode = 3;

		RandomSeed = -1; // -1이면 자동 시드
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

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	UMapNode* CurrentPlayerNode;

	void CreateNodesAtDepth(int32 Depth, int32 NodeCount);
	void AssignSpecialNodeTypes();
	void ConnectNodes();
	void ValidateAllNodesReachable();
	void UpdateNodeAccessibility();

	TArray<UMapNode*> GetNormalNodes() const;
	void ShuffleArray(TArray<UMapNode*>& Array) const;
	bool CanNodeReachDepth(UMapNode* StartNode, int32 TargetDepth) const;
	void DFS_CheckReachability(UMapNode* CurrentNode, int32 TargetDepth, TSet<UMapNode*>& VisitedNodes, bool& bCanReach) const;

private:
	FRandomStream RandomStream;
		
};
