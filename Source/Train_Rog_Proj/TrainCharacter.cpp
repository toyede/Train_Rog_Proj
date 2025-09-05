<<<<<<< HEAD
=======
// Fill out your copyright notice in the Description page of Project Settings.

#include "TrainCharacter.h"
>>>>>>> Develop
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
// Sets default values
ATrainCharacter::ATrainCharacter()
{
    // ÀÌ Ä³¸¯ÅÍ°¡ ¸Å ÇÁ·¹ÀÓ¸¶´Ù Tick()À» È£ÃâÇÏµµ·Ï ¼³Á¤ÇÕ´Ï´Ù. ÇÊ¿ä ¾ø´Ù¸é ¼º´ÉÀ» À§ÇØ ²¨µµ µË´Ï´Ù.
    PrimaryActorTick.bCanEverTick = true;

    // Spring Arm(Ä«¸Þ¶ó ºÕ) ÄÄÆ÷³ÍÆ® »ý¼º ¹× ·çÆ®¿¡ ºÎÂø
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);

    // Ä«¸Þ¶ó °Å¸®(ÆÈ ±æÀÌ) ¼³Á¤
    CameraBoom->TargetArmLength = 300.0f; // Ä«¸Þ¶ó¿Í Ä³¸¯ÅÍ »çÀÌ °Å¸® ¼³Á¤
    CameraBaseLength = CameraBoom->TargetArmLength; // ±âº» ±æÀÌ ÀúÀå

    // ÄÁÆ®·Ñ·¯ È¸Àü »ç¿ë ¼³Á¤
    //CameraBoom->bUsePawnControlRotation = true; // ÄÁÆ®·Ñ·¯ È¸Àü¿¡ µû¶ó ºÕ È¸Àü (¿©±â¼± ²¨µÒ)
    CameraBoom->bUsePawnControlRotation = false; // ºÕ ÀÚÃ¼°¡ ÄÁÆ®·Ñ·¯ È¸Àü ¾È µû¸§
    CameraBoom->bInheritPitch = true;
    CameraBoom->bInheritYaw = true;
    CameraBoom->bInheritRoll = true;

    // Ä«¸Þ¶ó ÄÄÆ÷³ÍÆ® »ý¼º, Spring Arm¿¡ ºÎÂø
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

    // Ä«¸Þ¶ó°¡ ÄÁÆ®·Ñ·¯ È¸ÀüÀ» µû¸£Áö ¾Ê°Ô ¼³Á¤
    FollowCamera->bUsePawnControlRotation = false;

    // ±âº» °È±â ¼Óµµ µî ÃÊ±â°ª ÀúÀå
    NormalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

    // Ä«¸Þ¶ó ¿ÀÇÁ¼Â ÀúÀå ¹× ¿õÅ©¸± ¶§ ¿ÀÇÁ¼Â ¼³Á¤
    DefaultCameraOffset = CameraBoom->TargetOffset;
    CrouchCameraOffset = DefaultCameraOffset + FVector(0.f, 0.f, -30.f);

    // ÄÁÆ®·Ñ·¯ È¸Àü »ç¿ë ¼³Á¤ (Pitch, Yaw »ç¿ë, RollÀº »ç¿ëÇÏÁö ¾ÊÀ½)
    bUseControllerRotationPitch = true;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;  // ·ÑÀº ²¨µÒ

    // ¿òÁ÷ÀÓ ¹æÇâ¿¡ µû¶ó Ä³¸¯ÅÍ È¸Àü ¾È ÇÏµµ·Ï ¼³Á¤
    GetCharacterMovement()->bOrientRotationToMovement = false;

    // Ä³¸¯ÅÍ È¸Àü ¼Óµµ ¼³Á¤
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);  // Yaw¸¸ ºü¸£°Ô È¸Àü
}

