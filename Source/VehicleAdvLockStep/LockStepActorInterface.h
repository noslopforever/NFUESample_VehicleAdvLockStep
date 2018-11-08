// Under MIT licensee, see github page (https://github.com/noslopforever/NFUESample_VehicleAdvLockStep) for more informations.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LockStepActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULockStepActorInterface : public UInterface
{
	GENERATED_BODY()
};

/** Interface for actors which need lockstep notifies.
 * 
 */
class VEHICLEADVLOCKSTEP_API ILockStepActorInterface
{
	GENERATED_BODY()
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/** Do step advance for blueprint use.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "LockStep")
	void ReceiveStepAdvance(int32 InStepIndex, float InDeltaTime, ETickingGroup InTickGroup = TG_PrePhysics);

};
