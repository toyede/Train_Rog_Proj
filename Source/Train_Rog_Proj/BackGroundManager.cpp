// Fill out your copyright notice in the Description page of Project Settings.


#include "BackGroundManager.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

static ETileMoveDirectoin TurnRight90(ETileMoveDirectoin Dir)
{
    switch (Dir)
    {
        case ETileMoveDirectoin::Forward:  
            return ETileMoveDirectoin::Right;

        case ETileMoveDirectoin::Right:    
            return ETileMoveDirectoin::Backward;

        case ETileMoveDirectoin::Backward: 
            return ETileMoveDirectoin::Left;

        case ETileMoveDirectoin::Left:     
            return ETileMoveDirectoin::Forward;
    }
    return Dir;
}

static ETileMoveDirectoin TurnLeft90(ETileMoveDirectoin Dir)
{
    switch (Dir)
    {
        case ETileMoveDirectoin::Forward:  
            return ETileMoveDirectoin::Left;

        case ETileMoveDirectoin::Left:    
             return ETileMoveDirectoin::Backward;

        case ETileMoveDirectoin::Backward: 
            return ETileMoveDirectoin::Right;

        case ETileMoveDirectoin::Right:    
            return ETileMoveDirectoin::Forward;
    }
    return Dir;
}

ETileSpawnDirection ABackGroundManager:: BoolToSpawnDir(bool MoveRight, bool MoveLeft)
{
    if(MoveRight)
    {
        switch (CurrentSpawnDirection)
        {
            case ETileSpawnDirection::Forward:  
                return ETileSpawnDirection::Right;
    
            case ETileSpawnDirection::Backward: 
                return ETileSpawnDirection::Left;
    
            case ETileSpawnDirection::Right:    
                return ETileSpawnDirection::Backward;
    
            case ETileSpawnDirection::Left:     
                return ETileSpawnDirection::Forward;
        }
    }

    else if(MoveLeft)
    {
        switch (CurrentSpawnDirection)
        {
            case ETileSpawnDirection::Forward:  
                return ETileSpawnDirection::Left;
    
            case ETileSpawnDirection::Backward: 
                return ETileSpawnDirection::Right;
    
            case ETileSpawnDirection::Right:    
                return ETileSpawnDirection::Forward;
    
            case ETileSpawnDirection::Left:     
                return ETileSpawnDirection::Backward;
        }
    }

    return ETileSpawnDirection::Forward;
}

ABackGroundManager::ABackGroundManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABackGroundManager::BeginPlay()
{
	Super::BeginPlay();

    for (ABackGroundObjectBase* Obj : TileArray)
    {
        // 예외처리
        if (!Obj) continue;

        // Object가 발생시키는 델리게이트에 Manager의 핸들러를 바인딩
        Obj->OnChangeDirection.AddDynamic(this, &ABackGroundManager::HandleChangeDirection);
    }

    // 초기 타일의 진행 방향 -> Backward
    CurrentMoveDirection = ETileMoveDirectoin::Backward;

    // 초기 타일의 스폰 방향 -> Forward
    CurrentSpawnDirection = ETileSpawnDirection::Forward;
}

void ABackGroundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    for (auto* Object : TileArray)
    {
        FVector NewLoc = Object->GetActorLocation();

        // TileArray속 타일들을 이동시켜주는 분기
        switch (CurrentMoveDirection)
        {
            case ETileMoveDirectoin::Backward:
                // UE_LOG(LogTemp, Warning, TEXT("Debug"));

                // 이동할 Location은 현재 위치에서 BackgroundMoveSpeed * DeltaTime한 값을 뺀 값
                NewLoc.X -= TileMoveSpeed * DeltaTime;
                Object->SetObjectLocation(NewLoc);
                break;
                
            case ETileMoveDirectoin::Right:
                // 이동할 Location은 현재 위치에서 BackgroundMoveSpeed * DeltaTime한 값을 뺀 값
                NewLoc.Y += TileMoveSpeed * DeltaTime;
                Object->SetObjectLocation(NewLoc);
                break;

            case ETileMoveDirectoin::Left:
                // 이동할 Location은 현재 위치에서 BackgroundMoveSpeed * DeltaTime한 값을 뺀 값
                NewLoc.Y -= TileMoveSpeed * DeltaTime;
                Object->SetObjectLocation(NewLoc);
                break;

            case ETileMoveDirectoin::Forward:
                // 이동할 Location은 현재 위치에서 BackgroundMoveSpeed * DeltaTime한 값을 뺀 값
                NewLoc.X += TileMoveSpeed * DeltaTime;
                Object->SetObjectLocation(NewLoc);
                break;

            default:
                break;
        }
    }

    // UE_LOG(LogTemp, Warning, TEXT("%d"), CurrentSpawnDirection);
    
    // 앞으로 순간이동 시켜야 할지 체크
	CheckAndRecycleObjects();
}

