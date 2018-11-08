// Under MIT licensee, see github page (https://github.com/noslopforever/NFUESample_VehicleAdvLockStep) for more informations.

#include "LockStepPlayerController.h"

void ALockStepPlayerController::LockStepPossess(APawn* InClientSidePawn)
{
	// -- Cleanup old focus pawn --
	if (ClientSidePawn) {
		// TODO tell the old pawn is no longer possessed.
		
		// ** Hack ** : Pass nullptr will force the pawn's bInputEnabled to false.
		ClientSidePawn->DisableInput(nullptr);

		// Detach this Controller from the old pawn.
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		ClientSidePawn = nullptr;
	}

	// Cleanup old view-target.
	SetViewTarget(nullptr);


	// -- Set new focus pawn. --
	if (InClientSidePawn) {
		// ** Hack ** : Pass nullptr will force the pawn's bInputEnabled to true.
		InClientSidePawn->EnableInput(nullptr);

		// Attach this Controller to the new pawn.
		AttachToActor(InClientSidePawn, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		// Set the new view target.
		SetViewTarget(InClientSidePawn);

		ClientSidePawn = InClientSidePawn;

		// Try restart client pawn.
		ClientSidePawn->Controller = this;
		ClientSidePawn->PawnClientRestart();

		SetPawn(ClientSidePawn);

		// TODO ChangeState, etc.

	}
}

