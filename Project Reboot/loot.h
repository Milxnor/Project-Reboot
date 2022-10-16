#pragma once

#include "structs.h"

namespace Looting
{
	inline bool bInitialized = false;

	UObject* GetLTD() { return FindObject("/Game/Items/Datatables/AthenaLootTierData_Client.AthenaLootTierData_Client"); }
	UObject* GetLP() { return FindObject("/Game/Items/Datatables/AthenaLootPackages_Client.AthenaLootPackages_Client"); }

	TArray<std::pair<UObject*, int>> PickLootDrops(const std::string& TierGroupName, int WorldLevel = 1, int ForcedLootTier = 1); // Definition, DropCount
}