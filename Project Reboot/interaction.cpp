#include "interaction.h"
#include "helper.h"

bool Interaction::ServerAttemptInteract(UObject* cController, UFunction*, void* Parameters)
{
	if (!Parameters)
		return false;

	UObject* Controller = Engine_Version < 423 ? cController : Helper::GetOwnerOfComponent(cController);

	auto ReceivingActor = *(UObject**)Parameters;

	if (!ReceivingActor)
		return false;

	static auto BuildingContainerClass = FindObject("/Script/FortniteGame.BuildingContainer");

	if (ReceivingActor->IsA(BuildingContainerClass))
	{

	}

	return false;
}