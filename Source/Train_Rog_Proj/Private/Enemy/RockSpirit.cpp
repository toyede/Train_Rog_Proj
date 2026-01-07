// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RockSpirit.h"

#include "Components/AbilityComponent.h"
#include "Components/HealthComponent.h"

void ARockSpirit::BeginPlay()
{
	Super::BeginPlay();

	//스테이지 별 능력치 적용
	switch (CurrentStage)
	{
	case STAGE::STAGE_1:
		HealthComponent->MaxHP = GetRandomStat(220, 0.05);
		AbilityComponent->Damage = GetRandomStat(12,0.05);
		AbilityComponent->Defense = GetRandomStat(30,0.05);
		AbilityComponent->AttackSpeed = 0.5;
		break;
	case STAGE::STAGE_2:
		HealthComponent->MaxHP = GetRandomStat(253, 0.05);
		AbilityComponent->Damage = GetRandomStat(14,0.05);
		AbilityComponent->Defense = GetRandomStat(34.5,0.05);
		AbilityComponent->AttackSpeed = 0.55;
		break;
	case STAGE::STAGE_3:
		HealthComponent->MaxHP = GetRandomStat(291, 0.05);
		AbilityComponent->Damage = GetRandomStat(16,0.05);
		AbilityComponent->Defense = GetRandomStat(40,0.05);
		AbilityComponent->AttackSpeed = 0.6;
		break;
	case STAGE::STAGE_4:
		HealthComponent->MaxHP = GetRandomStat(335, 0.05);
		AbilityComponent->Damage = GetRandomStat(18,0.05);
		AbilityComponent->Defense = GetRandomStat(46,0.05);
		AbilityComponent->AttackSpeed = 0.65;
		break;
	case STAGE::STAGE_5:
		HealthComponent->MaxHP = GetRandomStat(385, 0.05);
		AbilityComponent->Damage = GetRandomStat(20,0.05);
		AbilityComponent->Defense = GetRandomStat(53,0.05);
		AbilityComponent->Defense = 0.7;
		break;
	}
	
	HealthComponent->CurrentHP = HealthComponent->MaxHP;
}

void ARockSpirit::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!GEngine){
		FString Name = GetName();
		FString Value = "["+Name+"] | "+FString::FromInt(HealthComponent->CurrentHP)+" / "+FString::FromInt(HealthComponent->MaxHP);
		GEngine->AddOnScreenDebugMessage(0, DeltaSeconds, FColor::Emerald, Value);
	}
}

ARockSpirit::ARockSpirit()
{
	WeightValue = 3.0f;
}
