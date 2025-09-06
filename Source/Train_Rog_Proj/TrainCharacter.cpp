#include "TrainCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include <cstdlib>
#include <ctime>
//#include "InteractInterface.h"

// 캐릭터 클래스의 기본값 세팅
ATrainCharacter::ATrainCharacter()
{
    // 매 프레임 Tick() 호출 여부
    PrimaryActorTick.bCanEverTick = true;

    // ► 카메라 붐(SpringArm) 생성 및 캐릭터에 부착
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);

    // 카메라 기본 거리
    CameraBoom->TargetArmLength = 300.0f;
    CameraBaseLength = CameraBoom->TargetArmLength;

    // 카메라 회전 제어 (플레이어 입력에 따라 움직일지)
    CameraBoom->bUsePawnControlRotation = false;
    CameraBoom->bInheritPitch = true;
    CameraBoom->bInheritYaw = true;
    CameraBoom->bInheritRoll = true;

    // 카메라 컴포넌트 생성 및 연결
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // 기본 이동속도 저장
    NormalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

    // 기본 카메라 오프셋
    DefaultCameraOffset = CameraBoom->TargetOffset;
    CrouchCameraOffset = DefaultCameraOffset + FVector(0.f, 0.f, -30.f);

    // 캐릭터 회전 설정
    bUseControllerRotationPitch = true;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
}

// 게임 시작 시 초기화
void ATrainCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Enhanced Input 맵핑 등록
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
        {
            UEnhancedInputLocalPlayerSubsystem* Subsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
            
            if (Subsystem && DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }

    // 컨트롤러 회전 초기화
    if (Controller)
    {
        Controller->SetControlRotation(FRotator(0.f, 0.f, 0.f));
    }

    // 난수 초기화
    srand(static_cast<unsigned int>(time(NULL)));
}

// 매 프레임 호출
void ATrainCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// 입력 바인딩
void ATrainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

    // ▶ 이동
    if (EnhancedInputComponent && MoveAction)
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATrainCharacter::Move);

    // ▶ 점프
    if (JumpAction)
    {
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATrainCharacter::StartJump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ATrainCharacter::StopJump);
    }

    // ▶ 앉기
    if (CrouchAction)
    {
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ATrainCharacter::StartCrouch);
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ATrainCharacter::StopCrouch);
    }

    // ▶ 줌
    if (Zoom)
    {
        EnhancedInputComponent->BindAction(Zoom, ETriggerEvent::Started, this, &ATrainCharacter::StartZoom);
        EnhancedInputComponent->BindAction(Zoom, ETriggerEvent::Completed, this, &ATrainCharacter::StopZoom);
    }

    // ▶ 시점 이동(마우스)
    if (LookAction)
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATrainCharacter::Look);

    // ▶ 상호작용 (개발자 추가)
    if (InteractAction)
        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ATrainCharacter::Interact);

    // ▶ 달리기
    if (RunAction)
    {
        EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &ATrainCharacter::StartRun);
        EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ATrainCharacter::StopRun);
    }
}

// =================== 캐릭터 동작 =================== //

// 이동 처리
void ATrainCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    if (Controller == nullptr || MovementVector.IsNearlyZero())
        return;

    // 카메라 기준 방향 벡터 구함
    FRotator CameraRotation = Controller->GetControlRotation();
    FRotator YawRotation(0.f, CameraRotation.Yaw, 0.f);

    FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    FVector RightDir   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    FVector MoveDirection = ForwardDir * MovementVector.Y + RightDir * MovementVector.X;
    MoveDirection.Normalize();

    AddMovementInput(MoveDirection, 1.0f);

    // 이동하는 방향을 바라보도록 회전 보간
    FRotator TargetRotation = MoveDirection.Rotation();
    SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 10.f));
}

// 점프 시작
void ATrainCharacter::StartJump() { Jump(); }

// 점프 종료
void ATrainCharacter::StopJump() { StopJumping(); }

// 앉기 시작
void ATrainCharacter::StartCrouch()
{
    Crouch();
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed * 0.5f;
    CameraBoom->TargetOffset = CrouchCameraOffset;
}

// 앉기 해제
void ATrainCharacter::StopCrouch()
{
    UnCrouch();
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;
    CameraBoom->TargetOffset = DefaultCameraOffset;
}

// 줌 인/아웃
void ATrainCharacter::ZoomIn()
{
    CameraBaseLength = CameraBoom->TargetArmLength;
    CameraBoom->TargetArmLength = -100.f;
    bIsZooming = true;
}

