// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Train.generated.h"

UCLASS()
class TRAIN_ROG_PROJ_API ATrain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrain();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (ToolTip = "기차 체력"))
	float TrainHealth = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (ToolTip = "기차 체력"))
    float CurrentTrainHealth = TrainHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (ToolTip = "기차 방어력"))
	float TrainDefense = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (ToolTip = "기차 방어력"))
	int TrainStack = 0;
	
	UFUNCTION(BlueprintCallable, Category = "Train Stats Functions") void IncreaseTrainHealth(int stack);
	UFUNCTION(BlueprintCallable, Category = "Train Stats Functions") void IncreaseTrainDefense(int stack);
	UFUNCTION(BlueprintCallable, Category = "Train Stats Functions") void DecreaseTrainHealth(int stack);
	UFUNCTION(BlueprintCallable, Category = "Train Stats Functions") void DecreaseTrainDefense(int stack);
	UFUNCTION(BlueprintCallable, Category = "Train Stats Functions") void PlusTrain();
	UFUNCTION(BlueprintCallable, Category = "Train Stats Functions") void MinusTrain();
	UFUNCTION(BlueprintCallable, Category = "Train Stats Functions") void ClearTrain();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

};
