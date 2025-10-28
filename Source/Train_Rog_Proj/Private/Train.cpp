// Fill out your copyright notice in the Description page of Project Settings.


#include "Train.h"

// Sets default values
ATrain::ATrain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATrain::IncreaseTrainHealth(int stack)
{
	TrainHealth += stack * 200.0f;
}

void ATrain::IncreaseTrainDefense(int stack)
{
	TrainDefense += stack * 30.0f;
}

void ATrain::DecreaseTrainHealth(int stack)
{
	TrainHealth -= stack * 200.0f;
}

void ATrain::DecreaseTrainDefense(int stack)
{
	TrainDefense -= stack * 30.0f;
}

void ATrain::PlusTrain()
{
	TrainStack += 1;
}

void ATrain::MinusTrain()
{
	TrainStack -= 1;
}

void ATrain::ClearTrain()
{
	TrainStack = 0;
}

// Called when the game starts or when spawned
void ATrain::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

