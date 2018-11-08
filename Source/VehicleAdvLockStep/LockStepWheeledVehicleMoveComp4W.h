// Under MIT licensee, see github page (https://github.com/noslopforever/NFUESample_VehicleAdvLockStep) for more informations.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "LockStepWheeledVehicleMoveComp4W.generated.h"

/** Hack of the demo pawn's Wheeled4W Component.
 */
UCLASS()
class VEHICLEADVLOCKSTEP_API ULockStepWheeledVehicleMoveComp4W : public UWheeledVehicleMovementComponent4W
{
	GENERATED_BODY()
protected:

	// Begin Object interfaces
	virtual void UpdateState(float DeltaTime);
	// ~ End Object interfaces

};
