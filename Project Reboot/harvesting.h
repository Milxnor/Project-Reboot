#pragma once

#include "structs.h"

namespace Harvesting
{
	bool OnDamageServerHook(UObject* BuildingActor, UFunction* Function, void* Parameters);
	bool BlueprintCanAttemptGenerateResourcesHook(UObject* BuildingActor, UFunction* Function, void* Parameters);
}