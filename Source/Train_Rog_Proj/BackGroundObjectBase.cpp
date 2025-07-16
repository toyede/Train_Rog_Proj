// Fill out your copyright notice in the Description page of Project Settings.


#include "BackGroundObjectBase.h"

// Sets default values
ABackGroundObjectBase::ABackGroundObjectBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABackGroundObjectBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABackGroundObjectBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABackGroundObjectBase::SetObjectLocation(const FVector& NewLocation)
{
	SetActorLocation(NewLocation);
}