// Called when the game starts or when spawned
void ATrainCharacter::BeginPlay()
{
    Super::BeginPlay();

    // ÇÃ·¹ÀÌ¾î ÄÁÆ®·Ñ·¯°¡ ÀÖÀ¸¸é ÀÔ·Â ½Ã½ºÅÛ ¼³Á¤
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        // Enhanced Input Subsystem °¡Á®¿È (ÀÔ·Â ¸ÅÇÎ¿ë)
        if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
        {
            UEnhancedInputLocalPlayerSubsystem* Subsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
            // ÀÔ·Â ¸ÅÇÎ ÄÁÅØ½ºÆ®¸¦ ¼­ºê½Ã½ºÅÛ¿¡ µî·Ï
            if (Subsystem && DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0); // ¿ì¼±¼øÀ§ 0À¸·Î µî·Ï
            }
        }
    }
    if (Controller)
    {
        // ÄÁÆ®·Ñ·¯ ÃÊ±â È¸Àü ¼³Á¤ (Pitch, Yaw, Roll ¸ðµÎ 0)
        Controller->SetControlRotation(FRotator(0.f, 0.f, 0.f));
    }
}

// Called every frame
void ATrainCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATrainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // EnhancedInputComponent·Î Ä³½ºÆÃ ÈÄ, °¢ ¾×¼ÇÀ» ¹ÙÀÎµù
    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (EnhancedInputComponent && MoveAction)
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATrainCharacter::Move); // ÀÌµ¿
    }
    if (JumpAction)
    {
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATrainCharacter::StartJump); // Á¡ÇÁ ½ÃÀÛ
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ATrainCharacter::StopJump); // Á¡ÇÁ Á¾·á
    }
    if (CrouchAction)
    {
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ATrainCharacter::StartCrouch); // ¿õÅ©¸®±â ½ÃÀÛ
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ATrainCharacter::StopCrouch); // ¿õÅ©¸®±â ³¡
    }
    if (Zoom)
    {
        EnhancedInputComponent->BindAction(Zoom, ETriggerEvent::Started, this, &ATrainCharacter::StartZoom); // ÁÜÀÎ
        EnhancedInputComponent->BindAction(Zoom, ETriggerEvent::Completed, this, &ATrainCharacter::StopZoom); // ÁÜ¾Æ¿ô
    }
    if (LookAction)
    {
<<<<<<< HEAD
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATrainCharacter::Look); // ¸¶¿ì½º ÀÌµ¿/È¸Àü
    }
=======
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATrainCharacter::Look);
	}

    // ê¹€ìž¬í›ˆ ì¶”ê°€
    if (InteractAction)
    {
        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ATrainCharacter::Interact);
	}

>>>>>>> Develop
    if (RunAction)
    {
        EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &ATrainCharacter::StartRun); // ´Þ¸®±â ½ÃÀÛ
        EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ATrainCharacter::StopRun); // ´Þ¸®±â ³¡
    }
}

// ÀÌµ¿ Ã³¸® ÇÔ¼ö
void ATrainCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    // ÀÔ·Â °ªÀÌ °ÅÀÇ ¾ø°Å³ª ÄÁÆ®·Ñ·¯°¡ ¾øÀ¸¸é ¸®ÅÏ
    if (Controller == nullptr || MovementVector.IsNearlyZero())
        return;

    // Ä«¸Þ¶ó(ÄÁÆ®·Ñ·¯) È¸Àü Á¤º¸ ¾ò±â
    FRotator CameraRotation = Controller->GetControlRotation();
    // Yaw(ÁÂ¿ì)¸¸ »ç¿ë
    FRotator YawRotation(0.f, CameraRotation.Yaw, 0.f);

    // Forward, Right ¹æÇâ º¤ÅÍ °è»ê
    FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // ÀÔ·Â¿¡ µû¶ó ÃÖÁ¾ ÀÌµ¿ ¹æÇâ °è»ê ¹× Á¤±ÔÈ­
    FVector MoveDirection = ForwardDir * MovementVector.Y + RightDir * MovementVector.X;
    MoveDirection.Normalize();

    // Ä³¸¯ÅÍ ÀÌµ¿
    AddMovementInput(MoveDirection, 1.0f);

    // ÀÌµ¿ ¹æÇâ¿¡ µû¶ó ºÎµå·´°Ô Ä³¸¯ÅÍ È¸Àü
    FRotator TargetRotation = MoveDirection.Rotation();
    FRotator CurrentRotation = GetActorRotation();
    SetActorRotation(FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 10.f));
}

