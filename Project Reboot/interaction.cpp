#include "interaction.h"

bool Interaction::ServerAttemptInteract(UObject* cController, UFunction*, void* Parameters)
{
	if (!Parameters)
		return false;

	UObject* Controller = cController;

	if (Engine_Version >= 424)
	{
		// todo
	}

	auto ReceivingActor = *(UObject**)Parameters;

	if (!ReceivingActor)
		return false;

	static auto BuildingContainerClass = FindObject("Class /Script/FortniteGame.BuildingContainer");

	if (ReceivingActor->IsA(BuildingContainerClass))
	{

	}

	return false;
}