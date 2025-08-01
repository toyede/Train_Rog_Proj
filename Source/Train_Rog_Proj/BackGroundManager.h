// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BackGroundObjectBase.h"
#include "Engine/World.h"
#include "Math/Box.h"
#include "BackGroundManager.generated.h"

// 진행 방향 구분용 열거형
UENUM(BlueprintType)
enum class EBackGroundMoveDirectoin : uint8
{
    Forward,
    Backward,
    Left,
    Right
};

UENUM(BlueprintType)
enum class EMySpawnDirection : uint8
{
    Forward,
    Backward,
    Left,
    Right
};

UCLASS()
class TRAIN_ROG_PROJ_API ABackGroundManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ABackGroundManager();

    // 현재 진행 방향을 저장하는 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Direction")
    EBackGroundMoveDirectoin CurrentMoveDirection;

    // 현재 스폰 방향을 저장하는 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Direction")
    EMySpawnDirection CurrentSpawnDirection;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	// 나만의 작고 귀여운 배경 배열(처음 설치용)
	UPROPERTY(EditAnywhere)
    TArray<ABackGroundObjectBase*> BackGroundObjects;

	// 배경 배열(종류 구분용)
	UPROPERTY(EditAnywhere, Category="Spawning")
    TArray<TSubclassOf<ABackGroundObjectBase>> BackGroundTypes;

	// 기차나 카메라
    UPROPERTY(EditAnywhere)
    AActor* ReferenceActor;

	// 시야거리
    UPROPERTY(EditAnywhere)
    float sight = 0.f;

	// 배경 이동 속도
    UPROPERTY(EditAnywhere, Category="Movement")
    float BackgroundMoveSpeed = 1000.f; // 예시값

	// TestRight / TestLeft가 true면 각각 TileRight / TileLeft로 교체
    UPROPERTY(EditAnywhere, Category="Movement")
    bool TestRight = false;

    UPROPERTY(EditAnywhere, Category="Movement")
    bool TestLeft  = false;

    UPROPERTY(EditAnywhere, Category="Movement")
    FRotator CurrentRot;

    // 전환 직후 한 번만 특수 스폰을 하기 위한 플래그
    bool bJustSpawnedRL = false;

    // 마지막으로 전환 트리거를 발생시킨 타일
    UPROPERTY()
    ABackGroundObjectBase* LastRLSource = nullptr;

    void CheckAndRecycleObjects();
    void SetNextTileRL(auto* Object, FVector NewLoc);
    void SetNextTileDF(auto* Object, FVector NewLoc);

    // Object의 ChangeDirection 이벤트를 처리할 함수
    UFUNCTION()
    void HandleChangeDirection(ABackGroundObjectBase* SourceObject);
};
