#pragma once

#include "structs.h"

namespace Build
{
	bool ServerCreateBuildingActor(UObject* Controller, UFunction* Function, void* Parameters);
	bool ServerSpawnDeco(UObject* DecoTool, UFunction*, void* Parameters);
	bool ServerCreateBuildingAndSpawnDeco(UObject* DecoTool, UFunction*, void* Parameters);
	bool ServerRepairBuildingActor(UObject* Controller, UFunction*, void* Parameters);
}