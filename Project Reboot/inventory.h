#pragma once

#include <iostream>

#include "structs.h"

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

	static int* GetLoadedAmmo(__int64* Entry)
	{
		static auto LoadedAmmoOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortItemEntry", "LoadedAmmo");
		auto LoadedAmmo = (int*)(__int64(Entry) + LoadedAmmoOffset);

		return LoadedAmmo;
	}

	void SetLoadedAmmo(__int64* Entry, UObject* Controller, int NewLoadedAmmo);
	
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
	TArray<__int64>* GetReplicatedEntries(UObject* Controller);

	static FGuid GetWeaponGuid(UObject* Weapon)
	{
		static auto ItemEntryGuidOffset = Weapon->GetOffset("ItemEntryGuid");

		return *(FGuid*)(__int64(Weapon) + ItemEntryGuidOffset);
	}

	__int64* GetEntryFromWeapon(UObject* Controller, UObject* Weapon);

	// FUNCTIONS

	void Update(UObject* Controller, bool bAddOrRemove = false, FFastArraySerializerItem* ModifiedItem = nullptr);
	UObject* GiveItem(UObject* Controller, UObject* ItemDefinition, EFortQuickBars Bars, int Slot, int Count = 1, bool bUpdate = true);
	UObject* EquipWeapon(UObject* Controller, const FGuid& Guid, UObject* ItemDefinition);
	UObject* EquipWeapon(UObject* Controller, UObject* Instance);
	EFortQuickBars WhatQuickBars(UObject* Definition); // returns the quickbar the item should go in
	void TakeItem(UObject* Controller, const FGuid& Guid, int Count, bool bForceRemove = false);

	// FINDERS

	UObject* FindItemInInventory(UObject* Controller, const FGuid& Guid); // Returns an item instance
	UObject* FindItemInInventory(UObject* Controller, UObject* Definition); // Returns an item instance

	// HOOKS

	bool ServerExecuteInventoryItem(UObject* Controller, UFunction* Function, void* Parameters);
	bool ServerAttemptInventoryDrop(UObject* Controller, UFunction*, void* Parameters);
	bool ServerHandlePickup(UObject* Pawn, UFunction*, void* Parameters);
	void HandleReloadCost(UObject* Weapon, int AmountToRemove);
}