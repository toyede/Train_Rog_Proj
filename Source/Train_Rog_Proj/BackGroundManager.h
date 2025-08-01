// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BackGroundObjectBase.h"
#include "Engine/World.h"
#include "Math/Box.h"
#include "BackGroundManager.generated.h"

UENUM(BlueprintType)
// 타일의 진행 방향, Default: Backward
enum class ETileMoveDirectoin : uint8
{
    Forward,
    Backward,
    Left,
    Right
};

UENUM(BlueprintType)
// 타일의 스폰 방향, Default: Forward
enum class ETileSpawnDirection : uint8
{
    Forward, // 0
    Backward, // 1
    Left, // 2
    Right // 3
};

UCLASS()
class TRAIN_ROG_PROJ_API ABackGroundManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ABackGroundManager();

protected:
	virtual void BeginPlay() override;
    
public:	
    virtual void Tick(float DeltaTime) override;
    
protected:
    // 현재 타일 진행 방향을 저장하는 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Direction")
    ETileMoveDirectoin CurrentMoveDirection;

    // 현재 타일 스폰 방향을 저장하는 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Direction")
    ETileSpawnDirection CurrentSpawnDirection;

    // 현재 돌아가고있는 타일 배열 (초기 설정 필요)
    UPROPERTY(EditAnywhere, Category="Tiles")
    TArray<ABackGroundObjectBase*> TileArray;

	// 타일 배열(종류 구분용)
	UPROPERTY(EditAnywhere, Category="Tiles")
    TArray<TSubclassOf<ABackGroundObjectBase>> TileTypes;

	// 기준이 될 기차 (초기 설정 필요)
    UPROPERTY(EditAnywhere, Category="RefActor")
    AActor* ReferenceActor;

	// 시야거리
    UPROPERTY(EditAnywhere)
    float sight = 0.f;

	// 타일 이동 속도
    UPROPERTY(EditAnywhere, Category="Movement")
    float TileMoveSpeed = 1000.f; // 임시값

	// TestRight / TestLeft가 true면 각각 TileRight / TileLeft로 교체
    UPROPERTY(EditAnywhere, Category="Fork")
    bool Change_Right_Tile = false;

    UPROPERTY(EditAnywhere, Category="Fork")
    bool Change_Left_Tile  = false;

    UPROPERTY(EditAnywhere, Category="Rotation")
    FRotator CurrentTileRot;
    
    // 마지막으로 전환 트리거를 발생시킨 타일
    UPROPERTY()
    ABackGroundObjectBase* LastRLSourceTile = nullptr;

    // 전환 직후 한 번만 특수 스폰을 하기 위한 플래그
    bool Flag_SpawnedRLTile = false;
    
    // Object의 ChangeDirection 이벤트를 처리할 함수
    UFUNCTION()
    void HandleChangeDirection(ABackGroundObjectBase* SourceObject);

    // Tic마다 범위를 벗어난 타일이 있는지 체크하는 함수
    void CheckAndRecycleObjects();

    // 타일 종류 변경용 함수
    void SetNextTileRL(auto* Object, FVector NewLoc);
    void SetNextTileDF(auto* Object, FVector NewLoc);

    ETileSpawnDirection BoolToSpawnDir(bool MoveRight, bool MoveLeft);
};
