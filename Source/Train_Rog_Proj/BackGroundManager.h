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
	// Sets default values for this actor's properties
	ABackGroundManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// 나만의 작고 귀여운 배경 배열
	UPROPERTY(EditAnywhere)
    TArray<ABackGroundObjectBase*> BackGroundObjects;

	// 최대 시야 거리
	UPROPERTY(EditAnywhere)
    float ObjectLength = 1000.f;

	// 기차나 카메라
    UPROPERTY(EditAnywhere)
    AActor* ReferenceActor;

    void CheckAndRecycleObjects();
};
