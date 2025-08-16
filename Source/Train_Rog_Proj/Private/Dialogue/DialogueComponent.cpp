// DialogueComponent.cpp

#include "Dialogue/DialogueComponent.h"

UDialogueComponent::UDialogueComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    CurrentDialogueRow = nullptr;
}

void UDialogueComponent::BeginDialogue()
{
    // 데이터 테이블이나 시작 ID가 없으면 아무것도 안 함
    if (!DialogueDataTable || InitialDialogueID.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("데이터 테이블이나 시작 ID 없음. 재확인 필요"));
        return;
    }
    AdvanceDialogue(InitialDialogueID);
}

void UDialogueComponent::AdvanceDialogue(FName NextID)
{
    // ID가 "End"이거나 유효하지 않으면 대화 종료
    if (NextID.IsNone() || NextID == "End")
    {
        CurrentDialogueRow = nullptr;
        OnDialogueFinished.Broadcast(); // "대화 끝났음!" 이벤트 방송
        return;
    }

    // 데이터 테이블에서 ID에 해당하는 행을 찾음
    static const FString ContextString(TEXT("Dialogue System"));
    CurrentDialogueRow = DialogueDataTable->FindRow<FDialogueRow>(NextID, ContextString, true);

    if (CurrentDialogueRow)
    {
        // 행을 찾았으면 "대화 업데이트됨!" 이벤트 방송
        OnDialogueUpdated.Broadcast(*CurrentDialogueRow);
    }
    else
    {
        // 행을 못 찾았으면 대화 종료
        OnDialogueFinished.Broadcast();
    }
}