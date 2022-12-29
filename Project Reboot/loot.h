#pragma once

#include <sstream>
#include <fstream>

#include "definitions.h"

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

enum class ERarity
{
	None,
	Common,
	Uncommon,
	Rare,
	Epic,
	Legendary
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

	std::vector<UObject*> GetLTD();
	std::vector<UObject*> GetLP();

	void Initialize();
	void SpawnForagedItems();
#ifdef TEST_NEW_LOOTING
	std::vector<std::pair<UObject*, int>> PickLootDrops(const std::string& TierGroupName, int WorldLevel = 1, int ForcedLootTier = 1); // Definition, DropCount
#endif

	static const DefinitionInRow GetRandomItem(ItemType Type, int LootType = LootItems, ERarity Rarity = ERarity::None)
	{
		static auto TierOffset = FindOffsetStruct2("Class /Script/FortniteGame.FortItemDefinition", "Tier");

		if (!Defines::bCustomLootpool)
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
				auto ItemDef = Item.Definition;

				if (ItemDef)
				{
					if (Item.Type == Type)
					{
						bool bPassedRarityCheck = Rarity == ERarity::None;

						if (Rarity != ERarity::None)
						{
							// static auto TierOffset = ItemDef->GetOffset("Tier");
							auto Tier = *Get<uint8_t>(ItemDef, TierOffset);

							// std::cout << "Tier: " << (int)Tier << '\n';

							if ((int)Rarity == (int)Tier)
								bPassedRarityCheck = true;
						}

						if (bPassedRarityCheck)
							return Item;
					}
				}

				current++;
			}
		}
		else
		{
			std::fstream lootpool("lootpool.txt");

			std::vector<DefinitionInRow> rows;

			std::string line;

			while (std::getline(lootpool, line)) {
				// Extract the ID and count from the line
				std::string id;
				int count = 0;

				// IDK HERE

				auto def = FindObject(std::format("{}.{}", id, id));

				if (!def)
					continue;

				DefinitionInRow row;
				row.Definition = def;
				row.DropCount = count;

				rows.push_back(row);
			}

			int current = 0;

			while (current < 5000) // it shouldnt even be this much
			{
				auto& Item = rows[rand() % (rows.size())];
				auto ItemDef = Item.Definition;

				if (ItemDef)
				{
					if (Item.Type == Type)
					{
						bool bPassedRarityCheck = Rarity == ERarity::None;

						if (Rarity != ERarity::None)
						{
							// static auto TierOffset = ItemDef->GetOffset("Tier");
							auto Tier = *Get<uint8_t>(ItemDef, TierOffset);

							// std::cout << "Tier: " << (int)Tier << '\n';

							if ((int)Rarity == (int)Tier)
								bPassedRarityCheck = true;
						}

						if (bPassedRarityCheck)
							return Item;
					}
				}

				current++;
			}
		}

		return DefinitionInRow();
	}
}