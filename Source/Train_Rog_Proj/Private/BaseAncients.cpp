// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAncients.h"

// Sets default values
ABaseAncients::ABaseAncients()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseAncients::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseAncients::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ABaseAncients::SetHP()
{
	return AncientHP;
}

float ABaseAncients::SetMaxHP()
{
	return AncientMaxHP;
}

float ABaseAncients::SetPower()
{
	return AncientPower;
}

float ABaseAncients::SetDefense()
{
	return AncientDefense;
}

float ABaseAncients::SetATSpeed()
{
	return AncientATSpeed;
}

int ABaseAncients::SetCriChance()
{
	return AncientCriChance;
}

float ABaseAncients::SetCriDamage()
{
	return AncientCriDamage;
}

int ABaseAncients::SetMagazine()
{
	return AncientMagazine;
}

float ABaseAncients::SetReloading()
{
	return AncientReloading;
}

float ABaseAncients::SetRadious()
{
	return AncientRadious;
}


