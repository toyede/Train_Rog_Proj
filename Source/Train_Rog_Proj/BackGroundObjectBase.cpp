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

void ABackGroundObjectBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsRotating || !ActorToRotate) 
    {
        return;
    }

    // 경과 시간 누적
    ElapsedTime += DeltaTime;
    float Alpha = FMath::Clamp(ElapsedTime / RotationTime, 0.0f, 1.0f);

    // 선형 보간으로 회전값 계산
    FRotator NewRot = FMath::Lerp(StartRotation, TargetRotation, Alpha);
    ActorToRotate->SetActorRotation(NewRot);

    // 완료 시 상태 정리
    if (Alpha >= 1.0f)
    {
        bIsRotating   = false;
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

void ABackGroundObjectBase::OnChangeDirOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    // 예: 기차 액터만 필터링
    if (OtherActor && OtherActor != this && OtherComp->ComponentHasTag(FName("TrainCollision")))
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseCpp충돌알림"));
        // Delegate 를 통해 Manager 등 외부에 알린다
        
        OnChangeDirection.Broadcast(this);

        // 이미 회전 중이면 무시
        if (bIsRotating)
        {
            return;
        }

        ActorToRotate = OtherActor;
        StartRotation = OtherActor->GetActorRotation();
        ElapsedTime     = .0f;

        if(TileCategory == ETileCategory::TileRight)
        {
            bIsRotating     = true;
            TargetRotation  = StartRotation + FRotator(0, 90, 0);  // Yaw +90도
        }

        else if(TileCategory == ETileCategory::TileLeft)
        {
            bIsRotating     = true;
            TargetRotation  = StartRotation + FRotator(0, -90, 0);  // Yaw +90도
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