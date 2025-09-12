// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//체력 회복 함수 amount 만큼 체력을 회복.
void UHealthComponent::IncreaseHP(float amount)
{
	//현재 체력에서 amount 만큼 회복
	CurrentHP = CurrentHP + amount;

	//최대 체력 초과면
	if (CurrentHP > MaxHP)
	{
		//최대 체력으로 설정
		CurrentHP = MaxHP;
	}
}

//체력 감소 함수 amount 만큼 체력을 감소.
void UHealthComponent::DecreaseHP(float amount)
{
	//현재 체력에서 amount 만큼 감소
	CurrentHP = CurrentHP - amount;

	//만약 0이하면
	if (CurrentHP <= 0)
	{
		//현재 체력 0으로 설정
		CurrentHP = 0;
		//죽었다고 알림!!
		OnDeath.Broadcast();
	}
}

//Standard : 기준값, Offset : % >> ex)공격력 20, 랜덤 +-5% -> Standard = 20, Offset = 0.05
float UHealthComponent::GetRandomStat(float Standard, float Offset)
{
	float Result = Standard;
	//랜덤 +- 결정
	int32 Sign = FMath::RandBool() ? 1 : -1;
	Result = Standard + (Sign * Offset);
	
	return Result;
}

void UHealthComponent::IncreaseMaxHP(int stack)
{
	float increaseHP = 2;
	if (stack <= 100)
	{
		MaxHP += increaseHP * 1.f;
		CurrentHP += increaseHP * 1.f;
	}
	else if (stack <= 200)
	{
		MaxHP += increaseHP * 0.8f;
		CurrentHP += increaseHP * 0.8f;
	}
	else if (stack <= 300)
	{
		MaxHP += increaseHP * 0.5f;
		CurrentHP += increaseHP * 0.5f;
	}
}	

void UHealthComponent::DecreaseMaxHP(int stack)
{
	float decreaseHP = 2;
	if (stack <= 100)
	{
		MaxHP -= decreaseHP * 1.f;
		CurrentHP -= decreaseHP * 1.f;
	}
	else if (stack <= 200)
	{
		MaxHP -= decreaseHP * 0.8f;
		CurrentHP -= decreaseHP * 0.8f;
	}
	else if (stack <= 300)
	{
		MaxHP -= decreaseHP * 0.5f;
		CurrentHP -= decreaseHP * 0.5f;
	}
}
