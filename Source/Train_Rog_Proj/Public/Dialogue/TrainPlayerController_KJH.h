// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MediaSource.h"
#include "TrainPlayerController_KJH.generated.h"

class UMyVideoWidgetBase;
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

		// 상점 위젯 클래스
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
		TSubclassOf<UUserWidget> ShopWidgetClass;

		// 상점 위젯 인스턴스
		UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "UI")
		UUserWidget* ShopWidgetInstance;

		// [추가] 비디오 위젯 클래스 (블루프린트에서 UMyVideoWidgetBase 상속받은 것으로 설정)
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
		TSubclassOf<UMyVideoWidgetBase> VideoWidgetClass;

		// [추가] 비디오 위젯 인스턴스
		UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "UI")
		UMyVideoWidgetBase* VideoWidgetInstance;

		// [추가] 영상 해시맵 (블루프린트에서 채울 수 있도록 설정)
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Video")
		TMap<FName, UMediaSource*> VideoMap;

		// [추가] 대화 재개를 위한 상태 저장 변수
		UPROPERTY()
		TObjectPtr<UDialogueComponent> PausedDialogueComponent;

		// [추가] 대화 재개를 위한 다음 대화 ID 저장 변수
		UPROPERTY()
		FName ResumeDialogueID;

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

		// 상점 UI 호출 함수
		UFUNCTION(BlueprintCallable, Category = "UI")
		void ShowShopUI();

		// 상점 UI 닫기 함수
		UFUNCTION(BlueprintCallable, Category = "UI")
		void HideShopUI();

		// DialogueComponent의 OnEventTriggered 델리게이트에 바인딩될 함수
		UFUNCTION()
		void HandleDialogueEvent(FName EventTag);

		// [추가] 비디오 UI 호출 함수 (VideoKey를 받음)
		void ShowVideoUI(FName VideoKey);
			
		// [추가] 비디오 UI 닫기 함수
		UFUNCTION(BlueprintCallable, Category = "UI")
		void HideVideoUI();

		// [추가] 대화 재개 전용 함수
    	void ResumeDialogueUI(UDialogueComponent* ComponentToResume, FName NextID);
};
