// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TrainCharacter.generated.h"

UCLASS()
class TRAIN_ROG_PROJ_API ATrainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATrainCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// Input Mapping Context 에셋을 에디터에서 할당 가능하게 선언
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	// 이동 Input Action 에셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;


	// 이동 처리 함수 (Input Action에서 호출)
	void Move(const FInputActionValue& Value);
	// 3인칭 카메라를 위해 선언
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	bool bIsZooming;

	// 점프·웅크리기 액션 변수 선언
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* CrouchAction;

	// 원래 이동 속도 저장용
	float NormalWalkSpeed;

	// 웅크리기 시 카메라 이동
	FVector DefaultCameraOffset;
	FVector CrouchCameraOffset;

	// 함수 선언
	void StartJump();
	void StopJump();
	void StartCrouch();
	void StopCrouch();

	// 줌 액션 변수 선언
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* Zoom;

	// 줌 처리 함수 (Input Action에서 호출)
	void ZoomIn();
	void ZoomOut();
	void StartZoom();
	void StopZoom();

	// 카메라 줌을 위한 스프링 암 길이
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float CameraBaseLength;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float CameraZoomLengh;

	// Look 액션 변수 선언
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	void Look(const FInputActionValue& Value);



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
