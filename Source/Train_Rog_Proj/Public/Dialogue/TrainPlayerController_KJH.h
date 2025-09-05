// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TrainPlayerController_KJH.generated.h"

class UDialogueComponent;
class UMyDialogueWidgetBase; // UUserWidget 클래스를 사용하기 위한 전방 선언

UCLASS()
class TRAIN_ROG_PROJ_API ATrainPlayerController_KJH : public APlayerController
{
	GENERATED_BODY()

	protected:
		// **블루프린트에서 채워 줄** 위젯 클래스를 담을 변수
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
		TSubclassOf<UMyDialogueWidgetBase> DialogueWidgetClass;

		// 생성된 대화 위젯의 인스턴스를 담을 변수
		UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "UI")
		UMyDialogueWidgetBase* DialogueWidgetInstance;

		// 현재 활성화된 DialogueComponent를 저장하기 위한 포인터
		UPROPERTY()
		TObjectPtr<UDialogueComponent> ActiveDialogueComponent;

	public:
		// 대화 UI를 호출하는 함수
		UFUNCTION(BlueprintCallable, Category = "UI")
		void ShowDialogueUI(UDialogueComponent* ComponentToUse);
		
		// 대화 종료 시 호출할 함수
		UFUNCTION(BlueprintCallable, Category = "UI")
		void HideDialogueUI();

		// DialogueComponent의 OnEventTriggered 델리게이트에 바인딩될 함수
		UFUNCTION()
		void HandleDialogueEvent(FName EventTag);
};
