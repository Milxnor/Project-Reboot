#include "inventory.h"
#include "helper.h"
#include <functional>

int FFortItemEntry::GetStructSize()
{
	static auto Size = Helper::GetSizeOfClass(ItemEntryStruct);

	return Size;
}

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
	static auto ItemInstancesOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortItemList", "ItemInstances");
	auto Inventory = GetInventory(Controller);
	
	return Inventory ? (TArray<UObject*>*)(__int64(Inventory) + ItemInstancesOffset) : nullptr;
}

TArray<__int64>* Inventory::GetReplicatedEntries(UObject* Controller)
{
	static auto ReplicatedEntriesOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortItemList", "ReplicatedEntries");
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
	WorldInventory->ProcessEvent(WorldHandleInvUpdate); // Needed for the guids it sets it somehow

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
		static auto ClientForceUpdateQuickbar = FindObject<UFunction>("Function /Script/FortniteGame.FortPlayerController.ClientForceUpdateQuickbar");
		auto PrimaryQuickbar = EFortQuickBars::Primary;
		Controller->ProcessEvent(ClientForceUpdateQuickbar, &PrimaryQuickbar);

		auto SecondaryQuickbar = EFortQuickBars::Secondary;
		Controller->ProcessEvent(ClientForceUpdateQuickbar, &SecondaryQuickbar);
	}
}

static float GetMaxStackSize(UObject* ItemDefinition)
{
	static auto MaxStackSizeOffset = ItemDefinition->GetOffset("MaxStackSize");

	bool bIsScalableFloat = true; // I swear i saw a version with int

	return bIsScalableFloat ? ((FScalableFloat*)(__int64(ItemDefinition) + MaxStackSizeOffset))->Value :
		*(int*)(__int64(ItemDefinition) + MaxStackSizeOffset);
}

void LoopReplicatedEntries(UObject* Controller, std::function<bool(__int64*)> func)
{
	auto ReplicatedEntries = Inventory::GetReplicatedEntries(Controller);

	for (int i = 0; i < ReplicatedEntries->Num(); i++)
	{
		auto CurrentReplicatedEntry = ReplicatedEntries->AtPtr(i, FFortItemEntry::GetStructSize());

		if (CurrentReplicatedEntry)
		{
			if (func(CurrentReplicatedEntry))
				return;
		}
	}
}

