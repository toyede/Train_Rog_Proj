// Fill out your copyright notice in the Description page of Project Settings.


#include "MapUIWidget.h"
#include "MapNodeButton.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/Engine.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void UMapUIWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 기본 설정값 초기화
    DepthSpacing = 150.0f;
    RowSpacing = 100.0f;
    MapOrigin = FVector2D(100.0f, 100.0f);
    MapSize = FVector2D(1200.0f, 800.0f);
    MaxRowsPerDepth = 6; // 기본 6칸 그리드


    // 기찻길 기본 설정
    RailSegmentLength = 40.0f;
    RailSegmentWidth = 16.0f;

    // 카메라 설정 초기화 (블루프린트에서 설정하지 않은 경우만)
    if (CameraZoomLevel <= 0.0f)
        CameraZoomLevel = 1.0f;
    if (CameraTransitionSpeed <= 0.0f)
        CameraTransitionSpeed = 5.0f;
    if (CameraFocusPosition == FVector2D::ZeroVector)
        CameraFocusPosition = FVector2D(0.5f, 0.5f);

    // 현재 플레이어 노드 초기화
    CurrentPlayerNode = nullptr;

    // 닫기 버튼 이벤트 바인딩
    if (CloseButton)
    {
        CloseButton->OnClicked.AddDynamic(this, &UMapUIWidget::CloseMapUI);
    }

    UE_LOG(LogTemp, Log, TEXT("MapUIWidget constructed"));

    // UI Only 모드로 전환 (NativeConstruct 끝에서 호출)
    SetUIOnlyMode();

    UE_LOG(LogTemp, Log, TEXT("MapUIWidget constructed - UI Only mode activated"));
}



void UMapUIWidget::NativeDestruct()
{
    // 위젯이 파괴될 때 Game Only 모드로 복원
    RestoreGameMode();

    Super::NativeDestruct();
}

void UMapUIWidget::SetUIOnlyMode()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is null in SetUIOnlyMode"));
        return;
    }

    // 모든 입력 키 강제 해제 (눌려있던 키 입력 초기화)
    PlayerController->FlushPressedKeys();

    // UI Only 모드로 설정
    FInputModeUIOnly InputMode;
    //InputMode.SetWidgetToFocus(TakeWidget());
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

    PlayerController->SetInputMode(InputMode);

    // 마우스 커서 표시
    PlayerController->bShowMouseCursor = true;

    UE_LOG(LogTemp, Log, TEXT("Switched to UI Only mode with mouse cursor - Inputs flushed"));
}

void UMapUIWidget::RestoreGameMode()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        // Game Only 모드로 복원
        FInputModeGameOnly InputMode;
        PlayerController->SetInputMode(InputMode);
        // 마우스 커서 숨기기
        PlayerController->bShowMouseCursor = false;
        UE_LOG(LogTemp, Log, TEXT("Restored to Game Only mode and hid mouse cursor"));
    }
}



