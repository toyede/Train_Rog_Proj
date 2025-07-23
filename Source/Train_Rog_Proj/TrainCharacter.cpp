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

    // Spring Arm 생성 및 설정
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f; // 캐릭터와 카메라 거리 조절
	CameraBaseLength = CameraBoom->TargetArmLength; // 기본 카메라 길이 저장
    //CameraBoom->bUsePawnControlRotation = true; // 컨트롤러 기준으로 회전
    CameraBoom->bUsePawnControlRotation = false; // 컨트롤러 회전 무시
    CameraBoom->bInheritPitch = true;
    CameraBoom->bInheritYaw = true;
    CameraBoom->bInheritRoll = true;

    // Camera 생성 및 Spring Arm에 부착
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false; // 카메라는 직접 회전X

    // 기본 속도 기억
    NormalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

    // 웅크리기 시 카메라 이동
    DefaultCameraOffset = CameraBoom->TargetOffset;
    CrouchCameraOffset = DefaultCameraOffset + FVector(0.f, 0.f, -30.f);

	// 캐릭터 이동 설정
    bUseControllerRotationPitch = true;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;  // 마우스, 카메라 회전이 캐릭터(몸) 회전에 영향 X
    GetCharacterMovement()->bOrientRotationToMovement = false;  // 이동하면 그 방향으로 몸이 회전
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);  // 회전 속도(조정 가능)
}

// Called when the game starts or when spawned
void ATrainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    // 플레이어 컨트롤러 가져오기
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        // Enhanced Input Subsystem 가져오기 (로컬 플레이어 전용)
        if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
        {
            UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

            // Mapping Context가 에셋에 할당되어 있다면 등록
            if (Subsystem && DefaultMappingContext)
            {
                // 0은 기본 우선순위 (숫자가 높을수록 우선)
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
    if (Controller)
    {
        // 컨트롤러의 회전값을 초기값 (Pitch: 0, Yaw: 0, Roll: 0)으로 설정합니다.
        // 이렇게 하면 게임 시작 시 카메라가 정면을 바라보게 됩니다.
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
    // Enhanced Input Component로 캐스팅
    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (EnhancedInputComponent && MoveAction)
    {
        // 이동 액션 바인딩
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATrainCharacter::Move);

	}

    // 점프 액션 바인딩
    if (JumpAction)
    {
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATrainCharacter::StartJump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ATrainCharacter::StopJump);
    }
    // 웅크리기 액션 바인딩
    if (CrouchAction)
    {
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ATrainCharacter::StartCrouch);
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ATrainCharacter::StopCrouch);
	}
    // 줌 액션 바인딩
    if (Zoom)
    {
        EnhancedInputComponent->BindAction(Zoom, ETriggerEvent::Started, this, &ATrainCharacter::StartZoom);
        EnhancedInputComponent->BindAction(Zoom, ETriggerEvent::Completed, this, &ATrainCharacter::StopZoom);
    }

	// Look 액션 바인딩
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

    // 카메라 기준 회전 가져오기
    FRotator CameraRotation = Controller->GetControlRotation();
    FRotator YawRotation(0.f, CameraRotation.Yaw, 0.f); // Pitch, Roll 제거

    // Forward / Right 벡터 계산
    FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // 실제 이동 방향
    FVector MoveDirection = ForwardDir * MovementVector.Y + RightDir * MovementVector.X;
    MoveDirection.Normalize();

    // 이동
    AddMovementInput(MoveDirection, 1.0f);

    // 캐릭터를 이동 방향으로 회전
    FRotator TargetRotation = MoveDirection.Rotation();
    FRotator CurrentRotation = GetActorRotation();

    // 부드럽게 회전 (직접 회전하거나 보간 가능)
    SetActorRotation(FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 10.f));
}

void ATrainCharacter::StartJump()
{
    Jump(); // 내장 점프 함수 호출
}

void ATrainCharacter::StopJump()
{
    StopJumping(); // 점프 중지
}

void ATrainCharacter::StartCrouch()
{
    Crouch();

    // 속도 조정
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed * 0.5f;

    // 카메라 내려주기
    if (CameraBoom)
    {
        CameraBoom->TargetOffset = CrouchCameraOffset;
    }
}

void ATrainCharacter::StopCrouch()
{
    UnCrouch();

    // 속도 복구
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;

    // 카메라 원래대로
    if (CameraBoom)
    {
        CameraBoom->TargetOffset = DefaultCameraOffset;
    }
}


void ATrainCharacter::ZoomIn()
{
    // 카메라 줌 인
	CameraBaseLength = CameraBoom->TargetArmLength;
	CameraBoom->TargetArmLength = -100.0f; // 원하는 줌 인 길이로 설정
    bIsZooming = true;
}
void ATrainCharacter::ZoomOut()
{
    // 카메라 줌 아웃
    CameraBoom->TargetArmLength = CameraBaseLength;
    bIsZooming = false;
}
void ATrainCharacter::StartZoom()
{
    // 줌 인
    ZoomIn();
}
void ATrainCharacter::StopZoom()
{
    // 줌 아웃
    ZoomOut();
}

void ATrainCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxis = Value.Get<FVector2D>();

    float YawSensitivity = 1.0f;
    float PitchSensitivity = 1.0f;

    // Yaw 회전은 그대로 적용
    AddControllerYawInput(LookAxis.X * YawSensitivity);

    // 현재 컨트롤러 회전값
    FRotator ControlRot = Controller->GetControlRotation();

    // 새로운 Pitch 계산
    float NewPitch = ControlRot.Pitch + LookAxis.Y * PitchSensitivity * -1.0f;

    // 언리얼에서 Pitch는 360도 기반으로 270도 이상은 음수방향을 의미함
    // 따라서 ClampAngle 사용 (혹은 직접 범위 제한)
    NewPitch = FMath::ClampAngle(NewPitch, -45.0f, 45.0f);

    // 컨트롤러 회전값 갱신
    ControlRot.Pitch = NewPitch;
    Controller->SetControlRotation(ControlRot);
}

