// Under MIT licensee, see github page (https://github.com/noslopforever/NFUESample_VehicleAdvLockStep) for more informations.

#include "GameLockStepComponent.h"
#include "Engine/World.h"
#include "Public/EngineUtils.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "ClientLockStepComponent.h"

// Sets default values for this component's properties
UGameLockStepComponent::UGameLockStepComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bLockStepAutoAdvanced = 1;
	bLockStepStarted = 0;

	// ...
}


// Called when the game starts
void UGameLockStepComponent::BeginPlay()
{
	Super::BeginPlay();

	bool bWorldSupport = false;
	UWorld* World = GetWorldChecked(bWorldSupport);

	// In server mode, Close all in-map actor's replication and tick,
	// as it may be set in default but cannot be handled in lock-step state.
	if (World->GetNetMode() == NM_DedicatedServer || World->GetNetMode() == NM_ListenServer) {
		for (TActorIterator<AActor> It(World); It; ++It) {
			AActor* Actor = *It;

			// All GameState, GameMode, PlayerController, and PlayerState will stay in default sync route.
			if (!Actor || Actor->IsPendingKill()
				|| Actor->IsA(AGameStateBase::StaticClass())
				|| Actor->IsA(AGameModeBase::StaticClass())
				|| Actor->IsA(APlayerController::StaticClass())
				|| Actor->IsA(APlayerState::StaticClass())
				) {
				continue;
			}

			Actor->SetReplicates(false);
			Actor->SetActorTickEnabled(false);
		}
	}

	// Close world's default physics simulation route.
	// We do physics simulate in client's step advance.
	World->bShouldSimulatePhysics = false;


}

void UGameLockStepComponent::EndPlay(EEndPlayReason::Type InEndPlayReason)
{
	// TODO do some notifications.
}

// Called every frame
void UGameLockStepComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If in server-mode, 
	if (GetOwner() && GetOwner()->HasAuthority() && bLockStepStarted) {
		if (bLockStepAutoAdvanced) {
			// Do server step advance and tick.
			const float StepAdvTime = StaticGetStepAdvanceTime();

			// Accumulate delta time and check how much frames need to step.
			FrameTimeAccum += DeltaTime;
			while (FrameTimeAccum > StepAdvTime) {
				// Go steps by time.
				FrameTimeAccum -= StepAdvTime;

				// Do step advance.
				AuthDoStepAdvance();
			}
		}

		return;
	}
}

float UGameLockStepComponent::StaticGetStepAdvanceTime()
{
	return 5e-2f;
}

float UGameLockStepComponent::StaticGetActionsSyncTime()
{
	return 2e-1f;
}

void UGameLockStepComponent::AuthStartLockStepGame()
{
	// This method should only be called once during a game.
	if (bLockStepStarted) {
		return;
	}

	bool bWorldSupported = false;
	UWorld* World = GetWorldChecked(bWorldSupported);

	// Check if world is authority.
	checkf(World->GetAuthGameMode(), TEXT("AuthStartLockStepGame must be called with authority."));

	for (TPlayerControllerIterator<APlayerController>::ServerAll It(World); It; ++It) {
		APlayerController* PlayerCtrl = *It;

		// Ensure the player has a ClientLockStepComponent.
		UClientLockStepComponent* LockStepComp = PlayerCtrl->FindComponentByClass<UClientLockStepComponent>();
		if (!LockStepComp) {
			// New Component
			static FName Name_LockStepComp(TEXT("LockStepComp"));
			LockStepComp = ::NewObject<UClientLockStepComponent>(PlayerCtrl, UClientLockStepComponent::StaticClass(), Name_LockStepComp);

			// Tell Actor it has a new component.
			PlayerCtrl->AddOwnedComponent(LockStepComp);

			// Register the new component to the actor.
			LockStepComp->RegisterComponent();
		}
		// Let it replicable.
		if (ensure(LockStepComp)) {
			LockStepComp->SetIsReplicated(true);
		}
	}

	// Set LockStepStarted to true
	bLockStepStarted = 1;
}

//void UGameLockStepComponent::AuthManualStep(bool InUseManualStep /*= true*/)
//{
//	// Close server automatic step advance.
//	bServerAutoStep = 0;
//
//	// Do a step advance.
//	DoStepAdvance();
//}

void UGameLockStepComponent::AuthDoStepAdvance()
{
	bool bWorldSupported = false;
	UWorld* World = GetWorldChecked(bWorldSupported);

	// Check if world is authority.
	if (!World->GetAuthGameMode()) {
		UE_LOG(LogScript, Error, TEXT("AuthDoStepAdvance must be called in authority mode."));
		return;
	}

	// TODO : Save old step info to the store.

	// Mark the new step info.
	ServerCurrentStepInfo.StepIndex++;

	// Tell all player to step the advance.
	for (TPlayerControllerIterator<APlayerController>::ServerAll It(World); It; ++It) {
		APlayerController* PlayerCtrl = *It;

		UClientLockStepComponent* LockStepComp = PlayerCtrl->FindComponentByClass<UClientLockStepComponent>();
		if (!LockStepComp) {
			UE_LOG(LogScript, Error, TEXT("AuthDoStepAdvance : The game has started with some invalid players."));
			continue;
		}
		LockStepComp->S2C_StepAdvance(ServerCurrentStepInfo);
	}

	// Cleanup current step datas.
	ServerCurrentStepInfo.StepActions.Empty(256);
}
