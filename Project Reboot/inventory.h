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
		static auto CountOffset = ItemEntryStruct->GetOffset("Count", true);
		auto Count = (int*)(__int64(&*Entry) + CountOffset);

		return Count;
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

	// FINDERS

	UObject* FindItemInInventory(UObject* Controller, const FGuid& Guid); // Returns an item instance

	// HOOKS

	bool ServerExecuteInventoryItem(UObject* Controller, UFunction* Function, void* Parameters);
}