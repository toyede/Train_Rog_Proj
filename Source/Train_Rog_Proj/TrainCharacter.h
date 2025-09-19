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

#include "TrainCharacter.generated.h"

/**
 * 플레이어 캐릭터 클래스
 * - 이동, 점프, 앉기, 달리기, 줌, 상호작용 등을 지원
 * - Enhanced Input 시스템 활용
 * - 플레이어 스탯 성장 시스템 포함
 */
UCLASS()
class TRAIN_ROG_PROJ_API ATrainCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // 생성자
    ATrainCharacter();

protected:
    /** ========== 엔진 기본 오버라이드 함수들 ========== */
    virtual void BeginPlay() override;                     // 게임 시작 시 초기화
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override; // 입력 매핑 설정

    /** ========== 입력 매핑 관련 ========== */
    // 입력 매핑 컨텍스트 (에디터에서 BP로 지정 가능)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    /** 이동 입력 액션 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MoveAction;

    /** 점프 입력 액션 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* JumpAction;

    /** 앉기 입력 액션 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* CrouchAction;

    /** 줌 입력 액션 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* Zoom;

    /** 시점(Look) 입력 액션 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* LookAction;

    /** 상호작용 입력 액션 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* InteractAction;

    /** 달리기 입력 액션 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* RunAction;

    /** ========== 카메라 관련 ========== */
    // 3인칭 카메라 붐(SpringArm)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* CameraBoom;

    // 실제 카메라
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FollowCamera;

    // 현재 카메라 상태 (줌 인 여부)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
    bool bIsZooming;

    // 줌 전 기본 거리
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float CameraBaseLength;

    // 줌 시 카메라 거리
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float CameraZoomLength;

    // 앉았을 때 카메라 오프셋
    FVector DefaultCameraOffset;
    FVector CrouchCameraOffset;

    /** ========== 이동/기본 동작 관련 ========== */
    float NormalWalkSpeed;          // 기본 걷기 속도
    bool bIsRunning;                // 현재 달리기 상태 여부
    float RunSpeedMultiplier = 1.5f; // 달리기 배속 (x1.5)

    /** ========== 인터랙션 관련 ========== */
    // 라인트레이스 최대 거리 (상호작용 거리)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float TraceDistance = 500.0f; // 기본값 500

    /** ========== 입력 처리 함수들 ========== */
    void Move(const FInputActionValue& Value);    // 이동
    void StartJump(); void StopJump();            // 점프 시작/종료
    void StartCrouch(); void StopCrouch();        // 앉기/해제
    void ZoomIn(); void ZoomOut();                // 줌 거리 변경
    void StartZoom(); void StopZoom();            // 줌 입력 처리
    void Look(const FInputActionValue& Value);    // 마우스 Look 처리
    void Interact(const FInputActionValue& Value);// 상호작용 (라인 트레이스 기반)
    void StartRun(); void StopRun();              // 달리기 시작/종료

    // 키 매핑 리매핑 함수
    UFUNCTION(BlueprintCallable, Category = "Input")
    void RemapKey(UInputAction* InputAction, FKey OldKey, FKey NewKey);

public: 
    /** 매 프레임 호출 */
    virtual void Tick(float DeltaTime) override;

    //체력 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class UHealthComponent* HealthComponent;

    //능력치 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class UAbilityComponent* AbilityComponent;

    /** ========== 플레이어 레벨 ========== */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats") int Level;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats") int EXP = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats") int MaxEXP = 100;

    /** 스탯 성장치 (레벨업 시 증가량) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats Stacks") int HPStack;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats Stacks") int PowerStack;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats Stacks") int DefenseStack;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats Stacks") int AttackSpeedStack;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats Stacks") int CriticalChanceStack;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats Stacks") int CriticalDamageStack;

    /** 스탯 포인트 (유저가 직접 분배하는 포인트) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats Stacks")
    int StatPoint;

   
    /** 레벨업 함수 */
    UFUNCTION(BlueprintCallable, Category = "Player Stats Functions")
    void LevelUp();
    
    /** 데미지 함수 */
    UFUNCTION(BlueprintCallable, Category = "Player Battle")
    float CalDamage(float weaponDamage, float weaponConst, float criChance, float criDam, bool cri);
    UFUNCTION(BlueprintCallable, Category = "Player Battle")
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};
