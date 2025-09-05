// UMyDialogueWidgetBase.cpp

#include "Dialogue/UMyDialogueWidgetBase.h" // 헤더 파일 경로

// 생성자 구현을 추가합니다.
UMyDialogueWidgetBase::UMyDialogueWidgetBase(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 이 위젯이 포커스를 받을 수 있도록 설정합니다.
    bIsFocusable = true;
}

void UMyDialogueWidgetBase::SetDialogueComponent(UDialogueComponent* ComponentToSet)
{
    this->DialogueComponent = ComponentToSet;
}