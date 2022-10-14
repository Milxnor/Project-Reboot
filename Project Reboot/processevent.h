#include <unordered_map>
#include <functional>
#include <iostream>

#include "structs.h"

static std::unordered_map<UFunction*, std::function<bool(UObject*, UFunction*, void*)>> FunctionsToHook; // INSPIRED by kem0x/raider3.5 hooking

bool ServerAcknowledgePossession(UObject* Object, UFunction* Function, void* Parameters);
bool HandleStartingNewPlayer(UObject* Object, UFunction* Function, void* Parameters);
bool ReadyToStartMatch(UObject* GameMode, UFunction* Function, void* Parameters);

void AddHook(const std::string& str, std::function<bool(UObject*, UFunction*, void*)> func);
void ProcessEventDetour(UObject* Object, UFunction* Function, void* Parameters);