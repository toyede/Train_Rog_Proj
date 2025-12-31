// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Moth.h"

#include "Components/AbilityComponent.h"
#include "Components/HealthComponent.h"

void AMoth::BeginPlay()
{
	Super::BeginPlay();

	//스테이지 별 능력치 적용
	switch (CurrentStage)
	{
	case STAGE::STAGE_1:
		HealthComponent->MaxHP = GetRandomStat(70, 0.05);
		AbilityComponent->Damage = GetRandomStat(8,0.05);
		AbilityComponent->Defense = GetRandomStat(10,0.05);
		AbilityComponent->AttackSpeed = 2;
		break;
	case STAGE::STAGE_2:
		HealthComponent->MaxHP = GetRandomStat(80, 0.05);
		AbilityComponent->Damage = GetRandomStat(10,0.05);
		AbilityComponent->Defense = GetRandomStat(11.5,0.05);
		AbilityComponent->AttackSpeed = 2.2;
		break;
	case STAGE::STAGE_3:
		HealthComponent->MaxHP = GetRandomStat(92, 0.05);
		AbilityComponent->Damage = GetRandomStat(12,0.05);
		AbilityComponent->Defense = GetRandomStat(13,0.05);
		AbilityComponent->AttackSpeed = 2.4;
		break;
	case STAGE::STAGE_4:
		HealthComponent->MaxHP = GetRandomStat(106, 0.05);
		AbilityComponent->Damage = GetRandomStat(14,0.05);
		AbilityComponent->Defense = GetRandomStat(15,0.05);
		AbilityComponent->AttackSpeed = 2.6;
		break;
	case STAGE::STAGE_5:
		HealthComponent->MaxHP = GetRandomStat(122, 0.05);
		AbilityComponent->Damage = GetRandomStat(16,0.05);
		AbilityComponent->Defense = GetRandomStat(17,0.05);
		AbilityComponent->Defense = 2.8;
		break;
	}
	HealthComponent->CurrentHP = HealthComponent->MaxHP;
	
}
