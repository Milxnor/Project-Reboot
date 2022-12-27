#include <numeric>

#include "loot.h"
#include "helper.h"
#include "datatables.h"

struct FFortGameFeatureLootTableData
{
public:
	TSoftObjectPtr             LootTierData;                                      // 0x0(0x28)(Edit, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	TSoftObjectPtr            LootPackageData;                                   // 0x28(0x28)(Edit, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

std::vector<UObject*> Looting::GetLTD()
{
	static std::vector<UObject*> ltds;

	if (ltds.size() == 0)
	{
		static bool bPrinted = false;
		bool bFoundOverride = false;

		static auto FortGameFeatureDataClass = FindObject("/Script/FortniteGame.FortGameFeatureData");
		static auto DataTableClass = FindObject("/Script/Engine.DataTable");
		static auto CompositeDataTableClass = FindObject("/Script/Engine.CompositeDataTable");

		auto Playlist = *Helper::GetPlaylist();

		if (FortGameFeatureDataClass)
		{
			auto AllFortGameFeatureData = Helper::GetAllObjectsOfClass(FortGameFeatureDataClass);

			for (auto GameFeatureData : AllFortGameFeatureData)
			{
				static auto DefaultLootTableDataOffset = GameFeatureData->GetOffset("DefaultLootTableData");

				auto DefaultLootTableData = Get<FFortGameFeatureLootTableData>(GameFeatureData, DefaultLootTableDataOffset);

				auto LootPackageTableStr = DefaultLootTableData->LootTierData.ObjectID.AssetPathName.ToString();

				auto bIsComposite = LootPackageTableStr.contains("Composite");

				auto LootTierDataPtr = DefaultLootTableData->LootTierData.Get(bIsComposite ? CompositeDataTableClass : DataTableClass);

				if (LootTierDataPtr)
				{
					ltds.push_back(LootTierDataPtr);

					if (!bPrinted)
						DEV_LOG("LP: {}", LootTierDataPtr->GetFullName());
				}

				if (Playlist)
				{
					static auto PlaylistOverrideLootTableDataOffset = GameFeatureData->GetOffset("PlaylistOverrideLootTableData");
					auto PlaylistOverrideLootTableData = Get<TMap<FGameplayTag, FFortGameFeatureLootTableData>>(GameFeatureData, PlaylistOverrideLootTableDataOffset);

					auto PlaylistOverrideLootTableData_Data = PlaylistOverrideLootTableData->Pairs.Elements.Data;

					static auto GameplayTagContainerOffset = Playlist->GetOffset("GameplayTagContainer");
					auto GameplayTagContainer = Get<FGameplayTagContainer>(Playlist, GameplayTagContainerOffset);

					for (int i = 0; i < GameplayTagContainer->GameplayTags.Num(); i++)
					{
						auto Tag = GameplayTagContainer->GameplayTags.At(i);

						for (int j = 0; j < PlaylistOverrideLootTableData_Data.Num(); j++)
						{
							auto Value = PlaylistOverrideLootTableData_Data.at(j).ElementData.Value;
							auto CurrentOverrideTag = Value.First;

							if (Tag == CurrentOverrideTag)
							{
								auto OverrideLootTierDataStr = Value.Second.LootTierData.ObjectID.AssetPathName.ToString();
								auto bOverrideIsComposite = OverrideLootTierDataStr.contains("Composite");

								auto ptr = Value.Second.LootTierData.Get(bOverrideIsComposite ? CompositeDataTableClass : DataTableClass);

								if (ptr)
								{
									if (bOverrideIsComposite)
									{
										static auto ParentTablesOffset = ptr->GetOffset("ParentTables");

										auto ParentTables = Get<TArray<UObject*>>(ptr, ParentTablesOffset);

										for (int z = 0; z < ParentTables->size(); z++)
										{
											auto ParentTable = ParentTables->At(z);

											if (ParentTable)
											{
												// std::cout << "parentltd: " << ParentTable->GetFullName() << '\n';
												ltds.push_back(ParentTable);
											}
										}
									}

									ltds.push_back(ptr);

									// std::cout << "overriedlt: " << ptr->GetFullName() << '\n';
									bFoundOverride = true;
								}
							}
						}
					}
				}
			}
		}

		bool ahh = Playlist;

		if (ahh)
		{
			static auto LootTierDataOffset = Playlist->GetOffset("LootTierData");
			auto LootTierData = Get<TSoftObjectPtr>(Playlist, LootTierDataOffset);

			auto LootTierDataStr = LootTierData->ObjectID.AssetPathName.ToString();
			auto bIsComposite = LootTierDataStr.contains("Composite");

			auto ptr = LootTierData->Get(bIsComposite ? CompositeDataTableClass : DataTableClass);

			if (ptr)
				ltds.push_back(ptr);
			else
				ahh = false;
		}
		
		if (!ahh)
		{
			static auto LootTierDataTablesOffset = FindOffsetStruct2("Class /Script/FortniteGame.FortGameData", "LootTierDataTables", false, false, false);

			if (LootTierDataTablesOffset)
			{
				if (Engine_Version <= 419) // Idk
				{
					// auto LootTierDataTables = Get<TArray<TAssetPtr>>(Helper::GetGameData(), LootTierDataTablesOffset);
				}
				else
				{
					auto LootTierDataTables = Get<TArray<TSoftObjectPtr>>(Helper::GetGameData(), LootTierDataTablesOffset);
					auto LootTierDataTablesVec = LootTierDataTables->ToVector();

					std::cout << "LootTierDataTablesVec.size(): " << LootTierDataTablesVec.size() << '\n';

					for (auto& LootPackageTable : LootTierDataTablesVec)
					{
						auto LootPackageTableStr = LootPackageTable.ObjectID.AssetPathName.ToString();

						static auto DataTableClass = FindObject("/Script/Engine.DataTable");
						static auto CompositeDataTableClass = FindObject("/Script/Engine.CompositeDataTable");

						auto bIsComposite = LootPackageTableStr.contains("Composite");
						auto LootPackagePtr = LootPackageTable.Get(bIsComposite ? CompositeDataTableClass : DataTableClass);

						if (LootPackagePtr)
						{
							/* if (bIsComposite)
							{
								static auto ParentTablesOffset = LootPackagePtr->GetOffset("ParentTables");

								auto ParentTables = Get<TArray<UObject*>>(LootPackagePtr, ParentTablesOffset);
								auto ParentTablesVec = ParentTables->ToVector();

								lps.insert(lps.end(), ParentTablesVec.begin(), ParentTablesVec.end());
							} */

							ltds.push_back(LootPackagePtr);
						}
					}
				}
			}
			else
			{
				static auto LootTierDataTablesBROffset = FindOffsetStruct("Class /Script/FortniteGame.GameDataBR", "LootTierDataTablesBR");

				auto LootTierDataTablesBR = Get<TArray<TSoftObjectPtr>>(Helper::GetGameDataBR(), LootTierDataTablesBROffset);
				auto LootTierDataTablesBRVec = LootTierDataTablesBR->ToVector();

				std::cout << "LootPackageDataTablesBRVec.size(): " << LootTierDataTablesBRVec.size() << '\n';

				for (auto& LootPackageTable : LootTierDataTablesBRVec)
				{
					auto LootPackageTableStr = LootPackageTable.ObjectID.AssetPathName.ToString();

					static auto DataTableClass = FindObject("/Script/Engine.DataTable");
					static auto CompositeDataTableClass = FindObject("/Script/Engine.CompositeDataTable");

					auto bIsComposite = LootPackageTableStr.contains("Composite");
					auto LootPackagePtr = LootPackageTable.Get(bIsComposite ? CompositeDataTableClass : DataTableClass);

					if (LootPackagePtr)
					{
						/* if (bIsComposite)
						{
							static auto ParentTablesOffset = LootPackagePtr->GetOffset("ParentTables");

							auto ParentTables = Get<TArray<UObject*>>(LootPackagePtr, ParentTablesOffset);
							auto ParentTablesVec = ParentTables->ToVector();

							lps.insert(lps.end(), ParentTablesVec.begin(), ParentTablesVec.end());
						} */

						ltds.push_back(LootPackagePtr);
					}
				}
			}
		}

		bPrinted = true;
	}

	return ltds;
}

std::vector<UObject*> Looting::GetLP()
{
	static std::vector<UObject*> lps;

	if (lps.size() == 0)
	{
		static bool bPrinted = false;
		static auto FortGameFeatureDataClass = FindObject("/Script/FortniteGame.FortGameFeatureData");

		bool bFoundOverride = false;

		auto Playlist = *Helper::GetPlaylist();

		static auto DataTableClass = FindObject("/Script/Engine.DataTable");
		static auto CompositeDataTableClass = FindObject("/Script/Engine.CompositeDataTable");

		if (FortGameFeatureDataClass)
		{
			auto AllFortGameFeatureData = Helper::GetAllObjectsOfClass(FortGameFeatureDataClass);

			for (auto GameFeatureData : AllFortGameFeatureData)
			{
				static auto DefaultLootTableDataOffset = GameFeatureData->GetOffset("DefaultLootTableData");

				auto DefaultLootTableData = Get<FFortGameFeatureLootTableData>(GameFeatureData, DefaultLootTableDataOffset);

				auto LootPackageTableStr = DefaultLootTableData->LootPackageData.ObjectID.AssetPathName.ToString();

				auto bIsComposite = LootPackageTableStr.contains("Composite");

				auto LootPackagePtr = DefaultLootTableData->LootPackageData.Get(bIsComposite ? CompositeDataTableClass : DataTableClass);

				if (LootPackagePtr)
				{
					lps.push_back(LootPackagePtr);

					if (!bPrinted)
						DEV_LOG("LP: {}", LootPackagePtr->GetFullName());
				}

				if (Playlist)
				{
					static auto PlaylistOverrideLootTableDataOffset = GameFeatureData->GetOffset("PlaylistOverrideLootTableData");
					auto PlaylistOverrideLootTableData = Get<TMap<FGameplayTag, FFortGameFeatureLootTableData>>(GameFeatureData, PlaylistOverrideLootTableDataOffset);

					auto PlaylistOverrideLootTableData_Data = PlaylistOverrideLootTableData->Pairs.Elements.Data;

					static auto GameplayTagContainerOffset = Playlist->GetOffset("GameplayTagContainer");
					auto GameplayTagContainer = Get<FGameplayTagContainer>(Playlist, GameplayTagContainerOffset);

					for (int i = 0; i < GameplayTagContainer->GameplayTags.Num(); i++)
					{
						auto Tag = GameplayTagContainer->GameplayTags.At(i);

						for (int j = 0; j < PlaylistOverrideLootTableData_Data.Num(); j++)
						{
							auto Value = PlaylistOverrideLootTableData_Data.at(j).ElementData.Value;
							auto CurrentOverrideTag = Value.First;

							if (Tag == CurrentOverrideTag)
							{
								auto OverrideLootPackageTableStr = Value.Second.LootPackageData.ObjectID.AssetPathName.ToString();
								auto bOverrideIsComposite = OverrideLootPackageTableStr.contains("Composite");

								auto ptr = Value.Second.LootPackageData.Get(bOverrideIsComposite ? CompositeDataTableClass : DataTableClass);

								if (ptr)
								{
									if (bOverrideIsComposite)
									{
										static auto ParentTablesOffset = ptr->GetOffset("ParentTables");

										auto ParentTables = Get<TArray<UObject*>>(ptr, ParentTablesOffset);

										for (int z = 0; z < ParentTables->size(); z++)
										{
											auto ParentTable = ParentTables->At(z);

											if (ParentTable)
											{
												// std::cout << "parentlp: " << ParentTable->GetFullName() << '\n';
												lps.push_back(ParentTable);
											}
										}
									}

									lps.push_back(ptr);

									// std::cout << "overriedlp: " << ptr->GetFullName() << '\n';
									bFoundOverride = true;
								}
							}
						}
					}
				}
			}
		}

		bool ahh = Playlist;

		if (ahh)
		{
			static auto LootPackagesOffset = Playlist->GetOffset("LootPackages");
			auto LootPackages = Get<TSoftObjectPtr>(Playlist, LootPackagesOffset);

			auto LootPackageTableStr = LootPackages->ObjectID.AssetPathName.ToString();
			auto bIsComposite = LootPackageTableStr.contains("Composite");

			auto ptr = LootPackages->Get(bIsComposite ? CompositeDataTableClass : DataTableClass);

			if (ptr)
				lps.push_back(ptr);
			else
				ahh = false;
		}
		
		if (!ahh)
		{
			static auto LootPackageDataTablesOffset = FindOffsetStruct2("Class /Script/FortniteGame.FortGameData", "LootPackageDataTables", false, false, false);

			if (LootPackageDataTablesOffset)
			{
				if (Engine_Version <= 419) // Idk
				{
					// auto LootPackageDataTables = Get<TArray<TAssetPtr>>(Helper::GetGameData(), LootPackageDataTablesOffset);
				}
				else
				{
					auto LootPackageDataTables = Get<TArray<TSoftObjectPtr>>(Helper::GetGameData(), LootPackageDataTablesOffset);
					auto LootPackageDataTablesVec = LootPackageDataTables->ToVector();

					std::cout << "LootPackageDataTablesVec.size(): " << LootPackageDataTablesVec.size() << '\n';

					for (auto& LootPackageTable : LootPackageDataTablesVec)
					{
						auto LootPackageTableStr = LootPackageTable.ObjectID.AssetPathName.ToString();

						auto bIsComposite = LootPackageTableStr.contains("Composite");
						auto LootPackagePtr = LootPackageTable.Get(bIsComposite ? CompositeDataTableClass : DataTableClass);

						if (LootPackagePtr)
						{
							/* if (bIsComposite)
							{
								static auto ParentTablesOffset = LootPackagePtr->GetOffset("ParentTables");

								auto ParentTables = Get<TArray<UObject*>>(LootPackagePtr, ParentTablesOffset);
								auto ParentTablesVec = ParentTables->ToVector();

								lps.insert(lps.end(), ParentTablesVec.begin(), ParentTablesVec.end());
							} */

							lps.push_back(LootPackagePtr);
							std::cout << "skidda: " << LootPackageTableStr << '\n';
						}
					}
				}
			}
			else
			{
				static auto LootPackageDataTablesBROffset = FindOffsetStruct("Class /Script/FortniteGame.GameDataBR", "LootPackageDataTablesBR");

				auto LootPackageDataTablesBR = Get<TArray<TSoftObjectPtr>>(Helper::GetGameDataBR(), LootPackageDataTablesBROffset);
				auto LootPackageDataTablesBRVec = LootPackageDataTablesBR->ToVector();

				std::cout << "LootPackageDataTablesBRVec.size(): " << LootPackageDataTablesBRVec.size() << '\n';

				for (auto& LootPackageTable : LootPackageDataTablesBRVec)
				{
					auto LootPackageTableStr = LootPackageTable.ObjectID.AssetPathName.ToString();

					auto bIsComposite = LootPackageTableStr.contains("Composite");
					auto LootPackagePtr = LootPackageTable.Get(bIsComposite ? CompositeDataTableClass : DataTableClass);

					if (LootPackagePtr)
					{
						/* if (bIsComposite)
						{
							static auto ParentTablesOffset = LootPackagePtr->GetOffset("ParentTables");

							auto ParentTables = Get<TArray<UObject*>>(LootPackagePtr, ParentTablesOffset);
							auto ParentTablesVec = ParentTables->ToVector();

							lps.insert(lps.end(), ParentTablesVec.begin(), ParentTablesVec.end());
						} */

						lps.push_back(LootPackagePtr);
						std::cout << "skidda: " << LootPackageTableStr << '\n';
					}
				}
			}
		}

		std::cout << "lps size: " << lps.size() << '\n';

		bPrinted = true;
	}

	return lps;
}

using FFortLootTierData = void;
using FFortLootPackageData = void;

#if 0

static FFortLootTierData* GetLootTierData(std::vector<FFortLootTierData*>& LootTierData)
{
	static auto LTD_WeightOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortLootTierData", "Weight");

	float TotalWeight = 0;

	for (auto Item : LootTierData)
		TotalWeight += *Get<float>(Item, LTD_WeightOffset);

	float RandomNumber = GetRandomFloat(0, TotalWeight);

	FFortLootTierData* SelectedItem = nullptr;

	for (auto Item : LootTierData)
	{
		if (RandomNumber <= *Get<float>(Item, LTD_WeightOffset))
		{
			SelectedItem = Item;
			break;
		}

		RandomNumber = RandomNumber - *Get<float>(Item, LTD_WeightOffset);
	}

	if (!SelectedItem)
		return GetLootTierData(LootTierData);

	return SelectedItem;
}

static FFortLootPackageData* GetLootPackage(std::vector<FFortLootPackageData*>& LootPackages)
{
	static auto LP_WeightOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortLootPackageData", "Weight");

	float TotalWeight = 0;

	for (auto Item : LootPackages)
		TotalWeight += *Get<float>(Item, LP_WeightOffset);

	float RandomNumber = GetRandomFloat(0, TotalWeight);

	FFortLootPackageData* SelectedItem = nullptr;

	for (auto Item : LootPackages)
	{
		if (RandomNumber <= *Get<float>(Item, LP_WeightOffset))
		{
			SelectedItem = Item;
			break;
		}

		RandomNumber = RandomNumber - *Get<float>(Item, LP_WeightOffset);
	}

	if (!SelectedItem)
		return GetLootPackage(LootPackages);

	return SelectedItem;
}

#elif 0

template <typename MapType>
void* getChancedValue(const MapType& map) // thanks openai
{
	// Create a random number generator
	std::random_device rd;
	std::mt19937 gen(rd());

	// Create a distribution to generate random numbers
	// in the range [0, 1]
	std::uniform_real_distribution<> dist(0, 1);

	// Iterate over the map and keep track of the cumulative
	// probability as we go
	float cumulativeProb = 0;
	void* result = nullptr;

	for (const auto& [key, chance] : map)
	{
		// Increment the cumulative probability
		cumulativeProb += chance;

		// If the next random number is less than or equal to
		// the cumulative probability, then this is the
		// chanced value we want to return
		if (dist(gen) <= cumulativeProb)
		{
			result = key;
			break;
		}
	}

	return result;
}

static FFortLootTierData* GetLootTierData(std::vector<FFortLootTierData*>& LootTierData, int recursion = 0)
{
	float TotalWeight = 0;

	static auto LTD_WeightOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortLootTierData", "Weight");

	std::unordered_map<FFortLootTierData*, float> LTDWithChance;

	for (auto Item : LootTierData)
	{
		TotalWeight += *Get<float>(Item, LTD_WeightOffset);
	}

	for (auto Item : LootTierData)
	{
		float Chance = *Get<float>(Item, LTD_WeightOffset) / TotalWeight;

		if (Chance != 0)
			LTDWithChance.emplace(Item, Chance);
	}

	FFortLootTierData* SelectedItem = (FFortLootTierData*)getChancedValue(LTDWithChance);

	return SelectedItem;
}

static FFortLootPackageData* GetLootPackage(std::vector<FFortLootPackageData*>& LootPackages, int recursion = 0)
{
	static auto LP_WeightOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortLootPackageData", "Weight");

	float TotalWeight = 0;

	for (auto Item : LootPackages)
		TotalWeight += *Get<float>(Item, LP_WeightOffset);

	std::unordered_map<FFortLootTierData*, float> LPWithChance;

	for (auto Item : LootPackages)
	{
		TotalWeight += *Get<float>(Item, LP_WeightOffset);
	}

	for (auto Item : LootPackages)
	{
		float Chance = *Get<float>(Item, LP_WeightOffset) / TotalWeight;

		if (Chance != 0)
			LPWithChance.emplace(Item, Chance);
	}

	FFortLootPackageData* SelectedItem = (FFortLootPackageData*)getChancedValue(LPWithChance);

	if (!SelectedItem)
	{
		if (++recursion < 10)
			return GetLootPackage(LootPackages, recursion);
	}

	return SelectedItem;
}
#else
template<typename Value>
auto cumulative_weighted_choice(const std::vector<float>& weights, const std::vector<Value>& values) -> Value { // https://sharpdesigndigital.com/weighted-choice-in-c/
	static std::random_device hardware_seed;
	static std::mt19937_64 engine{ hardware_seed() };
	// assert(weights.size() == values.size());
	const auto max_weight{ weights.back() };
	std::uniform_real_distribution<float> distribution{ 0.0, max_weight };
	const auto raw_weight{ distribution(engine) };
	const auto valid_weight{ std::lower_bound(weights.cbegin(), weights.cend(), raw_weight) };
	const auto result_idx{ std::distance(weights.cbegin(), valid_weight) };
	return values[result_idx];
}


template<typename Weight>
auto cumulative_from_relative(const std::vector<Weight>& rel_weights) -> std::vector<Weight> {
	std::vector<Weight> cum_weights(rel_weights.size());
	std::partial_sum(rel_weights.cbegin(), rel_weights.cend(), cum_weights.begin());
	return cum_weights;
}

template<typename Weight>
auto relative_from_cumulative(const std::vector<Weight>& cum_weights) -> std::vector<Weight> {
	std::vector<Weight> rel_weights(cum_weights.size());
	std::adjacent_difference(cum_weights.cbegin(), cum_weights.cend(), rel_weights.begin());
	return rel_weights;
}

static FFortLootTierData* GetLootTierData(std::vector<FFortLootTierData*>& LootTierData, int recursion = 0)
{
	static auto LTD_WeightOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortLootTierData", "Weight");

	std::vector<float> Weights;

	for (auto Item : LootTierData)
		Weights.push_back(*Get<float>(Item, LTD_WeightOffset));

	const std::vector<float> cum_weights{ cumulative_from_relative(Weights) };

	auto picked = cumulative_weighted_choice(cum_weights, LootTierData);

	if (!picked)
	{
		if (++recursion < 10)
			return GetLootTierData(LootTierData, recursion);
	}

	return picked;
}

static FFortLootPackageData* GetLootPackage(std::vector<FFortLootPackageData*>& LootPackages, int recursion = 0)
{
	static auto LP_WeightOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortLootPackageData", "Weight");

	std::vector<float> Weights;

	for (auto Item : LootPackages)
		Weights.push_back(*Get<float>(Item, LP_WeightOffset));

	const std::vector<float> cum_weights{ cumulative_from_relative(Weights) };

	auto picked = cumulative_weighted_choice(cum_weights, LootPackages);
	
	if (!picked)
	{
		if (++recursion < 10)
			return GetLootPackage(LootPackages, recursion);
	}

	return picked;
}
#endif

#ifdef TEST_NEW_LOOTING

std::vector<std::pair<UObject*, int>> Looting::PickLootDrops(const std::string& TierGroupName, int WorldLevel, int ForcedLootTier)
{
	static auto LTD_NumLootPackageDropsOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortLootTierData", "NumLootPackageDrops");
	static auto LTD_LootPackageOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortLootTierData", "LootPackage");
	static auto LTD_TierGroupOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortLootTierData", "TierGroup");
	static auto LTD_WeightOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortLootTierData", "Weight");

	static auto LP_LootPackageCallOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortLootPackageData", "LootPackageCall");
	static auto LP_ItemDefinitionOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortLootPackageData", "ItemDefinition");
	static auto LP_LootPackageIDOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortLootPackageData", "LootPackageID");
	static auto LP_WeightOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortLootPackageData", "Weight");
	static auto LP_CountOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortLootPackageData", "Count");

	std::vector<std::pair<UObject*, int>> LootDrops;
	std::vector<FFortLootTierData*> LTD_OurTierGroup;
	std::vector<FFortLootPackageData*> TierGroupLPs;

	auto LP = GetLP();
	auto LTD = GetLTD();

	// std::cout << "LTD: " << LTD << '\n';

	if (LTD.empty() || LP.empty())
	{
		std::cout << "LTD size: " << LTD.size() << '\n';
		std::cout << "LP size: " << LP.size() << '\n';

		return LootDrops;
	}

	std::wstring TierGroupWStr = std::wstring(TierGroupName.begin(), TierGroupName.end());
	FString TierGroupFStr = TierGroupWStr.c_str();
	FName TierGroupFName = Helper::Conversion::StringToName(TierGroupFStr);

	for (int k = 0; k < LTD.size(); k++)
	{
		auto skidda = LTD.at(k);

		if (IsBadReadPtr(skidda))
			continue;

		// DEV_LOG("[{}] {}", k, skidda->GetFullName());

		TMap<FName, FFortLootTierData*> LTDRowMap = DataTables::GetRowMap<FFortLootTierData>(skidda);

		auto LTDData = LTDRowMap.Pairs.Elements.Data;

		if (IsBadReadPtr(LTDData.Data))
			continue;

		for (int i = 0; i < LTDData.Num(); i++)
		{
			auto& SparseArray = LTDData.At(i);
			auto& Pair = SparseArray.ElementData.Value;
			auto RowFName = Pair.First;
			FFortLootTierData* RowData = Pair.Second;

			if (!RowFName.ComparisonIndex || IsBadReadPtr(RowData))
				continue;

			auto tierGroup = Get<FName>(RowData, LTD_TierGroupOffset);

			if (TierGroupFName.ComparisonIndex == tierGroup->ComparisonIndex && *Get<float>(RowData, LTD_WeightOffset) != 0.f)
			{
				LTD_OurTierGroup.push_back(RowData);
			}
		}
	}

	DEV_LOG("LTD_OurTierGroup.size(): {}", LTD_OurTierGroup.size());

	if (LTD_OurTierGroup.size() == 0)
	{
		std::cout << "Could not find any data about tiergroup: " << TierGroupName << '\n';
		return LootDrops;
	}

	FFortLootTierData* ChosenRowData = GetLootTierData(LTD_OurTierGroup);

	if (!ChosenRowData)
	{
		std::cout << "Failed to choose a random row!\n";
		return LootDrops;
	}

	FName* ChosenRowData_LootPackage = Get<FName>(ChosenRowData, LTD_LootPackageOffset);
	std::string ChosenRowData_LootPackageStr = ChosenRowData_LootPackage->ToString();

	for (int k = 0; k < LP.size(); k++)
	{
		auto skidda = LP.at(k);

		if (IsBadReadPtr(skidda))
			continue;

		// DEV_LOG("[{}] {}", k, skidda->GetFullName());

		TMap<FName, FFortLootPackageData*> LPRowMap = DataTables::GetRowMap<FFortLootPackageData>(skidda);

		auto LPData = LPRowMap.Pairs.Elements.Data;

		if (IsBadReadPtr(LPData.Data))
			continue;

		// DEV_LOG("LPData.Num(): {}", LPData.Num());

		for (int i = 0; i < LPData.Num(); i++)
		{
			auto& SparseArray = LPData.At(i);
			auto& Pair = SparseArray.ElementData.Value;
			auto RowFName = Pair.First;
			FFortLootPackageData* RowData = Pair.Second;

			if (!RowFName.ComparisonIndex || IsBadReadPtr(RowData))
				continue;

			FName* LootPackageID = Get<FName>(RowData, LP_LootPackageIDOffset);

			// if (IsBadReadPtr(LootPackageID))
				// continue;

			// std::cout << std::format("{} {}\n", LootPackageID->ToString(), ChosenRowData_LootPackageStr);

			if (LootPackageID->ComparisonIndex == ChosenRowData_LootPackage->ComparisonIndex && *Get<float>(RowData, LP_WeightOffset) != 0.f)
			{
				TierGroupLPs.push_back(RowData);
			}
		}
	}

	DEV_LOG("TierGroupLPs: {}", TierGroupLPs.size());

	bool bIsWorldList = ChosenRowData_LootPackageStr.contains("WorldList");

	float NumLootPackageDrops = *Get<float>(ChosenRowData, LTD_NumLootPackageDropsOffset);

	/* int NumberLootDrops = (int)(float)((float)(NumLootPackageDrops + NumLootPackageDrops) - 0.5) >> 1; // NumLootPackageDrops - 1
	float v21 = NumLootPackageDrops - (float)NumberLootDrops; // this would just equal 1 no?

	if (v21 > 0.0000099999997)
	{
		std::cout << "gimme!\n";
		NumberLootDrops += 1; // v21 >= (float)((float)rand() * 0.000030518509);
	}

	NumLootPackageDrops = NumberLootDrops; */

	static int AmountToAdd = Engine_Version >= 424 ? 1 : 0;

	NumLootPackageDrops += AmountToAdd;

	LootDrops.reserve(NumLootPackageDrops);

	if (!NumLootPackageDrops)
		std::cout << "NumLootPackageDrops is 0!\n";

	for (int i = 0; i < NumLootPackageDrops; i++)
	{
		if (i >= TierGroupLPs.size())
			break;

		FFortLootPackageData* TierGroupLP = TierGroupLPs.at(i);

		if (IsBadReadPtr(TierGroupLP))
			continue;

		auto TierGroupLP_LPCall = ((FString*)(__int64(TierGroupLP) + LP_LootPackageCallOffset));

		if (IsBadReadPtr(TierGroupLP_LPCall->Data.Data))
			continue;

		std::string TierGroupLPStr = TierGroupLP_LPCall->ToString();

		std::vector<FFortLootPackageData*> lootPackageCalls;

		for (int k = 0; k < LP.size(); k++)
		{
			auto skidda = LP.at(k);

			if (IsBadReadPtr(skidda))
				continue;

			TMap<FName, FFortLootTierData*> LPRowMap = DataTables::GetRowMap<FFortLootPackageData>(skidda);

			auto LPData = LPRowMap.Pairs.Elements.Data;

			if (IsBadReadPtr(LPData.Data))
				continue;

			for (int z = 0; z < LPData.Num(); z++)
			{
				auto& SparseArray_2 = LPData.At(z);
				auto& Pair_2 = SparseArray_2.ElementData.Value;
				auto RowFName_2 = Pair_2.First;
				FFortLootPackageData* RowData_2 = Pair_2.Second;

				if (!RowFName_2.ComparisonIndex || IsBadReadPtr(RowData_2))
					continue;

				if (bIsWorldList)
				{
					lootPackageCalls.push_back(RowData_2);
				}
				else
				{
					FName* LootPackageID_2 = Get<FName>(RowData_2, LP_LootPackageIDOffset);

					if (LootPackageID_2->ToString() == TierGroupLPStr && *Get<float>(RowData_2, LP_WeightOffset) != 0 && *Get<float>(RowData_2, LP_CountOffset) != 0
						// && Get<TSoftObjectPtr>(RowData_2, LP_ItemDefinitionOffset)->Get(nullptr, true)
						)
					{
						lootPackageCalls.push_back(RowData_2);
					}
				}
			}
		}

		DEV_LOG("lootPackageCalls.size(): {}", lootPackageCalls.size());

		if (lootPackageCalls.size() == 0)
			continue;

		FFortLootPackageData* LootPackageCall = GetLootPackage(lootPackageCalls);

		if (!LootPackageCall)
			continue;

		UObject* ItemDefinition = Get<TSoftObjectPtr>(LootPackageCall, LP_ItemDefinitionOffset)->Get(nullptr, true);

		if (!ItemDefinition)
			continue;

		auto Count = *Get<int>(LootPackageCall, LP_CountOffset);

		std::pair<UObject*, int> Pair(ItemDefinition, Count);
		LootDrops.push_back(Pair);
	}

	DEV_LOG("LootDrops: {}", LootDrops.size());

	return LootDrops;
}

#endif

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
	std::cout << "lootahh!\n";

	/* if (Engine_Version >= 420)
	{
		auto Playlist = *Helper::GetPlaylist();

		static UObject* oldPlaylist = nullptr;

		if (oldPlaylist != Playlist)
		{
			oldPlaylist = Playlist;
		}
		else if (bInitialized)
		{
			std::cout << "[WARNING] Loot is already initialized!\n";
			return;
		}
	} */

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

	if (!LootPackages.size())
	{
		std::cout << "Failed to get LootPackages!\n";
		return;
	}

	for (int i = 0; i < LootPackages.size(); i++)
	{
		auto LootPackagesRowMap = DataTables::GetRowMap(LootPackages.at(i));

		auto fortnite = LootPackagesRowMap.Pairs.Elements.Data;

		bInitialized = fortnite.Num() > 0;

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

				if (ItemDef && ItemDef->ObjectID.AssetPathName.ComparisonIndex && *Weight != 0.f)
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
	}

	std::cout << "Initialized Looting V2!\n";

	return;
}