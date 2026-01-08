// 단순히 액터의 크기를 저장하는 함수


#include "BackGroundObjectBase.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

ABackGroundObjectBase::ABackGroundObjectBase()
{
	PrimaryActorTick.bCanEverTick = true; // Tic 사용 안함
}

void ABackGroundObjectBase::BeginPlay()
{
	Super::BeginPlay();

	// 배경 타일의 크기 측정
	// false: Colider 없는 Mesh도 포함, Origin: 월드 좌표 기준, BoxExtent: 액터의 절반 길이가 해당 주소에 저장됨
    GetActorBounds(false, Origin, BoxExtent); // this(타일 크기)

    // X방향(가로, 일반적으로 길이로 사용)의 실제 크기 저장
    MeasuredXLength = BoxExtent.X * 2.0f;
    MeasuredYLength = BoxExtent.Y * 2.0f;

    // 모든 StaticMeshComponent 중 태그가 ExclusionZone인 것만 수집
    TArray<UStaticMeshComponent*> MeshComps;
    GetComponents<UStaticMeshComponent>(MeshComps);

    ExclusionZones.Empty();
    for (UStaticMeshComponent* Mesh : MeshComps)
    {
        if (Mesh->ComponentHasTag(FName("ExclusionZone")))
        {
            ExclusionZones.Add(Mesh);
        }
    }

	// 소품 스폰 메서드 호출
    SpawnSmallBackGrounds();
}

// // 기차를 회전시키는 부분
// void ABackGroundObjectBase::Tick(float DeltaTime)
// {
//     Super::Tick(DeltaTime);

//     if (!bIsRotating || !ActorToRotate) 
//     {
//         return;
//     }

//     // 경과 시간 누적
//     ElapsedTime += DeltaTime;
//     float Alpha = FMath::Clamp(ElapsedTime / RotationTime, 0.0f, 1.0f);

//     // 선형 보간으로 회전값 계산
//     FRotator NewRot = FMath::Lerp(StartRotation, TargetRotation, Alpha);
//     ActorToRotate->SetActorRotation(NewRot);

//     // 완료 시 상태 정리
//     if (Alpha >= 1.0f)
//     {
//         bIsRotating   = false;
//         ActorToRotate = nullptr;
//     }
// }

// 기차 회전시키는 부분 업데이트
void ABackGroundObjectBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsRotating || !ActorToRotate) 
    {
        return;
    }

    // 경과 시간 및 진행률(Alpha) 계산
    ElapsedTime += DeltaTime;
    float Alpha = FMath::Clamp(ElapsedTime / RotationTime, 0.0f, 1.0f);

    // 1. 기차의 회전 (Rotation) 처리 - 기존과 동일하게 보간
    FRotator NewRot = FMath::Lerp(StartRotation, TargetRotation, Alpha);
    ActorToRotate->SetActorRotation(NewRot);

    // 2. 기차의 위치 (Location) 보정 - [핵심 로직]
    // 배경 타일이 움직이면(Manager에 의해), 타일의 월드 변환행렬도 변합니다.
    // 따라서 로컬 Pivot을 월드 좌표로 변환하면 "움직이는 Pivot"을 얻을 수 있습니다.
    FVector WorldPivot = GetActorTransform().TransformPosition(LocalPivotPoint);

    // 기차의 위치는 Pivot으로부터 "회전 반경"만큼 떨어진 곳이어야 합니다.
    // 우회전 중이라면, 기차는 Pivot의 "왼쪽"에 있어야 합니다.
    // 좌회전 중이라면, 기차는 Pivot의 "오른쪽"에 있어야 합니다.
    
    // 현재 기차의 Right Vector 구하기
    FVector CurrentRightVec = FRotationMatrix(NewRot).GetScaledAxis(EAxis::Y);

    FVector NewLoc;
    if (TileCategory == ETileCategory::TileRight)
    {
        // 우회전: Pivot은 기차의 오른쪽에 있음 -> 기차는 Pivot에서 -Right 방향으로 R만큼 이동
        NewLoc = WorldPivot - (CurrentRightVec * RotationRadius);
    }
    else // TileLeft
    {
        // 좌회전: Pivot은 기차의 왼쪽에 있음 -> 기차는 Pivot에서 +Right 방향으로 R만큼 이동
        NewLoc = WorldPivot + (CurrentRightVec * RotationRadius);
    }

    // 높이(Z) 유지 (필요 시)
    NewLoc.Z = ActorToRotate->GetActorLocation().Z;

    // 최종 위치 적용
    ActorToRotate->SetActorLocation(NewLoc);

    // 완료 처리
    if (Alpha >= 1.0f)
    {
        bIsRotating = false;
        ActorToRotate = nullptr;
    }
}

