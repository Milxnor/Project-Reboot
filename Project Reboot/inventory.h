#pragma once

#include "structs.h"
#include <iostream>

namespace FFortItemEntry
{
	inline UObject* ItemEntryStruct = nullptr;

	static UObject** GetItemDefinition(__int64* Entry)
	{
		static auto ItemDefinitionOffset = ItemEntryStruct->GetOffset("ItemDefinition", true);
		auto Definition = (UObject**)(__int64(&*Entry) + ItemDefinitionOffset);

		return Definition;
	}

	static FGuid* GetGuid(__int64* Entry)
	{
		static auto GuidOffset = ItemEntryStruct->GetOffset("ItemGuid", true);
		auto Guid = (FGuid*)(__int64(&*Entry) + GuidOffset);

		return Guid;
	}

	static int* GetCount(__int64* Entry)
	{
		static auto CountOffset = 0xC; // FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("Count"), true); // i tried everything
		auto Count = (int*)(__int64(&*Entry) + CountOffset);

		return Count;
	}

	static int* GetLoadedAmmo(__int64* Entry)
	{
		static auto LoadedAmmoOffset = ItemEntryStruct->GetOffset("LoadedAmmo", true);
		auto LoadedAmmo = (int*)(__int64(&*Entry) + LoadedAmmoOffset);

		return LoadedAmmo;
	}
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

	// FUNCTIONS

	void Update(UObject* Controller, bool bAddOrRemove = false, FFastArraySerializerItem* ModifiedItem = nullptr);
	UObject* GiveItem(UObject* Controller, UObject* ItemDefinition, EFortQuickBars Bars, int Slot, int Count = 1, bool bUpdate = true);
	UObject* EquipWeapon(UObject* Controller, UObject* ItemDefinition, const FGuid& Guid);
	UObject* EquipWeapon(UObject* Controller, UObject* Instance);
	EFortQuickBars WhatQuickBars(UObject* Definition); // returns the quickbar the item should go in

	// FINDERS

	UObject* FindItemInInventory(UObject* Controller, const FGuid& Guid); // Returns an item instance
	UObject* FindItemInInventory(UObject* Controller, UObject* Definition); // Returns an item instance

	// HOOKS

	bool ServerExecuteInventoryItem(UObject* Controller, UFunction* Function, void* Parameters);
	bool ServerAttemptInventoryDrop(UObject* Controller, UFunction*, void* Parameters);
	bool ServerHandlePickup(UObject* Pawn, UFunction*, void* Parameters);
}