// UMyVideoWidgetBase.cpp

#include "Dialogue/UMyVideoWidgetBase.h" // (경로는 실제 파일 위치에 맞게 수정)
#include "Dialogue/TrainPlayerController_KJH.h" // (경로는 실제 파일 위치에 맞게 수정)
#include "Kismet/GameplayStatics.h"

// 생성자 구현
UMyVideoWidgetBase::UMyVideoWidgetBase(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 이 위젯이 포커스를 받을 수 있도록 설정합니다. (CTRL 스킵 입력을 위해)
    bIsFocusable = true;
}

// 위젯이 영상 종료/스킵을 알리는 함수
void UMyVideoWidgetBase::NotifyVideoFinished()
{
    // 오너(PlayerController)를 찾아서 HideVideoUI 함수를 호출합니다.
    if (ATrainPlayerController_KJH* PC = Cast<ATrainPlayerController_KJH>(UGameplayStatics::GetPlayerController(this, 0)))
    {
        PC->HideVideoUI();
    }
}