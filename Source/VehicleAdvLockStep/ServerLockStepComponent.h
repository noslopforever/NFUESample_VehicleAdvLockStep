// Under MIT licensee, see github page (https://github.com/noslopforever/NFUESample_VehicleAdvLockStep) for more informations.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleAdvLockStep.h"
#include "ServerLockStepComponent.generated.h"

/** Action sent from clients.
*/
USTRUCT()
struct FActionInfo
{
	GENERATED_BODY()
public:
	FActionInfo() {
		PlayerId = -1;
	}

	/** PlayerId, equal to some PlayerState's PlayerId.
	*/
	UPROPERTY()
	int32 PlayerId;

	/** Exec code
	*	Should be passed to PlayerController::ConsoleCommand.
	*/
	UPROPERTY()
	FString ExecCode;

};

USTRUCT()
struct FStepActionInfo
{
	GENERATED_BODY()
public:
	FStepActionInfo() {
		StepIndex = -1;
	}

	/** Step index. */
	UPROPERTY()
	int32 StepIndex;

	/** Step actions. */
	UPROPERTY()
	TArray<FActionInfo> StepActions;

};
/** Game lockstep component.
*/
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEHICLEADVLOCKSTEP_API UServerLockStepComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UServerLockStepComponent();

	/** Actions gathered from all clients of this frame. */
	UPROPERTY()
	FStepActionInfo ServerCurrentStepInfo;

	/** Elapsed time. */
	UPROPERTY()
	float FrameTimeAccum;

	/** Server step automatic advanced. */
	UPROPERTY()
	uint32 bLockStepAutoAdvanced : 1;

	/** Is server lock step started. */
	UPROPERTY()
	uint32 bLockStepStarted : 1;

	///** Server step archives. */
	//UPROPERTY()
	//FArchive* ServerStepArchives;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type InEndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/** Get step tick time.
	*	How much time to send a step advance from server.
	*/
	static float StaticGetStepAdvanceTime();

	/** Get step sync time multiply.
	*	How much time the client actions will sync to server.
	*/
	static float StaticGetActionsSyncTime();

	/** Notify lockstep sync start. Always be called in GameMode's StartPlay route.
	*/
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "VehicleAdvLockStep")
	void AuthStartLockStepGame();

	///** Manually do a step in server. And automatically set the server step to manual mode.
	//*/
	//UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "VehicleAdvLockStep")
	//void AuthManualStep(bool InUseManualStep = true);

private:

	/** Do server step advance. */
	void AuthDoStepAdvance();

};
