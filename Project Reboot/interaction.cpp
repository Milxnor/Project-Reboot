#include "interaction.h"
#include "helper.h"
#include "loot.h"
#include "inventory.h"
#include "datatables.h"

bool Interaction::ServerAttemptInteract(UObject* cController, UFunction*, void* Parameters)
{
	if (!Parameters)
		return false;

	UObject* Controller = Engine_Version < 423 ? cController : Helper::GetOwnerOfComponent(cController);

	auto ReceivingActor = *(UObject**)Parameters;

	if (!ReceivingActor)
		return false;

	static auto BuildingContainerClass = FindObject("/Script/FortniteGame.BuildingContainer");

	auto ReceivingActorName = ReceivingActor->GetFullName();

	if (ReceivingActor->IsA(BuildingContainerClass))
	{
		auto BuildingContainer = ReceivingActor;

		static auto bAlreadySearchedOffset = BuildingContainer->GetOffset("bAlreadySearched");

		auto bAlreadySearchedBitfield = Get<PlaceholderBitfield>(BuildingContainer, bAlreadySearchedOffset);

		if (Engine_Version >= 420 && Fortnite_Season < 10)
		{
			if (bAlreadySearchedBitfield->Fourth)
				return false;

			bAlreadySearchedBitfield->Fourth = true;
		}
		else if (Fortnite_Season == 10)
		{
			if (bAlreadySearchedBitfield->Sixth)
				return false;

			bAlreadySearchedBitfield->Sixth = true;
		}
		else if (Engine_Version >= 424) // got on 13.40
		{
			if (bAlreadySearchedBitfield->Eighth)
				return false;

			bAlreadySearchedBitfield->Eighth = true;
		}

		static auto OnRep_bAlreadySearched = FindObject<UFunction>("/Script/FortniteGame.BuildingContainer.OnRep_bAlreadySearched");
		BuildingContainer->ProcessEvent(OnRep_bAlreadySearched);

		static auto SearchLootTierGroupOffset = BuildingContainer->GetOffset("SearchLootTierGroup");
		auto SearchLootTierGroup = Get<FName>(BuildingContainer, SearchLootTierGroupOffset);
		
		auto CorrectLocation = Helper::GetCorrectLocation(ReceivingActor);

		/* auto LootTierGroupName = SearchLootTierGroup->ToString();

		LootTierGroupName = LootTierGroupName == "Loot_Treasure" ? "Loot_AthenaTreasure" : LootTierGroupName;

		std::cout << "LootTierGroupName: " << LootTierGroupName << '\n'; */

		static auto ChestClass = FindObject("/Game/Building/ActorBlueprints/Containers/Tiered_Chest_Athena.Tiered_Chest_Athena_C");
		static auto ChestClass2 = FindObject("/Game/Building/ActorBlueprints/Containers/Tiered_Chest_6_Parent.Tiered_Chest_6_Parent_C");

		if (ReceivingActor->IsA(ChestClass) || ReceivingActor->IsA(ChestClass2))
		{
			auto DefInRow = Looting::GetRandomItem(ItemType::Weapon);
			{
				auto WeaponDef = DefInRow.Definition;

				if (!WeaponDef)
					return false;

				auto Ammo = Helper::GetAmmoForDefinition(WeaponDef);

				Helper::SummonPickup(nullptr, WeaponDef, CorrectLocation, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, DefInRow.DropCount, true);

				Helper::SummonPickup(nullptr, Ammo.first, CorrectLocation, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Ammo.second);

				auto ConsumableInRow = RandomBoolWithWeight(5, 1, 100) ? Looting::GetRandomItem(ItemType::Trap) : Looting::GetRandomItem(ItemType::Consumable);

				Helper::SummonPickup(nullptr, ConsumableInRow.Definition, CorrectLocation, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, ConsumableInRow.DropCount); // *Consumable->Member<int>(("DropCount")));

				static auto WoodItemData = FindObject("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
				static auto StoneItemData = FindObject("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
				static auto MetalItemData = FindObject("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");

				auto random = GetRandomInt(1, 3);

				int amountOfMaterialToDrop = 30; // Looting::GetRandomItem(ItemType::Resource, Looting::LootItems).DropCount;

				if (random <= 1)
					Helper::SummonPickup(nullptr, WoodItemData, CorrectLocation, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, amountOfMaterialToDrop);
				else if (random == 2)
					Helper::SummonPickup(nullptr, StoneItemData, CorrectLocation, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, amountOfMaterialToDrop);
				else
					Helper::SummonPickup(nullptr, MetalItemData, CorrectLocation, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, amountOfMaterialToDrop);

				// Helper::InitializeBuildingActor(Controller, BuildingContainer);

				/* static auto fn1 = FindObject<UFunction>("/Game/Building/ActorBlueprints/Containers/Tiered_Chest_Athena.Tiered_Chest_Athena_C.OnLoot");
				static auto fn2 = FindObject<UFunction>("/Game/Building/ActorBlueprints/Containers/Tiered_Chest_Athena.Tiered_Chest_Athena_C.OnSetSearched");

				BuildingContainer->ProcessEvent(fn2);
				BuildingContainer->ProcessEvent(fn1);

				static auto fn3 = FindObject<UFunction>("/Script/Engine.ActorComponent.Activate");

				static auto Loot_EffectOffset = BuildingContainer->GetOffset("Loot_Effect");
				static auto fn4 = FindObject<UFunction>("/Script/Engine.ActorComponent.SetIsReplicated");

				bool bShouldReplicate = true;

				auto Loot_Effect = *Get<UObject*>(BuildingContainer, Loot_EffectOffset);
				std::cout << "Loot_Effect: " << Loot_Effect << '\n';

				Loot_Effect->ProcessEvent(fn4, &bShouldReplicate);

				bool bReset = true;
				Loot_Effect->ProcessEvent(fn3, &bReset); */
			}
		}
	}

	if (ReceivingActorName.contains("Wumba")) // AB_Athena_Wumba_C
	{
		if (Fortnite_Season < 15) // idk but whenever they start taking gold we dont support
		{
			static auto WoodCostCurveOffset = ReceivingActor->GetOffset("WoodCostCurve");
			auto WoodCostCurve = *Get<FScalableFloat>(ReceivingActor, WoodCostCurveOffset);

			struct FWeaponUpgradeItemRow
			{
				void* FTableRowBaseInheritance;
				UObject* CurrentWeaponDef;                                  // 0x8(0x8)(Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				UObject* UpgradedWeaponDef;                                 // 0x10(0x8)(Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				EFortWeaponUpgradeCosts           WoodCost;                                          // 0x18(0x1)(Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				EFortWeaponUpgradeCosts           MetalCost;                                         // 0x19(0x1)(Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				EFortWeaponUpgradeCosts           BrickCost;                                         // 0x1A(0x1)(Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			};

			static auto WumbaDataTable = FindObject("/Game/Items/Datatables/AthenaWumbaData.AthenaWumbaData");

			static auto LootPackagesRowMap = DataTables::GetRowMap(WumbaDataTable);

			auto fortnite = LootPackagesRowMap.Pairs.Elements.Data;

			auto Pawn = Helper::GetPawnFromController(Controller);
			auto CurrentHeldWeapon = Helper::GetCurrentWeapon(Pawn);
			auto CurrentHeldWeaponDef = Helper::GetWeaponData(CurrentHeldWeapon);

			FWeaponUpgradeItemRow* FoundRow = nullptr;

			for (int i = 0; i < fortnite.Num() - 1; i++)
			{
				auto Man = fortnite.At(i);
				auto& Pair = Man.ElementData.Value;
				auto RowFName = Pair.First;

				if (!RowFName.ComparisonIndex)
					continue;

				auto RowName = RowFName.ToString();
				auto Row = (FWeaponUpgradeItemRow*)Pair.Second;

				if (Row->CurrentWeaponDef == CurrentHeldWeaponDef)
				{
					std::cout << "found at: " << i << '\n';
					FoundRow = Row;
					break;
				}
			}

			if (!FoundRow)
			{
				std::cout << "failed to find row!\n";
				return false;
			}

			auto NewDefinition = FoundRow->UpgradedWeaponDef;

			// probably improper code from here

			int WoodCost;
			int StoneCost;
			int MetalCost;

			{
				// using enum EFortWeaponUpgradeCosts;

				WoodCost = (int)FoundRow->WoodCost * 50;
				StoneCost = (int)FoundRow->BrickCost * 50;
				MetalCost = (int)FoundRow->MetalCost * 50;
			}

			// to here

			std::cout << "WoodCost: " << WoodCost << '\n';

			static auto WoodItemData = FindObject(("/Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
			static auto StoneItemData = FindObject(("/Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
			static auto MetalItemData = FindObject(("/Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

			auto WoodInstance = Inventory::FindItemInInventory(Controller, WoodItemData);
			auto WoodCount = UFortItem::GetCount(WoodInstance);

			// if (*WoodCount < WoodCost)
				// return false;

			auto StoneInstance = Inventory::FindItemInInventory(Controller, StoneItemData);
			auto StoneCount = UFortItem::GetCount(StoneInstance);

			// if (*StoneCount < StoneCost)
				// return false;

			auto MetalInstance = Inventory::FindItemInInventory(Controller, MetalItemData);
			auto MetalCount = UFortItem::GetCount(MetalInstance);

			// if (*MetalCount < MetalCost)
				// return false;

			auto setCount = [](__int64* Entry, UObject* Controller, int NewCount) -> void {
				*FFortItemEntry::GetCount(Entry) = NewCount;

				auto Inventory = Inventory::GetInventory(Controller);

				FastTArray::MarkItemDirty(Inventory, (FFastArraySerializerItem*)Entry);

				auto EntryGuid = *FFortItemEntry::GetGuid(Entry);

				auto ahah = [&Inventory, &EntryGuid, &NewCount](__int64* currentEntry) -> bool {
					if (*FFortItemEntry::GetGuid(currentEntry) == EntryGuid)
					{
						// std::cout << "Found!\n";
						*FFortItemEntry::GetCount(currentEntry) = NewCount;
						FastTArray::MarkItemDirty(Inventory, (FFastArraySerializerItem*)currentEntry);
						return true;
					}

					return false;
				};

				LoopReplicatedEntries(Controller, ahah);
			};

			// setCount(UFortItem::GetItemEntry(WoodInstance), Controller, *WoodCount - 50);
			// setCount(UFortItem::GetItemEntry(StoneInstance), Controller, *StoneCount - 50);
			// setCount(UFortItem::GetItemEntry(MetalInstance), Controller, *MetalCount - 50);

			Inventory::TakeItem(Controller, *UFortItem::GetGuid(WoodInstance), *WoodCount - 50);
			Inventory::TakeItem(Controller, *UFortItem::GetGuid(StoneInstance), *StoneCount - 50);
			Inventory::TakeItem(Controller, *UFortItem::GetGuid(MetalInstance), *MetalCount - 50);

			Inventory::TakeItem(Controller, Inventory::GetWeaponGuid(CurrentHeldWeapon), true);

			Inventory::GiveItem(Controller, NewDefinition, EFortQuickBars::Max_None, 0);
		}
	}

	return false;
}