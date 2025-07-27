// 단순히 액터의 크기를 저장하는 함수


#include "BackGroundObjectBase.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

ABackGroundObjectBase::ABackGroundObjectBase()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ABackGroundObjectBase::BeginPlay()
{
	Super::BeginPlay();

	// 액터의 바운딩 박스(경계) 구해서 길이 측정
	 // false: Colider 없는 Mesh도 포함, Origin: 월드 좌표 기준, BoxExtent: 액터의 절반 길이가 해당 주소에 저장됨
    GetActorBounds(false, Origin, BoxExtent);

    // X방향(가로, 일반적으로 길이로 사용)의 실제 크기 저장(단위 cm)
    MeasuredLength = BoxExtent.X * 2.0f;

	// 소품 스폰 메서드 호출
    SpawnObjectsInAllAreas();
}

void ABackGroundObjectBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABackGroundObjectBase::SetObjectLocation(const FVector& NewLocation)
{
	SetActorLocation(NewLocation);
}

void ABackGroundObjectBase::SpawnObjectsInAllAreas()
{
    // 액터의 전체 영역(바운딩 박스): 중심 좌표와 절반 크기(Extent) 사용
    FVector Center = Origin;         // BoxExtent 중심(월드 좌표)
    FVector HalfExtents = BoxExtent; // 절반 길이(Extent); 실제 박스 크기는 2배

    // 소품 종류(SmallObjects 배열에 담긴 각각의 소품 블루프린트 클래스)를 순회
    for (int32 Index = 0; Index < SmallObjects.Num(); ++Index)
    {
        TSubclassOf<AActor> SmallObjectClass = SmallObjects[Index]; // 현재 소품 클래스
        if (!SmallObjectClass) 
		{
			UE_LOG(LogTemp, Warning, TEXT("이상한거 있다 좆만아"));
			continue; // 유효하지 않으면 skip
		}

        // 예비용 기본값: 소품 콜라이더 크기(겹침 검사에 사용, fallback값)
        FVector ObjectExtent(50.f, 50.f, 50.f);

        // DefaultObject에서 RootComponent(PrimComp)의 콜라이더 크기 자동 추출 시도
        AActor* DefaultActor = SmallObjectClass->GetDefaultObject<AActor>();

        if (DefaultActor)
        {
            UPrimitiveComponent* PrimComp = nullptr;

            // 루트 컴포넌트가 PrimitveComponent(콜라이더/메시 등)라면 활용
            if (DefaultActor->GetRootComponent() && DefaultActor->GetRootComponent()->IsA<UPrimitiveComponent>())
			{
                PrimComp = Cast<UPrimitiveComponent>(DefaultActor->GetRootComponent());
			}
			
			if (PrimComp)
			{
				FBoxSphereBounds Bounds = PrimComp->GetLocalBounds();
				ObjectExtent = Bounds.BoxExtent;
			}
        }

        int32 SpawnedCount = 0;  // 이번 소품(클래스)별로 스폰된 개수 확인
        int32 MaxAttempts = 10;  // 소품마다 랜덤 좌표 시도 최대 10회 제한

        // 지정된 개수(NumSpawn)만큼 랜덤 위치 스폰을 시도
        while (SpawnedCount < NumSpawn)
        {
            bool bSuccess = false;   // 적합한(겹치지 않는) 위치 찾았는지 플래그
            FVector SpawnLocation;   // 랜덤으로 추출한 소품 위치

            // MaxAttempts(10회) 이내에서 랜덤한 위치 추출과 중첩 체크 반복
            for (int32 Try = 0; Try < MaxAttempts; ++Try)
            {
                // 전체 영역 내 임의 위치 추출 (Center 중심, HalfExtents 안)
                SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(Center, HalfExtents);

				// Z축 값을 고정(0으로)
				SpawnLocation.Z = BoxExtent.Z + 108.f;

                // 해당 위치에 소품 크기(ObjectExtent) 박스가 월드의 Static 오브젝트와 겹치는지 확인(Overlap)
                bool bOverlap = GetWorld()->OverlapBlockingTestByChannel(
                    SpawnLocation, FQuat::Identity, ECC_WorldStatic,
                    FCollisionShape::MakeBox(ObjectExtent)
                );

				if (bOverlap) // 겹치지 않으면 위치로 채택
                { 
					// OverlapMultiByChannel로 실제 겹치는 액터를 배열로 받아오기
					TArray<FOverlapResult> Overlaps;
					FCollisionQueryParams Params;
					Params.AddIgnoredActor(this); // 자신의 영역 액터는 검사 대상에서 제외

					bool bHasOverlap = GetWorld()->OverlapMultiByChannel(
						Overlaps,
						SpawnLocation,
						FQuat::Identity,
						ECC_WorldStatic,
						FCollisionShape::MakeBox(ObjectExtent),
						Params
					);

					if (bHasOverlap && Overlaps.Num() > 0)
					{
						for (const FOverlapResult& Result : Overlaps)
						{
							const AActor* OverlapActor = Result.GetActor();
							if (OverlapActor)
							{
								UE_LOG(LogTemp, Warning, TEXT("겹치는 오브젝트: %s"), *OverlapActor->GetName());
								DrawDebugBox(GetWorld(), SpawnLocation, ObjectExtent, FColor::Red, false, 10.0f);
							}
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("겹침 발생: 하지만 액터 레벨에서는 못 찾음 (ex. 컴포넌트 단위, 월드 콜리전 등)"));
					}
					
				}

                if (!bOverlap) // 겹치지 않으면 위치로 채택
                { 
					bSuccess = true; break; 
				}
            }

            if (bSuccess)
            {
                // 겹치지 않는 경우 실제 소품(액터) 생성(스폰)
                FActorSpawnParameters Params;
                // 혹시라도 충돌 발생 시 스폰 자체를 자동 skip하도록 설정
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;


                // 소품 액터 스폰 및 반환 AActor* 받기
                AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(SmallObjectClass, SpawnLocation, FRotator::ZeroRotator, Params);
                
				if (SpawnedActor)
                {
                    // 부모(BackGroundObjectBase)의 루트 컴포넌트에 Attach
                    SpawnedActor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
                }

                SpawnedCount++;
            }
            else break; // 10번 시도 내 적합 위치 못 찾으면 해당 소품은 남은 개수 스킵
        }
    }
}
