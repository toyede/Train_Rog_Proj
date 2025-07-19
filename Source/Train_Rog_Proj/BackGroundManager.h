// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BackGroundObjectBase.h"
#include "BackGroundManager.generated.h"

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
	// 나만의 작고 귀여운 배경 배열
	UPROPERTY(EditAnywhere)
    TArray<ABackGroundObjectBase*> BackGroundObjects;

	// 기차나 카메라
    UPROPERTY(EditAnywhere)
    AActor* ReferenceActor;

	// 시야거리
    UPROPERTY(EditAnywhere)
    float sight = 0.f;

	// 배경 이동 속도
    UPROPERTY(EditAnywhere, Category="Movement")
    float BackgroundMoveSpeed = 1000.f; // 예시값

    void CheckAndRecycleObjects();
};