void UMapUIWidget::SetupMapUI(const TArray<UMapNode*>& Nodes, UMapNode* CurrentNode)
{
    if (!MapCanvas)
    {
        UE_LOG(LogTemp, Error, TEXT("MapCanvas not found! Make sure to bind it in the widget blueprint."));
        return;
    }

    // 현재 플레이어 노드 저장
    CurrentPlayerNode = CurrentNode;

    // 각 노드에 대해 UI 버튼 생성
    for (UMapNode* Node : Nodes)
    {
        if (Node)
        {
            int32 Depth = Node->Position.Depth;
            DepthNodeCounts.FindOrAdd(Depth)++;
        }
    }

    // 각 깊이별로 랜덤 행 위치 미리 계산
    for (const auto& DepthCount : DepthNodeCounts)
    {
        int32 Depth = DepthCount.Key;
        int32 NodeCount = DepthCount.Value;

        TArray<int32> RowPositions = GetRandomRowPositions(NodeCount);
        DepthRowPositions.Add(Depth, RowPositions);

        FString PositionString;
        for (int32 i = 0; i < RowPositions.Num(); i++)
        {
            PositionString += FString::Printf(TEXT("%d"), RowPositions[i]);
            if (i < RowPositions.Num() - 1)
            {
                PositionString += TEXT(", ");
            }
        }

        UE_LOG(LogTemp, Log, TEXT("Depth %d: %d nodes at positions [%s]"),
            Depth, NodeCount, *PositionString);
    }

    // 각 노드에 대해 UI 버튼 생성
    for (UMapNode* Node : Nodes)
    {
        if (!Node)
        {
            continue;
        }

        FVector2D Position2D = CalculateOptimalNode2DPosition(Node);
        UMapNodeButton* NodeButton = CreateNodeButton(Node, Position2D);

        if (NodeButton)
        {
            FNodeUIInfo UIInfo;
            UIInfo.LinkedNode = Node;
            UIInfo.NodeButton = NodeButton;
            UIInfo.Position2D = Position2D;

            NodeUIElements.Add(UIInfo);

            UE_LOG(LogTemp, Log, TEXT("Created UI button for %s node at (%f, %f) - Depth %d, Row %d"),
                *Node->GetNodeTypeString(), Position2D.X, Position2D.Y,
                Node->Position.Depth, Node->Position.Row);
        }
    }

    UpdateNodeStates();
    DrawConnectionLines();

    // 현재 노드가 있으면 해당 노드에 카메라 포커스
    if (CurrentPlayerNode)
    {
        FocusCameraOnNode(CurrentPlayerNode);
        UE_LOG(LogTemp, Log, TEXT("Map UI setup complete with camera focused on current node"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Map UI setup complete without camera focus"));
    }

    UE_LOG(LogTemp, Log, TEXT("Total node buttons created: %d"), NodeUIElements.Num());
}

void UMapUIWidget::ClearMapUI()
{
    // 기존 노드 버튼들 제거
    for (const FNodeUIInfo& UIInfo : NodeUIElements)
    {
        if (UIInfo.NodeButton && IsValid(UIInfo.NodeButton))
        {
            UIInfo.NodeButton->RemoveFromParent();
        }
    }

    NodeUIElements.Empty();
    // 기존 기찻길들 제거
    ClearAllRails();
    UE_LOG(LogTemp, Log, TEXT("Map UI cleared"));
}

void UMapUIWidget::UpdateNodeStates()
{
    // 모든 노드 버튼의 상태를 업데이트
    for (const FNodeUIInfo& UIInfo : NodeUIElements)
    {
        if (UIInfo.NodeButton && UIInfo.LinkedNode)
        {
            UIInfo.NodeButton->UpdateNodeButtonState();
        }
    }
}

void UMapUIWidget::OnNodeButtonClicked(UMapNode* ClickedNode)
{
    // 노드 클릭 관련 이벤트는 블루프린트를 통해 처리
}

void UMapUIWidget::CloseMapUI()
{
    // UI 제거
    RemoveFromParent();

    UE_LOG(LogTemp, Log, TEXT("Map UI closed"));
}

void UMapUIWidget::ClearAllRails()
{
    // 기존 기찻길 Image들 제거
    for (UImage* RailImage : RailImages)
    {
        if (RailImage && IsValid(RailImage))
        {
            RailImage->RemoveFromParent();
        }
    }

    RailImages.Empty();
}



/*
FVector2D UMapUIWidget::CalculateNode2DPosition(UMapNode* Node) const
{
    if (!Node)
    {
        return FVector2D::ZeroVector;
    }

    // 노드의 깊이와 행을 기반으로 2D 위치 계산
    float X = MapOrigin.X + (Node->Position.Depth * DepthSpacing);
    float Y = MapOrigin.Y + (Node->Position.Row * RowSpacing);

    // 맵 크기 범위 내로 제한 (선택사항)
    X = FMath::Clamp(X, 0.0f, MapSize.X);
    Y = FMath::Clamp(Y, 0.0f, MapSize.Y);

    return FVector2D(X, Y);
}
*/



FVector2D UMapUIWidget::CalculateOptimalNode2DPosition(UMapNode* Node) const
{
    if (!Node)
    {
        return FVector2D::ZeroVector;
    }

    // X 좌표는 깊이 기반
    float X = MapOrigin.X + (Node->Position.Depth * DepthSpacing);

    // Y 좌표는 미리 계산된 랜덤 행 위치 사용
    float Y = MapOrigin.Y;

    int32 Depth = Node->Position.Depth;
    int32 Row = Node->Position.Row;

    const TArray<int32>* RowPositions = DepthRowPositions.Find(Depth);
    if (RowPositions && Row >= 0 && Row < RowPositions->Num())
    {
        int32 RowPosition = (*RowPositions)[Row];
        Y = CalculateYForRowPosition(RowPosition);
    }
    else
    {
        // fallback: 기존 방식
        Y = MapOrigin.Y + (Row * RowSpacing);
        UE_LOG(LogTemp, Warning, TEXT("Using fallback position for Node at Depth %d, Row %d"), Depth, Row);
    }

    // 맵 크기 범위 내로 제한
    X = FMath::Clamp(X, 0.0f, MapSize.X);
    Y = FMath::Clamp(Y, 0.0f, MapSize.Y);

    return FVector2D(X, Y);
}

UMapNodeButton* UMapUIWidget::CreateNodeButton(UMapNode* Node, const FVector2D& Position)
{
    if (!Node || !MapCanvas)
    {
        return nullptr;
    }

    // 노드 타입에 따른 버튼 클래스 선택
    TSubclassOf<UMapNodeButton> ButtonClass = GetButtonClassForNodeType(Node->NodeType);

    if (!ButtonClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("No button class set for node type: %s"), *Node->GetNodeTypeString());
        return nullptr;
    }

    // 버튼 위젯 생성
    UMapNodeButton* NodeButton = CreateWidget<UMapNodeButton>(GetWorld(), ButtonClass);

    if (!NodeButton)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create node button for type: %s"), *Node->GetNodeTypeString());
        return nullptr;
    }

    // 노드 데이터 설정
    NodeButton->SetupNodeButton(Node);

    // 버튼 클릭 이벤트 바인딩
    //NodeButton->OnNodeButtonClicked.AddDynamic(this, &UMapUIWidget::OnNodeButtonClicked);

    // 캔버스에 버튼 추가
    UCanvasPanelSlot* CanvasSlot = MapCanvas->AddChildToCanvas(NodeButton);

    if (CanvasSlot)
    {
        // 위치 설정
        CanvasSlot->SetPosition(Position);
        CanvasSlot->SetSize(FVector2D(80.0f, 80.0f)); // 기본 버튼 크기
        CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 0.0f, 0.0f)); // 좌상단 기준
    }

    UE_LOG(LogTemp, Log, TEXT("Created %s button at (%f, %f)"),
        *Node->GetNodeTypeString(), Position.X, Position.Y);

    return NodeButton;
}

