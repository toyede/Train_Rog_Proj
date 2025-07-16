// Fill out your copyright notice in the Description page of Project Settings.


#include "BackGroundManager.h"

// Sets default values
ABackGroundManager::ABackGroundManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABackGroundManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABackGroundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckAndRecycleObjects();
}

void ABackGroundManager::CheckAndRecycleObjects()
{
	// 예외처리
    if(!ReferenceActor)
	{
		return;
	}

	// 기준 엑터(기차)의 X좌표를 저장
    float RefX = ReferenceActor->GetActorLocation().X;

    for (auto* Object : BackGroundObjects) // BackgroundObjects의 배열에 있는 요소 하나씩 빼와서
    {
        if (Object->GetActorLocation().X < RefX - ObjectLength) // 레퍼런스 액터의 위치가 특정 임계점을 벗어나면
        {
			// 다른 오브젝트들 중 가장 앞 위치 (FLT_MAX는 무한대를 의미)
            float MaxX = -FLT_MAX;

            for (auto* Other : BackGroundObjects) // BackgroundObjects의 배열에 있는 요소 하나씩 빼와서
			{
                if (Other != Object) // 나를 제외한 것들에 대해
				{
                    MaxX = FMath::Max(MaxX, Other->GetActorLocation().X); // 가장 X축으로 가장 큰 값을 가진 요소를 찾아옴
				}
			}

            FVector NewLoc = Object->GetActorLocation(); // 새로 배치할 위치
            NewLoc.X = MaxX + ObjectLength;
            Object->SetObjectLocation(NewLoc);
        }
    }
}