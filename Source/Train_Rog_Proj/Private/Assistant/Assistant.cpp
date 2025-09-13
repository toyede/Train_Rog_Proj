// Fill out your copyright notice in the Description page of Project Settings.


#include "Assistant/Assistant.h"
#include "Components/HealthComponent.h"
#include "Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/FastReferenceCollector.h"

// Sets default values
AAssistant::AAssistant()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAssistant::BeginPlay()
{
	Super::BeginPlay();

	//게임 시작하면 탐색 시작 (탐색 타이머 실행)
	GetWorld()->GetTimerManager().SetTimer(
		SearchTimer,
		this,
		&AAssistant::SearchTarget,
		SearchRate,
		true		
	);
}

// Called every frame
void AAssistant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//적 탐색 실행 함수
void AAssistant::SearchTarget()
{
	FVector Start = GetActorLocation();
	FRotator ActorRot = GetActorRotation();

	if (RotationSearch)
	{
		AddActorLocalRotation(FRotator(0, 1, 0));
	}

	//호 모양으로 설정한 개수 만큼 라인 트레이스
	for (int i = 0; i < SearchLineNumber; i++)
	{
		float AngleStep = SearchAngle / (SearchLineNumber - 1);
		float AngleOffset = -SearchAngle / 2.0f + i * AngleStep;
		
		FRotator Rot = FRotator(0, AngleOffset, 0);
		FVector Direction = Rot.RotateVector(GetActorForwardVector());

		FVector End = Start + Direction * SearchDistance;

		FHitResult Result;

		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, SearchRate);

		//라인 트레이스
		if (GetWorld()->LineTraceSingleByChannel(
			Result,
			Start,
			End,
			ECC_Pawn
		))
		{
			UE_LOG(LogTemp, Warning, TEXT("SEARCH : Something Detected"));
			//포착된 물체가 적 클래스이면 Target에 넣고 트레이스 중단
			if (Result.GetActor()->IsA(TargetClass))
			{
				UE_LOG(LogTemp, Warning, TEXT("SEARCH : Target Found"));
				Target = Result.GetActor();
				//탐색 트레이스 중단
				GetWorld()->GetTimerManager().ClearTimer(SearchTimer);
				//공격 시작
				GetWorld()->GetTimerManager().SetTimer(
					AttackTimer,
					this,
					&AAssistant::Attack,
					AttackRate,
					true
					);
			}
		}
	}
}

//공격 실행 함수
void AAssistant::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("ATTACK : Attack!!"));
	
	if (AEnemy* Boo = Cast<AEnemy>(Target))
	{
		if (Boo->HealthComponent)
		{
			//Boo->ApplyDamage(Boo, AttackPower);
			UGameplayStatics::ApplyDamage(
				Boo,
				AttackPower,
				Boo->GetController(),
				this,
				NULL
				);
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Health Component"))
		}
	}
	
	//적이 죽었으면 공격 중단 후 탐색 재시작
	if (!IsValid(Target))
	{
		UE_LOG(LogTemp, Warning, TEXT("ATTACK : Target Eliminated"));
		Target = nullptr;
		//공격 중단
		GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
		//탐색 시작
		GetWorld()->GetTimerManager().SetTimer(
		SearchTimer,
		this,
		&AAssistant::SearchTarget,
		SearchRate,
		true		
	);
	}
}

