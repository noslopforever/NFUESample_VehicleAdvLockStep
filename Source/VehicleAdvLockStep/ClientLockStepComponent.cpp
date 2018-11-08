// Under MIT licensee, see github page (https://github.com/noslopforever/NFUESample_VehicleAdvLockStep) for more informations.
#include "ClientLockStepComponent.h"
#include "GameLockStepComponent.h"
#include "VehicleAdvLockStepPawn.h"
#include "LockStepActorInterface.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "PhysicsPublic.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Containers/StringConv.h"
#include "LockStepWheeledVehicleMoveComp4W.h"

// Sets default values for this component's properties
UClientLockStepComponent::UClientLockStepComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	ClientStepIndex = -1;
}


// Called when the game starts
void UClientLockStepComponent::BeginPlay()
{
	Super::BeginPlay();

	// Close the world's default physics simulation.
	check(GetWorld());
	GetWorld()->bShouldSimulatePhysics = false;

}

void UClientLockStepComponent::OnRegister()
{
	Super::OnRegister();

	// Attach tick delegate.
	if (!PostActorTickHandle.IsValid()) {
		PostActorTickHandle = FWorldDelegates::OnWorldPostActorTick.AddUObject(this, &UClientLockStepComponent::OnWorldPostActorTick);
	}
}

void UClientLockStepComponent::OnUnregister()
{
	Super::OnUnregister();

	// Detach tick delegate.
	if (PostActorTickHandle.IsValid()) {
		FWorldDelegates::OnWorldPostActorTick.Remove(PostActorTickHandle);
		PostActorTickHandle.Reset();
	}
}

// Called every frame
void UClientLockStepComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UClientLockStepComponent::S2C_StepAdvance_Validate(const FStepActionInfo& InStepActionInfo)
{
	return true;
}

void UClientLockStepComponent::S2C_StepAdvance_Implementation(const FStepActionInfo& InStepActionInfo)
{
	// Push step info to the queue.
	ClientQueuedSteps.Add(InStepActionInfo);
}

bool UClientLockStepComponent::C2S_RequestStep_Validate(const FString& InExecCode)
{
	return true;
}

void UClientLockStepComponent::C2S_RequestStep_Implementation(const FString& InExecCode)
{
	// Add client's request to game state's action list.
	APlayerController* PlayerController = CastChecked<APlayerController>(GetOwner());
	check(GetWorld());
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	checkf(GameMode, TEXT("C2S_RequestStep must be called with authority."));

	UGameLockStepComponent* GameLockStepComp = GameMode->FindComponentByClass<UGameLockStepComponent>();
	if (ensureMsgf(GameLockStepComp, TEXT("UClientLockStepComponent must pair with UGameLockStepComponent."))) {
		// Add actions to current step's action list.
		FActionInfo ActionInfo;
		ActionInfo.PlayerId = PlayerController->PlayerState->PlayerId;
		ActionInfo.ExecCode = InExecCode;
		GameLockStepComp->ServerCurrentStepInfo.StepActions.Add(ActionInfo);
	}
}


AVehicleAdvLockStepPawn* UClientLockStepComponent::StaticCreateLockStepPawn(UObject* InWorldContext, FTransform InTransform)
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(InWorldContext);
	// Do nothing for dedicated server, only run on clients.
	if (World->GetNetMode() == NM_DedicatedServer) {
		return nullptr;
	}

	APlayerController* Controller = GEngine->GetFirstLocalPlayerController(World);
	check(Controller);
	UClientLockStepComponent* LockStepComponent = Controller->FindComponentByClass<UClientLockStepComponent>();
	if (ensure(LockStepComponent)) {
		FActorSpawnParameters Parameters;
		Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AVehicleAdvLockStepPawn* Pawn = World->SpawnActor<AVehicleAdvLockStepPawn>(AVehicleAdvLockStepPawn::StaticClass(), InTransform, Parameters);
		if (Pawn) {
			Pawn->LockStepPawnId = LockStepComponent->Pawns.Add(Pawn);
			Pawn->OnDestroyed.AddDynamic(LockStepComponent, &UClientLockStepComponent::OnPawnDestroyed);
		}
		return Pawn;
	}

	return nullptr;
}

AVehicleAdvLockStepPawn* UClientLockStepComponent::StaticFindLockStepPawnById(UWorld* InWorld, int32 InPawnId)
{
	check(InWorld);

	// Do nothing for dedicated server, only run on clients.
	if (InWorld->GetNetMode() == NM_DedicatedServer) {
		return nullptr;
	}

	APlayerController* Controller = GEngine->GetFirstLocalPlayerController(InWorld);
	check(Controller);
	UClientLockStepComponent* LockStepComponent = Controller->FindComponentByClass<UClientLockStepComponent>();
	if (ensure(LockStepComponent)) {
		return LockStepComponent->Pawns[InPawnId];
	}
	return nullptr;
}
void UClientLockStepComponent::GetSortedPlayerArrays(AGameStateBase* InGameState, TArray<APlayerState*>& OutPlayerStates)
{
	if (!InGameState) {
		return;
	}

	OutPlayerStates = InGameState->PlayerArray;
	OutPlayerStates.Sort([](APlayerState& InLhs, APlayerState& InRhs) -> bool {
		return InLhs.PlayerId < InRhs.PlayerId;
	});
}

