// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Measurement")
    float MeasuredLength = 0.f;

	// 몇개정도 스폰 예정?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SmallObjects")
	int NumSpawn = 2;

	// 스폰할 소품 종류 리스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SmallObjects")
	TArray<TSubclassOf<AActor>> SmallObjects;

protected:
	virtual void BeginPlay() override;

	FVector Origin, BoxExtent;

public:	
	virtual void Tick(float DeltaTime) override;

	// 전체 영역에 대해 소품 스폰을 자동 분배하는 메서드
    void SpawnObjectsInAllAreas();
};
