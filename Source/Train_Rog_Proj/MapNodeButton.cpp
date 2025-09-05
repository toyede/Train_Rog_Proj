// Fill out your copyright notice in the Description page of Project Settings.


#include "MapNodeButton.h"
#include "Engine/Engine.h"

void UMapNodeButton::SetupNodeButton(UMapNode* Node)
{
    if (!Node)
    {
        UE_LOG(LogTemp, Warning, TEXT("Trying to setup node button with null node"));
        return;
    }

    LinkedNode = Node;
    UE_LOG(LogTemp, Log, TEXT("Node button setup for %s node"), *Node->GetNodeTypeString());
}

void UMapNodeButton::UpdateNodeButtonState()
{
}

void UMapNodeButton::SetNodeAccessible(bool bAccessible)
{
    if (LinkedNode)
    {
        LinkedNode->bIsAccessible = bAccessible;
    }
}

void UMapNodeButton::SetNodeCurrent(bool bCurrent)
{
}

