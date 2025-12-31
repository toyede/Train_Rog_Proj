// UMyVideoWidgetBase.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MediaSource.h"
#include "UMyVideoWidgetBase.generated.h"

UCLASS()
class TRAIN_ROG_PROJ_API UMyVideoWidgetBase : public UUserWidget
{
    GENERATED_BODY()

public:
    // 생성자 (포커스를 받아 스킵 키 입력을 감지하기 위함)
    UMyVideoWidgetBase(const FObjectInitializer& ObjectInitializer);

    // C++(PlayerController)에서 이 변수에 재생할 미디어를 채워줍니다.
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Video")
    UMediaSource* MediaSourceToPlay;

    // C++에서 이 이벤트를 호출하면, 블루프린트에서 실제 재생 로직이 실행됩니다.
    UFUNCTION(BlueprintImplementableEvent, Category = "Video")
    void PlayVideo();

    // 블루프린트 위젯에서 영상 재생이 끝나거나 스킵될 때 이 함수를 호출해야 합니다.
    // 그러면 이 함수가 PlayerController의 HideVideoUI를 호출합니다.
    UFUNCTION(BlueprintCallable, Category = "Video")
    void NotifyVideoFinished();
};