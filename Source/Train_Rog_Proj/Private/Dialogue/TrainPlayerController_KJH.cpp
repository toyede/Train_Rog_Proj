// TrainPlayerController_KJH.cpp

#include "Dialogue/TrainPlayerController_KJH.h" // 헤더 파일 경로
#include "Blueprint/UserWidget.h" // UUserWidget, CreateWidget 사용
#include "Dialogue/DialogueComponent.h" // UDialogueComponent 사용
#include "Dialogue/UMyDialogueWidgetBase.h" // 새로 만든 헤더 include
#include "Dialogue/MyGameInstanceBase.h" // 이제 C++ 헤더를 포함합니다.
#include "UObject/UnrealType.h" // FProperty 등 리플렉션 시스템 사용을 위해 필요
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
        }
        // 다른 명명된 이벤트들은 else if 로 추가...
    }
}