TSubclassOf<class UMapNodeButton> UMapUIWidget::GetButtonClassForNodeType(ENodeType NodeType) const
{
    switch (NodeType)
    {
    case ENodeType::Start:
        return StartNodeButtonClass;
    case ENodeType::Normal:
        return NormalNodeButtonClass;
    case ENodeType::Shop:
        return ShopNodeButtonClass;
    case ENodeType::Special:
        return SpecialNodeButtonClass;
    case ENodeType::Repair:
        return RepairNodeButtonClass;
    case ENodeType::Boss:
        return BossNodeButtonClass;
    default:
        return NormalNodeButtonClass; // 기본값
    }
}

void UMapUIWidget::DrawConnectionLines()
{
    // 기존 기찻길들 제거
    ClearAllRails();

    if (!RailSegmentTexture)
    {
        UE_LOG(LogTemp, Warning, TEXT("Rail segment texture not set! Please assign texture in blueprint."));
        return;
    }

    // 각 노드의 연결선 그리기
    for (const FNodeUIInfo& UIInfo : NodeUIElements)
    {
        if (!UIInfo.LinkedNode)
        {
            continue;
        }

        // 현재 노드에서 연결된 모든 노드로 선 그리기
        for (UMapNode* ConnectedNode : UIInfo.LinkedNode->ConnectedNodes)
        {
            if (!ConnectedNode)
            {
                continue;
            }

            // 연결된 노드의 위치 찾기
            FVector2D EndPos = GetNodePosition(ConnectedNode);
            if (EndPos != FVector2D::ZeroVector)
            {
                CreateRailBetweenNodes(UIInfo.Position2D, EndPos);
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Connection lines drawn: %d rail segments created"), RailImages.Num());
}

void UMapUIWidget::CreateRailBetweenNodes(FVector2D StartPos, FVector2D EndPos)
{
    // 거리와 방향 계산
    FVector2D Direction = EndPos - StartPos;
    float Distance = Direction.Size();

    if (Distance < 1.0f)
    {
        return; // 너무 가까우면 기찻길 생성하지 않음
    }

    // 회전 각도 계산 (라디안을 도로 변환)
    float RotationRadians = FMath::Atan2(Direction.Y, Direction.X);
    float RotationDegrees = FMath::RadiansToDegrees(RotationRadians);

    // 필요한 세그먼트 개수 계산
    int32 SegmentCount = FMath::CeilToInt(Distance / RailSegmentLength);

    // 각 세그먼트 배치
    for (int32 i = 0; i < SegmentCount; i++)
    {
        // 시작점에서 끝점까지 선형 보간
        float Alpha = (float)i / FMath::Max(1, SegmentCount - 1);
        FVector2D SegmentPosition = FMath::Lerp(StartPos, EndPos, Alpha);

        // 기찻길 세그먼트 생성
        UImage* RailSegment = CreateRailSegment(SegmentPosition, RotationDegrees);
        if (RailSegment)
        {
            RailImages.Add(RailSegment);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Created rail between (%.1f,%.1f) and (%.1f,%.1f) with %d segments, rotation %.1f degrees"),
        StartPos.X, StartPos.Y, EndPos.X, EndPos.Y, SegmentCount, RotationDegrees);
}

UImage* UMapUIWidget::CreateRailSegment(FVector2D Position, float RotationDegrees)
{
    if (!MapCanvas || !RailSegmentTexture)
    {
        return nullptr;
    }

    // Image 위젯 생성
    UImage* RailImage = NewObject<UImage>(this);
    if (!RailImage)
    {
        return nullptr;
    }

    // 텍스처 설정
    RailImage->SetBrushFromTexture(RailSegmentTexture);

    // 캔버스에 추가 (노드들보다 뒤에 배치하기 위해 낮은 Z-Order)
    UCanvasPanelSlot* CanvasSlot = MapCanvas->AddChildToCanvas(RailImage);
    if (CanvasSlot)
    {
        // 위치 설정 (중심점 기준으로 배치)
        FVector2D AdjustedPosition = Position - FVector2D(RailSegmentLength * 0.5f, RailSegmentWidth * 0.5f);
        CanvasSlot->SetPosition(AdjustedPosition);
        CanvasSlot->SetSize(FVector2D(RailSegmentLength, RailSegmentWidth));
        CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 0.0f, 0.0f));

        // 회전 설정
        RailImage->SetRenderTransformAngle(RotationDegrees);

        // Z-Order 설정 (노드들보다 뒤에)
        CanvasSlot->SetZOrder(-1);
    }

    return RailImage;
}

FVector2D UMapUIWidget::GetNodePosition(UMapNode* Node) const
{
    if (!Node)
    {
        return FVector2D::ZeroVector;
    }

    // NodeUIElements에서 해당 노드의 위치 찾기
    for (const FNodeUIInfo& UIInfo : NodeUIElements)
    {
        if (UIInfo.LinkedNode == Node)
        {
            return UIInfo.Position2D;
        }
    }

    // 찾지 못한 경우 위치 계산
    return CalculateOptimalNode2DPosition(Node);
}

TArray<int32> UMapUIWidget::GetRandomRowPositions(int32 NodeCount) const
{
    TArray<int32> Positions;

    if (NodeCount <= 0)
    {
        return Positions;
    }

    // 1개인 경우 중앙에 배치
    if (NodeCount == 1)
    {
        int32 CenterPosition = (MaxRowsPerDepth - 1) / 2;
        Positions.Add(CenterPosition);
        return Positions;
    }

    // MaxRowsPerDepth개 이상인 경우 모든 칸 사용
    if (NodeCount >= MaxRowsPerDepth)
    {
        for (int32 i = 0; i < MaxRowsPerDepth; i++)
        {
            Positions.Add(i);
        }
        return Positions;
    }

    // 2~(MaxRowsPerDepth-1)개인 경우 0~(MaxRowsPerDepth-1)번 칸 중에서 랜덤으로 선택
    TArray<int32> AvailablePositions;
    for (int32 i = 0; i < MaxRowsPerDepth; i++)
    {
        AvailablePositions.Add(i);
    }

    // 랜덤으로 NodeCount개만큼 선택
    for (int32 i = 0; i < NodeCount && AvailablePositions.Num() > 0; i++)
    {
        int32 RandomIndex = RandomStream.RandRange(0, AvailablePositions.Num() - 1);
        Positions.Add(AvailablePositions[RandomIndex]);
        AvailablePositions.RemoveAt(RandomIndex);
    }

    // 선택된 위치들을 정렬 (위에서 아래 순서로)
    Positions.Sort();

    return Positions;
}

float UMapUIWidget::CalculateYForRowPosition(int32 RowPosition) const
{
    return MapOrigin.Y + (RowPosition * RowSpacing);
}

void UMapUIWidget::FocusCameraOnNode(UMapNode* TargetNode)
{
    if (!TargetNode)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot focus camera - TargetNode is null"));
        return;
    }

    if (!MapCanvas)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot focus camera - MapCanvas is null"));
        return;
    }

    // 타겟 노드의 2D 위치 가져오기
    FVector2D TargetPosition = GetNodePosition(TargetNode);

    UE_LOG(LogTemp, Warning, TEXT("=== Camera Focus Debug ==="));
    UE_LOG(LogTemp, Warning, TEXT("Target Node: Depth %d, Row %d"), TargetNode->Position.Depth, TargetNode->Position.Row);
    UE_LOG(LogTemp, Warning, TEXT("Target Position: (%.1f, %.1f)"), TargetPosition.X, TargetPosition.Y);

    if (TargetPosition == FVector2D::ZeroVector)
    {
        UE_LOG(LogTemp, Error, TEXT("Target position is zero - Node might not be in NodeUIElements"));
        return;
    }

    // 캔버스 크기 가져오기
    FVector2D CanvasSize = MapCanvas->GetCachedGeometry().GetLocalSize();
    UE_LOG(LogTemp, Warning, TEXT("Canvas Size: (%.1f, %.1f)"), CanvasSize.X, CanvasSize.Y);

    // 캔버스 크기가 0이면 아직 렌더링 안된 상태
    if (CanvasSize.X < 1.0f || CanvasSize.Y < 1.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("Canvas not ready yet, trying again next frame"));

        // 다음 프레임에 다시 시도
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, TargetNode]()
        {
            FocusCameraOnNode(TargetNode);
        }, 0.1f, false);
        return;
    }

    // CameraFocusPosition을 사용하여 화면상 위치 계산
    // (0.5, 0.5) = 정중앙, (0.3, 0.5) = 왼쪽으로 치우침, (0.7, 0.5) = 오른쪽으로 치우침
    FVector2D FocusPoint = CanvasSize * CameraFocusPosition;
    UE_LOG(LogTemp, Warning, TEXT("Focus Point: (%.1f, %.1f) - CameraFocusPosition: (%.2f, %.2f)"),
        FocusPoint.X, FocusPoint.Y, CameraFocusPosition.X, CameraFocusPosition.Y);

    // 타겟 노드가 FocusPoint에 오도록 캔버스 오프셋 계산
    FVector2D NewOffset = FocusPoint - TargetPosition;
    UE_LOG(LogTemp, Warning, TEXT("Calculated Offset (before zoom): (%.1f, %.1f)"), NewOffset.X, NewOffset.Y);

    // 줌 레벨 적용
    NewOffset *= CameraZoomLevel;
    UE_LOG(LogTemp, Warning, TEXT("Final Offset (after zoom %.2f): (%.1f, %.1f)"), CameraZoomLevel, NewOffset.X, NewOffset.Y);

    // 캔버스 위치 설정 (RenderTransform 사용)
    MapCanvas->SetRenderTranslation(NewOffset);
    MapCanvas->SetRenderScale(FVector2D(CameraZoomLevel, CameraZoomLevel));

    UE_LOG(LogTemp, Log, TEXT("Camera focused successfully on node at Depth %d, Row %d"),
        TargetNode->Position.Depth, TargetNode->Position.Row);
    UE_LOG(LogTemp, Warning, TEXT("======================"));
}

void UMapUIWidget::CenterCameraOnCurrentNode()
{
    if (CurrentPlayerNode)
    {
        FocusCameraOnNode(CurrentPlayerNode);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot center camera - CurrentPlayerNode is null"));
    }
}

