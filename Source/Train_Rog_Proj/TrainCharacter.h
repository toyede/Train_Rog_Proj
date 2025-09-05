#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Dialogue/InteractInterface.h"
#include "DrawDebugHelpers.h"
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

	// Input Mapping Context ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Í¿ï¿½ï¿½ï¿½ ï¿½Ò´ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï°ï¿½ ï¿½ï¿½ï¿½ï¿½
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	// ï¿½Ìµï¿½ Input Action ï¿½ï¿½ï¿½ï¿½
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;


	// ï¿½Ìµï¿½ Ã³ï¿½ï¿½ ï¿½Ô¼ï¿½ (Input Actionï¿½ï¿½ï¿½ï¿½ È£ï¿½ï¿½)
	void Move(const FInputActionValue& Value);
	// 3ï¿½ï¿½Äª Ä«ï¿½Ş¶ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	bool bIsZooming;

	// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Å©ï¿½ï¿½ï¿½ï¿½ ï¿½×¼ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction;

	// ï¿½ï¿½ï¿½ï¿½ ï¿½Ìµï¿½ ï¿½Óµï¿½ ï¿½ï¿½ï¿½ï¿½ï¿?
	float NormalWalkSpeed;

	// ï¿½ï¿½Å©ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ Ä«ï¿½Ş¶ï¿½ ï¿½Ìµï¿½
	FVector DefaultCameraOffset;
	FVector CrouchCameraOffset;

	// ï¿½Ô¼ï¿½ ï¿½ï¿½ï¿½ï¿½
	void StartJump();
	void StopJump();
	void StartCrouch();
	void StopCrouch();

	// ï¿½ï¿½ ï¿½×¼ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* Zoom;

	// ï¿½ï¿½ Ã³ï¿½ï¿½ ï¿½Ô¼ï¿½ (Input Actionï¿½ï¿½ï¿½ï¿½ È£ï¿½ï¿½)
	void ZoomIn();
	void ZoomOut();
	void StartZoom();
	void StopZoom();

	// Ä«ï¿½Ş¶ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float CameraBaseLength;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float CameraZoomLengh;

	// Look ï¿½×¼ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	void Look(const FInputActionValue& Value);

	public:	
	// ï¿½Ş¸ï¿½ï¿½ï¿½ ï¿½×¼ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
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
	
	// ê¹€?¬í›ˆ ì¶”ê?
	void Interact(const FInputActionValue& Value);

	//´É·ÂÄ¡
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

	// ½ºÅÃ ¼öÄ¡
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

	// ½ºÅİ ºĞ¹è Æ÷ÀÎÆ®
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats Stacks")
	int StatPoint;

	//´É·ÂÄ¡ Áõ°¡ ÇÔ¼ö
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


	// ·¹º§¾÷ ÇÔ¼ö
	UFUNCTION(BlueprintCallable, Category = "Player Stats Functions")
	void LevelUp();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float TraceDistance = 500.0f; // ê¸°ë³¸ê°’ì„ 500?¼ë¡œ ?¤ì •

	
};
