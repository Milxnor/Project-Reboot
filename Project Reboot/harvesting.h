#pragma once

#include "structs.h"

namespace Harvesting
{
	bool OnDamageServer(UObject* BuildingActor, UFunction* Function, void* Parameters);
	bool BlueprintCanAttemptGenerateResources(UObject* BuildingActor, UFunction* Function, void* Parameters);
}