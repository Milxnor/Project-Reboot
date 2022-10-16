#include "inventory.h"
#include "helper.h"

UObject* Inventory::GetWorldInventory(UObject* Controller)
{
	static auto WorldInventoryOffset = Controller->GetOffset("WorldInventory");
	auto WorldInventoryP = Get<UObject*>(Controller, WorldInventoryOffset);

	return *WorldInventoryP;
}

__int64* Inventory::GetInventory(UObject* Controller)
{
	auto WorldInventory = GetWorldInventory(Controller);

	if (WorldInventory)
	{
		static auto InventoryOffset = WorldInventory->GetOffset("Inventory");
		auto Inventory = Get<__int64>(WorldInventory, InventoryOffset);

		return Inventory;
	}

	return nullptr;
}

TArray<UObject*>* Inventory::GetItemInstances(UObject* Controller)
{
	static auto ItemInstancesOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemList"), ("ItemInstances"));
	auto Inventory = GetInventory(Controller);
	
	return Inventory ? (TArray<UObject*>*)(__int64(Inventory) + ItemInstancesOffset) : nullptr;
}

TArray<__int64>* Inventory::GetReplicatedEntries(UObject* Controller)
{
	static auto ReplicatedEntriesOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemList"), ("ReplicatedEntries"));
	auto Inventory = GetInventory(Controller);

	return (TArray<__int64>*)(__int64(Inventory) + ReplicatedEntriesOffset);
}

UObject* GetQuickBars(UObject* Controller)
{
	static auto QuickBarsOffset = Controller->GetOffset("QuickBars");

	return *(UObject**)(__int64(Controller) + QuickBarsOffset);
}

void Inventory::Update(UObject* Controller, bool bAddOrRemove, FFastArraySerializerItem* ModifiedItem)
{
	auto Inventory = GetInventory(Controller);

	if (ModifiedItem)
		FastTArray::MarkItemDirty(Inventory, ModifiedItem);
	else if (bAddOrRemove)
		FastTArray::MarkArrayDirty(Inventory);

	auto WorldInventory = GetWorldInventory(Controller);

	static auto WorldHandleInvUpdate = FindObject<UFunction>("Function /Script/FortniteGame.FortInventory.HandleInventoryLocalUpdate");

	if (WorldHandleInvUpdate)
		WorldInventory->ProcessEvent(WorldHandleInvUpdate);

	{
		static auto PCHandleInvUpdate = FindObject<UFunction>("Function /Script/FortniteGame.FortPlayerController.HandleWorldInventoryLocalUpdate");

		if (PCHandleInvUpdate)
			Controller->ProcessEvent(PCHandleInvUpdate);
	}

	if (Fortnite_Version < 7.4)
	{
		const auto QuickBars = GetQuickBars(Controller);

		if (QuickBars)
		{
			static auto OnRep_PrimaryQuickBar = FindObject<UFunction>("Function /Script/FortniteGame.FortQuickBars.OnRep_PrimaryQuickBar");
			QuickBars->ProcessEvent(OnRep_PrimaryQuickBar);

			static auto OnRep_SecondaryQuickBar = FindObject<UFunction>("Function /Script/FortniteGame.FortQuickBars.OnRep_SecondaryQuickBar");
			QuickBars->ProcessEvent(OnRep_SecondaryQuickBar);

			static auto OnRep_QuickBar = FindObject<UFunction>("Function /Script/FortniteGame.FortPlayerController.OnRep_QuickBar");
			Controller->ProcessEvent(OnRep_QuickBar);
		}
	}
	else
	{
		/* static auto ClientForceUpdateQuickbar = Controller->Function(("ClientForceUpdateQuickbar"));
		auto PrimaryQuickbar = EFortQuickBars::Primary;
		Controller->ProcessEvent(ClientForceUpdateQuickbar, &PrimaryQuickbar);
		auto SecondaryQuickbar = EFortQuickBars::Secondary;
		Controller->ProcessEvent(ClientForceUpdateQuickbar, &SecondaryQuickbar);

		if (Fortnite_Version < 8)
		{
			static auto UpdateQuickBars = Controller->Function(("UpdateQuickBars"));

			if (UpdateQuickBars)
				Controller->ProcessEvent(UpdateQuickBars);
		} */
	}
}

