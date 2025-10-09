// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "MapNode.h"
#include "MapUIWidget.generated.h"


// 2D 노드 UI 정보 구조체
USTRUCT(BlueprintType)
struct FNodeUIInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Node UI")
    UMapNode* LinkedNode;

    UPROPERTY(BlueprintReadWrite, Category = "Node UI")
    class UMapNodeButton* NodeButton;

    UPROPERTY(BlueprintReadWrite, Category = "Node UI")
    FVector2D Position2D;

    FNodeUIInfo()
    {
        LinkedNode = nullptr;
        NodeButton = nullptr;
        Position2D = FVector2D::ZeroVector;
    }
};

/**
 * 
 */
UCLASS()
class TRAIN_ROG_PROJ_API UMapUIWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    UMapUIWidget(const FObjectInitializer& ObjectInitializer);

    // 아트 에셋 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Art Assets")
    class UTexture2D* MapBackgroundTexture; // 아트팀 지도 종이 이미지

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Art Assets")
    class UMaterialInterface* MapBackgroundMaterial; // 지도 종이 머티리얼

    // 기찻길 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail Settings")
    class UTexture2D* RailSegmentTexture; // 기찻길 조각 이미지

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail Settings")
    float RailSegmentLength; // 기찻길 조각 길이 (픽셀)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail Settings")
    float RailSegmentWidth; // 기찻길 조각 폭 (픽셀)

    // UI 레이아웃 설정 (아트 에셋에 맞춰 조정)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    float DepthSpacing; // 깊이 간 거리 (픽셀)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    float RowSpacing; // 행 간 거리 (픽셀)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    FVector2D MapOrigin; // 지도 시작 위치 (아트 에셋 기준)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    FVector2D MapSize; // 지도 전체 크기 (아트 에셋 크기)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    int32 MaxRowsPerDepth; // 각 깊이당 최대 행 수 (기본 6)


    // 노드 버튼 블루프린트 클래스들 (아트팀 디자인)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Art Assets")
    TSubclassOf<class UMapNodeButton> StartNodeButtonClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Art Assets")
    TSubclassOf<class UMapNodeButton> NormalNodeButtonClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Art Assets")
    TSubclassOf<class UMapNodeButton> ShopNodeButtonClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Art Assets")
    TSubclassOf<class UMapNodeButton> SpecialNodeButtonClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Art Assets")
    TSubclassOf<class UMapNodeButton> RepairNodeButtonClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Art Assets")
    TSubclassOf<class UMapNodeButton> BossNodeButtonClass;

    // UI 컴포넌트들
    UPROPERTY(BlueprintReadOnly, Category = "UI Components", meta = (BindWidget))
    class UCanvasPanel* MapCanvas;

    UPROPERTY(BlueprintReadOnly, Category = "UI Components", meta = (BindWidget))
    class UButton* CloseButton;

    // 카메라 관련 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
    float CameraZoomLevel; // 카메라 줌 레벨 (1.0 = 기본)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
    float CameraTransitionSpeed; // 카메라 이동 속도

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
    bool bEnableCameraSmoothing; // 부드러운 카메라 이동

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    FVector2D CameraFocusPosition; // 노드가 위치할 화면상 위치 (0.0~1.0, 0.5=중앙)

    // 생성된 노드 UI들
    UPROPERTY(BlueprintReadOnly, Category = "Generated UI")
    TArray<FNodeUIInfo> NodeUIElements;

    // 생성된 기찻길들
    UPROPERTY(BlueprintReadOnly, Category = "Generated Rails")
    TArray<class UImage*> RailImages;

    // 블루프린트에서 호출 가능한 함수들
    UFUNCTION(BlueprintCallable, Category = "Map UI")
    void SetupMapUI(const TArray<UMapNode*>& Nodes, UMapNode* CurrentNode = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Map UI")
    void ClearMapUI();

    UFUNCTION(BlueprintCallable, Category = "Map UI")
    void UpdateNodeStates();

    UFUNCTION(BlueprintCallable, Category = "Map UI")
    void OnNodeButtonClicked(UMapNode* ClickedNode);

    UFUNCTION(BlueprintCallable, Category = "Map UI")
    void CloseMapUI();

    UFUNCTION(BlueprintCallable, Category = "Map UI")
    void ClearAllRails();

    // 입력 모드 제어 함수들
    UFUNCTION(BlueprintCallable, Category = "Map UI")
    void SetUIOnlyMode();

    UFUNCTION(BlueprintCallable, Category = "Map UI")
    void RestoreGameMode();

    // 카메라 제어 함수들
    UFUNCTION(BlueprintCallable, Category = "Map UI")
    void FocusCameraOnNode(UMapNode* TargetNode);

    UFUNCTION(BlueprintCallable, Category = "Map UI")
    void CenterCameraOnCurrentNode();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


    // 내부 함수들
    FVector2D CalculateOptimalNode2DPosition(UMapNode* Node) const;
    class UMapNodeButton* CreateNodeButton(UMapNode* Node, const FVector2D& Position);
    TSubclassOf<class UMapNodeButton> GetButtonClassForNodeType(ENodeType NodeType) const;
    void DrawConnectionLines();
    void CreateRailBetweenNodes(FVector2D StartPos, FVector2D EndPos);
    class UImage* CreateRailSegment(FVector2D Position, float RotationDegrees);
    FVector2D GetNodePosition(UMapNode* Node) const;

    // 노드 배치 관련 함수들
    TArray<int32> GetRandomRowPositions(int32 NodeCount) const;
    float CalculateYForRowPosition(int32 RowPosition) const;

private:
    // 깊이별 노드 수 캐시
    UPROPERTY()
    TMap<int32, int32> DepthNodeCounts;

    // 깊이별 랜덤 행 위치 캐시 (깊이 → 행 위치 배열)
    mutable TMap<int32, TArray<int32>> DepthRowPositions;

    // 랜덤 스트림 (일관된 배치를 위해)
    mutable FRandomStream RandomStream;

    // 현재 플레이어 노드 (카메라 포커스용)
    UPROPERTY()
    UMapNode* CurrentPlayerNode;

    // 카메라 보간용 변수
    FVector2D CurrentCameraOffset;
    FVector2D TargetCameraOffset;
    float CurrentCameraScale;
    float TargetCameraScale;
    bool bIsCameraMoving;
};
