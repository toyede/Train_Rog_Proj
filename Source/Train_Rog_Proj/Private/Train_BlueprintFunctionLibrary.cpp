// Fill out your copyright notice in the Description page of Project Settings.

#include "Train_BlueprintFunctionLibrary.h"
#include "EnhancedActionKeyMapping.h"

UInputAction* UTrain_BlueprintFunctionLibrary::FindInputActionByKey(UInputMappingContext* MappingContext, FKey Key)
{
    if (!MappingContext)
        return nullptr;

    const TArray<FEnhancedActionKeyMapping>& Mappings = MappingContext->GetMappings();
    for (const FEnhancedActionKeyMapping& Mapping : Mappings)
    {
        if (Mapping.Key == Key)
        {
            return const_cast<UInputAction*>(Mapping.Action.Get()); // 매칭된 액션 반환
        }
    }
    return nullptr; // 해당 키가 매핑된 액션 없음
}

bool UTrain_BlueprintFunctionLibrary::IsSameKey(FKey KeyA, FKey KeyB)
{
    return KeyA == KeyB; // 두 키가 동일한지 비교
}