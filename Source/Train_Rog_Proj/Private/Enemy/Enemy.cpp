// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"

#include "Components/AbilityComponent.h"
#include "Components/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy::AEnemy()
{
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	AbilityComponent = CreateDefaultSubobject<UAbilityComponent>(TEXT("AbilityComponent"));
	
	Tags.Add(FName("Enemy"));

	HPBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBar"));
	HPBar->SetupAttachment(GetMesh());
	HPBar->SetRelativeLocation(FVector(0, 0, 210));
	
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//공격 함수 - C++ 구현부
void AEnemy::Attack_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ENEMY : Attack Activated."));
}

void AEnemy::ApplyDamage(AActor* TargetActor, float Damage)
{
	//공격 대상 액터가 체력, 능력 컴포넌트가 있는 지 확인
	UHealthComponent* HealthComp = TargetActor->FindComponentByClass<UHealthComponent>();
	UAbilityComponent* AbilityComp = TargetActor->FindComponentByClass<UAbilityComponent>();
	
	float FinalDamage = Damage;
	
	//능력 컴포넌트가 있으면 대미지를 능력과 계산
	if (AbilityComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("ENEMY : Target has Ability"));
		//지수 곡선으로 형태로 방어력 적용
		//'방어력이 높아질수록 대미지가 점점 줄지만 얼마나 줄어들 지 조정 가능' 라고 GPT가 말함.
		FinalDamage = Damage * FMath::Pow(0.9f, AbilityComp->Defense / 10.0f);
	}

	//체력 컴포넌트가 있으면 대미지 적용
	if (HealthComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("ENEMY : Target has Health"));
		HealthComp->DecreaseHP(FinalDamage);
	}
}

//사망 함수 - C++ 구현부
void AEnemy::Death_Implementation()
{	
	UE_LOG(LogTemp, Warning, TEXT("ENEMY : Death Activated."));
}

//월드에서 타겟 클래스의 인스턴스를 찾는 함수
bool AEnemy::FindTarget()
{
	//월드에서 타겟 클래스의 액터를 찾음
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), TargetClass);

	//액터가 있으면
	if (FoundActor)
	{
		//타겟 참조 액터를 찾은 액터로 설정
		Target = FoundActor;
		//성공 반환
		return true;
	}

	//실패 반환
	return false;
}
