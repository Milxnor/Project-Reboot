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

			*GetEditingPlayer(BuildingToEdit) = PlayerState;
			*GetEditActor(EditTool) = BuildingToEdit;
		}
	}

	return false;
}

bool Editing::ServerEditBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (!Parameters)
		return false;

	static auto bMirroredOffset = FindOffsetStruct2("/Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", "bMirrored", true, true);
	auto bMirrored = *Get<bool>(Parameters, bMirroredOffset);

	static auto RotationIterationsOffset = FindOffsetStruct2("/Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", "RotationIterations", true, true);
	auto RotationIterations = Fortnite_Version < 8.30 ? *Get<int>(Parameters, RotationIterationsOffset) : *Get<uint8_t>(Parameters, RotationIterationsOffset); // I HATE FORTNITE

	static auto BuildingActorToEditOffset = FindOffsetStruct2("/Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", "BuildingActorToEdit", true, true);
	auto BuildingActorToEdit = *(UObject**)(__int64(Parameters) + BuildingActorToEditOffset);

	static auto NewBuildingClassOffset = FindOffsetStruct2("/Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", "NewBuildingClass", true, true);
	auto NewBuildingClass = *(UObject**)(__int64(Parameters) + NewBuildingClassOffset);

	std::cout << "RotationIterationsOffset: " << RotationIterationsOffset << '\n';
	std::cout << "BuildingActorToEditOffset: " << BuildingActorToEditOffset << '\n';
	std::cout << "NewBuildingClassOffset: " << NewBuildingClassOffset << '\n';

	if (Defines::ReplaceBuildingActor && BuildingActorToEdit && NewBuildingClass)
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