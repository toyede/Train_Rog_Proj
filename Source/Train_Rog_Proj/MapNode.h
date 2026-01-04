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

// 위험도 레벨 열거형
UENUM(BlueprintType)
enum class EDangerLevel : uint8
{
	Level1 UMETA(DisplayName = "Level 1"),
	Level2 UMETA(DisplayName = "Level 2"),
	Level3 UMETA(DisplayName = "Level 3"),
	Elite UMETA(DisplayName = "Elite")
};

USTRUCT(BlueprintType)
struct FNodePosition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Depth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Row;

	FNodePosition()		//기본 생성자
	{
		Depth = 0;
		Row = 0;
	}

	FNodePosition(int32 InDepth, int32 InRow)		//매개변수 있는 생성자
	{
		Depth = InDepth;
		Row = InRow;
	}

	// 비교 연산자 오버로딩. 나중에 같은 위치에 노드가 생성될 수도 있으니 사용.
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

	//노드 기본 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Info")
	ENodeType NodeType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Info")
	FNodePosition Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Info")
	bool bIsVisited;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Info")
	bool bIsAccessible;

	// 난이도 관련 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty")
	EDangerLevel DangerLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty")
	int32 DifficultyWeight;  // 난이도 가중치

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty")
	float MonsterStatMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty")
	bool bIsEliteLevel;

	// 연결된 노드들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connections")
	TArray<UMapNode*> ConnectedNodes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connections")
	TArray<UMapNode*> PreviousNodes;

	//블루프린트에서 호출 가능한 함수들
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

	// 난이도 관련 함수
	UFUNCTION(BlueprintCallable, Category = "Difficulty")
	FString GetDangerLevelString() const;


private:
	//노드 연결 가능 여부 검사
	bool IsValidConnection(UMapNode* TargetNode) const;
};
