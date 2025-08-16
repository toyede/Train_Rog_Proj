// BaseNPC.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Dialogue/InteractInterface.h" // 3단계에서 만든 인터페이스 포함
#include "BaseNPC.generated.h"

class UDialogueComponent; // 전방 선언

UCLASS()
class TRAIN_ROG_PROJ_API ABaseNPC : public ACharacter, public IInteractInterface
{
    GENERATED_BODY()

public:
    ABaseNPC();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UDialogueComponent* DialogueComponent;

public:
    // 인터페이스 함수 구현 (UE에서는 구현함수명__Implementation라는 형식으로 구현함)
    virtual void Interact_Implementation(AActor* Interactor) override;
};