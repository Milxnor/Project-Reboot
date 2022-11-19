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
		FVector BuildingLocation = FVector();
		FRotator BuildingRotation = FRotator();
		bool bMirrored = false;

		if (Fortnite_Version >= 8.3)
		{
			struct FCreateBuildingActorData { uint32_t BuildingClassHandle; FVector BuildLoc; FRotator BuildRot; bool bMirrored; };

			auto Params = *(FCreateBuildingActorData*)Parameters;

			BuildingLocation = Params.BuildLoc;
			BuildingRotation = Params.BuildRot;
			bMirrored = Params.bMirrored;

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

		__int64 v32[2]{};
		char dababy;

		bool bCanBuild = !Defines::CantBuild(Helper::GetWorld(), BuildingClass, BuildingLocation, BuildingRotation, bMirrored, v32, &dababy);

		if (bCanBuild)
		{
			UObject* BuildingActor = Helper::Easy::SpawnActor(BuildingClass, BuildingLocation, BuildingRotation, Pawn);

			if (BuildingActor)
			{
				static auto ResourceTypeOffset = BuildingActor->GetOffset("ResourceType");
				auto ResourceType = *Get<TEnumAsByte<EFortResourceType>>(BuildingActor, ResourceTypeOffset);

				MatDefinition = ResourceType.Get() == EFortResourceType::Wood ? WoodItemData : ResourceType.Get() == EFortResourceType::Stone ? StoneItemData : MetalItemData;

				MatInstance = Inventory::FindItemInInventory(Controller, MatDefinition);

				auto MatCount = UFortItem::GetCount(MatInstance);

				bool bShouldDestroy = true;

				if (MatCount && *MatCount >= 10 && MatInstance) // && Helper::IsStructurallySupported(BuildingActor)
				{
					// __int64 (__fastcall* SomeBuildCollisionThing)(UObject* Build) = decltype(SomeBuildCollisionThing)(Memory::FindPattern("40 55 41 55 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 80 B9 ? ? ? ? ? 4C 8B E9 0F 85 ? ? ? ?"));

					// SomeBuildCollisionThing(BuildingActor); // 10.4

					SetBuildingActorTeam(BuildingActor, *Helper::GetTeamIndex(Helper::GetPlayerStateFromController(Controller)));

					Helper::InitializeBuildingActor(Controller, BuildingActor, true);

					bShouldDestroy = false;
				}

				if (bShouldDestroy)
				{

				}
				else
				{
					if (!Defines::bIsPlayground)
					{
						Inventory::TakeItem(Controller, *UFortItem::GetGuid(MatInstance), 10);
					}
				}
			}
		}

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