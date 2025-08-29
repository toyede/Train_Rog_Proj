// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TRAIN_ROG_PROJ_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityComponent();

	//공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (ToolTip = "공격력"))
	float Damage = 10.0f;

	//방어력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (ToolTip = "방어력"))
	float Defense = 5.0f;

	//공격 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (ToolTip = "공격 속도"))
	float AttackSpeed = 1.0f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
