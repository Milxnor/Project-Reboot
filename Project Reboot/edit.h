#pragma once

#include "helper.h"

namespace Editing
{
	bool ServerBeginEditingBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters);
	bool ServerEditBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters);
	bool ServerEndEditingBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters);
}