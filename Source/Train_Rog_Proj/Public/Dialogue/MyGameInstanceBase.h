// MyGameInstanceBase.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstanceBase.generated.h"

UCLASS()
class TRAIN_ROG_PROJ_API UMyGameInstanceBase : public UGameInstance
{
    GENERATED_BODY()

public:
	UMyGameInstanceBase();

    // 블루프린트에서 수정 및 읽기가 가능하도록 설정합니다.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
    float Bear;
};