UObject* Inventory::GiveItem(UObject* Controller, UObject* ItemDefinition, EFortQuickBars Bars, int Slot, int Count, bool bUpdate)
{
	if (!ItemDefinition)
		return nullptr;

	static auto FortWorldItemClass = FindObjectSlow("Class /Script/FortniteGame.FortWorldItem", false);
	static auto SizeOfItemEntryStruct = Helper::GetSizeOfClass(FFortItemEntry::ItemEntryStruct);

	auto ItemInstance = Helper::Easy::SpawnObject(FortWorldItemClass, Helper::GetTransientPackage());

	if (ItemInstance)
	{
		auto ItemEntry = UFortItem::GetItemEntry(ItemInstance);

		if (Fortnite_Version < 7.4)
		{
			static auto ServerAddItemInternal = FindObject<UFunction>("Function /Script/FortniteGame.FortQuickBars.ServerAddItemInternal");
			auto QuickBars = GetQuickBars(Controller);

			struct
			{
				FGuid Item;
				EFortQuickBars Quickbar;
				int Slot;
			} SAIIParams{ *FFortItemEntry::GetGuid(ItemEntry), Bars, Slot};

			QuickBars->ProcessEvent(ServerAddItemInternal, &SAIIParams);
		}

		*FFortItemEntry::GetItemDefinition(ItemEntry) = ItemDefinition;
		*FFortItemEntry::GetCount(ItemEntry) = Count;

		GetItemInstances(Controller)->Add(ItemInstance);
		GetReplicatedEntries(Controller)->Add(*ItemEntry, SizeOfItemEntryStruct);

		if (bUpdate)
			Update(Controller, true);

		return ItemInstance;
	}

	return nullptr;
}

UObject* Inventory::EquipWeapon(UObject* Controller, UObject* ItemDefinition, const FGuid& Guid)
{
	struct {
		UObject* Def;
		FGuid Guid;
		UObject* Wep;
	} params{ ItemDefinition, Guid };

	static auto EquipWeaponDefinition = FindObject<UFunction>("Function /Script/FortniteGame.FortPawn.EquipWeaponDefinition");

	auto Pawn = Helper::GetPawnFromController(Controller);
	Pawn->ProcessEvent(EquipWeaponDefinition, &params);

	// Helper::SetOwner(params.Wep, Pawn);
	
	return params.Wep;
}

UObject* Inventory::EquipWeapon(UObject* Controller, UObject* Instance)
{
	auto Def = UFortItem::GetDefinition(Instance);
	auto Guid = UFortItem::GetGuid(Instance);

	return Def && Guid ? EquipWeapon(Controller, *Def, *Guid) : nullptr;
}

EFortQuickBars Inventory::WhatQuickBars(UObject* Definition)
{
	static auto FortWeaponItemDefinitionClass = FindObject(("Class /Script/FortniteGame.FortWeaponItemDefinition"));
	static auto FortDecoItemDefinitionClass = FindObject("Class /Script/FortniteGame.FortDecoItemDefinition");

	if (Definition->IsA(FortWeaponItemDefinitionClass) && !Definition->IsA(FortDecoItemDefinitionClass))
		return EFortQuickBars::Primary;
	else
		return EFortQuickBars::Secondary;
}

UObject* Inventory::FindItemInInventory(UObject* Controller, const FGuid& Guid)
{
	auto ItemInstances = GetItemInstances(Controller);

	if (ItemInstances)
	{
		for (int i = 0; i < ItemInstances->Num(); i++)
		{
			auto ItemInstance = ItemInstances->At(i);

			if (ItemInstance)
			{
				auto ItemGuid = UFortItem::GetGuid(ItemInstance);

				if (ItemGuid && (*ItemGuid) == Guid)
					return ItemInstance;
			}
		}
	}

	return nullptr;
}

UObject* Inventory::FindItemInInventory(UObject* Controller, UObject* Definition)
{
	auto ItemInstances = GetItemInstances(Controller);

	if (ItemInstances)
	{
		for (int i = 0; i < ItemInstances->Num(); i++)
		{
			auto ItemInstance = ItemInstances->At(i);

			if (ItemInstance)
			{
				auto definition = UFortItem::GetDefinition(ItemInstance);

				if (definition && (*definition) == Definition)
					return ItemInstance;
			}
		}
	}

	return nullptr;
}



bool Inventory::ServerExecuteInventoryItem(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (!Parameters)
		return false;

	auto Guid = *(FGuid*)Parameters;

	auto Instance = FindItemInInventory(Controller, Guid);

	if (Instance)
	{
		auto Definition = UFortItem::GetDefinition(Instance);
		auto NewWeapon = EquipWeapon(Controller, Definition ? *Definition : nullptr, Guid);
	}
	else
		std::cout << "No ItemInstance!\n";

	return false;
}

bool Inventory::ServerHandlePickup(UObject* Pawn, UFunction*, void* Parameters)
{
	UObject* Pickup = Parameters ? *(UObject**)Parameters : nullptr;

	if (!Pickup)
		return false;

	auto PickupEntry = Helper::GetEntryFromPickup(Pickup);

	auto Definition = FFortItemEntry::GetItemDefinition(PickupEntry);
	auto Count = FFortItemEntry::GetCount(PickupEntry);

	auto Controller = Helper::GetControllerFromPawn(Pawn);

	GiveItem(Controller, *Definition, WhatQuickBars(*Definition), *Count);

	return false;
}