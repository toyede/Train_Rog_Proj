// Fill out your copyright notice in the Description page of Project Settings.


#include "MapNode.h"

UMapNode::UMapNode()
{
	NodeType = ENodeType::Normal;
	Position = FNodePosition(0, 0);
	bIsVisited = false;
	bIsAccessible = false;
	ConnectedNodes.Empty();
	PreviousNodes.Empty();
}

void UMapNode::AddConnection(UMapNode* TargetNode)
{
	if(!TargetNode || !CanConnectTo(TargetNode))
	{
		return;
	}

	if(ConnectedNodes.Contains(TargetNode))
	{
		return; // 이미 연결되어 있는 경우
	}

	ConnectedNodes.Add(TargetNode);
	TargetNode->PreviousNodes.Add(this);
}

bool UMapNode::CanConnectTo(UMapNode* TargetNode) const
{
	if (!TargetNode)
	{
		return false;
	}

	return IsValidConnection(TargetNode);
}

TArray<UMapNode*> UMapNode::GetAvailableNextNodes() const
{
	TArray<UMapNode*> AvailableNodes;

	for(UMapNode* ConnectedNode : ConnectedNodes)
	{
		if(ConnectedNode && ConnectedNode->bIsAccessible)
		{
			AvailableNodes.Add(ConnectedNode);
		}
	}
	return AvailableNodes;
}

void UMapNode::SetNodeType(ENodeType InNodeType)
{
	NodeType = InNodeType;
}

FString UMapNode::GetNodeTypeString() const
{
	switch (NodeType)
	{
	case ENodeType::Start:
		return TEXT("Start");
	case ENodeType::Normal:
		return TEXT("Normal");
	case ENodeType::Shop:
		return TEXT("Shop");
	case ENodeType::Special:
		return TEXT("Special");
	case ENodeType::Repair:
		return TEXT("Repair");
	case ENodeType::Boss:
		return TEXT("Boss");
	default:
		return TEXT("Unknown");
	}
}

bool UMapNode::IsAtDepth(int32 TargetDepth) const
{
	return Position.Depth == TargetDepth;
}

bool UMapNode::IsValidConnection(UMapNode* TargetNode) const
{
	if(!TargetNode)
	{
		return false;
	}
	if(TargetNode->Position.Depth != Position.Depth + 1)
	{
		return false; // 연결된 노드의 깊이가 현재 노드보다 한 단계 아래여야 함
	}

	// 보스 노드로의 연결은 행 제약 완화 (모든 상점이 보스에 도달 가능)
	if (TargetNode->NodeType == ENodeType::Boss)
	{
		return true;
	}

	// 시작 노드에서의 연결은 행 제약 완화 (모든 깊이1 노드에 도달 가능)
	if (NodeType == ENodeType::Start)
	{
		return true;
	}

	// 교차 방지: 행 차이가 1 이하여야 함 (±1칸 범위 내에서만 연결 허용)
	/*
	int32 RowDifference = FMath::Abs(TargetNode->Position.Row - Position.Row);
	if (RowDifference > 1)
	{
		return false; // 너무 멀리 떨어진 행으로는 연결 불가 (교차 발생 방지)
	}
	*/
	return true; // 기본적으로 연결 가능
}
