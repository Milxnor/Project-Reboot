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
		
		if (SearchLootTierGroup->ToString() == "Loot_Treasure")
		{
			FString correctTreasure = L"Loot_AthenaTreasure";
			*SearchLootTierGroup = Helper::Conversion::StringToName(correctTreasure);
			// correctTreasure.Free();
		}
		else if (SearchLootTierGroup->ToString() == "Loot_Ammo")
		{
			FString correctAmmo = L"Loot_AthenaAmmoLarge";
			*SearchLootTierGroup = Helper::Conversion::StringToName(correctAmmo);
			// correctAmmo.Free();
		}

		auto CorrectLocation = Helper::GetCorrectLocationDynamic(ReceivingActor);

#ifdef TEST_NEW_LOOTING
		auto LootDrops = Looting::PickLootDrops(SearchLootTierGroup->ToString());

		for (auto& LootDrop : LootDrops)
		{
			Helper::SummonPickup(nullptr, LootDrop.first, CorrectLocation, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Unset, LootDrop.second, true);
		}
#else

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
#endif
	} // END CONTAINER

	static auto PawnClass = FindObject("/Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C");

	if (ReceivingActor->IsA(PawnClass))
	{
		std::cout << "revive!\n";

		// static auto skidda = FindObject<UFunction>("/Game/Abilities/NPC/Generic/GAB_AthenaDBNO.GAB_AthenaDBNO_C.OnFinish_4C169D40441E45B462D83CBBA67F6E45");

		auto DBNOPawn = ReceivingActor;
		auto DBNOController = Helper::GetControllerFromPawn(DBNOPawn);
		auto DBNOPawnASC = Helper::GetAbilitySystemComponent(DBNOPawn);

		auto InstigatorController = Controller;
		auto InstigatorPawn = Helper::GetPawnFromController(InstigatorController);

		/* static auto GAB_AthenaDBNOReviveClass = FindObject("/Game/Abilities/NPC/Generic/GAB_AthenaDBNORevive.Default__GAB_AthenaDBNORevive_C");

		static auto GameplayEventDataStruct = FindObject("/Script/GameplayAbilities.GameplayEventData");
		static auto GameplayEventDataSize = Helper::GetSizeOfClass(GameplayEventDataStruct);

		__int64* EventData = (__int64*)Alloc(GameplayEventDataSize);

		if (!EventData)
			return false; */

		static auto GAB_AthenaDBNOClass = FindObject("/Game/Abilities/NPC/Generic/GAB_AthenaDBNO.Default__GAB_AthenaDBNO_C");

		static auto ClientCancelAbility = FindObject<UFunction>("/Script/GameplayAbilities.AbilitySystemComponent.ClientCancelAbility");
		static auto ClientEndAbility = FindObject<UFunction>("/Script/GameplayAbilities.AbilitySystemComponent.ClientEndAbility");
		static auto ServerEndAbility = FindObject<UFunction>("/Script/GameplayAbilities.AbilitySystemComponent.ServerEndAbility");

		void* spec = nullptr;

		UObject* ClassToFind = GAB_AthenaDBNOClass->ClassPrivate;

		auto compareAbilities = [&spec, &ClassToFind](__int64* Spec) {
			auto CurrentAbility = GetAbilityFromSpec(Spec);

			if ((*CurrentAbility)->ClassPrivate == ClassToFind)
			{
				spec = Spec;
				// AbilityToReturn = *CurrentAbility;
				return;
			}
		};

		LoopSpecs(DBNOPawnASC, compareAbilities);

		std::cout << spec << '\n';

		if (!spec)
			return false;

		static auto GameplayAbilityActivationInfoSize = Helper::GetSizeOfClass(FindObject("/Script/GameplayAbilities.GameplayAbilityActivationInfo"));
		static auto PredictionKeySize = Helper::GetSizeOfClass(FindObject("/Script/GameplayAbilities.PredictionKey"));

		int ParamsSize = sizeof(FGameplayAbilitySpecHandle) + GameplayAbilityActivationInfoSize + PredictionKeySize;

		auto CancelParams = Alloc(ParamsSize);

		if (!CancelParams)
			return false;

		RtlSecureZeroMemory(CancelParams, ParamsSize);

		static auto HandleOffset = FindOffsetStruct2("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Handle");

		static auto AbilityToCancelOffset = FindOffsetStruct2("/Script/GameplayAbilities.AbilitySystemComponent.ClientCancelAbility", "AbilityToCancel", true, true);
		static auto Param_ActivationInfoOffset = FindOffsetStruct2("/Script/GameplayAbilities.AbilitySystemComponent.ClientCancelAbility", "ActivationInfo", true, true);

		*Get<FGameplayAbilitySpecHandle>(CancelParams, AbilityToCancelOffset) = *Get<FGameplayAbilitySpecHandle>(spec, HandleOffset);

		// static auto CurrentActivationInfoOffset = (*GetAbilityFromSpec(spec))->GetOffset("CurrentActivationInfo");
		// auto CurrentActivationInfo = Get<void>(*GetAbilityFromSpec(spec), CurrentActivationInfoOffset);

		static auto Spec_ActivationInfoOffset = FindOffsetStruct2("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "ActivationInfo");
		auto ActivationInfo = Get<void>(spec, Spec_ActivationInfoOffset);

		memcpy_s(Get<void>(CancelParams, Param_ActivationInfoOffset), GameplayAbilityActivationInfoSize, ActivationInfo, GameplayAbilityActivationInfoSize);

		DBNOPawnASC->ProcessEvent(ClientCancelAbility, CancelParams);
		DBNOPawnASC->ProcessEvent(ClientEndAbility, CancelParams);
		DBNOPawnASC->ProcessEvent(ServerEndAbility, CancelParams);

		static auto bIsDBNOOffset = DBNOPawn->GetOffsetSlow("bIsDBNO");
		static auto bIsDBNOFieldMask = GetFieldMask(DBNOPawn->GetPropertySlow("bIsDBNO"));

		std::cout << "bIsDBNOFieldMask: " << (int)bIsDBNOFieldMask << '\n';

		auto bIsDBNO = Get<PlaceholderBitfield>(DBNOPawn, bIsDBNOOffset);
		SetBitfield(bIsDBNO, bIsDBNOFieldMask, false);

		static auto OnRep_bIsDBNO = FindObject<UFunction>("/Script/FortniteGame.FortPawn.OnRep_IsDBNO");
		DBNOPawn->ProcessEvent(OnRep_bIsDBNO);

		static auto clientonpoa = FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerZone.ClientOnPawnRevived");
		Helper::GetControllerFromPawn(DBNOPawn)->ProcessEvent(clientonpoa, &Controller);

		bool bSkydive = false;

		static auto RespawnPlayerAfterDeath = FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerAthena.RespawnPlayerAfterDeath");

		if (RespawnPlayerAfterDeath)
			Helper::GetControllerFromPawn(DBNOPawn)->ProcessEvent(RespawnPlayerAfterDeath, &bSkydive); // more skunked than sulfur 2.4.2

		Helper::SetHealth(DBNOPawn, 30);
	}

	if (ReceivingActorName.contains("Vehicle"))
	{
		std::cout << "vehicle: " << ReceivingActorName << '\n';

		return false;

		struct FVehiclePawnState
		{
			UObject* Vehicle;                                                  // 0x0000(0x0008) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			float                                              VehicleApexZ;                                             // 0x0008(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			unsigned char                                      SeatIndex;                                                // 0x000C(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			unsigned char                                      ExitSocketIndex;                                          // 0x000D(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			bool                                               bOverrideVehicleExit;                                     // 0x000E(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			unsigned char                                      UnknownData00[0x1];                                       // 0x000F(0x0001) MISSED OFFSET
			FVector                                     SeatTransitionVector;                                     // 0x0010(0x000C) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			float                                              EntryTime;                                                // 0x001C(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		};

		auto Pawn = Helper::GetPawnFromController(Controller);

		static auto VehicleStateRepOffset = Pawn->GetOffset("VehicleStateRep");
		auto VehicleStateRep = Get<FVehiclePawnState>(Pawn, VehicleStateRepOffset);

		VehicleStateRep->Vehicle = ReceivingActor;
		VehicleStateRep->SeatIndex = 0;
		VehicleStateRep->ExitSocketIndex = 0;
		VehicleStateRep->EntryTime = Helper::GetTimeSeconds();

		static auto VehicleStateLocalffset = Pawn->GetOffset("VehicleStateLocal");
		auto VehicleStateLocal = Get<FVehiclePawnState>(Pawn, VehicleStateLocalffset);

		VehicleStateLocal->Vehicle = ReceivingActor;
		VehicleStateLocal->SeatIndex = 0;
		VehicleStateLocal->ExitSocketIndex = 0;
		VehicleStateLocal->EntryTime = Helper::GetTimeSeconds();

		static auto VehicleStateLastTickOffset = Pawn->GetOffset("VehicleStateLastTick");
		auto VehicleStateLastTick = Get<FVehiclePawnState>(Pawn, VehicleStateLastTickOffset);

		VehicleStateLastTick->Vehicle = ReceivingActor;
		VehicleStateLastTick->SeatIndex = 0;
		VehicleStateLastTick->ExitSocketIndex = 0;
		VehicleStateLastTick->EntryTime = Helper::GetTimeSeconds();
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