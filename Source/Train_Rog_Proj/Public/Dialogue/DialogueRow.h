#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" // FTableRowBase를 사용하기 위해 꼭 필요!
#include "DialogueRow.generated.h" // USTRUCT 매크로를 위해 꼭 필요!

// USTRUCT 매크로를 붙여 언리얼이 인식하도록 함
USTRUCT(BlueprintType)
struct FDialogueChoice
{
    GENERATED_BODY()

    // 선택지에 표시될 텍스트
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString ChoiceText;

    // 이 선택지를 골랐을 때 다음으로 이어질 대화의 ID
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName NextDialogueID;
};

// 데이터 테이블의 한 행을 정의할 구조체
USTRUCT(BlueprintType)
struct FDialogueRow : public FTableRowBase
{
    GENERATED_BODY()

    // 대화의 화자 이름
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString SpeakerName;

    // 실제 대화 내용
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString DialogueText;
    
    // 플레이어 선택지 배열 (선택지가 없는 대사일 경우 비워둠)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FDialogueChoice> Choices;

    // (선택지가 없을 경우) 다음 대화로 바로 연결될 ID
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName NextDialogueID;
    
    // 이 대사가 출력될 때 발생시킬 이벤트 태그 (예: 퀘스트 시작, 호감도 상승 등)
    // 비워둬도 상관 없습니다.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName EventTag;
};