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
	float AttackSpeed = 0.005f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (ToolTip = "크리티컬 확률"))
	float CriticalChance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (ToolTip = "크리티컬 피해"))
	float CriticalDamage;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** 스탯 증가 함수들 (Blueprint에서도 호출 가능)*/
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions") void IncreaseDamage(int stack);
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions") void IncreaseDefense(int stack);
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions") void IncreaseAttackSpeed(int stack);
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions") void IncreaseCriticalChance(int stack);
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions") void IncreaseCriticalDamage(int stack);
	
	/** 스탯 감소 함수들 */
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions") void DecreaseDamage(int stack);
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions") void DecreaseDefense(int stack);
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions") void DecreaseAttackSpeed(int stack);
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions") void DecreaseCriticalChance(int stack);
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions") void DecreaseCriticalDamage(int stack);

	float stackDamage = 1;
	float stackDefense = 1;
	float stackASpeed = 0.005;
	float stackCriChance = 1;
	float stackCriDamage = 1;
};
