// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class VehicleAdvLockStepEditorTarget : TargetRules
{
	public VehicleAdvLockStepEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.Add("VehicleAdvLockStep");
	}
}
