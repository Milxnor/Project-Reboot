#include "edit.h"
#include "inventory.h"

UObject** GetEditingPlayer(UObject* BuildingActor)
{
	static auto EditingPlayerOffset = BuildingActor->GetOffsetSlow("EditingPlayer");

	return (UObject**)(__int64(BuildingActor) + EditingPlayerOffset);
}

UObject** GetEditActor(UObject* EditTool)
{
	static auto EditActorOffset = EditTool->GetOffsetSlow("EditActor");

	return (UObject**)(__int64(EditTool) + EditActorOffset);
}

bool Editing::ServerBeginEditingBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (!Parameters)
		return false;

	auto EditToolInstance = Inventory::FindItemInInventory(Controller, EditToolDefinition);

	auto BuildingToEdit = *(UObject**)Parameters;

	if (EditToolInstance && BuildingToEdit)
	{
		auto EditTool = Inventory::EquipWeapon(Controller, EditToolInstance);

		if (EditTool)
		{
			auto PlayerState = Helper::GetPlayerStateFromController(Controller);

			*GetEditingPlayer(BuildingToEdit) = BuildingToEdit;
			*GetEditActor(EditTool) = BuildingToEdit;
		}
	}

	return false;
}

bool Editing::ServerEditBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (!Parameters)
		return false;

	static auto bMirroredOffset = FindOffsetStruct("Function /Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", "bMirrored");
	auto bMirrored = *Get<bool>(Parameters, bMirroredOffset);

	static auto RotationIterationsOffset = FindOffsetStruct("Function /Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", "RotationIterations");
	auto RotationIterations = Fortnite_Version < 8.30 ? *Get<int>(Parameters, RotationIterationsOffset) : *Get<uint8_t>(Parameters, RotationIterationsOffset); // I HATE FORTNITE

	static auto BuildingActorToEditOffset = FindOffsetStruct("Function /Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", "BuildingActorToEdit");
	auto BuildingActorToEdit = *(UObject**)(__int64(Parameters) + BuildingActorToEditOffset);

	static auto NewBuildingClassOffset = FindOffsetStruct("Function /Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", "NewBuildingClass");
	auto NewBuildingClass = *(UObject**)(__int64(Parameters) + NewBuildingClassOffset);

	if (BuildingActorToEdit && NewBuildingClass)
	{
		Defines::ReplaceBuildingActor(BuildingActorToEdit, 1, NewBuildingClass, 0, RotationIterations, bMirrored, Controller);
	}

	return false;
}

bool Editing::ServerEndEditingBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (!Parameters || Helper::IsInAircraft(Controller))
		return false;

	auto BuildingActorToStopEditing = *(UObject**)Parameters;

	auto Pawn = Helper::GetPawnFromController(Controller);

	// TODO: Check BuildingActorToStopEditing->EditingPlayer and make sure its our player.

	/* if (Pawn)
	{
		auto CurrentWep = Helper::GetCurrentWeapon(Pawn);

		if (CurrentWep)
		{
			auto CurrentWepItemDef = Helper::GetWeaponData(CurrentWep);

			if (CurrentWepItemDef == EditToolDefinition) // Player CONFIRMED the edit
			{
				auto EditTool = CurrentWep;

				*GetEditActor(EditTool) = nullptr;
			}
		}
	} */

	if (BuildingActorToStopEditing)
	{
		*GetEditingPlayer(BuildingActorToStopEditing) = nullptr;
	}

	return false;
}