// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleAdvLockStepGameMode.h"
#include "VehicleAdvLockStepPawn.h"
#include "VehicleAdvLockStepHud.h"

AVehicleAdvLockStepGameMode::AVehicleAdvLockStepGameMode()
{
	DefaultPawnClass = AVehicleAdvLockStepPawn::StaticClass();
	HUDClass = AVehicleAdvLockStepHud::StaticClass();
}
