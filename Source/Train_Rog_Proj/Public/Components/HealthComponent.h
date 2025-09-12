// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

/**
 * 체력 컴포넌트 -체력이 필요한 모든 클래스에 적용 가능-
 */

//사망 알림 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TRAIN_ROG_PROJ_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	//최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHP = 100.0f;

	//현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHP = MaxHP;

	//사망 알림 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathDelegate OnDeath;

	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	//체력 회복 함수 amount 만큼 체력을 회복.
	UFUNCTION(BlueprintCallable, Category = Health)
	void IncreaseHP(float amount);

	//체력 감소 함수 amount 만큼 체력을 감소.
	UFUNCTION(BlueprintCallable, Category = Health)
	void DecreaseHP(float amount);

	UFUNCTION(BlueprintCallable)
	float GetRandomStat(float Standard, float Offset);
};