// Á¡ÇÁ ½ÃÀÛ
void ATrainCharacter::StartJump()
{
    Jump();
}

// Á¡ÇÁ ³¡
void ATrainCharacter::StopJump()
{
    StopJumping();
}

// ¿õÅ©¸®±â ½ÃÀÛ
void ATrainCharacter::StartCrouch()
{
    Crouch(); // ¿õÅ©¸®±â

    // ¿õÅ©¸± ¶§ ¼Óµµ °¨¼Ò
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed * 0.5f;

    // Ä«¸Þ¶ó ¿ÀÇÁ¼Â º¯°æ
    if (CameraBoom)
    {
        CameraBoom->TargetOffset = CrouchCameraOffset;
    }
}

// ¿õÅ©¸®±â ³¡
void ATrainCharacter::StopCrouch()
{
    UnCrouch(); // ¿õÅ©¸®±â Á¾·á

    // ¼Óµµ ¿ø·¡´ë·Î º¹¿ø
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;

    // Ä«¸Þ¶ó ¿ÀÇÁ¼Â º¹¿ø
    if (CameraBoom)
    {
        CameraBoom->TargetOffset = DefaultCameraOffset;
    }
}

// ÁÜ ÀÎ(Ä«¸Þ¶ó °Å¸® ´õ °¡±î¿ò)
void ATrainCharacter::ZoomIn()
{
    CameraBaseLength = CameraBoom->TargetArmLength; // ÇöÀç °Å¸® ÀúÀå
    CameraBoom->TargetArmLength = -100.0f; // ´õ °¡±î¿î °Å¸®·Î º¯°æ
    bIsZooming = true;
}

// ÁÜ ¾Æ¿ô(Ä«¸Þ¶ó °Å¸® º¹¿ø)
void ATrainCharacter::ZoomOut()
{
    CameraBoom->TargetArmLength = CameraBaseLength;
    bIsZooming = false;
}

// ÁÜ ½ÃÀÛ
void ATrainCharacter::StartZoom()
{
    ZoomIn();
}

// ÁÜ ³¡
void ATrainCharacter::StopZoom()
{
    ZoomOut();
}

// ¸¶¿ì½º µî Ä«¸Þ¶ó È¸Àü Ã³¸®
void ATrainCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxis = Value.Get<FVector2D>();
    float YawSensitivity = 1.0f;
    float PitchSensitivity = 1.0f;

    // ÁÂ¿ì(Yaw) ¹æÇâ È¸Àü Ã³¸®
    AddControllerYawInput(LookAxis.X * YawSensitivity);

    // À§¾Æ·¡(Pitch) ¹æÇâ È¸Àü Ã³¸®
    FRotator ControlRot = Controller->GetControlRotation();
    float NewPitch = ControlRot.Pitch + LookAxis.Y * PitchSensitivity * -1.0f;

    // Pitch °¢µµ Á¦ÇÑ (-45~45µµ)
    NewPitch = FMath::ClampAngle(NewPitch, -45.0f, 45.0f);

    // º¯°æµÈ Pitch ¹Ý¿µ
    ControlRot.Pitch = NewPitch;
    Controller->SetControlRotation(ControlRot);
}

