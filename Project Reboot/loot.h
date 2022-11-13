#pragma once

#include "structs.h"

enum class ItemType
{
	None,
	Weapon,
	Consumable,
	Ammo,
	Resource,
	Trap
};

static std::string ItemTypeToString(ItemType type)
{
	switch (type)
	{
		using enum ItemType;
	case Weapon:
		return "Weapon";
	case Consumable:
		return "Consumable";
	case Ammo:
		return "Ammo";
	case Resource:
		return "Resource";
	case Trap:
		return "Trap";
	default:
		return "NULL ItemType";
	}
}

struct DefinitionInRow // 50 bytes
{
	UObject* Definition = nullptr;
	float Weight;
	int DropCount = 1;
	std::string RowName;
	ItemType Type = ItemType::None;

	std::string Describe(bool bNewLines = false)
	{
		if (bNewLines)
			return std::format("{}\n{}\n{}\n{}\n{}\n", Definition ? Definition->GetFullName() : "NULL Definition", std::to_string(Weight), std::to_string(DropCount), RowName, ItemTypeToString(Type));
		else
			return std::format("{} {} {} {} {}", Definition ? Definition->GetFullName() : "NULL Definition", std::to_string(Weight), std::to_string(DropCount), RowName, ItemTypeToString(Type));
	}
};

namespace Looting
{
	inline bool bInitialized = false;
	inline std::vector<std::vector<DefinitionInRow>> Items; // best way? Probably not
	inline std::vector<std::vector<float>> Weights;

	inline int LootItems = 0;
	inline int SupplyDropItems = 1;
	inline int LlamaItems = 2;
	inline int FactionLootItems = 3;

	UObject* GetLTD();
	UObject* GetLP();
	// { return FindObject("/Game/Athena/Playlists/Playground/AthenaLootPackages_Client.AthenaLootPackages_Client"); }

	void Initialize();
	void SpawnForagedItems();
	// std::vector<std::pair<UObject*, int>> PickLootDrops(const std::string& TierGroupName, int WorldLevel = 1, int ForcedLootTier = 1); // Definition, DropCount

	static const DefinitionInRow GetRandomItem(ItemType Type, int LootType = LootItems)
	{
		if (LootType <= Items.size() && Items.size() >= 1 ? Items[LootType].empty() : true)
		{
			std::cout << std::format("[WARNING] Tried getting a {} with loot type {} but the table is null!\n", ItemTypeToString(Type), std::to_string(LootType));
			return DefinitionInRow();
		}

		auto& TableToUse = Items[LootType];

		int current = 0;

		while (current < 5000) // it shouldnt even be this much
		{
			auto& Item = TableToUse[rand() % (TableToUse.size())];

			if (Item.Type == Type && Item.Definition)
				return Item;

			current++;
		}

		return DefinitionInRow();
	}
}