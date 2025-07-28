// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "BackGroundObjectBase.generated.h"

UCLASS()
class TRAIN_ROG_PROJ_API ABackGroundObjectBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ABackGroundObjectBase();
	
	// 필요 시, 이동/재사용 관련 메서드 선언
    UFUNCTION(BlueprintCallable) // UFUNCTION: 블루프린트에서 SetObjectLocation 함수를 노드로 사용할 수 있게 함
	void SetObjectLocation(const FVector& NewLocation);

	// 타일 길이 저장용 변수
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Tile")
    float MeasuredLength = 0.f;

	// 몇개정도 스폰 예정?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SmallObjects")
	int32 NumSpawn = 10;

	// 스폰할 소품 종류 리스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SmallObjects")
	TArray<TSubclassOf<AActor>> SmallObjects;

	// 배치 금지 영역을 정의하는 씬 컴포넌트 배열
    UPROPERTY(VisibleAnywhere, Category = "Setup")
    TArray<UStaticMeshComponent*> ExclusionZones;

	// 소품 간 및 금지 영역과의 최소 거리 (마진)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (ClampMin = "0.0"))
    float SpawnMargin = 50.0f;

	// 최대 위치 탐색 시도 횟수
    UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "1"))
    int32 MaxSpawnAttempts = 20;

protected:
	virtual void BeginPlay() override;

	FVector Origin, BoxExtent;

public:	
	virtual void Tick(float DeltaTime) override;

	// 전체 영역에 대해 소품 스폰을 자동 분배하는 메서드
    void SpawnObjectsInAllAreas();

	// 이미 스폰된 소품 위치 저장용 배열
    TArray<FVector> SpawnedLocations;

	// 소품을 스폰해도 되는 위치인지 확인하는 함수
	bool IsSpawnLocationValid(const FVector& Candidate);

	// 소품을 스폰하는 함수
	void SpawnSmallBackGrounds();
};
