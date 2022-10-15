#include "edit.h"

bool Editing::ServerBeginEditingBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	return false;
}

bool Editing::ServerEditBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	return false;
}

bool Editing::ServerEndEditingBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	return false;
}