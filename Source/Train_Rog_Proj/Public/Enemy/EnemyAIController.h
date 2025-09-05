// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 몬스터의 스폰을 관리하는 매니저 클래스
 */
UCLASS()
class TRAIN_ROG_PROJ_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:

	//실행할 비헤이비어트리
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;
	
	virtual void BeginPlay() override;
    	
};
