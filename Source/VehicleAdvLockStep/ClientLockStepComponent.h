// Under MIT licensee, see github page (https://github.com/noslopforever/NFUESample_VehicleAdvLockStep) for more informations.

#pragma once

#include "CoreMinimal.h"
#include "ServerLockStepComponent.h"
#include "Components/ActorComponent.h"
#include "ClientLockStepComponent.generated.h"

/** Player lockstep management component.
*/
UCLASS(ClassGroup=(Custom), Within=PlayerController)
class VEHICLEADVLOCKSTEP_API UClientLockStepComponent
	: public UActorComponent
{
	GENERATED_BODY()
public:	
	// Sets default values for this component's properties
	UClientLockStepComponent();

	/** Client queued steps
	*/
	UPROPERTY()
	TArray<FStepActionInfo> ClientQueuedSteps;

	/** Client step index */
	UPROPERTY()
	uint32 ClientStepIndex;

	/** Client actions from last step, will be sent to server. */
	UPROPERTY()
	TArray<FActionInfo> ClientActions;

	/** PostActorTick delegate handle. */
	FDelegateHandle PostActorTickHandle;

	/** Client pawns. */
	TSparseArray<class AVehicleAdvLockStepPawn*> Pawns;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Run in client only, handling 'step advance' message from server. */
	UFUNCTION(Client, Reliable, WithValidation)
	void S2C_StepAdvance(const FStepActionInfo& InStepActionInfo);

	/** Run in server only, handling client's step request. */
	UFUNCTION(Server, Reliable, WithValidation)
	void C2S_RequestStep(const FString& InExecCode);

	/** Push client command.
	*/
	void PushCommand(const FString& InCmd);

public:

	/** Spawn pawn and assign id for it. Replace default SpawnActor node.
	*/
	UFUNCTION(BlueprintCallable, Category = "LockStep", meta = (WorldContext = InWorldContext))
	static class AVehicleAdvLockStepPawn* StaticCreateLockStepPawn(UObject* InWorldContext, FTransform InTransform);

	/** Find Vehicle by Pawn Id. */
	static class AVehicleAdvLockStepPawn* StaticFindLockStepPawnById(UWorld* InWorld, int32 InPawnId);

	/** Get the InGameState's PlayerState array and sort it by PlayerId. */
	UFUNCTION(BlueprintPure, Category = "LockStep")
	static void GetSortedPlayerArrays(AGameStateBase* InGameState, TArray<APlayerState*>& OutPlayerStates);

public:

	//
	// # Common game exec functions
	//

	UFUNCTION(exec)
	void ExecMoveForward(int32 InPawnId, float InValue);

	UFUNCTION(exec)
	void ExecMoveRight(int32 InPawnId, float InValue);

	UFUNCTION(exec)
	void ExecHandbrake(int32 InPawnId, int32 InValue);


private:

	/** Called when pawn destroyed. */
	UFUNCTION()
	void OnPawnDestroyed(AActor* InDestroyedActor);

	/** world steps tick.
	*/
	void OnWorldPostActorTick(UWorld* InWorld, ELevelTick InTickType, float InDeltaSeconds);



	/** Process queued steps.
	*/
	void ProcessQueuedSteps();

	/** Update client physics.
	*/
	static void StaticUpdateClientPhysics(UWorld* InWorldContext, float InFrameTickTime);

	/** Run in client only, execute player action code.
	*/
	void ExecutePlayerAction(int32 InPlayerId, const FString& InExecCode);
};
