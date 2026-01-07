// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class AEnemy;

UCLASS()
class TRAIN_ROG_PROJ_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	
	UPROPERTY(EditDefaultsOnly)
	TMap<TSubclassOf<AEnemy>, float> MonsterTable;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UNavigationSystemV1* NavSys;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SpawnableAreaRadius = 500.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float UnSpawnableAreaRadius = 100.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxSpawnHeight = 1000.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinSpawnHeight = 1100.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RailOffset = 50.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float WeightLimit;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CurrentWeight;
		
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void MakeMonsterQueue(TArray<TSubclassOf<AEnemy>>& Queue, float Limit);
	
	UFUNCTION(BlueprintCallable)
	void SpawnMonsters(float MinWeight, float MaxWeight);

};
