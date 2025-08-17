// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputMappingContext.h"
#include "InputAction.h"
#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Train_BlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TRAIN_ROG_PROJ_API UTrain_BlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Input")
	static UInputAction* FindInputActionByKey(UInputMappingContext* MappingContext, FKey Key);

	UFUNCTION(BlueprintPure, Category = "Input")
	static bool IsSameKey(FKey KeyA, FKey KeyB);
};
