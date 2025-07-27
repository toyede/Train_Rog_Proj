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
        // 이동할 Location은 현재 위치에서 BackgroundMoveSpeed * DeltaTime한 값을 뺀 값
        FVector NewLoc = Object->GetActorLocation();
        NewLoc.X -= BackgroundMoveSpeed * DeltaTime;
        Object->SetObjectLocation(NewLoc);
    }
    
    // 앞으로 순간이동 시켜야 할지 체크
	CheckAndRecycleObjects();
}

void ABackGroundManager::CheckAndRecycleObjects()
{
	// 레퍼런스(기차나 카메라) 없을 때 예외처리
    if(!ReferenceActor)
	{
        UE_LOG(LogTemp, Warning, TEXT("레퍼런스 넣어라 좆만아"));
		return;
	}

	// 기준(기차)의 X좌표 (항상 고정)
    float RefX = ReferenceActor->GetActorLocation().X;

    for (auto* Object : BackGroundObjects)
    {
        // 배경 타일의 X방향의 실제 크기
        float ObjectLength = Object->MeasuredLength;

        // 기본 시야값은 배경 타일의 X방향 크기 * 2 (임시로 지정함. 나중에 수정할 것.)
        sight = ObjectLength*2;
        
        // 배경 타일이 화면 뒤쪽으로 충분히 벗어나면
        if (Object->GetActorLocation().X < RefX - ObjectLength - sight)
        {
            // 가장 앞쪽(오른쪽) 오브젝트의 X값 찾기
            float MaxX = -FLT_MAX; // 초기값은 마이너스 무한대

            for (auto* Other : BackGroundObjects)
            {
                // 본인 제외
                if (Other != Object)
                {
                    // 기존에 저장한 MaxX값과 배경 타일의 X값을 비교하여 더 큰 X값을 MaxX에 저장
                    MaxX = FMath::Max(MaxX, Other->GetActorLocation().X);
                }
            }

            // 가장 앞쪽 타일의 앞으로 텔레포트(순환)
            FVector NewLoc = Object->GetActorLocation();
            
            // 가장 앞 타일 + 타일 길이 위치에서 스폰되게 할 것
            NewLoc.X = MaxX + ObjectLength;
            Object->SetObjectLocation(NewLoc);
        }
    }
}