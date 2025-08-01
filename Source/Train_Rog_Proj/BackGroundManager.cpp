// Fill out your copyright notice in the Description page of Project Settings.


#include "BackGroundManager.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"

ABackGroundManager::ABackGroundManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABackGroundManager::BeginPlay()
{
	Super::BeginPlay();

    // BackGroundObjects 배열에 담긴 모든 Object 인스턴스에 대해
    for (ABackGroundObjectBase* Obj : BackGroundObjects)
    {
        if (!Obj) continue;
        // Object가 발생시키는 델리게이트에 Manager의 핸들러를 바인딩
        Obj->OnChangeDirection.AddDynamic(this, &ABackGroundManager::HandleChangeDirection);
    }

    CurrentMoveDirection = EBackGroundMoveDirectoin::Forward;
    CurrentSpawnDirection = EMySpawnDirection::Forward;
}

void ABackGroundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // 모든 배경 오브젝트를 -X 방향(뒤)으로 이동
    for (auto* Object : BackGroundObjects)
    {
        if(CurrentMoveDirection == EBackGroundMoveDirectoin::Forward)
        {
            // 이동할 Location은 현재 위치에서 BackgroundMoveSpeed * DeltaTime한 값을 뺀 값
            FVector NewLoc = Object->GetActorLocation();
            NewLoc.X -= BackgroundMoveSpeed * DeltaTime;
            Object->SetObjectLocation(NewLoc);
        }

        if(CurrentMoveDirection == EBackGroundMoveDirectoin::Left)
        {
            // 이동할 Location은 현재 위치에서 BackgroundMoveSpeed * DeltaTime한 값을 뺀 값
            FVector NewLoc = Object->GetActorLocation();
            NewLoc.Y += BackgroundMoveSpeed * DeltaTime;
            Object->SetObjectLocation(NewLoc);
        }

        if(CurrentMoveDirection == EBackGroundMoveDirectoin::Right)
        {
            // 이동할 Location은 현재 위치에서 BackgroundMoveSpeed * DeltaTime한 값을 뺀 값
            FVector NewLoc = Object->GetActorLocation();
            NewLoc.Y -= BackgroundMoveSpeed * DeltaTime;
            Object->SetObjectLocation(NewLoc);
        }
    }
    
    // 앞으로 순간이동 시켜야 할지 체크
	CheckAndRecycleObjects();
}

// Triger Overlap 핸들 함수
void ABackGroundManager::HandleChangeDirection(ABackGroundObjectBase* SourceObject)
{
    UE_LOG(LogTemp, Warning, TEXT("%s가 방향 전환 트리거에 걸렸습니다."), *SourceObject->GetName());

    if(SourceObject->TileCategory == ETileCategory::TileRight)
    {
        CurrentMoveDirection = EBackGroundMoveDirectoin::Right;
    }

    else if(SourceObject->TileCategory == ETileCategory::TileLeft)
    {
        CurrentMoveDirection = EBackGroundMoveDirectoin::Left;
    }
}

void ABackGroundManager::SetNextTileRL(auto* Object, FVector NewLoc)
{
    UE_LOG(LogTemp, Warning, TEXT("디버그1"));

    // 원하는 카테고리 결정
    ETileCategory Desired = TestRight ? ETileCategory::TileRight 
                                    : ETileCategory::TileLeft;

    // BackGroundTypes에서 확인
    for (auto& ProtoClass : BackGroundTypes)
    {
        if (!*ProtoClass) 
            continue;  // null 체크

        // 클래스 디폴트 오브젝트로부터 카테고리 읽기
        const ABackGroundObjectBase* CDO = ProtoClass->GetDefaultObject<ABackGroundObjectBase>();
        if (CDO->TileCategory != Desired)
        {
            continue;
        }

        // 진짜 스폰: TSubclassOf를 직접 넘겨줌
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ABackGroundObjectBase* NewTile = GetWorld()->SpawnActor<ABackGroundObjectBase>(
            ProtoClass,
            NewLoc,
            Object->GetActorRotation(),
            Params);

        if (NewTile)
        {
            BackGroundObjects.Add(NewTile);

            NewTile->SetTrigerBox();
            NewTile->OnChangeDirection.AddDynamic(this, &ABackGroundManager::HandleChangeDirection);

            BackGroundObjects.Remove(Object);
            Object->RemoveSmallBackGrounds();
            Object->Destroy();

            // **여기서 한 번만 특수 Y 스폰지점으로 설정**  
            bJustSpawnedRL = true;
            LastRLSource  = NewTile;

            CurrentSpawnDirection = TestRight ? EMySpawnDirection::Right : EMySpawnDirection::Left;
        }
        break;
    }

    TestRight = false;
    TestLeft = false;
}

