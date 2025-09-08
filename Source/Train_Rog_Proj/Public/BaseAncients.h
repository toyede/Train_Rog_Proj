// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseAncients.generated.h"

UCLASS()
class TRAIN_ROG_PROJ_API ABaseAncients : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseAncients();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**	유물 추가 스텟 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ancients Status") float AncientHP; // 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ancients Status") float AncientMaxHP; // 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ancients Status") float AncientPower; // 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ancients Status") float AncientDefense; // 방어력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ancients Status") float AncientATSpeed; // 공격속도 (FireLate)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ancients Status") int AncientCriChance; // 크리티컬 확률
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ancients Status") float AncientCriDamage; // 크리티컬 피해
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ancients Status") int AncientMagazine; // 최대 탄창 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ancients Status") float AncientReloading; // 장전시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ancients Status") float AncientRadious; // 명중률
	
	UFUNCTION(BlueprintCallable, Category = "Ancients") float SetHP();
	UFUNCTION(BlueprintCallable, Category = "Ancients") float SetMaxHP();
	UFUNCTION(BlueprintCallable, Category = "Ancients") float SetPower();
	UFUNCTION(BlueprintCallable, Category = "Ancients") float SetDefense();
	UFUNCTION(BlueprintCallable, Category = "Ancients") float SetATSpeed();
	UFUNCTION(BlueprintCallable, Category = "Ancients") int SetCriChance();
	UFUNCTION(BlueprintCallable, Category = "Ancients") float SetCriDamage();
	UFUNCTION(BlueprintCallable, Category = "Ancients") int SetMagazine();
	UFUNCTION(BlueprintCallable, Category = "Ancients") float SetReloading();
	UFUNCTION(BlueprintCallable, Category = "Ancients") float SetRadious();
	
};