void ABackGroundObjectBase::SetTrigerBox()
{
    // BP에서 추가한 BoxCollision 컴포넌트 찾기 (태그가 달렸다고 가정)
    TArray<UBoxComponent*> Boxes;
    GetComponents<UBoxComponent>(Boxes);
    for (auto* Box : Boxes)
    {
        if (Box->ComponentHasTag(FName("ChangeDirectionZone")))
        {
            ChangeDirTrigger = Box;
            break;
        }
    }

    if (ChangeDirTrigger)
    {
        // Overlap 이벤트 바인딩
        ChangeDirTrigger->OnComponentBeginOverlap.AddDynamic(
            this, &ABackGroundObjectBase::OnChangeDirOverlap);
        
        UE_LOG(LogTemp, Warning, TEXT("ChangeDirTrigger를 찾았습니다."));
    }

    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ChangeDirTrigger를 찾지 못했습니다."));
    }
}

// void ABackGroundObjectBase::OnChangeDirOverlap(
//     UPrimitiveComponent* OverlappedComp,
//     AActor* OtherActor,
//     UPrimitiveComponent* OtherComp,
//     int32 OtherBodyIndex,
//     bool bFromSweep,
//     const FHitResult& SweepResult)
// {
//     // 예: 기차 액터만 필터링
//     if (OtherActor && OtherActor != this && OtherComp->ComponentHasTag(FName("TrainCollision")))
//     {
//         UE_LOG(LogTemp, Warning, TEXT("BaseCpp충돌알림"));
//         // Delegate 를 통해 Manager 등 외부에 알린다
        
//         OnChangeDirection.Broadcast(this);

//         // 이미 회전 중이면 무시
//         if (bIsRotating)
//         {
//             return;
//         }

//         ActorToRotate = OtherActor;
//         StartRotation = OtherActor->GetActorRotation();
//         ElapsedTime     = .0f;

//         if(TileCategory == ETileCategory::TileRight)
//         {
//             bIsRotating     = true;
//             TargetRotation  = StartRotation + FRotator(0, 90, 0);  // Yaw +90도
//         }

//         else if(TileCategory == ETileCategory::TileLeft)
//         {
//             bIsRotating     = true;
//             TargetRotation  = StartRotation + FRotator(0, -90, 0);  // Yaw +90도
//         }
//     }
// }

void ABackGroundObjectBase::OnChangeDirOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    // 기차 액터 필터링
    if (OtherActor && OtherActor != this && OtherComp->ComponentHasTag(FName("TrainCollision")))
    {
        // 중복 트리거 방지
        if (bIsRotating) return;

        OnChangeDirection.Broadcast(this);

        ActorToRotate = OtherActor;
        StartRotation = OtherActor->GetActorRotation();
        ElapsedTime = 0.0f;
        bIsRotating = true;

        // --- [수정된 부분] Pivot 및 궤적 계산 ---

        // 회전 반경은 타일 크기의 절반 (가로/세로 동일하다고 가정)
        // BoxExtent는 절반 크기이므로 MeasuredYLength / 2 와 같습니다.
        RotationRadius = BoxExtent.Y; 

        // 타일 로컬 좌표 기준 Pivot 설정
        // 우회전(Right) 선로: 진입점 기준 오른쪽 구석이 중심
        // 좌회전(Left) 선로: 진입점 기준 왼쪽 구석이 중심
        // (기차가 +X 방향을 보고 진입한다고 가정할 때의 로컬 좌표 계산)
        
        if (TileCategory == ETileCategory::TileRight)
        {
            // 우회전: 90도 회전
            TargetRotation = StartRotation + FRotator(0, 90.0f, 0);
            RotationSign = 1.0f; 
            
            // 로컬 Pivot: 기차 진행방향(Forward) 기준 오른쪽(Right) 구석
            // 타일 중심(0,0)에서 (ExtentX, ExtentY) 위치라고 가정
            // 정확한 위치는 타일의 Pivot Point 설정에 따라 다를 수 있으나, 
            // 보통 타일 끝 모서리가 원의 중심입니다.
            // [수정] BoxExtent.X -> -BoxExtent.X (입구 쪽 모서리)
            LocalPivotPoint = FVector(-BoxExtent.X, BoxExtent.Y, 0.0f);
        }
        else if (TileCategory == ETileCategory::TileLeft)
        {
            // 좌회전: -90도 회전
            TargetRotation = StartRotation + FRotator(0, -90.0f, 0);
            RotationSign = -1.0f;

            // [수정] BoxExtent.X -> -BoxExtent.X (입구 쪽 모서리)
            LocalPivotPoint = FVector(-BoxExtent.X, -BoxExtent.Y, 0.0f);
        }
    }
}