void ABackGroundManager::SetNextTileDF(auto* Object, FVector NewLoc)
{
    UE_LOG(LogTemp, Warning, TEXT("디버그2"));

    // Default 카테고리 클래스 찾기
    for (auto& ProtoClass : BackGroundTypes)
    {
        // 예외처리
        if (!*ProtoClass) 
        {
            continue;
        }

        const auto* CDO = ProtoClass->GetDefaultObject<ABackGroundObjectBase>();

        // 예외처리
        if (CDO->TileCategory != ETileCategory::TileDefault)
        {
            continue;
        }

        // 소품 먼저 정리
        Object->RemoveSmallBackGrounds();

        // 새 Default 타일 스폰
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = 
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ABackGroundObjectBase* NewTile = GetWorld()->SpawnActor<ABackGroundObjectBase>(
            ProtoClass,
            NewLoc,
            Object->GetActorRotation(),
            Params);

        if (NewTile)
        {
            // 리스트 교체
            BackGroundObjects.Add(NewTile);
            NewTile->OnChangeDirection.AddDynamic(this, &ABackGroundManager::HandleChangeDirection);
            BackGroundObjects.Remove(Object);
            Object->Destroy();

            // CurrentSpawnDirection = EMySpawnDirection::Forward;
        }
        break;
    }
}

void ABackGroundManager::CheckAndRecycleObjects()
{
    // 레퍼런스(기차나 카메라) 없을 때 예외처리
    if(!ReferenceActor)
	{
        UE_LOG(LogTemp, Warning, TEXT("레퍼런스 넣어라 좆만아"));
		return;
	}

    // 기준(기차)의 좌표 (항상 고정)
    const FVector RefLoc = ReferenceActor->GetActorLocation();

    for (auto* Object : BackGroundObjects)
    {
        sight = Object->MeasuredYLength * 3;

        // 2) Threshold: 타일의 한 변 변 + sight (기존 세팅)
        float Radius = Object->MeasuredXLength + sight;
    
        // 3) 정사각형 영역 생성 (RefLoc ± Radius)
        const FBox SpawnBox(RefLoc - FVector(Radius), RefLoc + FVector(Radius)
);

        const FVector ObjLoc = Object->GetActorLocation();

        // 4) 영역 안에 있으면 재배치 불필요
        if (SpawnBox.IsInside(ObjLoc))
        {
            continue;
        }

        // 5) 위치 분기: 영역 밖으로 벗어난 타일을 처리
        FVector NewLoc = ObjLoc;

        switch (CurrentSpawnDirection)
        {
            case EMySpawnDirection::Forward:
            // 기본 진행 방향: X축 가장 앞쪽에 붙임
            {
                float MaxX = -FLT_MAX;
                for (auto* Other : BackGroundObjects)
                {
                    if (Other != Object)
                    {
                        MaxX = FMath::Max(MaxX, Other->GetActorLocation().X);
                    }
                }
                NewLoc.X = MaxX + Object->MeasuredXLength;
            }
                break;

            case EMySpawnDirection::Right:
            // Y축 + 방향에 붙임
            {
                // 첫 RL 스폰 직후 한 번만 
                if (bJustSpawnedRL)
                {
                    UE_LOG(LogTemp, Warning, TEXT("디버그R1"));
                    NewLoc.X = LastRLSource->GetActorLocation().X;
                    NewLoc.Y = LastRLSource->GetActorLocation().Y + LastRLSource->MeasuredYLength;

                    // 플래그 해제
                    bJustSpawnedRL = false;
                    LastRLSource  = nullptr;
                }

                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("디버그R2"));

                    float MaxX = -FLT_MAX;
                    float MaxY = -FLT_MAX;
                    for (auto* Other : BackGroundObjects)
                    {
                        if (Other != Object)
                        {
                            MaxX = FMath::Max(MaxX, Other->GetActorLocation().X);
                            MaxY = FMath::Max(MaxY, Other->GetActorLocation().Y);
                        }
                    }
                    NewLoc.X = MaxX;
                    NewLoc.Y = MaxY + Object->MeasuredYLength;
                }

                CurrentRot = Object->GetActorRotation();
                CurrentRot.Yaw = 90.0f;
                Object->SetActorRotation(CurrentRot);
            }
                break;

            case EMySpawnDirection::Left:
            // Y축 - 방향에 붙임
            {
                if (bJustSpawnedRL)
                {
                    UE_LOG(LogTemp, Warning, TEXT("디버그L1"));
                    NewLoc.X = LastRLSource->GetActorLocation().X;
                    NewLoc.Y = LastRLSource->GetActorLocation().Y - LastRLSource->MeasuredYLength;

                    // 플래그 해제
                    bJustSpawnedRL = false;
                    LastRLSource  = nullptr;
                }

                else
                {
                    float MinY = FLT_MAX;
                    for (auto* Other : BackGroundObjects)
                    {
                        if (Other != Object)
                        {
                            MinY = FMath::Min(MinY, Other->GetActorLocation().Y);
                        }
                    }
                    NewLoc.Y = MinY - Object->MeasuredYLength;
                }

            }
                break;

            default:
                break;
        }

        // 6) 타일 종류에 따른 스폰 분기: TestRight/TestLeft → RL 스폰, 아니면 현재 타일 카테고리로 DF 스폰
        if (TestRight || TestLeft)
        {
            SetNextTileRL(Object, NewLoc);
        }

        else if (Object->TileCategory == ETileCategory::TileRight || Object->TileCategory == ETileCategory::TileLeft)
        {
            // LR선로 타일 순환 방지용: 다음으로 생성될 타일을 Default로 설정
            SetNextTileDF(Object, NewLoc);
        }

        else
        {
            Object->RemoveSmallBackGrounds();
            Object->SetObjectLocation(NewLoc);
            Object->SpawnSmallBackGrounds();
        }

        // 처리 후 바로 다음 오브젝트로
        continue;
    }
}