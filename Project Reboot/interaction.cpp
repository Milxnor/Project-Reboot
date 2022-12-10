#include "interaction.h"
#include "helper.h"
#include "loot.h"
#include "inventory.h"
#include "datatables.h"
#include "abilities.h"

#include <intrin.h>
#include <MinHook.h>

inline __int64 rettruae() 
{ 
	std::cout << "skidda! " << __int64(_ReturnAddress()) - __int64(GetModuleHandleW(0)) << '\n';
	return 1; 
}

bool Interaction::ServerAttemptInteract(UObject* cController, UFunction*, void* Parameters)
{
	if (!Parameters)
		return false;

	UObject* Controller = Engine_Version < 423 ? cController : Helper::GetOwnerOfComponent(cController);

	auto ReceivingActor = *(UObject**)Parameters;

	if (!ReceivingActor)
		return false;

	std::cout << "aa!\n";

	/* auto funcaa = Memory::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 33 DB 48 8B FA 48 8B F1 48 85 D2 0F 84 ? ? ? ? 8B 82");
	UObject* (__fastcall* asf23f)(UObject* a1, UObject* a2) = decltype(asf23f)(funcaa);

	void* a = asf23f(ReceivingActor, FindObject("/Script/FortniteGame.FortInteractInterface"));

	// std::cout << "aname: " << (*(UObject**)(__int64(a) - 0x330))->GetFullName() << '\n';

	void** vtble = ((UObject*)a)->VFTable;
	std::cout << "FortInteractInterface VTABLE: " << __int64(vtble) - __int64(GetModuleHandleW(0)) << '\n';
	auto funca = (PVOID)vtble[0x5];

	std::cout << "ABuildingContainer::ServerOnAttemptInteract: " << __int64(funca) - __int64(GetModuleHandleW(0)) << '\n';

	// DWORD Old;

	// VirtualProtect((PVOID)vtble[0x5], 8, PAGE_READWRITE, &Old);
	// *(PVOID*)vtble[0x5] = &rettruae;
	// VirtualProtect((PVOID)vtble[0x5], 8, Old, &Old);

	// MH_CreateHook((PVOID)funca, rettruae, nullptr);
	// MH_EnableHook((PVOID)funca);

	return false; */

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
		
		auto CorrectLocation = Helper::GetCorrectLocationDynamic(ReceivingActor);

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

		else if (ReceivingActorName.contains("Ammo"))
		{
			auto AmmoInRow = Looting::GetRandomItem(ItemType::Ammo);
			auto AmmoDef = AmmoInRow.Definition;

			if (AmmoDef)
			{
				auto DropCount = AmmoInRow.DropCount; // *AmmoDef->Member<int>(("DropCount"));
				Helper::SummonPickup(nullptr, AmmoDef, CorrectLocation, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::AmmoBox, DropCount, true, false);
			}

			auto AmmoInRow2 = Looting::GetRandomItem(ItemType::Ammo);
			auto AmmoDef2 = AmmoInRow2.Definition;

			if (AmmoDef2)
			{
				auto DropCount2 = AmmoInRow2.DropCount; // *AmmoDef->Member<int>(("DropCount"));
				Helper::SummonPickup(nullptr, AmmoDef2, CorrectLocation, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::AmmoBox, DropCount2, true, false);
			}
		}

		else
		{
			UObject* classPrivate = BuildingContainer->ClassPrivate;
			static auto ClassClass = FindObject("/Script/CoreUObject.Class");

			while (classPrivate && classPrivate != ClassClass)
			{
				std::cout << "Class Private Name: " << classPrivate->GetFullName() << '\n';
				classPrivate = classPrivate->ClassPrivate;
			}
		}

	} // CONTAINER

	static auto PawnClass = FindObject("/Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C");

	if (ReceivingActor->IsA(PawnClass))
	{
		std::cout << "revive!\n";

		return false;

		// static auto skidda = FindObject<UFunction>("/Game/Abilities/NPC/Generic/GAB_AthenaDBNO.GAB_AthenaDBNO_C.OnFinish_4C169D40441E45B462D83CBBA67F6E45");

		auto DBNOPawn = ReceivingActor;
		auto DBNOPawnASC = Helper::GetAbilitySystemComponent(DBNOPawn);

		/* static auto skidad = FindObject<UFunction>("/Script/GameplayAbilities.AbilitySystemComponent.TryActivateAbilityByClass");

		struct {
			UObject* InAbilityToActivate;                               // 0x0(0x8)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			bool                                         bAllowRemoteActivation;                            // 0x8(0x1)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			bool                                         ReturnValue;
		} para{ FindObject("/Game/Abilities/NPC/Generic/GAB_AthenaDBNORevive.GAB_AthenaDBNORevive_C"), true };

		DeadPawnASC->ProcessEvent(skidad, &para);

		*/

		/* auto skadgfv = Abilities::DoesASCHaveAbility(DeadPawnASC, FindObject("/Game/Abilities/NPC/Generic/GAB_AthenaDBNO.Default__GAB_AthenaDBNO_C"));

		std::cout << "skadgf.size(): " << skadgfv.size() << '\n';

		if (skadgfv.size() >= 2) */
		{
			auto palyerstatename = Helper::GetPlayerStateFromController(Helper::GetControllerFromPawn(DBNOPawnASC))->GetName();
			std::cout << "palyerstatename: " << palyerstatename << '\n';
			auto skadgf = FindObjectSlow(std::format("{}.GAB_AthenaDBNO_C_", palyerstatename)); // skadgfv[1];
			std::cout << "skadgf: " << skadgf << '\n';

			if (skadgf)
			{
				std::cout << "name: " << skadgf->GetFullName() << '\n';

				static auto aa = FindObject<UFunction>("/Script/GameplayAbilities.GameplayAbility.K2_CancelAbility");
				skadgf->ProcessEvent(aa);

				/*
				struct
				{
				public:
					FGameplayAbilitySpecHandle            AbilityToCancel;                                   // 0x0(0x4)(Parm, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					FGameplayAbilityActivationInfo        ActivationInfo;                                    // 0x8(0x20)(Parm, NoDestructor, NativeAccessSpecifierPublic)
				} UAbilitySystemComponent_ServerCancelAbility_Params{skadgf->CurrentActivationInfo};
				*/
			}
		}

		static auto clientonpoa = FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerZone.ClientOnPawnRevived");
		Helper::GetControllerFromPawn(DBNOPawn)->ProcessEvent(clientonpoa, &Controller);
	}

	if (Engine_Version >= 424 && ReceivingActorName.contains("Wumba")) // AB_Athena_Wumba_C
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