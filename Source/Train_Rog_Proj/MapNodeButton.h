// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "MapNode.h"
#include "MapNodeButton.generated.h"

/**
 * 
 */
UCLASS()
class TRAIN_ROG_PROJ_API UMapNodeButton : public UUserWidget
{
	GENERATED_BODY()
	
	public:
    // 연결된 노드 데이터
    UPROPERTY(BlueprintReadOnly, Category = "Node Data")
    UMapNode* LinkedNode;

    // UI 컴포넌트들
    UPROPERTY(BlueprintReadOnly, Category = "UI Components", meta = (BindWidget))
    class UButton* NodeButton;

    UPROPERTY(BlueprintReadOnly, Category = "UI Components", meta = (BindWidget))
    class UImage* NodeIcon;

    UPROPERTY(BlueprintReadOnly, Category = "UI Components", meta = (BindWidget))
    class UTextBlock* NodeText;

    // 노드 상태별 스타일
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Styles")
    class UTexture2D* DefaultIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Styles")
    class UTexture2D* AccessibleIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Styles")
    class UTexture2D* VisitedIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Styles")
    class UTexture2D* CurrentIcon;

    // 블루프린트에서 호출 가능한 함수들
    UFUNCTION(BlueprintCallable, Category = "Node Button")
    void SetupNodeButton(UMapNode* Node);

    UFUNCTION(BlueprintCallable, Category = "Node Button")
    void UpdateNodeButtonState();

    UFUNCTION(BlueprintCallable, Category = "Node Button")
    void SetNodeAccessible(bool bAccessible);

    UFUNCTION(BlueprintCallable, Category = "Node Button")
    void SetNodeCurrent(bool bCurrent);
};
