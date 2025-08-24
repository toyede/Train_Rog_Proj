// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
// 김재훈 추가
#include "Dialogue/InteractInterface.h"
#include "DrawDebugHelpers.h"
// 김재훈 추가
#include "TrainCharacter.generated.h"

UCLASS()
class TRAIN_ROG_PROJ_API ATrainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATrainCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// Input Mapping Context ������ �����Ϳ��� �Ҵ� �����ϰ� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	// �̵� Input Action ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;


	// �̵� ó�� �Լ� (Input Action���� ȣ��)
	void Move(const FInputActionValue& Value);
	// 3��Ī ī�޶� ���� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	bool bIsZooming;

	// ��������ũ���� �׼� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction;

	// ���� �̵� �ӵ� �����
	float NormalWalkSpeed;

	// ��ũ���� �� ī�޶� �̵�
	FVector DefaultCameraOffset;
	FVector CrouchCameraOffset;

	// �Լ� ����
	void StartJump();
	void StopJump();
	void StartCrouch();
	void StopCrouch();

	// �� �׼� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* Zoom;

	// �� ó�� �Լ� (Input Action���� ȣ��)
	void ZoomIn();
	void ZoomOut();
	void StartZoom();
	void StopZoom();

	// ī�޶� ���� ���� ������ �� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float CameraBaseLength;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float CameraZoomLengh;

	// Look �׼� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	void Look(const FInputActionValue& Value);

	public:	
	// �޸��� �׼� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* RunAction;

	bool bIsRunning;
	float RunSpeedMultiplier = 1.5f;

	void StartRun();
	void StopRun();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void RemapKey(UInputAction* InputAction, FKey OldKey, FKey NewKey);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// 김재훈 추가
	void Interact(const FInputActionValue& Value);

	// 상호작용 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float TraceDistance = 500.0f; // 기본값을 500으로 설정
	
};
