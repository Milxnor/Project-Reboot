#include "build.h"
#include "helper.h"
#include "inventory.h"

namespace Build
{
	bool ServerCreateBuildingActor(UObject* Controller, UFunction* Function, void* Parameters)
	{
		if (!Parameters)
			return false;

		static auto WoodItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
		static auto StoneItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
		static auto MetalItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

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

		bool bCanBuild = true;

		__int64 v32[2]{};
		char dababy;

		bCanBuild = !Defines::CantBuild(Helper::GetWorld(), BuildingClass, BuildingLocation, BuildingRotation, bMirrored, v32, &dababy);

		if (bCanBuild)
		{
			UObject* BuildingActor = Helper::Easy::SpawnActor(BuildingClass, BuildingLocation, BuildingRotation, Pawn);

			if (BuildingActor)
			{
				static auto ResourceTypeOffset = BuildingActor->GetOffset("ResourceType");
				auto ResourceType = *Get<TEnumAsByte<EFortResourceType>>(BuildingActor, ResourceTypeOffset);

				MatDefinition = ResourceType.Get() == EFortResourceType::Wood ? WoodItemData : ResourceType.Get() == EFortResourceType::Stone ? StoneItemData : MetalItemData;

				MatInstance = Inventory::FindItemInInventory(Controller, MatDefinition);

				bool bShouldDestroy = true;

				if (MatInstance) // && Helper::IsStructurallySupported(BuildingActor)
				{
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

					}
				}
			}
		}
	}
}