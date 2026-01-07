// MyGameInstanceBase.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Train_Rog_Proj/MapGenerator.h"
#include "Train_Rog_Proj/MapNode.h"
#include "MyGameInstanceBase.generated.h"

UCLASS()
class TRAIN_ROG_PROJ_API UMyGameInstanceBase : public UGameInstance
{
    GENERATED_BODY()

public:
	UMyGameInstanceBase();

    // 블루프린트에서 수정 및 읽기가 가능하도록 설정합니다.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
    float Bear;

    // ========== 맵 관련 데이터 ==========
	UPROPERTY(BlueprintReadOnly, Category = "Map")
    UMapGenerator* MapGenerator;

    UPROPERTY(BlueprintReadOnly, Category = "Map")
    TArray<UMapNode*> GeneratedNodes;

    UPROPERTY(BlueprintReadOnly, Category = "Map")
    UMapNode* CurrentPlayerNode;

    // ========== 현재 전투 정보 ==========
    // 현재 선택된 노드의 전투 난이도 정보
    UPROPERTY(BlueprintReadWrite, Category = "Current Battle")
    int32 CurrentDangerLevel;  // 1, 2, 3, 4 (4가 정예)

    UPROPERTY(BlueprintReadWrite, Category = "Current Battle")
    int32 CurrentDifficultyWeight;  // 최종 가중치 값

    UPROPERTY(BlueprintReadWrite, Category = "Current Battle")
    float CurrentMonsterStatBonus;  // 1.0, 1.2, 1.3 (배율)

    UPROPERTY(BlueprintReadWrite, Category = "Current Battle")
    bool bIsEliteBattle;  // 정예 전투 여부

    UPROPERTY(BlueprintReadWrite, Category = "Current Battle")
    ENodeType CurrentStationType;  // Normal, Shop, Repair (전투 후 정거장 타입)

    // 맵 생성 여부 확인
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map")
    bool IsMapGenerated() const;

    // 맵 생성 (BeginPlay에서 한 번만 호출)
    UFUNCTION(BlueprintCallable, Category = "Map")
    void GenerateMap(FMapGenerationSettings Settings);

    // 현재 플레이어 노드 설정
    UFUNCTION(BlueprintCallable, Category = "Map")
    void SetCurrentPlayerNode(UMapNode* NewNode);

    // 맵 데이터 초기화 (게임 오버 시 사용)
    UFUNCTION(BlueprintCallable, Category = "Map")
    void ResetMapData();

    // 생성된 노드 배열 반환 (cpp로 구현)
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map")
    TArray<UMapNode*> GetGeneratedNodes() const;

    // 현재 플레이어 노드 반환 (cpp로 구현)
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map")
    UMapNode* GetCurrentPlayerNode() const;

    // 현재 플레이어 노드에서 타겟 노드로 이동 가능한지 확인
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Map")
    bool CanMoveToNode(UMapNode* TargetNode) const;

    // 노드 클릭 시 전투 정보 저장 (Blueprint에서 호출)
    UFUNCTION(BlueprintCallable, Category = "Current Battle")
    void SetCurrentBattleInfo(UMapNode* SelectedNode);

    // 전투 종료 후 전투 정보 초기화 (Blueprint에서 호출)
    UFUNCTION(BlueprintCallable, Category = "Current Battle")
    void ClearCurrentBattleInfo();

protected:
	virtual void Init() override;

private:
	bool bIsMapGenerated;
};