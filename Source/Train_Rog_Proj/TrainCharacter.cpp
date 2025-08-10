// Fill out your copyright notice in the Description page of Project Settings.

#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TrainCharacter.h"

// Sets default values
ATrainCharacter::ATrainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Spring Arm ???? ?? ????
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f; // ?????? ???? ??? ????
	CameraBaseLength = CameraBoom->TargetArmLength; // ?? ???? ???? ????
    //CameraBoom->bUsePawnControlRotation = true; // ?????? ???????? ???
    CameraBoom->bUsePawnControlRotation = false; // ?????? ??? ????
    CameraBoom->bInheritPitch = true;
    CameraBoom->bInheritYaw = true;
    CameraBoom->bInheritRoll = true;

    // Camera ???? ?? Spring Arm?? ????
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false; // ????? ???? ???X

    // ?? ??? ???
    NormalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

    // ??????? ?? ???? ???
    DefaultCameraOffset = CameraBoom->TargetOffset;
    CrouchCameraOffset = DefaultCameraOffset + FVector(0.f, 0.f, -30.f);

	// ????? ??? ????
    bUseControllerRotationPitch = true;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;  // ????, ???? ????? ?????(??) ????? ???? X
    GetCharacterMovement()->bOrientRotationToMovement = false;  // ?????? ?? ???????? ???? ???
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);  // ??? ???(???? ????)
}

// Called when the game starts or when spawned
void ATrainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    // ?÷???? ?????? ????????
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        // Enhanced Input Subsystem ???????? (???? ?÷???? ????)
        if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
        {
            UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

            // Mapping Context?? ???¿? ????? ???? ???
            if (Subsystem && DefaultMappingContext)
            {
                // 0?? ?? ?????? (????? ???????? ??)
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
    if (Controller)
    {
        // ???????? ??????? ??? (Pitch: 0, Yaw: 0, Roll: 0)???? ????????.
        // ????? ??? ???? ???? ?? ???? ?????? ???? ????.
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
    // Enhanced Input Component?? ?????
    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (EnhancedInputComponent && MoveAction)
    {
        // ??? ??? ?????
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATrainCharacter::Move);

	}

    // ???? ??? ?????
    if (JumpAction)
    {
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATrainCharacter::StartJump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ATrainCharacter::StopJump);
    }
    // ??????? ??? ?????
    if (CrouchAction)
    {
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ATrainCharacter::StartCrouch);
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ATrainCharacter::StopCrouch);
	}
    // ?? ??? ?????
    if (Zoom)
    {
        EnhancedInputComponent->BindAction(Zoom, ETriggerEvent::Started, this, &ATrainCharacter::StartZoom);
        EnhancedInputComponent->BindAction(Zoom, ETriggerEvent::Completed, this, &ATrainCharacter::StopZoom);
    }

	// Look ??? ?????
    if (LookAction)
    {
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATrainCharacter::Look);
	}
}

void ATrainCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    if (Controller == nullptr || MovementVector.IsNearlyZero())
        return;

    // ???? ???? ??? ????????
    FRotator CameraRotation = Controller->GetControlRotation();
    FRotator YawRotation(0.f, CameraRotation.Yaw, 0.f); // Pitch, Roll ????

    // Forward / Right ???? ???
    FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // ???? ??? ????
    FVector MoveDirection = ForwardDir * MovementVector.Y + RightDir * MovementVector.X;
    MoveDirection.Normalize();

    // ???
    AddMovementInput(MoveDirection, 1.0f);

    // ?????? ??? ???????? ???
    FRotator TargetRotation = MoveDirection.Rotation();
    FRotator CurrentRotation = GetActorRotation();

    // ????? ??? (???? ??????? ???? ????)
    SetActorRotation(FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 10.f));
}

void ATrainCharacter::StartJump()
{
    Jump(); // ???? ???? ??? ???
}

void ATrainCharacter::StopJump()
{
    StopJumping(); // ???? ????
}

void ATrainCharacter::StartCrouch()
{
    Crouch();

    // ??? ????
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed * 0.5f;

    // ???? ???????
    if (CameraBoom)
    {
        CameraBoom->TargetOffset = CrouchCameraOffset;
    }
}

void ATrainCharacter::StopCrouch()
{
    UnCrouch();

    // ??? ????
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;

    // ???? ???????
    if (CameraBoom)
    {
        CameraBoom->TargetOffset = DefaultCameraOffset;
    }
}


void ATrainCharacter::ZoomIn()
{
    // ???? ?? ??
	CameraBaseLength = CameraBoom->TargetArmLength;
	CameraBoom->TargetArmLength = -100.0f; // ????? ?? ?? ????? ????
    bIsZooming = true;
}
void ATrainCharacter::ZoomOut()
{
    // ???? ?? ???
    CameraBoom->TargetArmLength = CameraBaseLength;
    bIsZooming = false;
}
void ATrainCharacter::StartZoom()
{
    // ?? ??
    ZoomIn();
}
void ATrainCharacter::StopZoom()
{
    // ?? ???
    ZoomOut();
}

void ATrainCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxis = Value.Get<FVector2D>();

    float YawSensitivity = 1.0f;
    float PitchSensitivity = 1.0f;

    // Yaw ????? ???? ????
    AddControllerYawInput(LookAxis.X * YawSensitivity);

    // ???? ?????? ?????
    FRotator ControlRot = Controller->GetControlRotation();

    // ????? Pitch ???
    float NewPitch = ControlRot.Pitch + LookAxis.Y * PitchSensitivity * -1.0f;

    // ???? Pitch?? 360?? ??????? 270?? ????? ?????????? ?????
    // ???? ClampAngle ??? (??? ???? ???? ????)
    NewPitch = FMath::ClampAngle(NewPitch, -45.0f, 45.0f);

    // ?????? ????? ????
    ControlRot.Pitch = NewPitch;
    Controller->SetControlRotation(ControlRot);
}

