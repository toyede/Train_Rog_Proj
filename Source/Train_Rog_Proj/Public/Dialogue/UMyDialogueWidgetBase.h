// UMyDialogueWidgetBase.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMyDialogueWidgetBase.generated.h"

class UDialogueComponent; // 전방 선언

UCLASS()
class TRAIN_ROG_PROJ_API UMyDialogueWidgetBase : public UUserWidget
{
    GENERATED_BODY()

public:
    // 생성자를 추가합니다.
    UMyDialogueWidgetBase(const FObjectInitializer& ObjectInitializer);

    // 블루프린트에서 이 컴포넌트를 읽어서 사용할 수 있도록 BlueprintReadOnly로 설정합니다.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialogue")
    UDialogueComponent* DialogueComponent;

    // PlayerController가 호출할 public 함수입니다.
    // UFUNCTION으로 선언할 필요는 없습니다. C++ 전용 함수입니다.
    void SetDialogueComponent(UDialogueComponent* ComponentToSet);
};