bool IncreaseItemCount(UObject* Controller, UObject* Instance, int IncreaseBy)
{
	if (Controller && Instance)
	{
		auto Inventory = Inventory::GetInventory(Controller);
		auto ItemEntry = UFortItem::GetItemEntry(Instance);

		auto ItemEntryGuid = *FFortItemEntry::GetGuid(ItemEntry);
		auto CurrentCount = FFortItemEntry::GetCount(ItemEntry);

		auto NewCount = *CurrentCount + IncreaseBy;
		auto OldCount = *CurrentCount;
		*CurrentCount = NewCount;

		auto ChangeCount = [&Inventory, &Controller, &ItemEntryGuid, &NewCount](__int64* Entry) -> bool {
			if (ItemEntryGuid == *FFortItemEntry::GetGuid(Entry))
			{
				*FFortItemEntry::GetCount(Entry) = NewCount;
				FastTArray::MarkItemDirty(Inventory, (FFastArraySerializerItem*)Entry);

				return true;
			}

			return false;
		};

		LoopReplicatedEntries(Controller, ChangeCount);

		FastTArray::MarkItemDirty(Inventory, (FFastArraySerializerItem*)ItemEntry);

		return true;
	}

	return false;
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
		bool bDontCreateNewStack = false;
		bool bShouldStack = false;

		static auto FortResourceItemDefinition = FindObject(("Class /Script/FortniteGame.FortResourceItemDefinition"));

		if (ItemDefinition->IsA(FortResourceItemDefinition))
			bDontCreateNewStack = true;

		auto ItemEntry = UFortItem::GetItemEntry(ItemInstance);
		auto ItemInstances = Inventory::GetItemInstances(Controller);

		// start bad code

		__int64* StackingItemEntry = nullptr;
		UObject* StackingItemInstance = nullptr;

		int OverStack = 0;

		std::vector<UObject*> InstancesOfItem;

		for (int i = 0; i < ItemInstances->Num(); i++)
		{
			auto CurrentItemInstance = ItemInstances->At(i);

			if (CurrentItemInstance && *UFortItem::GetDefinition(CurrentItemInstance) == ItemDefinition)
			{
				InstancesOfItem.push_back(CurrentItemInstance);
			}
		}

		if (InstancesOfItem.size() > 0)
		{
			auto MaxStackCount = GetMaxStackSize(ItemDefinition);

			// We need this skunked thing because if they have 2 full stacks and half a stack then we want to find the lowest stack and stack to there.
			for (auto InstanceOfItem : InstancesOfItem)
			{
				if (InstanceOfItem)
				{
					auto CurrentItemEntry = UFortItem::GetItemEntry(InstanceOfItem);

					if (ItemEntry)
					{
						auto currentCount = FFortItemEntry::GetCount(ItemEntry);

						if (currentCount && *currentCount < MaxStackCount)
						{
							StackingItemInstance = InstanceOfItem;
							StackingItemEntry = CurrentItemEntry;
							break;
						}
					}
				}
			}
		}

		if (StackingItemEntry)
		{
			auto currentCount = FFortItemEntry::GetCount(StackingItemEntry);

			if (currentCount)
			{
				//			      3	      +   2   -      6       =   -1
				OverStack = *currentCount + Count - GetMaxStackSize(ItemDefinition);

				// checks if it is going to overstack, if it is then we subtract the incoming count by the overstack, but its not then we just use the incoming count.
				int AmountToStack = OverStack > 0 ? Count - OverStack : Count;

				IncreaseItemCount(Controller, StackingItemInstance, AmountToStack);

				if (OverStack <= 0) // there is no overstack, we can now return peacefully.
					return StackingItemInstance;
			}
		}

		if (bDontCreateNewStack && StackingItemInstance) 
			return ItemInstance;

		// to here

		auto CreateAndAddItem = [&](int countForItem) {
			if (Fortnite_Version < 7.4)
			{
				static auto ServerAddItemInternal = FindObject<UFunction>("Function /Script/FortniteGame.FortQuickBars.ServerAddItemInternal");
				auto QuickBars = GetQuickBars(Controller);

				struct
				{
					FGuid Item;
					EFortQuickBars Quickbar;
					int Slot;
				} SAIIParams{ *FFortItemEntry::GetGuid(ItemEntry), Bars, Slot };

				QuickBars->ProcessEvent(ServerAddItemInternal, &SAIIParams);
			}

			*FFortItemEntry::GetItemDefinition(ItemEntry) = ItemDefinition;
			*FFortItemEntry::GetCount(ItemEntry) = countForItem;

			GetItemInstances(Controller)->Add(ItemInstance);
			GetReplicatedEntries(Controller)->Add(*ItemEntry, SizeOfItemEntryStruct);

			if (bUpdate)
				Update(Controller, true);
		};

		if (OverStack > 0)
			CreateAndAddItem(OverStack);
		else
			CreateAndAddItem(Count);
	}

	return ItemInstance;
}

UObject* Inventory::EquipWeapon(UObject* Controller, const FGuid& Guid, UObject* ItemDefinition)
{
	// if (Helper::IsInAircraft(Controller))
		// return nullptr;

	struct {
		UObject* Def;
		FGuid Guid;
		UObject* Wep;
	} params{ ItemDefinition, Guid };

	static auto EquipWeaponDefinition = FindObject<UFunction>("Function /Script/FortniteGame.FortPawn.EquipWeaponDefinition");

	auto Pawn = Helper::GetPawnFromController(Controller);

	if (Pawn)
		Pawn->ProcessEvent(EquipWeaponDefinition, &params);
	
	return params.Wep;
}

UObject* Inventory::EquipWeapon(UObject* Controller, UObject* Instance)
{
	auto Def = UFortItem::GetDefinition(Instance);
	auto Guid = UFortItem::GetGuid(Instance);

	return Def && Guid ? EquipWeapon(Controller, *Guid, *Def) : nullptr;
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

				if (*ItemGuid == Guid)
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

				if (*definition == Definition)
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
		auto NewWeapon = EquipWeapon(Controller, Guid, Definition ? *Definition : nullptr);
	}
	else
		std::cout << "Unable to find Guid in Inventory for ServerExecuteInventoryItem!\n";

	return false;
}

bool Inventory::ServerAttemptInventoryDrop(UObject* Controller, UFunction*, void* Parameters)
{
	if (!Parameters)
		return false;

	struct AFortPlayerController_ServerAttemptInventoryDrop_Params { FGuid ItemGuid; int Count; };

	auto Params = (AFortPlayerController_ServerAttemptInventoryDrop_Params*)Parameters;

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

	Helper::DestroyActor(Pickup);

	return false;
}