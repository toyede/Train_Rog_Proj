// Fill out your copyright notice in the Description page of Project Settings.


#include "BackGroundManager.h"

ABackGroundManager::ABackGroundManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABackGroundManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABackGroundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // 모든 배경 오브젝트를 -X 방향(뒤)으로 이동
    for (auto* Object : BackGroundObjects)
    {
        FVector NewLoc = Object->GetActorLocation();
        NewLoc.X -= BackgroundMoveSpeed * DeltaTime;
        Object->SetObjectLocation(NewLoc);
    }
    
	CheckAndRecycleObjects();
}

void ABackGroundManager::CheckAndRecycleObjects()
{
	// 예외처리
    if(!ReferenceActor)
	{
		return;
	}

	// 기준이 되는 X좌표 (항상 고정)
    float RefX = ReferenceActor->GetActorLocation().X;

    for (auto* Object : BackGroundObjects)
    {
        float ObjectLength = Object->MeasuredLength;
        sight = ObjectLength*2;
        
        // 오브젝트가 화면 뒤쪽으로 충분히 벗어나면
        if (Object->GetActorLocation().X < RefX - ObjectLength - sight)
        {
            // 가장 앞쪽(오른쪽) 오브젝트의 X값 찾기 (본인 제외)
            float MaxX = -FLT_MAX;
            for (auto* Other : BackGroundObjects)
            {
                if (Other != Object)
                {
                    MaxX = FMath::Max(MaxX, Other->GetActorLocation().X);
                }
            }

            // 해당 위치 앞으로 텔레포트(순환)
            FVector NewLoc = Object->GetActorLocation();
            NewLoc.X = MaxX + ObjectLength;
            Object->SetObjectLocation(NewLoc);
        }
    }
}