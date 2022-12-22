#pragma once

#include <iostream>
#include <functional>

#include "structs.h"

void LoopReplicatedEntries(UObject* Controller, std::function<bool(__int64*)> func);

namespace FFortItemEntry
{
	inline UObject* ItemEntryStruct = nullptr;

	static UObject** GetItemDefinition(__int64* Entry)
	{
		static auto ItemDefinitionOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortItemEntry", "ItemDefinition");
		auto Definition = (UObject**)(__int64(Entry) + ItemDefinitionOffset);

		return Definition;
	}

	static FGuid* GetGuid(__int64* Entry)
	{
		static auto GuidOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortItemEntry", "ItemGuid");
		auto Guid = (FGuid*)(__int64(Entry) + GuidOffset);

		return Guid;
	}

	static int* GetCount(__int64* Entry)
	{
		static auto CountOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortItemEntry", "Count");
		auto Count = (int*)(__int64(Entry) + CountOffset);

		return Count;
	}

	static bool* GetIsReplicatedCopy(__int64* Entry)
	{
		static auto bIsReplicatedCopyOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortItemEntry", "bIsReplicatedCopy");
		auto bIsReplicatedCopy = (bool*)(__int64(Entry) + bIsReplicatedCopyOffset);

		return bIsReplicatedCopy;
	}

	static bool* GetIsDirty(__int64* Entry)
	{
		static auto bIsDirtyOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortItemEntry", "bIsDirty");
		auto bIsDirty = (bool*)(__int64(Entry) + bIsDirtyOffset);
		
		return bIsDirty;
	}

	static int* GetLoadedAmmo(__int64* Entry)
	{
		static auto LoadedAmmoOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortItemEntry", "LoadedAmmo");
		auto LoadedAmmo = (int*)(__int64(Entry) + LoadedAmmoOffset);

		return LoadedAmmo;
	}

	void SetLoadedAmmo(__int64* Entry, UObject* Controller, int NewLoadedAmmo);
	void ModifyStateValue(UObject* Controller, __int64* Entry, FFortItemEntryStateValue* StateValue, bool bAdd = true, bool bMarkDirty = true);

	int GetStructSize();
}

namespace UFortItem
{
	static __int64* GetItemEntry(UObject* ItemInstance)
	{
		static auto ItemEntryOffset = ItemInstance->GetOffset("ItemEntry");
		return Get<__int64>(ItemInstance, ItemEntryOffset);
	}

	static FGuid* GetGuid(UObject* ItemInstance) 
	{
		auto ItemEntry = GetItemEntry(ItemInstance);
		return ItemEntry ? FFortItemEntry::GetGuid(ItemEntry) : nullptr;
	}

	static UObject** GetDefinition(UObject* ItemInstance)
	{
		auto ItemEntry = GetItemEntry(ItemInstance);
		return ItemEntry ? FFortItemEntry::GetItemDefinition(ItemEntry) : nullptr;
	}

	static int* GetCount(UObject* ItemInstance)
	{
		auto ItemEntry = GetItemEntry(ItemInstance);
		return ItemEntry ? FFortItemEntry::GetCount(ItemEntry) : nullptr;
	}
}

namespace Inventory
{
	UObject* GetWorldInventory(UObject* Controller);
	__int64* GetInventory(UObject* Controller);
	TArray<UObject*>* GetItemInstances(UObject* Controller);

	template <typename EntryStruct = __int64>
	TArray<EntryStruct>* GetReplicatedEntries(UObject* Controller);

	static FGuid GetWeaponGuid(UObject* Weapon)
	{
		static auto ItemEntryGuidOffset = Weapon->GetOffset("ItemEntryGuid");

		return *(FGuid*)(__int64(Weapon) + ItemEntryGuidOffset);
	}

	__int64* GetEntryFromWeapon(UObject* Controller, UObject* Weapon);

	// FUNCTIONS

	void Update(UObject* Controller, bool bAddOrRemove = false, FFastArraySerializerItem* ModifiedItem = nullptr);
	UObject* GiveItem(UObject* Controller, UObject* ItemDefinition, EFortQuickBars Bars, int Slot, int Count = 1, bool bUpdate = true, int LoadedAmmo = -1, bool* bDidStack = nullptr);
	UObject* EquipWeapon(UObject* Controller, const FGuid& Guid, UObject* ItemDefinition, int Ammo = 0);
	UObject* EquipWeapon(UObject* Controller, UObject* Instance, int Ammo = 0);
	EFortQuickBars WhatQuickBars(UObject* Definition); // returns the quickbar the item should go in
	UObject* TakeItem(UObject* Controller, const FGuid& Guid, int Count, bool bForceRemove = false);
	void WipeInventory(UObject* Controller, bool bTakePickaxe = true);

	// FINDERS

	UObject* FindItemInInventory(UObject* Controller, const FGuid& Guid); // Returns an item instance
	UObject* FindItemInInventory(UObject* Controller, UObject* Definition); // Returns an item instance

	// HOOKS

	bool ServerExecuteInventoryItem(UObject* Controller, UFunction* Function, void* Parameters);
	bool ServerAttemptInventoryDrop(UObject* Controller, UFunction*, void* Parameters);
	bool ServerExecuteInventoryWeapon(UObject* Controller, UFunction*, void* Parameters);
	bool ServerHandlePickup(UObject* Pawn, UFunction*, void* Parameters);
	void HandleReloadCost(UObject* Weapon, int AmountToRemove);
}