#include "loot.h"
#include "helper.h"
#include "datatables.h"

UObject* Looting::GetLTD()
{
	auto ltd = FindObject("/Game/Items/Datatables/AthenaLootTierData_Client.AthenaLootTierData_Client");
	return ltd;

	auto Playlist = *Helper::GetPlaylist();

	if (!Playlist)
		return nullptr;

	static auto LootTierDataOffset = FindOffsetStruct("Class /Script/FortniteGame.FortPlaylist", "LootTierData");
	auto LootTierDataSoft = Get<TSoftObjectPtr>(Playlist, LootTierDataOffset);

	auto LootTierDataName = LootTierDataSoft->ObjectID.AssetPathName.ToString();

	auto ClassToUse = (LootTierDataName.contains("Composite")) ?
		FindObject("/Script/Engine.CompositeDataTable") : FindObject("/Script/Engine.DataTable");

	std::cout << "LootTierDataName: " << LootTierDataName << '\n';

	return StaticLoadObject(ClassToUse, nullptr, LootTierDataName);
}

UObject* Looting::GetLP()
{
	auto Playlist = *Helper::GetPlaylist();

	if (!Playlist)
		return nullptr;

	static UObject* lastPlaylist = nullptr;
	static UObject* lp = nullptr;

	if (Playlist != lastPlaylist)
	{
		lastPlaylist = Playlist;

		static auto LootPackagesOffset = FindOffsetStruct("Class /Script/FortniteGame.FortPlaylist", "LootPackages"); // Playlist->GetOffset("LootPackages");
		auto LootPackagesSoft = Get<TSoftObjectPtr>(Playlist, LootPackagesOffset);

		auto LootPackagesName = LootPackagesSoft->ObjectID.AssetPathName.ComparisonIndex && Engine_Version < 424
			? LootPackagesSoft->ObjectID.AssetPathName.ToString() : "/Game/Items/Datatables/AthenaLootPackages_Client.AthenaLootPackages_Client";

		auto ClassToUse = (LootPackagesName.contains("Composite")) ?
			FindObject("/Script/Engine.CompositeDataTable") : FindObject("/Script/Engine.DataTable");

		lp = StaticLoadObject(ClassToUse, nullptr, LootPackagesName);
	}

	return lp;
}

void Looting::SpawnForagedItems()
{
	Defines::bShouldSpawnForagedItems = true;
}

void AddItemAndWeight(int Index, const DefinitionInRow& Item, float Weight)
{
	if (Index > Looting::Items.size() || Index > Looting::Weights.size())
		return;

	Looting::Items[Index].push_back(Item);
	Looting::Weights[Index].push_back(Weight);
}

void Looting::Initialize()
{
	if (bInitialized)
	{
		std::cout << "[WARNING] Loot is already initialized!\n";
		return;
	}

	if (!StaticFindObjectO)
	{
		std::cout << "Not going to initialize looting because it will be too slow!\n";
		return;
	}

	for (int i = 0; i < 2; i++)
	{
		Items.push_back(std::vector<DefinitionInRow>());
	}

	for (int i = 0; i < 2; i++)
	{
		Weights.push_back(std::vector<float>());
	}

	// Items.reserve(2);

	auto LootPackages = GetLP();

	if (!LootPackages)
	{
		std::cout << "Failed to get LootPackages!\n";
		return;
	}

	bInitialized = true;

	auto LootPackagesRowMap = DataTables::GetRowMap(LootPackages);

	auto fortnite = LootPackagesRowMap.Pairs.Elements.Data;

	std::cout << "Amount of rows: " << fortnite.Num() << '\n';

	for (int i = 0; i < fortnite.Num() - 1; i++)
	{
		auto Man = fortnite.At(i);
		auto& Pair = Man.ElementData.Value;
		auto RowFName = Pair.First;

		if (!RowFName.ComparisonIndex)
			continue;

		auto RowName = RowFName.ToString();
		auto LootPackageDataOfRow = Pair.Second; // FortniteGame.FortLootPackageData

		if (LootPackageDataOfRow) // pretty sure this is wrong // todo: check rowisa athenaloot package or something io think
		{
			int Index = -1;

			if (RowName.starts_with("WorldList.AthenaLoot"))
				Index = LootItems;
			else if (RowName.starts_with("WorldPKG.AthenaSupplyDrop")) // wrong
				Index = SupplyDropItems;
			else if (RowName.starts_with("WorldList.FactionLoot"))
				Index = FactionLootItems;
			// else if (RowName.starts_with("WorldList.AthenaLlama"))
				// Index = LlamaItems;

			if (Index == -1)
				continue;

			static auto off = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortLootPackageData", "ItemDefinition");
			static auto countOff = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortLootPackageData", "Count");
			static auto weightOff = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortLootPackageData", "Weight");

			auto ItemDef = (TSoftObjectPtr*)(__int64(LootPackageDataOfRow) + off);
			auto Count = (int*)(__int64(LootPackageDataOfRow) + countOff);
			auto Weight = (float*)(__int64(LootPackageDataOfRow) + weightOff);

			// std::cout << std::format("Count: {} ItemDef: {}\n", *Count, ItemDef->ObjectID.AssetPathName.ToString());

			DefinitionInRow currentItem;

			if (ItemDef && ItemDef->ObjectID.AssetPathName.ComparisonIndex)
			{
				auto DefinitionString = ItemDef->ObjectID.AssetPathName.ToString();
				currentItem.Definition = FindObject(DefinitionString);

				if (Count)
					currentItem.DropCount = *Count;

				if (Weight)
					currentItem.Weight = *Weight;

				currentItem.RowName = RowName;

				// brain damage

				constexpr bool bAllowContextTraps = false;

				if (DefinitionString.contains("Weapon"))
					currentItem.Type = ItemType::Weapon;
				else if (DefinitionString.contains("Consumable"))
					currentItem.Type = ItemType::Consumable;
				else if (DefinitionString.contains("Ammo"))
					currentItem.Type = ItemType::Ammo;
				else if (DefinitionString.contains("ResourcePickups"))
					currentItem.Type = ItemType::Resource;
				else if (DefinitionString.contains("TID") && bAllowContextTraps ? true : !DefinitionString.contains("Context"))
					currentItem.Type = ItemType::Trap;

				if (Weight)
					AddItemAndWeight(Index, currentItem, *Weight);
			}
		}
	}

	std::cout << "Initialized Looting V2!\n";

	return;
}