// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TrainPlayerController_KJH.generated.h"

class UDialogueComponent;

UCLASS()
class TRAIN_ROG_PROJ_API ATrainPlayerController_KJH : public APlayerController
{
	GENERATED_BODY()

	public:
		// 이 함수의 실제 구현(로직)은 자식 블루프린트에 있음
		UFUNCTION(BlueprintImplementableEvent, Category = "UI")
		void ShowDialogueUI(UDialogueComponent* ComponentToUse);
};
