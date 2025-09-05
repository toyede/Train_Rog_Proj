#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TrainCharacter.h"

// Sets default values
// Sets default values
ATrainCharacter::ATrainCharacter()
{
    // 이 캐릭터가 매 프레임마다 Tick()을 호출하도록 설정합니다. 필요 없다면 성능을 위해 꺼도 됩니다.
    PrimaryActorTick.bCanEverTick = true;

    // Spring Arm(카메라 붐) 컴포넌트 생성 및 루트에 부착
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);

    // 카메라 거리(팔 길이) 설정
    CameraBoom->TargetArmLength = 300.0f; // 카메라와 캐릭터 사이 거리 설정
    CameraBaseLength = CameraBoom->TargetArmLength; // 기본 길이 저장

    // 컨트롤러 회전 사용 설정
    //CameraBoom->bUsePawnControlRotation = true; // 컨트롤러 회전에 따라 붐 회전 (여기선 꺼둠)
    CameraBoom->bUsePawnControlRotation = false; // 붐 자체가 컨트롤러 회전 안 따름
    CameraBoom->bInheritPitch = true;
    CameraBoom->bInheritYaw = true;
    CameraBoom->bInheritRoll = true;

    // 카메라 컴포넌트 생성, Spring Arm에 부착
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

    // 카메라가 컨트롤러 회전을 따르지 않게 설정
    FollowCamera->bUsePawnControlRotation = false;

    // 기본 걷기 속도 등 초기값 저장
    NormalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

    // 카메라 오프셋 저장 및 웅크릴 때 오프셋 설정
    DefaultCameraOffset = CameraBoom->TargetOffset;
    CrouchCameraOffset = DefaultCameraOffset + FVector(0.f, 0.f, -30.f);

    // 컨트롤러 회전 사용 설정 (Pitch, Yaw 사용, Roll은 사용하지 않음)
    bUseControllerRotationPitch = true;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;  // 롤은 꺼둠

    // 움직임 방향에 따라 캐릭터 회전 안 하도록 설정
    GetCharacterMovement()->bOrientRotationToMovement = false;

    // 캐릭터 회전 속도 설정
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);  // Yaw만 빠르게 회전
}

// Called when the game starts or when spawned
void ATrainCharacter::BeginPlay()
{
    Super::BeginPlay();

    // 플레이어 컨트롤러가 있으면 입력 시스템 설정
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        // Enhanced Input Subsystem 가져옴 (입력 매핑용)
        if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
        {
            UEnhancedInputLocalPlayerSubsystem* Subsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
            // 입력 매핑 컨텍스트를 서브시스템에 등록
            if (Subsystem && DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0); // 우선순위 0으로 등록
            }
        }
    }
    if (Controller)
    {
        // 컨트롤러 초기 회전 설정 (Pitch, Yaw, Roll 모두 0)
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

    // EnhancedInputComponent로 캐스팅 후, 각 액션을 바인딩
    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (EnhancedInputComponent && MoveAction)
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATrainCharacter::Move); // 이동
    }
    if (JumpAction)
    {
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATrainCharacter::StartJump); // 점프 시작
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ATrainCharacter::StopJump); // 점프 종료
    }
    if (CrouchAction)
    {
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ATrainCharacter::StartCrouch); // 웅크리기 시작
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ATrainCharacter::StopCrouch); // 웅크리기 끝
    }
    if (Zoom)
    {
        EnhancedInputComponent->BindAction(Zoom, ETriggerEvent::Started, this, &ATrainCharacter::StartZoom); // 줌인
        EnhancedInputComponent->BindAction(Zoom, ETriggerEvent::Completed, this, &ATrainCharacter::StopZoom); // 줌아웃
    }
    if (LookAction)
    {
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATrainCharacter::Look); // 마우스 이동/회전
    }
    if (RunAction)
    {
        EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &ATrainCharacter::StartRun); // 달리기 시작
        EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ATrainCharacter::StopRun); // 달리기 끝
    }
}

