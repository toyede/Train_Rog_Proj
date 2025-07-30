// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/HealthComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AEnemy::AEnemy()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	Mesh->SetCollisionProfileName("BlockAll");
	SetRootComponent(Mesh);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	
	Tags.Add(FName("Enemy"));

	HPBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBar"));
	HPBar->SetupAttachment(Mesh);
	
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::Attack_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ENEMY : Attack Activated."));
}

void AEnemy::Death_Implementation()
{	
	HealthComponent->Death();
	UE_LOG(LogTemp, Warning, TEXT("ENEMY : Death Activated."));
}
