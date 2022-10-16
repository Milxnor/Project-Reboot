#pragma once

#include "helper.h"

namespace Editing
{
	inline UObject* EditToolDefinition = nullptr;

	bool ServerBeginEditingBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters);
	bool ServerEditBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters);
	bool ServerEndEditingBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters);
}