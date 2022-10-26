#include "interaction.h"
#include "helper.h"
#include "loot.h"

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

		if (Engine_Version >= 420 && Engine_Version < 424)
		{
			if (bAlreadySearchedBitfield->Fourth)
				return false;

			bAlreadySearchedBitfield->Fourth = true;

			static auto OnRep_bAlreadySearched = FindObject<UFunction>("/Script/FortniteGame.BuildingContainer.OnRep_bAlreadySearched");
			BuildingContainer->ProcessEvent(OnRep_bAlreadySearched);
		}

		if (ReceivingActorName.contains("Chest"))
		{
			auto DefInRow = Looting::GetRandomItem(ItemType::Weapon);
			{
				auto WeaponDef = DefInRow.Definition;

				if (!WeaponDef)
					return false;

				auto Ammo = Helper::GetAmmoForDefinition(WeaponDef);

				auto Location = Helper::GetCorrectLocation(BuildingContainer);

				Helper::SummonPickup(nullptr, WeaponDef, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, DefInRow.DropCount, true);

				Helper::SummonPickup(nullptr, Ammo.first, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Ammo.second);

				auto ConsumableInRow = RandomBoolWithWeight(5, 1, 100) ? Looting::GetRandomItem(ItemType::Trap) : Looting::GetRandomItem(ItemType::Consumable);

				Helper::SummonPickup(nullptr, ConsumableInRow.Definition, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, ConsumableInRow.DropCount); // *Consumable->Member<int>(("DropCount")));

				static auto WoodItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
				static auto StoneItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
				static auto MetalItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

				auto random = rand() % 3;

				int amountOfMaterialToDrop = 30; // Looting::GetRandomItem(ItemType::Resource, Looting::LootItems).DropCount;

				if (random == 1)
					Helper::SummonPickup(nullptr, WoodItemData, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, amountOfMaterialToDrop);
				else if (random == 2)
					Helper::SummonPickup(nullptr, StoneItemData, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, amountOfMaterialToDrop);
				else
					Helper::SummonPickup(nullptr, MetalItemData, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, amountOfMaterialToDrop);
			}
		}
	}

	return false;
}