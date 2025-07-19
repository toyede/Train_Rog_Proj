// Fill out your copyright notice in the Description page of Project Settings.


#include "BackGroundObjectBase.h"

ABackGroundObjectBase::ABackGroundObjectBase()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ABackGroundObjectBase::BeginPlay()
{
	Super::BeginPlay();

	// 액터의 바운딩 박스(경계) 구해서 길이 측정
    GetActorBounds(true, Origin, BoxExtent); // true: 전체 컴포넌트 포함

    // X방향(가로, 일반적으로 길이로 사용)의 실제 크기 저장(단위 cm)
    MeasuredLength = BoxExtent.X * 2.0f;
}

void ABackGroundObjectBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABackGroundObjectBase::SetObjectLocation(const FVector& NewLocation)
{
	SetActorLocation(NewLocation);
}