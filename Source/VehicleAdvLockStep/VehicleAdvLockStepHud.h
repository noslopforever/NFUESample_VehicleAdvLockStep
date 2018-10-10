// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "VehicleAdvLockStepHud.generated.h"

UCLASS(config = Game)
class AVehicleAdvLockStepHud : public AHUD
{
	GENERATED_BODY()

public:
	AVehicleAdvLockStepHud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface

};