void UClientLockStepComponent::ExecMoveForward(int32 InPawnId, float InValue)
{
	AVehicleAdvLockStepPawn* Pawn = UClientLockStepComponent::StaticFindLockStepPawnById(GetWorld(), InPawnId);
	if (ensure(Pawn)) {
		Pawn->GetVehicleMovementComponent()->SetThrottleInput(InValue);
	}
}

void UClientLockStepComponent::ExecMoveRight(int32 InPawnId, float InValue)
{
	AVehicleAdvLockStepPawn* Pawn = UClientLockStepComponent::StaticFindLockStepPawnById(GetWorld(), InPawnId);
	if (ensure(Pawn)) {
		Pawn->GetVehicleMovementComponent()->SetSteeringInput(InValue);
	}
}

void UClientLockStepComponent::ExecHandbrake(int32 InPawnId, int32 InValue)
{
	AVehicleAdvLockStepPawn* Pawn = UClientLockStepComponent::StaticFindLockStepPawnById(GetWorld(), InPawnId);
	if (ensure(Pawn)) {
		Pawn->GetVehicleMovementComponent()->SetHandbrakeInput(InValue != 0);
	}
}

void UClientLockStepComponent::OnPawnDestroyed(AActor* InDestroyedActor)
{
	AVehicleAdvLockStepPawn* Pawn = CastChecked<AVehicleAdvLockStepPawn>(InDestroyedActor);
	Pawns.RemoveAt(Pawn->LockStepPawnId);
	Pawn->LockStepPawnId = INDEX_NONE;
}

void UClientLockStepComponent::OnWorldPostActorTick(UWorld* InWorld, ELevelTick InTickType, float InDeltaSeconds)
{
	if (InWorld != this->GetWorld()) {
		return;
	}

	// Do nothing for dedicated server, only run on clients.
	if (InWorld->GetNetMode() == NM_DedicatedServer) {
		return;
	}

	// Try do client steps.
	ProcessQueuedSteps();
}

void UClientLockStepComponent::ProcessQueuedSteps()
{
	for (int StepIndex = 0; StepIndex < ClientQueuedSteps.Num(); StepIndex++)
	{
		FStepActionInfo StepActionInfo = ClientQueuedSteps[StepIndex];

		// # Client do step advance immediately.

		// Check if Step index matched.
		if (StepActionInfo.StepIndex != (ClientStepIndex + 1)) {
			UE_LOG(LogScript, Error, TEXT("Error : StepAdvance Message cannot match client state!"));
			return;
		}

		// Client now has the same step index with the server.
		ClientStepIndex = StepActionInfo.StepIndex;

		// # Execute all actions got from server.
		for (int ActionIndex = 0; ActionIndex < StepActionInfo.StepActions.Num(); ActionIndex++) {
			const FActionInfo& ActionInfo = StepActionInfo.StepActions[ActionIndex];

			// Execute player action.
			ExecutePlayerAction(ActionInfo.PlayerId, ActionInfo.ExecCode);
		}

		const float DeltaTime = UGameLockStepComponent::StaticGetStepAdvanceTime();

		// # Do logic and physics tick

		// ## PrePhysics Tick

		// For each LockStepActorInterface.
		TArray<AActor*> LockStepActors;
		UGameplayStatics::GetAllActorsWithInterface(this, ULockStepActorInterface::StaticClass(), LockStepActors);
		for (int ActorIndex = 0; ActorIndex < LockStepActors.Num(); ActorIndex++) {
			AActor* Actor = LockStepActors[ActorIndex];

			// Tell them to do a step advance.
			ILockStepActorInterface::Execute_ReceiveStepAdvance(Actor, ClientStepIndex, DeltaTime, TG_PrePhysics);
		}

		// ## Physics update
		check(GetWorld());
		StaticUpdateClientPhysics(GetWorld(), DeltaTime);

		// ## PostPhysics Tick
		for (int ActorIndex = 0; ActorIndex < LockStepActors.Num(); ActorIndex++) {
			AActor* Actor = LockStepActors[ActorIndex];

			// Tell them to do a step advance.
			ILockStepActorInterface::Execute_ReceiveStepAdvance(Actor, ClientStepIndex, DeltaTime, TG_PostPhysics);
		}
	}
	ClientQueuedSteps.Empty();
}

void UClientLockStepComponent::StaticUpdateClientPhysics(UWorld* InWorldContext, float InFrameTickTime)
{
	const float FrameTickTime = InFrameTickTime;


	// Update physics scene.
	FPhysScene* PhysScene = InWorldContext->GetPhysicsScene();

#if WITH_PHYSX
	// When ticking the main scene, clean up any physics engine resources (once a frame)
	DeferredPhysResourceCleanup();
#endif

	// Update gravity in case it changed
	const FVector Gravity(0, 0, UPhysicsSettings::Get()->DefaultGravityZ);
	PhysScene->SetUpForFrame(&Gravity, FrameTickTime, FrameTickTime);

	// Scene start / wait / end routes.
	PhysScene->StartFrame();
	PhysScene->WaitPhysScenes();
	PhysScene->EndFrame(nullptr);

	// TODO: Do some clamp to make it same with all clients.
}

void UClientLockStepComponent::ExecutePlayerAction(int32 InPlayerId, const FString& InExecCode)
{
	// Check and run console command in client.
	ProcessConsoleExec(*InExecCode, *GLog, nullptr);
}

void UClientLockStepComponent::PushCommand(const FString& InCmd)
{
	C2S_RequestStep(InCmd);
}

