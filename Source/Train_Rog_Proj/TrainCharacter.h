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
	ATrainCharacter();

protected:
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

	// 달리기 액션 변수 선언
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

	//능력치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float MaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Power;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Defense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float AttackSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float CriticalChance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float CriticalDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int EXP;

	// 스택 수치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats Stacks")
	int HPStack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats Stacks")
	int PowerStack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats Stacks")
	int DefenseStack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats Stacks")
	float AttackSpeedStack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats Stacks")
	float CriticalChanceStack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats Stacks")
	float CriticalDamageStack;

	// 스텟 분배 포인트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats Stacks")
	int StatPoint;

	//능력치 증가 함수
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions")
	int IncreaseHP();
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions")
	int IncreasePower();
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions")
	int IncreaseDefense();
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions")
	float IncreaseAttackSpeed();
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions")
	float IncreaseCriticalChance();
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions")
	float IncreaseCriticalDamage();


	// 레벨업 함수
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions")
	void LevelUp();

	
};
