#include "Dialogue/BaseNPC.h"
#include "Dialogue/DialogueComponent.h"
#include "Kismet/GameplayStatics.h"      // UGameplayStatics 사용을 위해 반드시 필요합니다.
#include "Dialogue/TrainPlayerController_KJH.h"  // ATrainPlayerController 사용을 위해 반드시 필요합니다. (경로는 실제 파일 위치에 맞게 수정해주세요)

ABaseNPC::ABaseNPC()
{
    PrimaryActorTick.bCanEverTick = false;

    // DialogueComponent 생성 및 부착
    DialogueComponent = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
}

// Interact 인터페이스의 C++ 구현 (헤더와 일치하는 AActor* 버전만 남깁니다)
void ABaseNPC::Interact_Implementation(AActor* Interactor)
{
    // 1. 월드에 대한 유효한 포인터가 있는지 확인합니다.
    UWorld* World = GetWorld();
    if (World == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("NoPointer"));
        return;
    }

    // 2. 플레이어 컨트롤러를 가져옵니다.
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
    if (PlayerController == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("NoController"));
        return;
    }

    // 3. 우리가 만든 C++ 기반 Player Controller로 형 변환(Cast)을 시도합니다.
    ATrainPlayerController_KJH* TrainPC = Cast<ATrainPlayerController_KJH>(PlayerController);
    
    // 4. 형 변환이 성공했다면, ShowDialogueUI 함수를 호출합니다.
    if (TrainPC)
    {
        // 블루프린트에서 구현된 ShowDialogueUI 이벤트를 호출합니다.
        // 파라미터로 이 NPC 자신의 DialogueComponent를 전달합니다.
        TrainPC->ShowDialogueUI(DialogueComponent);
    }
}