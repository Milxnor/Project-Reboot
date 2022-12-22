#include "harvesting.h"
#include "helper.h"
#include "inventory.h"
#include "datatables.h"

void Harvest(UObject* Controller, UObject* BuildingActor, float Damage)
{
	auto CurrentWeapon = Helper::GetCurrentWeapon(Helper::GetPawnFromController(Controller));

	// std::cout << "CurrentWeapon: " << CurrentWeapon->GetFullName() << '\n';

	if (!CurrentWeapon || Helper::GetWeaponData(CurrentWeapon) != Helper::GetPickaxeDef(Controller))
		return;

	// std::cout << "ret!\n";

	static auto ResourceTypeOffset = BuildingActor->GetOffset("ResourceType");
	static auto CarClass = FindObject("/Game/Building/ActorBlueprints/Prop/Car_DEFAULT.Car_DEFAULT_C");

	if (!CarClass)
		CarClass = FindObject("/Game/Building/ActorBlueprints/Prop/Car_Copper.Car_Copper_C");

	bool bHitWeakspot = Damage == 100.f;
	bool bDestroyed = false;
	bool bIsCar = BuildingActor->IsA(CarClass);
	auto ResourceType = *(TEnumAsByte<EFortResourceType>*)(__int64(BuildingActor) + ResourceTypeOffset);
	
	if (bDestroyed)
		return;

	/* float */ int ResourcesToGive = 0;

	if (!bIsCar)
	{
		static auto BuildingResourceAmountOverrideOffset = BuildingActor->GetOffset("BuildingResourceAmountOverride");
		auto BuildingResourceAmountOverride = (FCurveTableRowHandle*)(__int64(BuildingActor) + BuildingResourceAmountOverrideOffset);

		if (!BuildingResourceAmountOverride->RowName.ComparisonIndex) // player placed build
			return;
	}

	float MaxResourcesToSpawn = 6;
	ResourcesToGive = round(GetRandomDouble(MaxResourcesToSpawn / 2.f, MaxResourcesToSpawn));

	ResourcesToGive += bHitWeakspot ? round(GetRandomDouble(3, 5)) : 0;

	std::cout << "ResourcesToGive: " << ResourcesToGive << '\n';

	// std::cout << "ResourcesToGive: " << ResourcesToGive << '\n';

	struct { UObject* BuildingSMActor; TEnumAsByte<EFortResourceType> PotentialResourceType; int PotentialResourceCount; bool bDestroyed; bool bJustHitWeakspot; }
	ClientReportDamagedResourceBuilding_Params{ BuildingActor,
		bIsCar ? EFortResourceType::Metal : ResourceType, ResourcesToGive, bDestroyed, bHitWeakspot };

	static auto ClientReportDamagedResourceBuilding = FindObject<UFunction>("/Script/FortniteGame.FortPlayerController.ClientReportDamagedResourceBuilding");
	Controller->ProcessEvent(ClientReportDamagedResourceBuilding, &ClientReportDamagedResourceBuilding_Params);

	auto Pawn = Helper::GetPawnFromController(Controller);

	static auto WoodItemData = FindObject(("/Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
	static auto StoneItemData = FindObject(("/Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
	static auto MetalItemData = FindObject(("/Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

	UObject* ItemDef = WoodItemData;

	if (ClientReportDamagedResourceBuilding_Params.PotentialResourceType.Get() == EFortResourceType::Stone)
		ItemDef = StoneItemData;

	if (ClientReportDamagedResourceBuilding_Params.PotentialResourceType.Get() == EFortResourceType::Metal)
		ItemDef = MetalItemData;

	auto MaterialInstance = Inventory::FindItemInInventory(Controller, ItemDef);

	int AmountToGive = ClientReportDamagedResourceBuilding_Params.PotentialResourceCount;

	if (MaterialInstance && Pawn)
	{
		auto Entry = UFortItem::GetItemEntry(MaterialInstance);

		// BUG: You lose some mats if you have like 998 or idfk
		if (*FFortItemEntry::GetCount(Entry) >= 999)
		{
			Helper::SummonPickup(Pawn, ItemDef, Helper::GetActorLocation(Pawn), EFortPickupSourceTypeFlag::Other, EFortPickupSpawnSource::Unset, AmountToGive, false);
			return;
		}
	}

	Inventory::GiveItem(Controller, ItemDef, EFortQuickBars::Secondary, -1, AmountToGive);
}

bool Harvesting::OnDamageServer(UObject* BuildingActor, UFunction* Function, void* Parameters)
{
	if (!Parameters) // possible??
		return false;

	static auto CarDefault_OnDamageServer_Function = FindObject("/Game/Building/ActorBlueprints/Prop/Car_DEFAULT.Car_DEFAULT_C.OnDamageServer");
	static auto CarCopper_OnDamageServer_Function = FindObject("/Game/Building/ActorBlueprints/Prop/Car_Copper.Car_Copper_C.OnDamageServer");
	static auto BuildingActor_OnDamageServerFunction = FindObject("/Script/FortniteGame.BuildingActor.OnDamageServer");

	// UGH

	static auto CarDefault_InstigatedByOffset = CarDefault_OnDamageServer_Function->GetOffset("InstigatedBy", true); // CarDefault_OnDamageServer_Function ? FindOffsetStruct("Function /Game/Building/ActorBlueprints/Prop/Car_DEFAULT.Car_DEFAULT_C.OnDamageServer", "InstigatedBy") : 0;
	static auto CarDefault_DamageCauserOffset = CarDefault_OnDamageServer_Function->GetOffset("InstigatedBy", true);// CarDefault_OnDamageServer_Function ? FindOffsetStruct("Function /Game/Building/ActorBlueprints/Prop/Car_DEFAULT.Car_DEFAULT_C.OnDamageServer", "DamageCauser") : 0;
	static auto CarDefault_DamageOffset = CarDefault_OnDamageServer_Function->GetOffset("InstigatedBy", true); // CarDefault_OnDamageServer_Function ? FindOffsetStruct("Function /Game/Building/ActorBlueprints/Prop/Car_DEFAULT.Car_DEFAULT_C.OnDamageServer", "Damage") : 0;

	static auto CarCopper_InstigatedByOffset = CarCopper_OnDamageServer_Function->GetOffset("InstigatedBy", true); // CarCopper_OnDamageServer_Function ? FindOffsetStruct("Function /Game/Building/ActorBlueprints/Prop/Car_Copper.Car_Copper_C.OnDamageServer", "InstigatedBy") : 0;
	static auto CarCopper_DamageCauserOffset = CarCopper_OnDamageServer_Function->GetOffset("InstigatedBy", true); // CarCopper_OnDamageServer_Function ? FindOffsetStruct("Function /Game/Building/ActorBlueprints/Prop/Car_Copper.Car_Copper_C.OnDamageServer", "DamageCauser") : 0;
	static auto CarCopper_DamageOffset = CarCopper_OnDamageServer_Function->GetOffset("InstigatedBy", true); // CarCopper_OnDamageServer_Function ? FindOffsetStruct("Function /Game/Building/ActorBlueprints/Prop/Car_Copper.Car_Copper_C.OnDamageServer", "Damage") : 0;

	static auto BuildingActor_InstigatedByOffset = BuildingActor_OnDamageServerFunction->GetOffset("InstigatedBy", true); // FindOffsetStruct("Function /Script/FortniteGame.BuildingActor.OnDamageServer", "InstigatedBy");
	static auto BuildingActor_DamageCauserOffset = BuildingActor_OnDamageServerFunction->GetOffset("DamageCauser", true); // FindOffsetStruct("Function /Script/FortniteGame.BuildingActor.OnDamageServer", "DamageCauser");
	static auto BuildingActor_DamageOffset = BuildingActor_OnDamageServerFunction->GetOffset("Damage", true); // FindOffsetStruct("Function /Script/FortniteGame.BuildingActor.OnDamageServer", "Damage");

	static auto BuildingSMActorClass = FindObject(("/Script/FortniteGame.BuildingSMActor"));

	if (BuildingActor->IsA(BuildingSMActorClass))
	{
		auto InstigatedByOffset = Function == BuildingActor_OnDamageServerFunction ? BuildingActor_InstigatedByOffset
			: (Function == CarCopper_OnDamageServer_Function ? CarCopper_InstigatedByOffset : CarDefault_InstigatedByOffset);
		
		// std::cout << "InstigatedByOffset: " << InstigatedByOffset << '\n';

		auto DamageCauserOffset = Function == BuildingActor_OnDamageServerFunction ? BuildingActor_DamageCauserOffset
			: (Function == CarCopper_OnDamageServer_Function ? CarCopper_DamageCauserOffset : CarDefault_DamageCauserOffset);

		// std::cout << "DamageCauserOffset: " << DamageCauserOffset << '\n';

		auto DamageOffset = Function == BuildingActor_OnDamageServerFunction ? BuildingActor_DamageOffset
			: (Function == CarCopper_OnDamageServer_Function ? CarCopper_DamageOffset : CarDefault_DamageOffset);

		// std::cout << "DamageOffset: " << DamageOffset << '\n';

		static auto MeleeClass = Fortnite_Version < 10.00 ? FindObject("/Game/Weapons/FORT_Melee/Blueprints/B_Melee_Generic.B_Melee_Generic_C") :
			FindObject("/Game/Weapons/FORT_Melee/Blueprints/B_Athena_Pickaxe_Generic.B_Athena_Pickaxe_Generic_C");

		static auto FortWeaponPickaxeAthenaClass = FindObject("/Script/FortniteGame.FortWeaponPickaxeAthena");

		auto InstigatedBy = *(UObject**)(__int64(Parameters) + InstigatedByOffset);
		auto DamageCauser = *(UObject**)(__int64(Parameters) + DamageCauserOffset);
		auto Damage = (float*)(__int64(Parameters) + DamageOffset);

		if (!InstigatedBy || !DamageCauser || !Damage)
		{
			// std::cout << "fail5!\n";
			return false;
		}

		if (!Helper::IsPlayerController(InstigatedBy))
		{
			// std::cout << "fail4!\n";
			return false;
		}

		// static auto PlayerControllerClass = FindObject("/Game/Athena/Athena_PlayerController.Athena_PlayerController_C");

		if (!DamageCauser->IsA(FortWeaponPickaxeAthenaClass) && !DamageCauser->IsA(MeleeClass))
		{			
			/* UObject* Super = DamageCauser->ClassPrivate;

			while (Super)
			{
				std::cout << "Super Name: " << Super->GetFullName() << '\n';
				Super = *(UObject**)(__int64(Super) + SuperStructOffset);
			} */

			return false;
		}

		Harvest(InstigatedBy, BuildingActor, *Damage);
	}

	return false;
}

bool Harvesting::BlueprintCanAttemptGenerateResources(UObject* BuildingActor, UFunction* Function, void* Parameters)
{
	if (!Parameters)
		return false;

	struct BlueprintCanAttemptGenerateResources_Params { FGameplayTagContainer InTags; UObject* InstigatorController; bool ret; };

	auto Params = (BlueprintCanAttemptGenerateResources_Params*)Parameters;

	auto Controller = Params->InstigatorController;

	// std::cout << "Controller: " << Controller->GetFullName() << '\n';

	if (!Controller || !Helper::IsPlayerController(Controller))
		return false;

	Harvest(Controller, BuildingActor, 50.f);

	return false;
}