// Triger Overlap 핸들 함수
void ABackGroundManager::HandleChangeDirection(ABackGroundObjectBase* SourceObject)
{
    UE_LOG(LogTemp, Warning, TEXT("%s가 방향 전환 트리거에 걸렸습니다."), *SourceObject->GetName());

    // 기차와 닿은 타일의 카테고리 확인
    if (SourceObject->TileCategory == ETileCategory::TileRight)
    {
        CurrentMoveDirection  = TurnRight90(CurrentMoveDirection);
    }

    else if (SourceObject->TileCategory == ETileCategory::TileLeft)
    {
        CurrentMoveDirection  = TurnLeft90(CurrentMoveDirection);
    }

    else
    {
        return; // Default 타일에겐 회전 없음
    }
}

// 타일 종류 변경용 함수: 다음 스폰될 타일을 Right또는 Left타일로 변경
void ABackGroundManager::SetNextTileRL(auto* Object, FVector NewLoc)
{
    UE_LOG(LogTemp, Warning, TEXT("Run"));
    // 원하는 카테고리 결정
    ETileCategory Desired = Change_Right_Tile ? ETileCategory::TileRight : ETileCategory::TileLeft;

    // TileTypes에서 원하는 종류의 타일 꺼내오기
    for (auto& FoundTileType : TileTypes)
    {
        if (!*FoundTileType)
        {
            // 예외처리
            UE_LOG(LogTemp, Warning, TEXT("Error1"));
            continue;
        }

        // 오브젝트로부터 카테고리 읽고 원하는 종류의 타일이 아니면 빠져나오기
        const ABackGroundObjectBase* DesiredTile = FoundTileType->GetDefaultObject<ABackGroundObjectBase>();

        if (DesiredTile->TileCategory != Desired)
        {
            // 원하는 타일이 타일종류배열에 없을 때 오류 방지용
            UE_LOG(LogTemp, Warning, TEXT("Error2"));
            continue;
        }

        // 스폰해주기: TSubclassOf를 직접 넘겨줌
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        // NewTile이라는 이름으로 원하는 카테고리의 타일 스폰 준비
        ABackGroundObjectBase* NewTile = GetWorld()->SpawnActor<ABackGroundObjectBase>(
            FoundTileType, // 해당 종류의 타일로
            NewLoc, // 인자로 넘겨받은 위치에
            Object->GetActorRotation(), // 기본 Rotation으로
            Params); // 스폰

        // 타일 스폰 준비가 되었으면 배열에 추가 및 소품들 삭제하고 재생성
        if (NewTile)
        {
            // Triger 설정용
            NewTile->SetTrigerBox();
            NewTile->OnChangeDirection.AddDynamic(this, &ABackGroundManager::HandleChangeDirection);
            
            // 범위를 벗어났던 기존 타일을 삭제하고 RL타입 타일을 새로 추가
            TileArray.Add(NewTile);
            TileArray.Remove(Object);

            // 소품 관리
            Object->RemoveSmallBackGrounds();
            Object->Destroy();

            // RL타일이 생성된 이후 다음으로 생성될 타일의 위치를 RL타일의 위치로 함
            Flag_SpawnedRLTile = true;
            LastRLSourceTile  = NewTile;

            // RL타일이 생성된 후 타일이 생성되는 방향을 변경해줌
            CurrentSpawnDirection = BoolToSpawnDir(Change_Right_Tile, Change_Left_Tile);
        }
        break;
    }

    // 딱 한번만 RL타일이 나오게 다시 변수 꺼주기
    Change_Right_Tile = false;
    Change_Left_Tile = false;
}

void ABackGroundManager::SetNextTileDF(auto* Object, FVector NewLoc)
{
    for (auto& FoundTileType : TileTypes)
    {
        if (!*FoundTileType) 
        {
            // 예외처리
            continue;
        }

        const auto* DesiredTile = FoundTileType->GetDefaultObject<ABackGroundObjectBase>();

        if (DesiredTile->TileCategory != ETileCategory::TileDefault)
        {
            // 예외처리
            continue;
        }

        // 소품 먼저 정리
        Object->RemoveSmallBackGrounds();

        // 새 Default 타일 스폰
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ABackGroundObjectBase* NewTile = GetWorld()->SpawnActor<ABackGroundObjectBase>(
            FoundTileType,
            NewLoc,
            Object->GetActorRotation(),
            Params);

        if (NewTile)
        {
            // 리스트 교체
            NewTile->OnChangeDirection.AddDynamic(this, &ABackGroundManager::HandleChangeDirection);

            TileArray.Add(NewTile);
            TileArray.Remove(Object);

            Object->Destroy();
        }
        break;
    }
}

