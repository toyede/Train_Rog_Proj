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