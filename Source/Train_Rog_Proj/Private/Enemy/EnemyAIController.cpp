// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAIController.h"

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	//비헤이비어 트리 실행!
	RunBehaviorTree(BehaviorTree);
}
