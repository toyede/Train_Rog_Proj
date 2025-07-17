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
		return; // �̹� ����Ǿ� �ִ� ���
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
		return false; // ����� ����� ���̰� ���� ��庸�� �� �ܰ� �Ʒ����� ��
	}

	return true; // �⺻������ ���� ����
}
