// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Delegates/DelegateCombinations.h"
#include "BackGroundObjectBase.generated.h"

// 타일 종류 구분용 열거형
UENUM(BlueprintType)
enum class ETileCategory : uint8
{
    TileDefault UMETA(DisplayName="Default"),
    TileRight   UMETA(DisplayName="Right"),
    TileLeft    UMETA(DisplayName="Left"),
};

// ChangeDirection 이벤트 델리게이트 선언 (Object 자신을 인자로 던짐)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnChangeDirectionEvent, 
    ABackGroundObjectBase*, 
    Source
);

UCLASS()
class TRAIN_ROG_PROJ_API ABackGroundObjectBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ABackGroundObjectBase();

	// 블루프린트/Manager에서 바인딩할 수 있게 공개
    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnChangeDirectionEvent OnChangeDirection;
	
	// 필요 시, 이동/재사용 관련 메서드 선언
    UFUNCTION(BlueprintCallable) // UFUNCTION: 블루프린트에서 SetObjectLocation 함수를 노드로 사용할 수 있게 함
	void SetObjectLocation(const FVector& NewLocation);

	// 타일 길이 저장용 변수
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Tile")
    float MeasuredXLength = 0.f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Tile")
    float MeasuredYLength = 0.f;

	// 몇개정도 스폰 예정?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SmallObjects")
	int32 NumSpawn = 10;

	// 스폰할 소품 종류 리스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SmallObjects")
	TArray<TSubclassOf<AActor>> SmallObjects;

	// 배치 금지 영역을 정의하는 씬 컴포넌트 배열
    UPROPERTY(VisibleAnywhere, Category = "Setup")
    TArray<UStaticMeshComponent*> ExclusionZones;

	// 소품 간 및 금지 영역과의 최소 거리 (마진)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (ClampMin = "0.0"))
    float SpawnMargin = 50.0f;

	// 최대 위치 탐색 시도 횟수
    UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "1"))
    int32 MaxSpawnAttempts = 20;

	// BP에서 이 프로토타입이 어느 카테고리인가 지정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
    ETileCategory TileCategory;
	
	ETileCategory Direction;

protected:
	virtual void BeginPlay() override;

	FVector Origin, BoxExtent;

	// Blueprint에서 추가된 Box Collision 컴포넌트를 찾아 여기에 담습니다
    UPROPERTY()
    UBoxComponent* ChangeDirTrigger;

    // Overlap 이벤트 핸들러
    UFUNCTION()
    void OnChangeDirOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    // 부드럽게 회전시킬 대상
    AActor* ActorToRotate = nullptr;

    // 기차 회전 애니메이션 상태
    bool bIsRotating = false;
    FRotator StartRotation;
    FRotator TargetRotation;
    float RotationTime = 1.0f;    // 회전에 걸릴 총 시간(초)
    float ElapsedTime = 0.0f;

    // 기차 회전 관련 추가 변수들
    FVector LocalPivotPoint; // 회전의 중심점 (타일 로컬 좌표)
    float RotationRadius = 0.0f; // 회전 반경
    float RotationSign = 0.0f; // 회전 방향 (1이 우회전이고 -1이 좌회전임)


public:	
	virtual void Tick(float DeltaTime) override;

	// 이미 스폰된 소품 위치 저장용 배열
    TArray<FVector> SpawnedLocations;

	// 소품을 스폰해도 되는 위치인지 확인하는 함수
	bool IsSpawnLocationValid(const FVector& Candidate);

	// 소품을 스폰하는 함수
	void SpawnSmallBackGrounds();

	void RemoveSmallBackGrounds();

	void SetTrigerBox();
};
