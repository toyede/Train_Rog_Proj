// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Toad.h"

#include "Components/AbilityComponent.h"
#include "Components/HealthComponent.h"

void AToad::BeginPlay()
{
	Super::BeginPlay();

	//스테이지 별 능력치 적용
	switch (CurrentStage)
	{
		case STAGE::STAGE_1:
			HealthComponent->MaxHP = GetRandomStat(150, 0.05);
			AbilityComponent->Damage = GetRandomStat(20,0.05);
			AbilityComponent->Defense = GetRandomStat(20,0.05);
			AbilityComponent->AttackSpeed = 0.5;
			break;
		case STAGE::STAGE_2:
			HealthComponent->MaxHP = GetRandomStat(172, 0.05);
			AbilityComponent->Damage = GetRandomStat(23,0.05);
			AbilityComponent->Defense = GetRandomStat(23,0.05);
			AbilityComponent->AttackSpeed = 0.55;
			break;
		case STAGE::STAGE_3:
			HealthComponent->MaxHP = GetRandomStat(198, 0.05);
			AbilityComponent->Damage = GetRandomStat(26,0.05);
			AbilityComponent->Defense = GetRandomStat(26,0.05);
			AbilityComponent->AttackSpeed = 0.6;
			break;
		case STAGE::STAGE_4:
			HealthComponent->MaxHP = GetRandomStat(228, 0.05);
			AbilityComponent->Damage = GetRandomStat(30,0.05);
			AbilityComponent->Defense = GetRandomStat(30,0.05);
			AbilityComponent->AttackSpeed = 0.65;
			break;
		case STAGE::STAGE_5:
			HealthComponent->MaxHP = GetRandomStat(262, 0.05);
			AbilityComponent->Damage = GetRandomStat(35,0.05);
			AbilityComponent->Defense = GetRandomStat(35,0.05);
			AbilityComponent->Defense = 0.7;
			break;
	}
}