void ABackGroundManager::CheckAndRecycleObjects()
{
    if(!ReferenceActor)
	{
        // 레퍼런스(기차나 카메라) 없을 때 예외처리
        UE_LOG(LogTemp, Warning, TEXT("기차 레퍼런스 확인"));
		return;
	}

    // 기준(기차)의 좌표 (항상 고정)
    const FVector RefLoc = ReferenceActor->GetActorLocation();

    for (auto* Object : TileArray)
    {
        // 임시로 시야 설정
        sight = Object->MeasuredXLength * 4.0f;

        // 사각형 범위의 지름은 타일길이 + sight
        float Threshold = sight;

        FVector CustomExtentXLong(Threshold, Threshold/2.0f, Threshold);
        FVector CustomExtentYLong(Threshold/2.0f, Threshold, Threshold);
    
        // 정사각형 모양의 범위 설정
        const FBox SpawnBoxX(RefLoc - CustomExtentXLong, RefLoc + CustomExtentXLong);
        const FBox SpawnBoxY(RefLoc - CustomExtentYLong, RefLoc + CustomExtentYLong);

        // 디버깅을 위한 시각화 코드 추가
        #if ENABLE_DRAW_DEBUG
            // DrawDebugBox를 사용하기 위해 DrawDebugHelpers.h 헤더 파일이 필요합니다.
            // 파일 상단에 #include "DrawDebugHelpers.h"가 있는지 확인하세요.
            // (BackGroundObjectBase.cpp에는 이미 포함되어 있습니다.)

            // SpawnBoxX를 녹색으로 그립니다.
            DrawDebugBox(GetWorld(), SpawnBoxX.GetCenter(), SpawnBoxX.GetExtent(), FColor::Green, false, 0.1f, 0, 5.f);

            // SpawnBoxY를 파란색으로 그립니다.
            DrawDebugBox(GetWorld(), SpawnBoxY.GetCenter(), SpawnBoxY.GetExtent(), FColor::Blue, false, 0.1f, 0, 5.f);
        #endif

        // 현재 검사중인 타일의 위치
        const FVector ObjLoc = Object->GetActorLocation();

        // 타일이 사각형 범위 안에 들어가있으면 재배치 불필요하므로 벗어나기
        if (CurrentMoveDirection == ETileMoveDirectoin::Forward || CurrentMoveDirection == ETileMoveDirectoin::Backward)
        {
            if(SpawnBoxX.IsInside(ObjLoc))
            {
                continue;
            }
        }

        else if (CurrentMoveDirection == ETileMoveDirectoin::Right || CurrentMoveDirection == ETileMoveDirectoin::Left)
        {
            if(SpawnBoxY.IsInside(ObjLoc))
            {
                continue;
            }
        }

        // 타일을 이동할 위치 변수
        FVector NewLoc = ObjLoc;
        
        // 위 분기에서 벗어나지 못함 == 지금 검사중인 타일은 범위를 벗어난 타일임

        // =============================================================================================================
        // // 타일의 스폰 방향에 따라 타일을 이동할 위치 조정
        // =============================================================================================================

        switch (CurrentSpawnDirection)
        {
            case ETileSpawnDirection::Forward:
            // 타일의 스폰방향이 Forward == 타일의 진행 방향은 Backward
            {
                // 직전에 RL타일이 스폰되었을때 다음으로 스폰될 타일은 RL타일을 기준으로 붙여주기
                if (Flag_SpawnedRLTile)
                {
                    NewLoc.X = LastRLSourceTile->GetActorLocation().X + LastRLSourceTile->MeasuredYLength;
                    NewLoc.Y = LastRLSourceTile->GetActorLocation().Y;
                    
                    // Flag 해제
                    Flag_SpawnedRLTile = false;
                    // LastRLSourceTile  = nullptr;
                }
                
                else
                {
                    if(LastRLSourceTile)
                    {
                        if(LastRLSourceTile->TileCategory == ETileCategory::TileRight)
                        {
                            float MaxX = -FLT_MAX; // 스폰방향이 Forward가 되도록 X가 가장 큰 위치의 타일에 붙여줘야 함
                            float MinY = FLT_MAX; // RL타일을 만나 방향이 바뀌었을 때를 대비하여 Y값을 업데이트해줌
                            
                            // 타일 배열에서 나를 제외한 오브젝트중 목표로 하는 위치의 타일 위치 저장
                            for (auto* Other : TileArray)
                            {
                                if (Other != Object)
                                {
                                    MaxX = FMath::Max(MaxX, Other->GetActorLocation().X);
                                    MinY = FMath::Min(MinY, Other->GetActorLocation().Y);
                                }
                            }
        
                            // 목표로하는 위치의 타일 위치 + 타일 길이만큼의 거리가 스폰하고자 하는 위치 
                            NewLoc.X = MaxX + Object->MeasuredXLength;
                            NewLoc.Y = MinY;
                        }
    
                        else if(LastRLSourceTile->TileCategory == ETileCategory::TileLeft)
                        {
                            float MaxX = -FLT_MAX; // 스폰방향이 Forward가 되도록 X가 가장 큰 위치의 타일에 붙여줘야 함
                            float MaxY = -FLT_MAX; // RL타일을 만나 방향이 바뀌었을 때를 대비하여 Y값을 업데이트해줌
                            
                            // 타일 배열에서 나를 제외한 오브젝트중 목표로 하는 위치의 타일 위치 저장
                            for (auto* Other : TileArray)
                            {
                                if (Other != Object)
                                {
                                    MaxX = FMath::Max(MaxX, Other->GetActorLocation().X);
                                    MaxY = FMath::Max(MaxY, Other->GetActorLocation().Y);
                                }
                            }
        
                            // 목표로하는 위치의 타일 위치 + 타일 길이만큼의 거리가 스폰하고자 하는 위치 
                            NewLoc.X = MaxX + Object->MeasuredXLength;
                            NewLoc.Y = MaxY;
                        }
                    }

                    else
                    {
                        float MaxX = -FLT_MAX; // 스폰방향이 Forward가 되도록 X가 가장 큰 위치의 타일에 붙여줘야 함
                        float MinY = FLT_MAX; // RL타일을 만나 방향이 바뀌었을 때를 대비하여 Y값을 업데이트해줌
                        
                        // 타일 배열에서 나를 제외한 오브젝트중 목표로 하는 위치의 타일 위치 저장
                        for (auto* Other : TileArray)
                        {
                            if (Other != Object)
                            {
                                MaxX = FMath::Max(MaxX, Other->GetActorLocation().X);
                                MinY = FMath::Min(MinY, Other->GetActorLocation().Y);
                            }
                        }
    
                        // 목표로하는 위치의 타일 위치 + 타일 길이만큼의 거리가 스폰하고자 하는 위치 
                        NewLoc.X = MaxX + Object->MeasuredXLength;
                        NewLoc.Y = MinY;
                    }
                }
                
                // 선로 Mesh 회전시켜주기
                CurrentTileRot = Object->GetActorRotation();
                CurrentTileRot.Yaw = .0f;
                Object->SetActorRotation(CurrentTileRot);
            }
                break;

            case ETileSpawnDirection::Backward:
            // 타일의 스폰방향이 Backward == 타일의 진행 방향은 Forward
            {
                if (Flag_SpawnedRLTile)
                {
                    // 수정이 필요한 부분: R->R / L->L 두개의 case 나누어 생각해야 함
                    NewLoc.X = LastRLSourceTile->GetActorLocation().X - LastRLSourceTile->MeasuredYLength;
                    NewLoc.Y = LastRLSourceTile->GetActorLocation().Y;

                    Flag_SpawnedRLTile = false;
                }

                else
                {
                    if(LastRLSourceTile->TileCategory == ETileCategory::TileRight) // << 여기서 간혈적으로 오류 발생중 (null pointer) 예의주시 할 것
                    {
                        float MinX = FLT_MAX;
                        float MaxY = -FLT_MAX;
        
                        for (auto* Other : TileArray)
                        {
                            if (Other != Object)
                            {
                                MinX = FMath::Min(MinX, Other->GetActorLocation().X);
                                MaxY = FMath::Max(MaxY, Other->GetActorLocation().Y);
                            }
                        }
                        NewLoc.X = MinX - Object->MeasuredXLength;
                        NewLoc.Y = MaxY;
                    }

                    else
                    {
                        float MinX = FLT_MAX;
                        float MinY = FLT_MAX;
        
                        for (auto* Other : TileArray)
                        {
                            if (Other != Object)
                            {
                                MinX = FMath::Min(MinX, Other->GetActorLocation().X);
                                MinY = FMath::Min(MinY, Other->GetActorLocation().Y);
                            }
                        }
                        NewLoc.X = MinX - Object->MeasuredXLength;
                        NewLoc.Y = MinY;
                    }
                }

                CurrentTileRot = Object->GetActorRotation();
                CurrentTileRot.Yaw = 180.0f;
                Object->SetActorRotation(CurrentTileRot);
            }
                break;

            case ETileSpawnDirection::Right:
            // 타일의 스폰방향이 Right == 타일의 진행 방향은 Left
            {
                if (Flag_SpawnedRLTile)
                {
                    NewLoc.X = LastRLSourceTile->GetActorLocation().X;
                    NewLoc.Y = LastRLSourceTile->GetActorLocation().Y + LastRLSourceTile->MeasuredYLength;

                    Flag_SpawnedRLTile = false;
                }

                else
                {
                    if(LastRLSourceTile->TileCategory == ETileCategory::TileRight)
                    {
                        float MaxX = -FLT_MAX;
                        float MaxY = -FLT_MAX;
    
                        for (auto* Other : TileArray)
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

                    else
                    {
                        float MinX = FLT_MAX;
                        float MaxY = -FLT_MAX;
    
                        for (auto* Other : TileArray)
                        {
                            if (Other != Object)
                            {
                                MinX = FMath::Min(MinX, Other->GetActorLocation().X);
                                MaxY = FMath::Max(MaxY, Other->GetActorLocation().Y);
                            }
                        }
                        NewLoc.X = MinX;
                        NewLoc.Y = MaxY + Object->MeasuredYLength;
                    }
                }

                CurrentTileRot = Object->GetActorRotation();
                CurrentTileRot.Yaw = 90.0f;
                Object->SetActorRotation(CurrentTileRot);
            }
                break;

            case ETileSpawnDirection::Left:
            // 타일의 스폰방향이 Left == 타일의 진행 방향은 Right
            {
                if (Flag_SpawnedRLTile)
                {
                    NewLoc.X = LastRLSourceTile->GetActorLocation().X;
                    NewLoc.Y = LastRLSourceTile->GetActorLocation().Y - LastRLSourceTile->MeasuredYLength;

                    Flag_SpawnedRLTile = false;
                }

                else
                {
                    // Right를 통해 Left로 진입했을 경우
                    if(LastRLSourceTile->TileCategory == ETileCategory::TileRight)
                    {
                        float MinX = FLT_MAX;
                        float MinY = FLT_MAX;
    
                        for (auto* Other : TileArray)
                        {
                            if (Other != Object)
                            {
                                MinX = FMath::Min(MinX, Other->GetActorLocation().X);
                                MinY = FMath::Min(MinY, Other->GetActorLocation().Y);
                            }
                        }
    
                        NewLoc.X = MinX;
                        NewLoc.Y = MinY - Object->MeasuredYLength;
                    }

                    // Left를 통해 Left로 진입했을경우
                    else
                    {
                        float MaxX = -FLT_MAX;
                        float MinY = FLT_MAX;
    
                        for (auto* Other : TileArray)
                        {
                            if (Other != Object)
                            {
                                MaxX = FMath::Max(MaxX, Other->GetActorLocation().X);
                                MinY = FMath::Min(MinY, Other->GetActorLocation().Y);
                            }
                        }
    
                        NewLoc.X = MaxX;
                        NewLoc.Y = MinY - Object->MeasuredYLength;
                    }
                }
                
                CurrentTileRot = Object->GetActorRotation();
                CurrentTileRot.Yaw = -90.0f;
                Object->SetActorRotation(CurrentTileRot);
            }
                break;

            default:
                break;
        }

        
        // =============================================================================================================
        // // 타일 종류로 분기를 나눠 위에서 조정한 위치로 타일 스폰
        // =============================================================================================================

        // 현재 검사중인 타일이 Default타입이고 RL타입 타일로 바꾸어야 함 -> RL타입 스폰 메서드 호출
        if (Change_Right_Tile || Change_Left_Tile)
        {
            SetNextTileRL(Object, NewLoc);
        }

        // 현재 검사중인 타일이 RL타입이고 이제 Default타입 타일로 바꾸어야 함 -> Default타입 스폰 메서드 호출
        else if (Object->TileCategory == ETileCategory::TileRight || Object->TileCategory == ETileCategory::TileLeft)
        {
            // LR선로 타일 순환 방지용: 다음으로 생성될 타일을 Default로 설정
            SetNextTileDF(Object, NewLoc);
        }

        // 현재 검사중인 타일이 Default타일이고 RL타입 타일로 바꿀 필요가 없으면 소품 초기화해주며 타일 재활용
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