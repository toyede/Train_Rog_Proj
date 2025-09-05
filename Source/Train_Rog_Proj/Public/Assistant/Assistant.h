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

	//공격 타이머
	FTimerHandle AttackTimer;

	//탐색 타이머
	FTimerHandle SearchTimer;

	//탐색 각도
	UPROPERTY(EditAnywhere, Category = Search, meta = (ToolTip = "탐색 각도"))
	float SearchAngle = 120.0f;

	//탐색 라인 수
	UPROPERTY(EditAnywhere, Category = Search, meta = (ToolTip = "탐색 라인 수"))
	int32 SearchLineNumber = 12;

	//탐색 거리
	UPROPERTY(EditAnywhere, Category = Search, meta = (ToolTip = "탐색 거리"))
	float SearchDistance = 100.0f;

	//탐색 빈도
	UPROPERTY(EditAnywhere, Category = Search, meta = (ToolTip = "탐색 빈도"))
	float SearchRate = 0.1f;

	//회전 탐색 여부
	UPROPERTY(EditAnywhere, Category = Search, meta = (ToolTip = "회전 탐색 여부"))
	bool RotationSearch = true;

	//공격 빈도
	UPROPERTY(EditAnywhere, Category = Attack, meta = (ToolTip = "공격 빈도"))
	float AttackRate = 1.0f;

	//공격력
	UPROPERTY(EditAnywhere, Category = Attack, meta = (ToolTip = "공격 빈도"))
	float AttackPower = 10.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//발견하면 때릴 클래스
	UPROPERTY(EditAnywhere, Category = Search)
	TSubclassOf<AActor> TargetClass;

	//발견한 클래스 액터 참조
	UPROPERTY()
	AActor* Target;

	//탐색 라인트레이스
	UFUNCTION(BlueprintCallable, Category = Search)
	void SearchTarget();

	//공격
	UFUNCTION(BlueprintCallable, Category = Attack)
	void Attack();
};
