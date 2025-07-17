// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MapNode.generated.h"

UENUM(BlueprintType)
enum class ENodeType : uint8
{
	Start UMETA(DisplayName = "Start Node"),
	Normal UMETA(DisplayName = "Normal Node"),
	Shop UMETA(DisplayName = "Shop Node"),
	Special UMETA(DisplayName = "Special Event Node"),
	Repair UMETA(DisplayName = "Repair Node"),
	Boss UMETA(DisplayName = "Boss Node")
};

USTRUCT(BlueprintType)
struct FNodePosition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Depth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Row;

	FNodePosition()		//�⺻ ������
	{
		Depth = 0;
		Row = 0;
	}

	FNodePosition(int32 InDepth, int32 InRow)		//�Ű����� �ִ� ������
	{
		Depth = InDepth;
		Row = InRow;
	}

	// �� ������ �����ε�. ���߿� ���� ��ġ�� ��尡 ������ ���� ������ ���.
	bool operator==(const FNodePosition& Other) const
	{
		return Depth == Other.Depth && Row == Other.Row;
	}

	bool operator!=(const FNodePosition& Other) const
	{
		return !(*this == Other);
	}
};


UCLASS(BlueprintType)
class TRAIN_ROG_PROJ_API UMapNode : public UObject
{
	GENERATED_BODY()

public:
	UMapNode();

	//��� �⺻ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Info")
	ENodeType NodeType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Info")
	FNodePosition Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Info")
	bool bIsVisited;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Info")
	bool bIsAccessible;

	// ����� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connections")
	TArray<UMapNode*> ConnectedNodes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connections")
	TArray<UMapNode*> PreviousNodes;

	//�������Ʈ���� ȣ�� ������ �Լ���
	UFUNCTION(BlueprintCallable, Category = "Node Functions")
	void AddConnection(UMapNode* TargetNode);

	UFUNCTION(BlueprintCallable, Category = "Node Functions")
	bool CanConnectTo(UMapNode* TargetNode) const;

	UFUNCTION(BlueprintCallable, Category = "Node Functions")
	TArray<UMapNode*> GetAvailableNextNodes() const;

	UFUNCTION(BlueprintCallable, Category = "Node Functions")
	void SetNodeType(ENodeType InNodeType);

	UFUNCTION(BlueprintCallable, Category = "Node Functions")
	FString GetNodeTypeString() const;

	UFUNCTION(BlueprintCallable, Category = "Node Functions")
	bool IsAtDepth(int32 TargetDepth) const;

private:
	//��� ���� ���� ���� �˻�
	bool IsValidConnection(UMapNode* TargetNode) const;
};
