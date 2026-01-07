// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemySpawner.h"

#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "Enemy/Enemy.h"
#include "Enemy/Moth.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
}

void AEnemySpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
#if WITH_EDITOR
	DrawDebugSphere(
		GetWorld(),
		GetActorLocation(),
		SpawnableAreaRadius,
		64,
		FColor::Green,
		false,
		0,
		0,
		2);
	
	DrawDebugSphere(
		GetWorld(),
		GetActorLocation(),
		UnSpawnableAreaRadius,
		64,
		FColor::Red,
		false,
		0,
		0,
		2);
#endif
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawner::MakeMonsterQueue(TArray<TSubclassOf<AEnemy>>& Queue, float Limit)
{
	Queue.Empty();

	float Remaining = Limit;
	
	const int32 MaxIterations = 100;
	int32 Iteration = 0;

	while (Remaining > 0.f && Iteration++ < MaxIterations)
	{
		//현재 남은 가중치로 가능한 후보 수집
		TArray<TSubclassOf<AEnemy>> Candidates;

		for (const TPair<TSubclassOf<AEnemy>, float>& Pair : MonsterTable)
		{
			if (Pair.Key && Pair.Value <= Remaining)
			{
				Candidates.Add(Pair.Key);
			}
		}

		//더 이상 선택 불가
		if (Candidates.Num() == 0)
		{
			break;
		}

		//랜덤 선택
		int32 Index = FMath::RandRange(0, Candidates.Num() - 1);
		TSubclassOf<AEnemy> ChosenClass = Candidates[Index];

		//큐에 추가 + 예산 차감
		Queue.Add(ChosenClass);
		Remaining -= MonsterTable[ChosenClass];		
	}
	
	//클래스 큐 확인용 로그 출력
	for ( TSubclassOf<AEnemy> Class : Queue)
	{
		UE_LOG(LogTemp, Error, TEXT("%s | "), *Class->GetName());
	}
}

void AEnemySpawner::SpawnMonsters(float MinWeight, float MaxWeight)
{
	//가중치 제한값 설정
	float Limit = FMath::RandRange(MinWeight, MaxWeight);
	
	//몬스터 스폰 큐 생성
	TArray<TSubclassOf<AEnemy>> Queue;
	
	//몬스터 스폰 큐 설정
	MakeMonsterQueue(Queue, Limit);
	
	NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	
	//몬스터 스폰
	for (TSubclassOf<AEnemy> EnemyClass : Queue)
	{
		FNavLocation NavLocation;
		
		//최대 30번 반복
		for (int i=0; i<30; i++)
		{
			//스폰 지점 설정하기
			if (NavSys)
			{
				//내비메시에서 랜덤 지점 뽑기.
				bool IsFound = NavSys->GetRandomPointInNavigableRadius(
					GetActorLocation(), 
					SpawnableAreaRadius,
					NavLocation);
			
				//포인트가 뽑혔으면
				if (IsFound)
				{
					//중앙과의 거리 계산
					float Distance = FVector::Dist2D(GetActorLocation(), NavLocation.Location);
				
					//소환 불가능 반경보다 크면 반복문 탈출
					if (Distance >= UnSpawnableAreaRadius)
						break;
				}
			}
		}
		
		if (EnemyClass->IsChildOf(AMoth::StaticClass()))
		{
			NavLocation.Location.Z += FMath::RandRange(MinSpawnHeight, MaxSpawnHeight);
		} else
		{
			UE_LOG(LogTemp, Error, TEXT("%s is Not Fly Enemy"), *EnemyClass->GetName());
		}
		
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(NavLocation.Location,GetActorLocation());
		
		FTransform SpawnTransform(
			Rotation,
			NavLocation.Location,
			FVector::OneVector);
		
		FActorSpawnParameters SpawnParams = FActorSpawnParameters();
		
		AEnemy* Enemy = GetWorld()->SpawnActorDeferred<AEnemy>(
			EnemyClass, 
			SpawnTransform, 
			this, 
			nullptr,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		
		if (Enemy)
		{
			UE_LOG(LogTemp, Error, TEXT("SPAWNED POINT : %s"), *NavLocation.Location.ToString());
			Enemy->FinishSpawning(SpawnTransform);
		}
	}
}

