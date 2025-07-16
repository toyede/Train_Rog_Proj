// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BackGroundObjectBase.generated.h"

UCLASS()
class TRAIN_ROG_PROJ_API ABackGroundObjectBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABackGroundObjectBase();
	
	// 필요 시, 이동/재사용 관련 메서드 선언
    UFUNCTION(BlueprintCallable)
	void SetObjectLocation(const FVector& NewLocation);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
