// Under MIT licensee, see github page (https://github.com/noslopforever/NFUESample_VehicleAdvLockStep) for more informations.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LockStepPlayerController.generated.h"

/** LockStepPlayerController
*	In LockStep, all pawns will be spawned in client-side and can't be possessed by the default PlayerController.
*	So we have to do some extend to manage them manually.
*/
UCLASS()
class VEHICLEADVLOCKSTEP_API ALockStepPlayerController : public APlayerController
{
	GENERATED_BODY()
public:

	/** Current pawn spawned in client-side only */
	UPROPERTY()
	APawn* ClientSidePawn;

	/** Possess client-side pawn.
	*	The InClientSidePawn should be used as the new view and input target.
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "LockStep")
	void LockStepPossess(APawn* InClientSidePawn);

};
