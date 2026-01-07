// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

//스테이지 구분 임시 ENUM

UENUM(BlueprintType)
enum class STAGE : uint8
{
	STAGE_1 UMETA(DisplayName = "STAGE 1"),
	STAGE_2 UMETA(DisplayName = "STAGE 2"),
	STAGE_3 UMETA(DisplayName = "STAGE 3"),
	STAGE_4 UMETA(DisplayName = "STAGE 4"),
	STAGE_5 UMETA(DisplayName = "STAGE 5")
};

/**
 * 적 몬스터 부모 클래스
 */
UCLASS()
class TRAIN_ROG_PROJ_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemy();
	
	//몬스터 가중치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float WeightValue = 0.0f;

	//추격해 때릴 타겟 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "0_Settings")
	TSubclassOf<AActor> TargetClass;

	//월드에서 찾은 타겟 클래스의 참조
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AActor* Target;

	//체력 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UHealthComponent* HealthComponent;

	//능력치 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAbilityComponent* AbilityComponent;

	//테스트 임시 체력바
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UWidgetComponent* HPBar;

	//능력치 설정 스테이지 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "0_Settings")
	STAGE CurrentStage = STAGE::STAGE_1;

	//실행할 비헤이비어 트리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0_Settings")
	class UBehaviorTree* BehaviorTree;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//공격 함수 - C++과 블루프린트 모두에서 구현 가능
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Attack)
	void Attack();
	virtual void Attack_Implementation();

	//Deprecated
	//입력되는 TargetActor에 대미지를 적용하는 함수 (Health 컴포넌트가 있을 시 체력 감소)
	//UFUNCTION(BlueprintCallable, Category = Attack)
	//void ApplyDamage(AActor* TargetActor, float Damage);

	//사망 함수 - C++과 블루프린트 모두에서 구현 가능
	UFUNCTION(BlueprintNativeEvent)
	void Death();
	virtual void Death_Implementation();

	//월드에서 타겟 클래스의 인스턴스를 찾는 함수
	UFUNCTION(BlueprintCallable)
	bool FindTarget();

	UFUNCTION(BlueprintCallable)
	float GetRandomStat(float standard, float offset);

	UFUNCTION(BlueprintCallable)
	void DebugDisplay(float Duration);

	FColor DebugColor = FColor::MakeRandomColor();

	//대미지 수신 함수
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};


