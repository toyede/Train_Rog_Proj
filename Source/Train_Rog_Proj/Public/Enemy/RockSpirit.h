// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "RockSpirit.generated.h"

/**
 * 
 */
UCLASS()
class TRAIN_ROG_PROJ_API ARockSpirit : public AEnemy
{
	GENERATED_BODY()

	void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	ARockSpirit();
};
