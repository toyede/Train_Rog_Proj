// TrainPlayerController_KJH.cpp

#include "Dialogue/TrainPlayerController_KJH.h" // 헤더 파일 경로
#include "Blueprint/UserWidget.h" // UUserWidget, CreateWidget 사용
#include "Dialogue/DialogueComponent.h" // UDialogueComponent 사용
#include "Dialogue/UMyDialogueWidgetBase.h" // 새로 만든 헤더 include
#include "Dialogue/MyGameInstanceBase.h" // 이제 C++ 헤더를 포함합니다.
#include "UObject/UnrealType.h" // FProperty 등 리플렉션 시스템 사용을 위해 필요
#include "Dialogue/UMyVideoWidgetBase.h" // [추가] 비디오 위젯 베이스 헤더
#include "Dialogue/DialogueComponent.h"   // [추가] DialogueComponent 헤더 (GetCurrentDialogueRow를 위해)
#include "Kismet/GameplayStatics.h"

void ATrainPlayerController_KJH::ShowDialogueUI(UDialogueComponent* ComponentToUse)
{
    // 1. 위젯 클래스가 유효한지, 그리고 이미 위젯이 생성되어 있지 않은지 확인
    if (!DialogueWidgetClass || DialogueWidgetInstance)
    {
        return;
    }
    
    // 2. 위젯 생성
    DialogueWidgetInstance = CreateWidget<UMyDialogueWidgetBase>(this, DialogueWidgetClass);
    if (!DialogueWidgetInstance)
    {
        return;
    }

    // 3. (중요) 생성된 위젯에 DialogueComponent를 전달
    // Cast 대상을 UMyDialogueWidgetBase로 변경합니다
    if (UMyDialogueWidgetBase* DialogueWidget = Cast<UMyDialogueWidgetBase>(DialogueWidgetInstance))
    {
        DialogueWidget->SetDialogueComponent(ComponentToUse);
    }
    
    // 4. 위젯을 화면에 추가
    DialogueWidgetInstance->AddToViewport();
    
    // 5. 입력 모드를 UI Only로 변경
    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(DialogueWidgetInstance->TakeWidget()); // 포커스를 위젯에 줌
    SetInputMode(InputMode);
    
    // 6. 마우스 커서 표시
    bShowMouseCursor = true;

    // ▼▼▼ 7. 델리게이트 바인딩(연결) ▼▼▼
    // PlayerController의 ActiveDialogueComponent 변수에 현재 컴포넌트를 저장하고,
    // OnEventTriggered가 방송될 때 HandleDialogueEvent 함수를 실행하도록 연결합니다.
    ActiveDialogueComponent = ComponentToUse;
    ActiveDialogueComponent->OnEventTriggered.AddDynamic(this, &ATrainPlayerController_KJH::HandleDialogueEvent);

    // 7. C++에서 직접 대화 시작
    ComponentToUse->BeginDialogue();
}

void ATrainPlayerController_KJH::HideDialogueUI()
{
    if (!DialogueWidgetInstance)
    {
        return;
    }

    // ▼▼▼ 1. 델리게이트 바인딩 해제 ▼▼▼
    // 대화가 끝나므로, 연결해두었던 델리게이트를 해제하여 메모리 누수를 방지합니다.
    if (ActiveDialogueComponent)
    {
        ActiveDialogueComponent->OnEventTriggered.RemoveDynamic(this, &ATrainPlayerController_KJH::HandleDialogueEvent);
        ActiveDialogueComponent = nullptr;
    }

    // 1. 입력 모드를 Game Only로 되돌림
    SetInputMode(FInputModeGameOnly());

    // 2. 마우스 커서 숨김
    bShowMouseCursor = false;

    // 3. 위젯을 화면에서 제거
    DialogueWidgetInstance->RemoveFromParent();

    // 4. 위젯 인스턴스 정리
    DialogueWidgetInstance = nullptr;
}

