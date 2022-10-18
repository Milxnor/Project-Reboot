#pragma once

#include "structs.h"

namespace Looting
{
	inline bool bInitialized = false;

	static UObject* GetLTD() { return FindObject("/Game/Items/Datatables/AthenaLootTierData_Client.AthenaLootTierData_Client"); }
	static UObject* GetLP() { return FindObject("/Game/Items/Datatables/AthenaLootPackages_Client.AthenaLootPackages_Client"); }

	void SpawnForagedItems();
	std::vector<std::pair<UObject*, int>> PickLootDrops(const std::string& TierGroupName, int WorldLevel = 1, int ForcedLootTier = 1); // Definition, DropCount
}