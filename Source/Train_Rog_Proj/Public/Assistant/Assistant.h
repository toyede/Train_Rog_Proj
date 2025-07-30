// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Assistant.generated.h"

UCLASS()
class TRAIN_ROG_PROJ_API AAssistant : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAssistant();

	FTimerHandle AttackTimer;

	FTimerHandle SearchTimer;

	UPROPERTY(EditAnywhere, Category = Search)
	float SearchAngle = 120.0f;
	UPROPERTY(EditAnywhere, Category = Search)
	int32 SearchLineNumber = 12;
	UPROPERTY(EditAnywhere, Category = Search)
	float SearchDistance = 100.0f;
	UPROPERTY(EditAnywhere, Category = Search)
	float SearchRate = 0.1f;

	UPROPERTY(EditAnywhere, Category = Attack)
	float AttackRate = 1.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//발견하면 때릴 클래스
	UPROPERTY(EditAnywhere, Category = Search)
	TSubclassOf<AActor> TargetClass;

	UPROPERTY()
	AActor* Target;

	//탐색 라인트레이스
	UFUNCTION(BlueprintCallable, Category = Search)
	void SearchTarget();

	UFUNCTION(BlueprintCallable, Category = Attack)
	void Attack();
};