// 이벤트 핸들링 함수
void ATrainPlayerController_KJH::HandleDialogueEvent(FName EventTag)
{
    FString EventString = EventTag.ToString();
    EventString.TrimStartAndEndInline(); // 앞뒤 공백 제거

    UMyGameInstanceBase* GameInstance = Cast<UMyGameInstanceBase>(UGameplayStatics::GetGameInstance(this));
    if (!GameInstance) return;

    // 1. 변수 수정 포맷인지 확인 (+Var * Val 또는 -Var * Val)
    if (EventString.StartsWith("+") || EventString.StartsWith("-"))
    {
        UE_LOG(LogTemp, Warning, TEXT("Debug1"));
        FString Operator = EventString.Left(1);
        FString Remainder = EventString.RightChop(1); // 연산자를 제외한 나머지 부분

        FString VariableName;
        FString ValueString;
        if (Remainder.Split("*", &VariableName, &ValueString))
        {
            VariableName.TrimStartAndEndInline();
            ValueString.TrimStartAndEndInline();

            // GameInstance 클래스에서 VariableName과 일치하는 이름의 프로퍼티(변수)를 찾음
            FProperty* FoundProperty = GameInstance->GetClass()->FindPropertyByName(*VariableName);
            if (!FoundProperty)
            {
                UE_LOG(LogTemp, Warning, TEXT("Event Tag Error: Variable '%s' not found in GameInstance."), *VariableName);
                return;
            }

            // 찾은 프로퍼티의 타입에 따라 다른 처리
            // Float 타입 변수인 경우
            if (FFloatProperty* FloatProp = CastField<FFloatProperty>(FoundProperty))
            {
                float ValueToModify = FCString::Atof(*ValueString);
                float* ValuePtr = FloatProp->ContainerPtrToValuePtr<float>(GameInstance);
                if (Operator == "+") *ValuePtr += ValueToModify;
                else *ValuePtr -= ValueToModify;
                
                UE_LOG(LogTemp, Warning, TEXT("Variable '%s' updated to: %f"), *VariableName, *ValuePtr);
            }
            // Integer 타입 변수인 경우 (추후 확장을 위해)
            else if (FIntProperty* IntProp = CastField<FIntProperty>(FoundProperty))
            {
                int32 ValueToModify = FCString::Atoi(*ValueString);
                int32* ValuePtr = IntProp->ContainerPtrToValuePtr<int32>(GameInstance);
                if (Operator == "+") *ValuePtr += ValueToModify;
                else *ValuePtr -= ValueToModify;

                UE_LOG(LogTemp, Warning, TEXT("Variable '%s' updated to: %d"), *VariableName, *ValuePtr);
            }
            // 다른 타입이 필요하면 여기에 추가...
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Event Tag Error: Variable '%s' is not a Float or Integer."), *VariableName);
            }
        }
    }
    // 2. 명명된 이벤트 포맷인지 확인 (예: Shop_Event)
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Debug2"));
        if (EventTag == "Shop_Event")
        {
            // 상점 UI를 여는 등, 특별한 로직을 여기에 작성
            UE_LOG(LogTemp, Warning, TEXT("Named event 'Shop_Event' triggered!"));

            // 상점 UI 열기 전에 대화 UI 닫기
            if (DialogueWidgetInstance)
            {
                // 델리게이트 바인딩 해제
                if (ActiveDialogueComponent)
                {
                    ActiveDialogueComponent->OnEventTriggered.RemoveDynamic(this, &ATrainPlayerController_KJH::HandleDialogueEvent);
                    ActiveDialogueComponent = nullptr;
                }
                
                // 위젯 제거
                DialogueWidgetInstance->RemoveFromParent();
                DialogueWidgetInstance = nullptr;
            }

            // 새 상점 UI를 엽니다.
            ShowShopUI();
        }

        else if (EventString.StartsWith("Play_Video_"))
        {
            // "Play_Video_Intro"에서 "Intro" 키 추출
            FString KeyString = EventString.RightChop(FString("Play_Video_").Len());
            FName VideoKey = FName(*KeyString);

            // [추가] 3. 대화 재개를 위한 상태 저장
            // 🚨 중요: DialogueComponent.h에 GetCurrentDialogueRow() 함수를 public으로 추가해야 합니다.
            // ( FDialogueRow* GetCurrentDialogueRow() const { return CurrentDialogueRow; } )
            if (ActiveDialogueComponent && ActiveDialogueComponent->GetCurrentDialogueRow())
            {
                ResumeDialogueID = ActiveDialogueComponent->GetCurrentDialogueRow()->NextDialogueID;
                PausedDialogueComponent = ActiveDialogueComponent;
            }
            else
            {
                ResumeDialogueID = NAME_None;
                PausedDialogueComponent = nullptr;
            }

            // 기존 대화 UI 닫기
            if (DialogueWidgetInstance)
            {
                if (ActiveDialogueComponent)
                {
                    ActiveDialogueComponent->OnEventTriggered.RemoveDynamic(this, &ATrainPlayerController_KJH::HandleDialogueEvent);
                    ActiveDialogueComponent = nullptr;
                }
                DialogueWidgetInstance->RemoveFromParent();
                DialogueWidgetInstance = nullptr;
            }

            // 추출한 'Key'로 영상 UI 호출
            ShowVideoUI(VideoKey);
        }
        // 다른 명명된 이벤트들은 else if 로 추가...

    }
}

// 상점 UI 열기
void ATrainPlayerController_KJH::ShowShopUI()
{
    // 1. 클래스가 유효한지, 이미 열려있지 않은지 확인
    if (!ShopWidgetClass || ShopWidgetInstance)
    {
        return;
    }

    // 2. 상점 위젯 생성
    ShopWidgetInstance = CreateWidget<UUserWidget>(this, ShopWidgetClass);
    if (!ShopWidgetInstance)
    {
        return;
    }

    // 3. 뷰포트에 추가
    ShopWidgetInstance->AddToViewport();

    // 4. 입력 모드를 UI Only로 유지/설정
    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(ShopWidgetInstance->TakeWidget());
    SetInputMode(InputMode);
    
    // 5. 마우스 커서 표시 유지
    bShowMouseCursor = true;
}