// ê¹€ìž¬í›ˆ ì¶”ê°€
void ATrainCharacter::Interact(const FInputActionValue& Value)
{
    if (Controller == nullptr || FollowCamera == nullptr)
	{
		return;
	}

	// ë¼ì¸ íŠ¸ë ˆì´ìŠ¤ ì‹œìž‘ì ê³¼ ëì  ê³„ì‚°
	FVector StartLocation = FollowCamera->GetComponentLocation();
	FVector ForwardVector = FollowCamera->GetForwardVector();
	FVector EndLocation = StartLocation + (ForwardVector * TraceDistance);

	// ì¶©ëŒ ì²´í¬ìš© ë³€ìˆ˜ ì„¸íŒ…
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // ìžê¸° ìžì‹ ì€ íŠ¸ë ˆì´ìŠ¤ì—ì„œ ì œì™¸

	// ë¼ì¸ íŠ¸ë ˆì´ìŠ¤ ì‹¤í–‰
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility, // Visibility ì±„ë„ì— ìžˆëŠ” ì˜¤ë¸Œì íŠ¸ë§Œ ê°ì§€
		CollisionParams
	);

	DrawDebugLine(GetWorld(), StartLocation, EndLocation, bHit ? FColor::Green : FColor::Red, false, 1.0f, 0, 1.0f);

	// ë¬´ì–¸ê°€ì— ë¶€ë”ªí˜”ëŠ”ì§€ í™•ì¸
	if (bHit)
	{
		// ë¶€ë”ªížŒ ì•¡í„° ê°€ì ¸ì˜¤ê¸°
		AActor* HitActor = HitResult.GetActor();

		// ë¶€ë”ªížŒ ì•¡í„°ê°€ InteractInterfaceë¥¼ êµ¬í˜„í–ˆëŠ”ì§€ í™•ì¸
		if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
		{
            // ì²«ë²ˆì§¸ ì¸ìž(HitActor)ëŠ” Interactí•¨ìˆ˜ë¥¼ ì‹¤í–‰í•  ê°ì²´, thisëŠ” Interactí•¨ìˆ˜ì—ì„œ ì¸ìžë¡œ ë°›ì„ ê°ì²´.
			IInteractInterface::Execute_Interact(HitActor, this);
            // UE_LOG(LogTemp, Warning, TEXT("Debug"));
		}
	}
}

void ATrainCharacter::StartRun()
{
    // ï¿½Þ¸ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    bIsRunning = true;
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed * RunSpeedMultiplier;
}

void ATrainCharacter::StopRun()
{
    // ï¿½Þ¸ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    bIsRunning = false;
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;
}

void ATrainCharacter::RemapKey(UInputAction* InputAction, FKey OldKey, FKey NewKey)
{
    if (!DefaultMappingContext || !InputAction) return;

    // 1. ï¿½ï¿½ï¿½ï¿½ Å° ï¿½ï¿½ï¿½
    DefaultMappingContext->UnmapKey(InputAction, OldKey);

    // 2. ï¿½ï¿½ Å° ï¿½ï¿½ï¿½ï¿½
    DefaultMappingContext->MapKey(InputAction, NewKey);

    // 3. ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->RemoveMappingContext(DefaultMappingContext);
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
<<<<<<< HEAD
}

int ATrainCharacter::IncreaseHP()
{
	HP += HPStack * 2;
    return HP;
}

int ATrainCharacter::IncreasePower()
{
    Power += PowerStack * 2;
    return Power;
}

int ATrainCharacter::IncreaseDefense()
{
    Defense += DefenseStack * 2;
    return Defense;
}

float ATrainCharacter::IncreaseAttackSpeed()
{
    AttackSpeed += AttackSpeedStack * 0.02f;
    return AttackSpeed;
}

float ATrainCharacter::IncreaseCriticalChance()
{
    CriticalChance += CriticalChanceStack * 0.01f;
    return CriticalChance;
}

float ATrainCharacter::IncreaseCriticalDamage()
{
    CriticalDamage += CriticalDamageStack * 0.05f;
    return CriticalDamage;
=======
>>>>>>> Develop
}