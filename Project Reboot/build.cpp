#include "build.h"
#include "helper.h"
#include "inventory.h"

void SetBuildingActorTeam(UObject* BuildingActor, int NewTeamIndex)
{
	static auto TeamOffset = BuildingActor->GetOffsetSlow("Team");

	if (TeamOffset != 0)
	{
		auto Team = (TEnumAsByte<uint8_t>*)(__int64(BuildingActor) + TeamOffset);
		*Team = NewTeamIndex; // ??
	}

	static auto Building_TeamIndexOffset = BuildingActor->GetOffsetSlow("TeamIndex");

	if (Building_TeamIndexOffset != 0)
	{
		auto TeamIndex = (uint8_t*)(__int64(BuildingActor) + Building_TeamIndexOffset);
		*TeamIndex = NewTeamIndex;
	}
}

namespace Build
{
	bool ServerCreateBuildingActor(UObject* Controller, UFunction* Function, void* Parameters)
	{
		if (!Parameters)
			return false;

		static auto WoodItemData = FindObject("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
		static auto StoneItemData = FindObject("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
		static auto MetalItemData = FindObject("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");

		UObject* MatDefinition = nullptr;
		UObject* MatInstance = nullptr;

		auto Pawn = Helper::GetPawnFromController(Controller);

		UObject* BuildingClass = nullptr;
		BothVector BuildingLocation;
		BothRotator BuildingRotation;
		bool bMirrored = false;

		if (Fortnite_Version >= 8.3)
		{
			if (Fortnite_Season < 20)
			{
				struct FCreateBuildingActorData { uint32_t BuildingClassHandle; FVector BuildLoc; FRotator BuildRot; bool bMirrored; };

				auto Params = *(FCreateBuildingActorData*)Parameters;

				BuildingLocation = Params.BuildLoc;
				BuildingRotation = Params.BuildRot;
				bMirrored = Params.bMirrored;
			}
			else
			{
				struct FCreateBuildingActorData { uint32_t BuildingClassHandle; DVector BuildLoc; DRotator BuildRot; bool bMirrored; };

				auto Params = *(FCreateBuildingActorData*)Parameters;

				BuildingLocation = Params.BuildLoc;
				BuildingRotation = Params.BuildRot;
				bMirrored = Params.bMirrored;
			}

			static auto BroadcastRemoteClientInfoOffset = Controller->GetOffset("BroadcastRemoteClientInfo");
			auto RemoteClientInfo = *(UObject**)(__int64(Controller) + BroadcastRemoteClientInfoOffset);

			static auto RemoteBuildableClassOffset = RemoteClientInfo->GetOffset("RemoteBuildableClass");
			BuildingClass = *(UObject**)(__int64(RemoteClientInfo) + RemoteBuildableClassOffset);
		}
		else
		{
			struct FBuildingClassData { UObject* BuildingClass; int PreviousBuildingLevel; int UpgradeLevel; };
			struct SCBAParams { FBuildingClassData BuildingClassData; FVector BuildLoc; FRotator BuildRot; bool bMirrored; };

			auto Params = (SCBAParams*)Parameters;
			
			BuildingClass = Params->BuildingClassData.BuildingClass;
			BuildingLocation = Params->BuildLoc;
			BuildingRotation = Params->BuildRot;
			bMirrored = Params->bMirrored;
		}

		if (!BuildingClass)
			return false;

		TArray<UObject*> ExistingBuildings;
		// __int64 v32[2]{};
		char dababy;

		bool bCanBuild = Fortnite_Season < 20 ? !Defines::CantBuild(Helper::GetWorld(), BuildingClass, BuildingLocation.fV, BuildingRotation.fR, bMirrored, &ExistingBuildings, &dababy) :
			!Defines::CantBuildDouble(Helper::GetWorld(), BuildingClass, BuildingLocation.dV, BuildingRotation.dR, bMirrored, &ExistingBuildings, &dababy);;

		if (bCanBuild)
		{
			std::cout << "ExistingBuildings.Num(): " << ExistingBuildings.Num() << '\n';

			for (int i = 0; i < ExistingBuildings.Num(); i++)
			{
				auto ExistingBuilding = ExistingBuildings.At(i);

				Helper::DestroyActor(ExistingBuilding);
			}

			UObject* BuildingActor = Helper::Easy::SpawnActorDynamic(BuildingClass, BuildingLocation, BuildingRotation, Pawn);

			if (BuildingActor)
			{
				static auto ResourceTypeOffset = BuildingActor->GetOffset("ResourceType");
				auto ResourceType = *Get<TEnumAsByte<EFortResourceType>>(BuildingActor, ResourceTypeOffset);

				MatDefinition = ResourceType.Get() == EFortResourceType::Wood ? WoodItemData : ResourceType.Get() == EFortResourceType::Stone ? StoneItemData : MetalItemData;

				MatInstance = Inventory::FindItemInInventory(Controller, MatDefinition);

				bool bShouldDestroy = true;

				if (!IsBadReadPtr(MatInstance))
				{
					auto MatCount = UFortItem::GetCount(MatInstance);

					if (!IsBadReadPtr(MatCount)) // && Helper::IsStructurallySupported(BuildingActor)
					{
						SetBuildingActorTeam(BuildingActor, *Helper::GetTeamIndex(Helper::GetPlayerStateFromController(Controller)));

						Helper::InitializeBuildingActor(Controller, BuildingActor, true);

						bShouldDestroy = false;
					}
				}

				if (bShouldDestroy)
				{

				}
				else
				{
					if (!Defines::bIsPlayground && !Defines::bInfiniteMats)
					{
						Inventory::TakeItem(Controller, *UFortItem::GetGuid(MatInstance), 10);
					}
				}
			}
		}

		ExistingBuildings.Free();

		return false;
	}

	bool ServerSpawnDeco(UObject* DecoTool, UFunction*, void* Parameters)
	{
		if (!Parameters)
			return false;

		// void ServerSpawnDeco(const struct FVector& Location, const struct FRotator& Rotation, class ABuildingSMActor* AttachedActor, TEnumAsByte<EBuildingAttachmentType> InBuildingAttachmentType);
		// 	void ServerSpawnDeco(const struct FVector& Location, const struct FRotator& Rotation, class ABuildingSMActor* AttachedActor);

		struct ServerSpawnDeco_Params { FVector Location; FRotator Rotation; UObject* AttachedActor; };

		auto Params = (ServerSpawnDeco_Params*)Parameters;

		if (!Params->AttachedActor)
			return false;

		auto Pawn = Helper::GetOwner(DecoTool);

		if (!Pawn)
			return false;

		auto Controller = Helper::GetControllerFromPawn(Pawn);

		static auto ItemDefinitionOffset = DecoTool->GetOffset("ItemDefinition");
		auto TrapItemDefinition = *(UObject**)(__int64(DecoTool) + ItemDefinitionOffset);

		static auto GetBlueprintClass = FindObject<UFunction>("/Script/FortniteGame.FortDecoItemDefinition.GetBlueprintClass");
		UObject* BlueprintClass = nullptr;
		TrapItemDefinition->ProcessEvent(GetBlueprintClass, &BlueprintClass);

		if (!BlueprintClass)
			return false;

		auto NewTrap = Helper::Easy::SpawnActor(BlueprintClass, Params->Location, Params->Rotation);

		if (!NewTrap)
			return false;

		if (!Defines::bIsPlayground)
		{
			// Inventory::DecreaseItemCount(Controller, Inventory::FindItemInInventory(Controller, TrapItemDefinition), 1);
		}

		std::cout << "New Trap name: " << NewTrap->GetFullName() << '\n';

		static auto AttachedToOffset = NewTrap->GetOffset("AttachedTo");
		auto AttachedTo = (UObject**)(__int64(NewTrap) + AttachedToOffset);
		*AttachedTo = Params->AttachedActor;

		static auto OnRep_AttachedTo = FindObject<UFunction>("/Script/FortniteGame.BuildingTrap.OnRep_AttachedTo");
		NewTrap->ProcessEvent(OnRep_AttachedTo);

		// BuildingActor->BuildingAttachmentType = Params->InBuildingAttachmentType;

		// SetBuildingActorTeam(NewTrap, *Teams::GetTeamIndex(Helper::GetPlayerStateFromController(Controller)));

		static auto TrapDataOffset = NewTrap->GetOffset("TrapData");
		auto TrapData = (UObject**)(__int64(NewTrap) + TrapDataOffset);
		*TrapData = TrapItemDefinition; // probably useless

		static auto TrapLevelOffset = NewTrap->GetOffset("TrapLevel");
		auto TrapLevel = (int*)(__int64(NewTrap) + TrapLevelOffset);
		// *TrapLevel = 1; // ??

		SetBuildingActorTeam(NewTrap, *Helper::GetTeamIndex(Helper::GetPlayerStateFromController(Controller)));

		Helper::InitializeBuildingActor(Controller, NewTrap);

		auto TrapInstance = Inventory::FindItemInInventory(Controller, TrapItemDefinition);

		if (TrapInstance)
			Inventory::TakeItem(Controller, *UFortItem::GetGuid(TrapInstance), 1);

		return false;
	}

	bool ServerCreateBuildingAndSpawnDeco(UObject* DecoTool, UFunction*, void* Parameters)
	{
		// First, the params were Location and Rotation, then BuildingLocation, BuildingRotation, Location, and Rotation.

		static auto BuildingRotationOffset = FindOffsetStruct2("/Script/FortniteGame.FortDecoTool.ServerSpawnDeco", "BuildingRotation", true, true);

		UObject* BuildingClass = nullptr;
		FVector TrapLocation;
		FRotator TrapRotation{};
		UObject* NewBuilding = nullptr;

		struct ServerSpawnDeco_Params { FVector Location; FRotator Rotation; UObject* AttachedActor; };

		if (BuildingRotationOffset != 0) // skunked
		{
			// const struct FVector_NetQuantize10& BuildingLocation, const struct FRotator& BuildingRotation, const struct FVector_NetQuantize10& Location, const struct FRotator& Rotation, TEnumAsByte<EBuildingAttachmentType> InBuildingAttachmentType
			struct parms { FVector BuildingLocation; FRotator BuildingRotation; FVector Location; FRotator Rotation; };

			auto Params = (parms*)Parameters;

			TrapLocation = Params->Location;
			TrapRotation = Params->Rotation;
		}

		ServerSpawnDeco_Params ServerSpawnDeco_params = { TrapLocation, TrapRotation, NewBuilding };

		ServerSpawnDeco(DecoTool, nullptr, &ServerSpawnDeco_params); // Spawn the trap
	}
}

double __fastcall idkmansomebuildting(UObject* BuildingActor)
{
	float result; // xmm0_8
	float v3; // xmm0_4

	result = 1.0f; // (*(double(__fastcall**)(__int64))(*(_QWORD*)BuildingActor + 0x768i64))(BuildingActor);// 1896 = IGameplayAbilitiesModule_Get
	if (result <= 0.0)
		return 0.0;
	v3 = 1.0f; // (*(float(__fastcall**)(__int64))(*(_QWORD*)BuildingActor + 1888i64))(BuildingActor) / *(float*)&result;// 1888 == idkbuildingthing
	if (v3 < 0.0)
		return 0.0;
	*(float*)&result = fminf(v3, 1.0);           // fminf returns whichever one is lower
	return result;
}

float GetBuildingRepairCostMultiplier(uint8_t ResourceType)
{
	auto GameData = Helper::GetGameData();

	static auto BuildingRepairCostMultiplierHandlesOffset = GameData->GetOffset("BuildingRepairCostMultiplierHandles");

	auto BuildingRepairCostMultiplierHandles = Get<FCurveTableRowHandle[0x4]>(GameData, BuildingRepairCostMultiplierHandlesOffset);

	FCurveTableRowHandle BuildingRepairCostMultiplierHandle = (*BuildingRepairCostMultiplierHandles)[ResourceType];

	float Multiplier;
	BuildingRepairCostMultiplierHandle.Eval(1.0f, &Multiplier);
	std::cout << "Multiplier: " << Multiplier << '\n';
	return Multiplier;
}

bool Build::ServerRepairBuildingActor(UObject* Controller, UFunction*, void* Parameters)
{
	if (!Parameters)
		return false;

	auto BuildingActorToRepair = *(UObject**)Parameters;

	// if (GameState->bFreeBuildingRepairs) return false;

	auto BuildingCost = 10;

	float v11 = 1.f;

	static auto ResourceTypeOffset = BuildingActorToRepair->GetOffset("ResourceType");
	EFortResourceType* ResourceType = Get<EFortResourceType>(BuildingActorToRepair, ResourceTypeOffset);

	static auto CurrentBuildingLevelOffset = BuildingActorToRepair->GetOffset("CurrentBuildingLevel");
	auto CurrentBuildingLevel = Get<int>(BuildingActorToRepair, CurrentBuildingLevelOffset);

	int RepairCost = (float)(BuildingCost * (float)(1.0 - *(float*)&v11))
		* GetBuildingRepairCostMultiplier(
			(uint8_t)*ResourceType);

	struct { UObject* pc; int resourcespent; } RepairBuilding_Params{Controller, RepairCost};

	std::cout << "RepairCost: " << RepairCost << '\n';

	static auto WoodItemData = FindObject("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
	static auto StoneItemData = FindObject("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
	static auto MetalItemData = FindObject("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");

	auto MatDefinition = *ResourceType == EFortResourceType::Wood ? WoodItemData : *ResourceType == EFortResourceType::Stone ? StoneItemData : MetalItemData;

	auto MatInstance = Inventory::FindItemInInventory(Controller, MatDefinition);

	Inventory::TakeItem(Controller, *UFortItem::GetGuid(MatInstance), RepairCost);

	static auto RepairBuilding = FindObject<UFunction>("/Script/FortniteGame.BuildingSMActor.RepairBuilding");
	BuildingActorToRepair->ProcessEvent(RepairBuilding, &RepairBuilding_Params);

	return false;
}