// 상점 UI 닫기
void ATrainPlayerController_KJH::HideShopUI()
{
    if (!ShopWidgetInstance)
    {
        return;
    }

    // 1. 위젯 제거
    ShopWidgetInstance->RemoveFromParent();
    ShopWidgetInstance = nullptr;

    // 2. 입력 모드 게임으로 복귀
    SetInputMode(FInputModeGameOnly());

    // 3. 마우스 커서 숨김
    bShowMouseCursor = false;
}

// [신규] 비디오 UI 표시 함수
void ATrainPlayerController_KJH::ShowVideoUI(FName VideoKey)
{
    // 1. 위젯 클래스가 유효한지, 인스턴스가 없는지 확인
    if (!VideoWidgetClass || VideoWidgetInstance)
    {
        return;
    }

    // 2. [추가] TMap에서 VideoKey로 MediaSource 애셋 찾기
    UMediaSource* FoundMediaSource = VideoMap.FindRef(VideoKey);

    // 2-1. 영상을 못 찾았을 경우
    if (!FoundMediaSource)
    {
        UE_LOG(LogTemp, Warning, TEXT("ShowVideoUI Error: VideoKey '%s'를 VideoMap에서 찾을 수 없습니다!"), *VideoKey.ToString());
        HideVideoUI(); // 즉시 닫고 대화로 복귀
        return;
    }

    // 3. 비디오 위젯 생성
    VideoWidgetInstance = CreateWidget<UMyVideoWidgetBase>(this, VideoWidgetClass);
    if (!VideoWidgetInstance)
    {
        return;
    }
    
    // 4. [추가] 위젯에 재생할 미디어 소스 전달
    VideoWidgetInstance->MediaSourceToPlay = FoundMediaSource;
    
    // 5. 뷰포트에 추가
    VideoWidgetInstance->AddToViewport();

    // 6. [추가] 위젯의 블루프린트 이벤트(PlayVideo)를 호출하여 재생 시작
    VideoWidgetInstance->PlayVideo();
    
    // 7. 입력 모드 설정 및 마우스 커서 숨김
    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(VideoWidgetInstance->TakeWidget()); // 포커스를 줘야 스킵(CTRL) 키 입력 가능
    SetInputMode(InputMode);
    
    // bShowMouseCursor = false;
}

// [신규] 비디오 UI 닫기 함수
void ATrainPlayerController_KJH::HideVideoUI()
{
    if (!VideoWidgetInstance)
    {
        return;
    }

    // 1. 영상 위젯 제거
    VideoWidgetInstance->RemoveFromParent();
    VideoWidgetInstance = nullptr;

    // 2. [추가] 3. 대화 재개 로직
    if (PausedDialogueComponent && !ResumeDialogueID.IsNone())
    {
        // 저장해둔 컴포넌트와 다음 ID로 대화를 재개합니다.
        ResumeDialogueUI(PausedDialogueComponent, ResumeDialogueID);
    }
    else
    {
        // 재개할 대화가 없으면 게임으로 복귀
        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = false;
    }

    // 3. 임시 변수 초기화
    PausedDialogueComponent = nullptr;
    ResumeDialogueID = NAME_None;
}

// [신규] 대화 재개 함수
void ATrainPlayerController_KJH::ResumeDialogueUI(UDialogueComponent* ComponentToResume, FName NextID)
{
    // 1. 위젯 클래스 확인
    if (!DialogueWidgetClass || DialogueWidgetInstance || !ComponentToResume)
    {
        return;
    }
    
    // 2. 대화 위젯 다시 생성
    DialogueWidgetInstance = CreateWidget<UMyDialogueWidgetBase>(this, DialogueWidgetClass);
    if (!DialogueWidgetInstance) return;

    // 3. 컴포넌트 전달
    if (UMyDialogueWidgetBase* DialogueWidget = Cast<UMyDialogueWidgetBase>(DialogueWidgetInstance))
    {
        DialogueWidget->SetDialogueComponent(ComponentToResume);
    }
    
    // 4. 뷰포트에 추가
    DialogueWidgetInstance->AddToViewport();
    
    // 5. 입력 모드 UI로 설정
    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(DialogueWidgetInstance->TakeWidget());
    SetInputMode(InputMode);
    
    // 6. 마우스 커서 표시
    bShowMouseCursor = true;

    // 7. 델리게이트 다시 바인딩
    ActiveDialogueComponent = ComponentToResume;
    ActiveDialogueComponent->OnEventTriggered.AddDynamic(this, &ATrainPlayerController_KJH::HandleDialogueEvent);

    // 8. [중요] BeginDialogue가 아닌 AdvanceDialogue로 저장된 ID부터 시작
    ActiveDialogueComponent->AdvanceDialogue(NextID);
}