void ATrainCharacter::ZoomOut()
{
    CameraBoom->TargetArmLength = CameraBaseLength;
    bIsZooming = false;
}

void ATrainCharacter::StartZoom() { ZoomIn(); }
void ATrainCharacter::StopZoom()  { ZoomOut(); }

// 마우스 Look 입력 처리
void ATrainCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxis = Value.Get<FVector2D>();

    AddControllerYawInput(LookAxis.X);  // 좌우 회전

    // 상하 회전 제한 (-45도 ~ 45도)
    FRotator ControlRot = Controller->GetControlRotation();
    float NewPitch = FMath::ClampAngle(ControlRot.Pitch + LookAxis.Y * -1.0f, -45.0f, 45.0f);
    ControlRot.Pitch = NewPitch;
    Controller->SetControlRotation(ControlRot);
}

// 상호작용: 카메라 전방 라인트레이스 검사
void ATrainCharacter::Interact(const FInputActionValue& Value)
{
    if (!Controller || !FollowCamera) return;

    FVector StartLocation = FollowCamera->GetComponentLocation();
    FVector ForwardVector = FollowCamera->GetForwardVector();
    FVector EndLocation = StartLocation + (ForwardVector * TraceDistance);

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult, StartLocation, EndLocation,
        ECC_Visibility, CollisionParams
    );

    // 디버그 라인 표시
    DrawDebugLine(GetWorld(), StartLocation, EndLocation, bHit ? FColor::Green : FColor::Red, false, 1.0f, 0, 1.0f);

    if (bHit)
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
        {
            IInteractInterface::Execute_Interact(HitActor, this);
        }
    }
}

// 달리기
void ATrainCharacter::StartRun()
{
    bIsRunning = true;
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed * RunSpeedMultiplier;
}

void ATrainCharacter::StopRun()
{
    bIsRunning = false;
    GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;
}

// === 키 리매핑 기능 === //
void ATrainCharacter::RemapKey(UInputAction* InputAction, FKey OldKey, FKey NewKey)
{
    if (!DefaultMappingContext || !InputAction) return;

    DefaultMappingContext->UnmapKey(InputAction, OldKey);
    DefaultMappingContext->MapKey(InputAction, NewKey);

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->RemoveMappingContext(DefaultMappingContext);
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

// === 스탯 증가 함수들 === //
float ATrainCharacter::IncreaseHP()
{
    HP += HPStack * 2;
    return HP;
}

float ATrainCharacter::IncreasePower()
{
    Power += PowerStack * 2;
    return Power;
}

float ATrainCharacter::IncreaseDefense()
{
    Defense += DefenseStack * 2;
    return Defense;
}

float ATrainCharacter::IncreaseAttackSpeed()
{
    AttackSpeed += AttackSpeedStack * 0.02f;
    return AttackSpeed;
}

int ATrainCharacter::IncreaseCriticalChance()
{
    CriticalChance += CriticalChanceStack;
    return CriticalChance;
}

float ATrainCharacter::IncreaseCriticalDamage()
{
    CriticalDamage += CriticalDamageStack * 0.05f;
    return CriticalDamage;
}

void ATrainCharacter::LevelUp()
{
    EXP = 0;
    MaxEXP += 50;
    Level++;
}

float ATrainCharacter::DecreaseHP()
{
    HP -= HPStack * 2;
    return HP;
}

float ATrainCharacter::DecreasePower()
{
    Power -= PowerStack * 2;
    return Power;
}

float ATrainCharacter::DecreaseDefense()
{
    Defense -= DefenseStack * 2;
    return Defense;
}

float ATrainCharacter::DecreaseAttackSpeed()
{
    AttackSpeed -= AttackSpeedStack * 0.02f;
    return AttackSpeed;
}

int ATrainCharacter::DecreaseCriticalChance()
{
    CriticalChance -= CriticalChanceStack;
    return CriticalChance;
}

float ATrainCharacter::DecreaseCriticalDamage()
{
    CriticalDamage -= CriticalDamageStack * 0.05f;
    return CriticalDamage;
}

float ATrainCharacter::TakeDamage(float weaponDamage, float weaponConst, int criChance, float cirDam, bool cri)
{
    float totalDamage = weaponDamage + Power * weaponConst;

    int Critical = rand() % 100;
    if (cri == true && Critical <= CriticalChance + criChance)
    {
        totalDamage += totalDamage * (CriticalDamage + cirDam);
        totalDamage = FMath::RoundToFloat(totalDamage * 10.0f) / 10.0f;
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, TEXT("크리티컬 데미지"));
    }
    return totalDamage;
}

