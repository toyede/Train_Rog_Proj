// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue/MyGameInstanceBase.h"
#include "Engine/Engine.h"

// 생성자를 추가하여 초기값을 설정할 수 있습니다.
UMyGameInstanceBase::UMyGameInstanceBase()
{
    Bear = 0.0f;

    // 맵 관련 초기화
    MapGenerator = nullptr;
    CurrentPlayerNode = nullptr;
    bIsMapGenerated = false;

    // 전투 정보 초기화
    CurrentDangerLevel = 1;
    CurrentDifficultyWeight = 0;
    CurrentMonsterStatBonus = 1.0f;
    bIsEliteBattle = false;
    CurrentStationType = ENodeType::Normal;
}

void UMyGameInstanceBase::Init()
{
    Super::Init();
}

bool UMyGameInstanceBase::IsMapGenerated() const
{
    return bIsMapGenerated && GeneratedNodes.Num() > 0;
}

void UMyGameInstanceBase::GenerateMap(FMapGenerationSettings Settings)
{
    // 이미 생성된 경우 중복 생성 방지
    if (bIsMapGenerated)
    {
        UE_LOG(LogTemp, Warning, TEXT("Map already generated! Use ResetMapData() to regenerate."));
        return;
    }

    // MapGenerator 생성
    if (!MapGenerator)
    {
        MapGenerator = NewObject<UMapGenerator>(this);
        if (!MapGenerator)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create MapGenerator!"));
            return;
        }
    }

    // 설정 적용
    MapGenerator->GenerationSettings = Settings;

    // 맵 생성
    MapGenerator->GenerateMap();

    // 생성된 노드들 저장
    GeneratedNodes = MapGenerator->GetAllNodes();

    // 시작 노드를 현재 플레이어 노드로 설정
    CurrentPlayerNode = MapGenerator->GetStartNode();

    // 맵 생성 완료 플래그
    bIsMapGenerated = true;

    UE_LOG(LogTemp, Log, TEXT("Map generated in GameInstance: %d nodes created"), GeneratedNodes.Num());

    if (CurrentPlayerNode)
    {
        UE_LOG(LogTemp, Log, TEXT("Current player node set to Start node (Depth %d, Row %d)"),
            CurrentPlayerNode->Position.Depth, CurrentPlayerNode->Position.Row);
    }
}

void UMyGameInstanceBase::SetCurrentPlayerNode(UMapNode* NewNode)
{
    if (!NewNode)
    {
        UE_LOG(LogTemp, Warning, TEXT("Trying to set null node as current player node"));
        return;
    }

    // 이전 노드를 방문 처리
    if (CurrentPlayerNode)
    {
        CurrentPlayerNode->bIsVisited = true;
        UE_LOG(LogTemp, Log, TEXT("Previous node (Depth %d, Row %d) marked as visited"),
            CurrentPlayerNode->Position.Depth, CurrentPlayerNode->Position.Row);
    }

    // 새 노드로 업데이트
    CurrentPlayerNode = NewNode;
    CurrentPlayerNode->bIsVisited = true;

    // MapGenerator의 플레이어 위치도 업데이트
    if (MapGenerator)
    {
        MapGenerator->SetPlayerPosition(NewNode);
    }

    UE_LOG(LogTemp, Log, TEXT("Current player node updated to (Depth %d, Row %d)"),
        CurrentPlayerNode->Position.Depth, CurrentPlayerNode->Position.Row);
}

void UMyGameInstanceBase::ResetMapData()
{
    // 맵 데이터 초기화
    if (MapGenerator)
    {
        MapGenerator->ClearMap();
    }

    GeneratedNodes.Empty();
    CurrentPlayerNode = nullptr;
    bIsMapGenerated = false;

    UE_LOG(LogTemp, Log, TEXT("Map data reset - ready for new game"));
}

TArray<UMapNode*> UMyGameInstanceBase::GetGeneratedNodes() const
{
    return GeneratedNodes;
}

UMapNode* UMyGameInstanceBase::GetCurrentPlayerNode() const
{
    return CurrentPlayerNode;
}

bool UMyGameInstanceBase::CanMoveToNode(UMapNode* TargetNode) const
{
    // 타겟 노드가 null인 경우
    if (!TargetNode)
    {
        UE_LOG(LogTemp, Warning, TEXT("CanMoveToNode: TargetNode is null"));
        return false;
    }

    // 현재 플레이어 노드가 없는 경우
    if (!CurrentPlayerNode)
    {
        UE_LOG(LogTemp, Warning, TEXT("CanMoveToNode: CurrentPlayerNode is null"));
        return false;
    }

    // 현재 노드에서 타겟 노드로 연결되어 있는지 확인
    bool bIsConnected = CurrentPlayerNode->ConnectedNodes.Contains(TargetNode);

    if (bIsConnected)
    {
        UE_LOG(LogTemp, Log, TEXT("CanMoveToNode: Move allowed from (Depth %d, Row %d) to (Depth %d, Row %d)"),
            CurrentPlayerNode->Position.Depth, CurrentPlayerNode->Position.Row,
            TargetNode->Position.Depth, TargetNode->Position.Row);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("CanMoveToNode: Move NOT allowed from (Depth %d, Row %d) to (Depth %d, Row %d) - Not connected!"),
            CurrentPlayerNode->Position.Depth, CurrentPlayerNode->Position.Row,
            TargetNode->Position.Depth, TargetNode->Position.Row);
    }

    return bIsConnected;
}

void UMyGameInstanceBase::SetCurrentBattleInfo(UMapNode* SelectedNode)
{
    if (!SelectedNode)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot set battle info - SelectedNode is null"));
        return;
    }

    // EDangerLevel을 int32로 변환 (1~4)
    CurrentDangerLevel = static_cast<int32>(SelectedNode->DangerLevel) + 1;
    CurrentDifficultyWeight = SelectedNode->DifficultyWeight;
    CurrentMonsterStatBonus = SelectedNode->MonsterStatMultiplier;
    bIsEliteBattle = SelectedNode->bIsEliteLevel;
    CurrentStationType = SelectedNode->NodeType;

    UE_LOG(LogTemp, Log, TEXT("=== Battle Info Set ==="));
    UE_LOG(LogTemp, Log, TEXT("Danger Level: %d"), CurrentDangerLevel);
    UE_LOG(LogTemp, Log, TEXT("Difficulty Weight: %d"), CurrentDifficultyWeight);
    UE_LOG(LogTemp, Log, TEXT("Monster Stat Bonus: %.2f"), CurrentMonsterStatBonus);
    UE_LOG(LogTemp, Log, TEXT("Is Elite: %s"), bIsEliteBattle ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Log, TEXT("Station Type: %s"), *UEnum::GetValueAsString(CurrentStationType));
    UE_LOG(LogTemp, Log, TEXT("======================="));
}

void UMyGameInstanceBase::ClearCurrentBattleInfo()
{
    CurrentDangerLevel = 1;
    CurrentDifficultyWeight = 0;
    CurrentMonsterStatBonus = 1.0f;
    bIsEliteBattle = false;
    CurrentStationType = ENodeType::Normal;

    UE_LOG(LogTemp, Log, TEXT("Battle info cleared"));
}

