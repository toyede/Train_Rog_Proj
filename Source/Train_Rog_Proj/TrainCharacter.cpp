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

    // Spring Arm ���� �� ����
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f; // ĳ���Ϳ� ī�޶� �Ÿ� ����
	CameraBaseLength = CameraBoom->TargetArmLength; // �⺻ ī�޶� ���� ����
    //CameraBoom->bUsePawnControlRotation = true; // ��Ʈ�ѷ� �������� ȸ��
    CameraBoom->bUsePawnControlRotation = false; // ��Ʈ�ѷ� ȸ�� ����
    CameraBoom->bInheritPitch = true;
    CameraBoom->bInheritYaw = true;
    CameraBoom->bInheritRoll = true;

    // Camera ���� �� Spring Arm�� ����
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false; // ī�޶�� ���� ȸ��X

    // �⺻ �ӵ� ���
    NormalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

    // ��ũ���� �� ī�޶� �̵�
    DefaultCameraOffset = CameraBoom->TargetOffset;
    CrouchCameraOffset = DefaultCameraOffset + FVector(0.f, 0.f, -30.f);

	// ĳ���� �̵� ����
    bUseControllerRotationPitch = true;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;  // ���콺, ī�޶� ȸ���� ĳ����(��) ȸ���� ���� X
    GetCharacterMovement()->bOrientRotationToMovement = false;  // �̵��ϸ� �� �������� ���� ȸ��
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);  // ȸ�� �ӵ�(���� ����)
}

// Called when the game starts or when spawned
void ATrainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    // �÷��̾� ��Ʈ�ѷ� ��������
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        // Enhanced Input Subsystem �������� (���� �÷��̾� ����)
        if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
        {
            UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

            // Mapping Context�� ���¿� �Ҵ�Ǿ� �ִٸ� ���
            if (Subsystem && DefaultMappingContext)
            {
                // 0�� �⺻ �켱���� (���ڰ� �������� �켱)
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
    if (Controller)
    {
        // ��Ʈ�ѷ��� ȸ������ �ʱⰪ (Pitch: 0, Yaw: 0, Roll: 0)���� �����մϴ�.
        // �̷��� �ϸ� ���� ���� �� ī�޶� ������ �ٶ󺸰� �˴ϴ�.
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
    // Enhanced Input Component�� ĳ����
    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (EnhancedInputComponent && MoveAction)
    {
        // �̵� �׼� ���ε�
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATrainCharacter::Move);

	}

    // ���� �׼� ���ε�
    if (JumpAction)
    {
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATrainCharacter::StartJump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ATrainCharacter::StopJump);
    }
    // ��ũ���� �׼� ���ε�
    if (CrouchAction)
    {
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ATrainCharacter::StartCrouch);
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ATrainCharacter::StopCrouch);
	}
    // �� �׼� ���ε�
    if (Zoom)
    {
        EnhancedInputComponent->BindAction(Zoom, ETriggerEvent::Started, this, &ATrainCharacter::StartZoom);
        EnhancedInputComponent->BindAction(Zoom, ETriggerEvent::Completed, this, &ATrainCharacter::StopZoom);
    }

	// Look �׼� ���ε�
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

    // ī�޶� ���� ȸ�� ��������
    FRotator CameraRotation = Controller->GetControlRotation();
    FRotator YawRotation(0.f, CameraRotation.Yaw, 0.f); // Pitch, Roll ����

    // Forward / Right ���� ���
    FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // ���� �̵� ����
    FVector MoveDirection = ForwardDir * MovementVector.Y + RightDir * MovementVector.X;
    MoveDirection.Normalize();

    // �̵�
    AddMovementInput(MoveDirection, 1.0f);

    // ĳ���͸� �̵� �������� ȸ��
    FRotator TargetRotation = MoveDirection.Rotation();
    FRotator CurrentRotation = GetActorRotation();

    // �ε巴�� ȸ�� (���� ȸ���ϰų� ���� ����)
    SetActorRotation(FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 10.f));
}

void ATrainCharacter::StartJump()
{
    Jump(); // ���� ���� �Լ� ȣ��
}

void ATrainCharacter::StopJump()
{
    StopJumping(); // ���� ����
}

void ATrainCharacter::StartCrouch()
{
    Crouch();

    // �ӵ� ����
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed * 0.5f;

    // ī�޶� �����ֱ�
    if (CameraBoom)
    {
        CameraBoom->TargetOffset = CrouchCameraOffset;
    }
}

void ATrainCharacter::StopCrouch()
{
    UnCrouch();

    // �ӵ� ����
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;

    // ī�޶� �������
    if (CameraBoom)
    {
        CameraBoom->TargetOffset = DefaultCameraOffset;
    }
}


void ATrainCharacter::ZoomIn()
{
    // ī�޶� �� ��
	CameraBaseLength = CameraBoom->TargetArmLength;
	CameraBoom->TargetArmLength = -100.0f; // ���ϴ� �� �� ���̷� ����
    bIsZooming = true;
}
void ATrainCharacter::ZoomOut()
{
    // ī�޶� �� �ƿ�
    CameraBoom->TargetArmLength = CameraBaseLength;
    bIsZooming = false;
}
void ATrainCharacter::StartZoom()
{
    // �� ��
    ZoomIn();
}
void ATrainCharacter::StopZoom()
{
    // �� �ƿ�
    ZoomOut();
}

void ATrainCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxis = Value.Get<FVector2D>();

    float YawSensitivity = 1.0f;
    float PitchSensitivity = 1.0f;

    // Yaw ȸ���� �״�� ����
    AddControllerYawInput(LookAxis.X * YawSensitivity);

    // ���� ��Ʈ�ѷ� ȸ����
    FRotator ControlRot = Controller->GetControlRotation();

    // ���ο� Pitch ���
    float NewPitch = ControlRot.Pitch + LookAxis.Y * PitchSensitivity * -1.0f;

    // �𸮾󿡼� Pitch�� 360�� ������� 270�� �̻��� ���������� �ǹ���
    // ���� ClampAngle ��� (Ȥ�� ���� ���� ����)
    NewPitch = FMath::ClampAngle(NewPitch, -45.0f, 45.0f);

    // ��Ʈ�ѷ� ȸ���� ����
    ControlRot.Pitch = NewPitch;
    Controller->SetControlRotation(ControlRot);
}

