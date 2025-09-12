// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AbilityComponent.h"

// Sets default values for this component's properties
UAbilityComponent::UAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAbilityComponent::IncreaseDamage(int stack)
{
	if (stack <= 100)
	{
		Damage += stackDamage * 1.f;
	}
	else if (stack <= 200)
	{
		Damage += stackDamage * 0.8f;
	}
	else if (stack <= 300)
	{
		Damage += stackDamage * 0.5f;
	}
	else
	{
		Damage += stackDamage * 0.2f;
	}
}
void UAbilityComponent::IncreaseDefense(int stack)
{
	if (stack <= 100)
	{
		Defense += stackDefense * 1.f;
	}
	else if (stack <= 200)
	{
		Defense += stackDefense * 0.8f;
	}
	else if (stack <= 300)
	{
		Defense += stackDefense * 0.5f;
	}
	else
	{
		Defense += stackDefense * 0.2f;
	}
}

void UAbilityComponent::IncreaseAttackSpeed(int stack)
{
	if (stack <= 100)
	{
		AttackSpeed += stackASpeed * 1.f;
	}
	else if (stack <= 200)
	{
		AttackSpeed += stackASpeed * 0.8f;
	}
	else if (stack <= 300)
	{
		AttackSpeed += stackASpeed * 0.5f;
	}
	else
	{
		AttackSpeed += stackASpeed * 0.2f;
	}
}

void UAbilityComponent::IncreaseCriticalChance(int stack)
{
	if (stack <= 100)
	{
		CriticalChance += stackCriChance * 1.f;
	}
	else if (stack <= 200)
	{
		CriticalChance += stackCriChance * 0.8f;
	}
	else if (stack <= 300)
	{
		CriticalChance += stackCriChance * 0.5f;
	}
	else
	{
		CriticalChance += stackCriChance * 0.2f;
	}
}

void UAbilityComponent::IncreaseCriticalDamage(int stack)
{
	if (stack <= 100)
	{
		CriticalDamage += stackCriDamage * 1.f;
	}
	else if (stack <= 200)
	{
		CriticalDamage += stackCriDamage * 0.8f;
	}
	else if (stack <= 300)
	{
		CriticalDamage += stackCriDamage * 0.5f;
	}
	else
	{
		CriticalDamage += stackCriDamage * 0.2f;
	}
}

void UAbilityComponent::DecreaseDamage(int stack)
{
	if (stack <= 100)
	{
		Damage -= stackDamage * 1.f;
	}
	else if (stack <= 200)
	{
		Damage -= stackDamage * 0.8f;
	}
	else if (stack <= 300)
	{
		Damage -= stackDamage * 0.5f;
	}
	else
	{
		Damage -= stackDamage * 0.2f;
	}
}

void UAbilityComponent::DecreaseDefense(int stack)
{
	if (stack <= 100)
	{
		Defense -= stackDefense * 1.f;
	}
	else if (stack <= 200)
	{
		Defense -= stackDefense * 0.8f;
	}
	else if (stack <= 300)
	{
		Defense -= stackDefense * 0.5f;
	}
	else
	{
		Defense -= stackDefense * 0.2f;
	}
}

void UAbilityComponent::DecreaseAttackSpeed(int stack)
{
	if (stack <= 100)
	{
		AttackSpeed -= stackASpeed * 1.f;
	}
	else if (stack <= 200)
	{
		AttackSpeed -= stackASpeed * 0.8f;
	}
	else if (stack <= 300)
	{
		AttackSpeed -= stackASpeed * 0.5f;
	}
	else
	{
		AttackSpeed -= stackASpeed * 0.2f;
	}
}

void UAbilityComponent::DecreaseCriticalChance(int stack)
{
	if (stack <= 100)
	{
		CriticalChance -= stackCriChance * 1.f;
	}
	else if (stack <= 200)
	{
		CriticalChance -= stackCriChance * 0.8f;
	}
	else if (stack <= 300)
	{
		CriticalChance -= stackCriChance * 0.5f;
	}
	else
	{
		CriticalChance -= stackCriChance * 0.2f;
	}
}

void UAbilityComponent::DecreaseCriticalDamage(int stack)
{
	if (stack <= 100)
	{
		CriticalDamage -= stackCriDamage * 1.f;
	}
	else if (stack <= 200)
	{
		CriticalDamage -= stackCriDamage * 0.8f;
	}
	else if (stack <= 300)
	{
		CriticalDamage -= stackCriDamage * 0.5f;
	}
	else
	{
		CriticalDamage -= stackCriDamage * 0.2f;
	}
}


