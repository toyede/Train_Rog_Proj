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

protected:
	virtual void Init() override;

private:
	bool bIsMapGenerated;
};