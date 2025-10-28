// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAIController.h"
#include "Enemy/Enemy.h"

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//각 몬스터의 비헤이비어 트리 가져오기
	if (AEnemy* Enemy = Cast<AEnemy>(InPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("Get Enemy BT"));
		BehaviorTree = Enemy->BehaviorTree;
	}

	if (BehaviorTree)
	{
		//비헤이비어 트리 실행!
		RunBehaviorTree(BehaviorTree);
	}	
}
