// DialogueComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Dialogue/DialogueRow.h" // 1단계에서 만든 헤더파일 포함
#include "DialogueComponent.generated.h"

// UI로 데이터 전달 및 이벤트 알림을 위한 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueUpdated, const FDialogueRow&, DialogueRow);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventTriggered, FName, EventTag);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TRAIN_ROG_PROJ_API UDialogueComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    UDialogueComponent();

    // 이 컴포넌트가 사용할 데이터 테이블
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
    UDataTable* DialogueDataTable;

    // 대화 시작 시 사용할 첫 대사의 ID
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
    FName InitialDialogueID;

    // UI가 바인딩(구독)할 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Dialogue")
    FOnDialogueUpdated OnDialogueUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Dialogue")
    FOnDialogueFinished OnDialogueFinished;

    UPROPERTY(BlueprintAssignable, Category = "Dialogue")
    FOnEventTriggered OnEventTriggered;

    // 대화를 시작하는 함수
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void BeginDialogue();

    // 다음 대사로 진행하는 함수 (선택지 또는 ID를 통해)
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void AdvanceDialogue(FName NextID);

    FDialogueRow* GetCurrentDialogueRow() const { return CurrentDialogueRow; }

protected:
    // 현재 대화 데이터
    FDialogueRow* CurrentDialogueRow;
};