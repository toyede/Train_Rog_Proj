// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

//참고사항
/*- **몬스터의 경우 지상, 공중이 존재**
	- 지상 몬스터의 경우 체력 40, 공중 몬스터의 경우 체력 30를 보유 (확정 X)
- **스폰 조건의 경우** 기차가 달리고 있는 타일에서 기차에서 일정 거리 떨어진 곳과 **배경 오브젝트가 사이에서 랜덤 생성(일정거리는 알아서 판단…)**
	- **근거리 몬스터가 나오자마자 기차를 공격하지 못하기 위해**
- **몬스터가 나올 수 있는 방향은 총 4개로 좌, 우, 뒤, 공중입니다.**
	- **한번에 생성될 수 있는 방향은 총 2가지**로 좌/우, 좌/뒤, 좌/공중, 우/뒤, 우/공중, 뒤/공중 이렇게 총 6가지입니다.
- **30초까지는 5/13/21초에 몬스터의 개수가 3~5마리, 31초부터 1분까지는 30/38/45초에 5~7마리 랜덤 스폰***/

UCLASS()
class TRAIN_ROG_PROJ_API AEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Setting")
	float Wave;

	UPROPERTY(EditAnywhere, Category = "Setting")
	float AmountPerPoint;

	UFUNCTION(BlueprintCallable)
	void SpawnMonsters();
	
	
	

};
