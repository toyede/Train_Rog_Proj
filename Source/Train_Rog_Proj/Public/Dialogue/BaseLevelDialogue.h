// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dialogue/InteractInterface.h" // 3단계에서 만든 인터페이스 포함
#include "BaseLevelDialogue.generated.h"

class UDialogueComponent; // 전방 선언

UCLASS()
class TRAIN_ROG_PROJ_API ABaseLevelDialogue : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseLevelDialogue();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UDialogueComponent* DialogueComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 인터페이스 함수 구현 (UE에서는 구현함수명__Implementation라는 형식으로 구현함)
    virtual void Interact_Implementation(AActor* Interactor) override;

};