// 이동 처리 함수
void ATrainCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    // 입력 값이 거의 없거나 컨트롤러가 없으면 리턴
    if (Controller == nullptr || MovementVector.IsNearlyZero())
        return;

    // 카메라(컨트롤러) 회전 정보 얻기
    FRotator CameraRotation = Controller->GetControlRotation();
    // Yaw(좌우)만 사용
    FRotator YawRotation(0.f, CameraRotation.Yaw, 0.f);

    // Forward, Right 방향 벡터 계산
    FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // 입력에 따라 최종 이동 방향 계산 및 정규화
    FVector MoveDirection = ForwardDir * MovementVector.Y + RightDir * MovementVector.X;
    MoveDirection.Normalize();

    // 캐릭터 이동
    AddMovementInput(MoveDirection, 1.0f);

    // 이동 방향에 따라 부드럽게 캐릭터 회전
    FRotator TargetRotation = MoveDirection.Rotation();
    FRotator CurrentRotation = GetActorRotation();
    SetActorRotation(FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 10.f));
}

// 점프 시작
void ATrainCharacter::StartJump()
{
    Jump();
}

// 점프 끝
void ATrainCharacter::StopJump()
{
    StopJumping();
}

// 웅크리기 시작
void ATrainCharacter::StartCrouch()
{
    Crouch(); // 웅크리기

    // 웅크릴 때 속도 감소
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed * 0.5f;

    // 카메라 오프셋 변경
    if (CameraBoom)
    {
        CameraBoom->TargetOffset = CrouchCameraOffset;
    }
}

// 웅크리기 끝
void ATrainCharacter::StopCrouch()
{
    UnCrouch(); // 웅크리기 종료

    // 속도 원래대로 복원
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;

    // 카메라 오프셋 복원
    if (CameraBoom)
    {
        CameraBoom->TargetOffset = DefaultCameraOffset;
    }
}

// 줌 인(카메라 거리 더 가까움)
void ATrainCharacter::ZoomIn()
{
    CameraBaseLength = CameraBoom->TargetArmLength; // 현재 거리 저장
    CameraBoom->TargetArmLength = -100.0f; // 더 가까운 거리로 변경
    bIsZooming = true;
}

// 줌 아웃(카메라 거리 복원)
void ATrainCharacter::ZoomOut()
{
    CameraBoom->TargetArmLength = CameraBaseLength;
    bIsZooming = false;
}

// 줌 시작
void ATrainCharacter::StartZoom()
{
    ZoomIn();
}

// 줌 끝
void ATrainCharacter::StopZoom()
{
    ZoomOut();
}

// 마우스 등 카메라 회전 처리
void ATrainCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxis = Value.Get<FVector2D>();
    float YawSensitivity = 1.0f;
    float PitchSensitivity = 1.0f;

    // 좌우(Yaw) 방향 회전 처리
    AddControllerYawInput(LookAxis.X * YawSensitivity);

    // 위아래(Pitch) 방향 회전 처리
    FRotator ControlRot = Controller->GetControlRotation();
    float NewPitch = ControlRot.Pitch + LookAxis.Y * PitchSensitivity * -1.0f;

    // Pitch 각도 제한 (-45~45도)
    NewPitch = FMath::ClampAngle(NewPitch, -45.0f, 45.0f);

    // 변경된 Pitch 반영
    ControlRot.Pitch = NewPitch;
    Controller->SetControlRotation(ControlRot);
}

void ATrainCharacter::StartRun()
{
    // 달리기 시작
    bIsRunning = true;
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed * RunSpeedMultiplier;
}

void ATrainCharacter::StopRun()
{
    // 달리기 중지
    bIsRunning = false;
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;
}

void ATrainCharacter::RemapKey(UInputAction* InputAction, FKey OldKey, FKey NewKey)
{
    if (!DefaultMappingContext || !InputAction) return;

    // 1. 기존 키 언맵
    DefaultMappingContext->UnmapKey(InputAction, OldKey);

    // 2. 새 키 맵핑
    DefaultMappingContext->MapKey(InputAction, NewKey);

    // 3. 변경 적용
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->RemoveMappingContext(DefaultMappingContext);
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
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
}