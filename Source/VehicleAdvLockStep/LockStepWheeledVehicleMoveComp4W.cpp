// Under MIT licensee, see github page (https://github.com/noslopforever/NFUESample_VehicleAdvLockStep) for more informations.

#include "LockStepWheeledVehicleMoveComp4W.h"

#if UE_BUILD_SHIPPING
#define DEBUG_CALLSPACE(Format, ...)
#else
#define DEBUG_CALLSPACE(Format, ...) UE_LOG(LogNet, VeryVerbose, Format, __VA_ARGS__);
#endif

void ULockStepWheeledVehicleMoveComp4W::UpdateState(float DeltaTime)
{
	// # Copied from super, remove controller check, and do everything as client-only.

	if (bReverseAsBrake)
	{
		//for reverse as state we want to automatically shift between reverse and first gear
		if (FMath::Abs(GetForwardSpeed()) < WrongDirectionThreshold)	//we only shift between reverse and first if the car is slow enough. This isn't 100% correct since we really only care about engine speed, but good enough
		{
			if (RawThrottleInput < -KINDA_SMALL_NUMBER && GetCurrentGear() >= 0 && GetTargetGear() >= 0)
			{
				SetTargetGear(-1, true);
			}
			else if (RawThrottleInput > KINDA_SMALL_NUMBER && GetCurrentGear() <= 0 && GetTargetGear() <= 0)
			{
				SetTargetGear(1, true);
			}
		}
	}


	if (bUseRVOAvoidance)
	{
		CalculateAvoidanceVelocity(DeltaTime);
		UpdateAvoidance(DeltaTime);
	}

	SteeringInput = SteeringInputRate.InterpInputValue(DeltaTime, SteeringInput, CalcSteeringInput());
	ThrottleInput = ThrottleInputRate.InterpInputValue(DeltaTime, ThrottleInput, CalcThrottleInput());
	BrakeInput = BrakeInputRate.InterpInputValue(DeltaTime, BrakeInput, CalcBrakeInput());
	HandbrakeInput = HandbrakeInputRate.InterpInputValue(DeltaTime, HandbrakeInput, CalcHandbrakeInput());
	SetTargetGear(GetCurrentGear(), true);

}


