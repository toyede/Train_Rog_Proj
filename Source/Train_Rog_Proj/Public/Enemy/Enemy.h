// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class TRAIN_ROG_PROJ_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemy();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> TargetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AActor* Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAbilityComponent* AbilityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UWidgetComponent* HPBar;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintNativeEvent, Category = Attack)
	void Attack();
	virtual void Attack_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void Death();
	virtual void Death_Implementation();

	UFUNCTION(BlueprintCallable)
	bool FindTarget();

};