// BackGroundManager에서 배경 타일을 이동시킬 때 사용할 함수
void ABackGroundObjectBase::SetObjectLocation(const FVector& NewLocation)
{
	SetActorLocation(NewLocation);
}

bool ABackGroundObjectBase::IsSpawnLocationValid(const FVector& Candidate)
{
    // 금지 영역과의 충돌 검사
    for (UStaticMeshComponent* Zone : ExclusionZones)
    {
        // 금지 영역의 중심 점
        FVector ZoneLoc = Zone->GetComponentLocation();
        
        // 금지 영역의 범위/2
        FVector Extent = Zone 
        ? Zone->Bounds.BoxExtent : FVector::ZeroVector;  // X, Y, Z 절반 크기

        // X, Y 축별로 사각형 영역 내 여부 확인
        // 후보 위치와 금지구역의 중심점거리를 구해 금지구역의 반넓이보다 작으면 금지구역안에 있다는 의미
        if (FMath::Abs(Candidate.X - ZoneLoc.X) < Extent.X &&
            FMath::Abs(Candidate.Y - ZoneLoc.Y) < Extent.Y)
        {
            return false;  // 금지 영역과 겹침
        }
    }

    // 기존 스폰된 소품과의 거리 검사 (원거리 마진)
    for (const FVector& Other : SpawnedLocations)
    {
        if (FVector::Dist(Other, Candidate) < SpawnMargin)
        {
            return false;  // 다른 소품과 너무 가까움
        }
    }

    return true;  // 유효한 위치
}

void ABackGroundObjectBase::SpawnSmallBackGrounds()
{
    // 클래스 배열이 비어 있으면 종료
    if (SmallObjects.Num() == 0) return;

    // 타일 위치
    FTransform ParentTransform = RootComponent->GetComponentTransform();
    
    Origin = ParentTransform.GetLocation();

    // 스폰하고자 할 개수만큼 반복
    for (int32 i = 0; i < NumSpawn; ++i)
    {
        FVector Candidate; // 후보 지역 저장 변수
        bool bFound = false; // 적절한 위치 찾았는지 확인하는 변수

        // 최대 시도 횟수만큼 랜덤 위치 생성
        for (int32 Attempt = 0; Attempt < MaxSpawnAttempts; ++Attempt)
        {
            Candidate = Origin + FVector(
                FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
                FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
                0
            );

            if (IsSpawnLocationValid(Candidate))
            {
                bFound = true;
                break;
            }
        }

        // 유효한 위치 시 랜덤 클래스 선택 후 액터 스폰 및 위치 저장
        if (bFound)
        {
            // 입력받은 소품 종류 중 랜덤 1개 선택
            int32 Index = FMath::RandRange(0, SmallObjects.Num() - 1);
            TSubclassOf<AActor> ChosenClass = SmallObjects[Index];

            FActorSpawnParameters Params;
            AActor* Spawned = GetWorld()->SpawnActor<AActor>(ChosenClass, Candidate, FRotator::ZeroRotator, Params);

            if (Spawned)
            {
                SpawnedLocations.Add(Candidate);
                Spawned->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
            }
        }
    }
}

void ABackGroundObjectBase::RemoveSmallBackGrounds()
{
    // 자식 액터로 Attach된 소품들을 가져와 삭제
    TArray<AActor*> AttachedActors;
    GetAttachedActors(AttachedActors);

    for (AActor* Child : AttachedActors)
    {
        if (Child && Child != this)
        {
            Child->Destroy();
        }
    }

    // 위치 기록 초기화
    SpawnedLocations.Empty();
}