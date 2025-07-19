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
	ABackGroundObjectBase();
	
	// 필요 시, 이동/재사용 관련 메서드 선언
    UFUNCTION(BlueprintCallable)
	void SetObjectLocation(const FVector& NewLocation);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Measurement")
    float MeasuredLength = 0.f;

protected:
	virtual void BeginPlay() override;

	FVector Origin, BoxExtent;

public:	
	virtual void Tick(float DeltaTime) override;

};
