#include <MinHook.h>

#include "patterns.h"
#include "server.h"

void Server::PauseBeaconRequests(bool bPause)
{
	Defines::PauseBeaconRequests(BeaconHost, bPause);
	SetWorld(Helper::GetWorld());
}

void Server::SetWorld(UObject* World)
{
	if (Engine_Version < 426)
	{
		if (Defines::SetWorld)
			Defines::SetWorld(NetDriver, World);
		else
			std::cout << "Invalid SetWorld!\n";
	}
	else
	{
		if (!Defines::SetWorld)
		{
			int SetWorldIndex = Fortnite_Version < 19.00 ? 0x72 : (Fortnite_Version >= 20.00 ? (Fortnite_Version >= 21 ? 0x7C : 0x7B) : 0x7A); // s13-s18 = 0x72 s19 = 0x7A s20 = 7B s21 = 7c 

			std::cout << "SetWorldIndex: " << SetWorldIndex << '\n';

			Defines::SetWorld = decltype(Defines::SetWorld)(NetDriver->VFTable[SetWorldIndex]);
		}

		if (Defines::SetWorld)
			Defines::SetWorld(NetDriver, World);
		else
			std::cout << "Invalid SetWorld!\n";
	}
}

bool Server::Listen(int Port)
{
	if (bUseBeacons)
	{
		static UObject* BeaconClass = FindObject("Class /Script/FortniteGame.FortOnlineBeaconHost"); // We use the Fort one because then FindObject will not mistake for the BeaconHostObject.

		BeaconHost = Helper::Easy::SpawnActor(BeaconClass, FVector());

		if (!BeaconHost)
		{
			std::cout << "Failed to spawn Beacon!\n";
			return false;
		}

		// set listen port

		static auto ListenPortOffset = BeaconHost->GetOffset("ListenPort");
		auto ListenPort = Get<int>(BeaconHost, ListenPortOffset);

		*ListenPort = Engine_Version < 426 ? Port - 1 : Port;

		if (Defines::InitHost(BeaconHost))
		{
			std::cout << "Successfully initialized beacon!\n";

			static auto NetDriverOffset = BeaconHost->GetOffset("NetDriver");
			NetDriver = *Get<UObject*>(BeaconHost, NetDriverOffset);
		}
		else
		{
			std::cout << "Failed to init host!\n";
			return false;
		}
	}
	else
	{

	}

	if (!NetDriver)
	{
		std::cout << "Missing NetDriver!\n";
		return false;
	}

	// setup netdriver

	FString Error;
	auto InURL = FURL();
	InURL.Port = Port;

	UObject* World = Helper::GetWorld();

	static auto NetDriverNameOffset = NetDriver->GetOffset("NetDriverName");
	
	FString NetDriverNameFStr = L"GameNetDriver"; // to free
	*Get<FName>(NetDriver, NetDriverNameOffset) = Helper::Conversion::StringToName(NetDriverNameFStr);

	auto InitListenResult = Defines::InitListen(NetDriver, World, InURL, false, Error);

	// end setup

	static auto LevelCollectionStruct = FindObject("ScriptStruct /Script/Engine.LevelCollection");
	static auto LevelCollectionsOffset = World->GetOffset("LevelCollections");
	auto LevelCollections = Get<TArray<UObject>>(World, LevelCollectionsOffset);

	if (LevelCollections && LevelCollections->Data)
	{
		static auto LevelCollectionSize = Helper::GetSizeOfClass(LevelCollectionStruct);

		auto FirstLevelCollection = LevelCollections->AtPtr(0, LevelCollectionSize);

		static auto LC_NetDriverOffset = LevelCollectionStruct->GetOffset("NetDriver", true);

		*Get<UObject*>(FirstLevelCollection, LC_NetDriverOffset) = NetDriver;
		*Get<UObject*>(LevelCollections->AtPtr(1, LevelCollectionSize), LC_NetDriverOffset) = NetDriver;
	}

	PauseBeaconRequests(false);

	if (Fortnite_Version >= 3.3)
	{
		static auto ReplicationDriverOffset = NetDriver->GetOffset("ReplicationDriver");
		auto ReplicationDriver = *Get<UObject*>(NetDriver, ReplicationDriverOffset);

		Defines::ServerReplicateActors = decltype(Defines::ServerReplicateActors)(ReplicationDriver->VFTable[ServerReplicateActorsOffset]);
	}

	static auto World_NetDriverOffset = World->GetOffsetSlow("NetDriver");

	if (World_NetDriverOffset != 0)
		*Get<UObject*>(World, World_NetDriverOffset) = NetDriver;

	std::cout << "Listening on port: " << Port << '\n';

	return true;
}

void Server::Hooks::Initialize()
{
	std::cout << MH_StatusToString(MH_CreateHook((PVOID)TickFlushAddress, Server::Hooks::TickFlush, (PVOID*)&Defines::TickFlush)) << '\n';
	std::cout << MH_StatusToString(MH_EnableHook((PVOID)TickFlushAddress)) << '\n';

	std::cout << MH_StatusToString(MH_CreateHook((PVOID)KickPlayerAddress, Server::Hooks::KickPlayer, (PVOID*)&Defines::KickPlayer)) << '\n';
	std::cout << MH_StatusToString(MH_EnableHook((PVOID)KickPlayerAddress)) << '\n';

	std::cout << MH_StatusToString(MH_CreateHook((PVOID)ValidationFailureAddress, Server::Hooks::ValidationFailure, (PVOID*)&Defines::ValidationFailure)) << '\n';
	std::cout << MH_StatusToString(MH_EnableHook((PVOID)ValidationFailureAddress)) << '\n';

	std::cout << MH_StatusToString(MH_CreateHook((PVOID)NoReserveAddress, Server::Hooks::NoReservation, (PVOID*)&Defines::NoReservation)) << '\n';
	std::cout << MH_StatusToString(MH_EnableHook((PVOID)NoReserveAddress)) << '\n';
}

void Server::Hooks::TickFlush(UObject* thisNetDriver, float DeltaSeconds)
{
	auto World = Helper::GetWorld();

	if (World)
	{
		static auto NetDriverOffset = World->GetOffsetSlow("NetDriver");
		auto NetDriver = *Get<UObject*>(World, NetDriverOffset);

		if (NetDriver)
		{
			if (Fortnite_Version <= 3.3)
			{
				// ReplicateActors(NetDriver, World);
			}
			else
			{
				static auto ReplicationDriverOffset = NetDriver->GetOffset("ReplicationDriver");
				auto ReplicationDriver = Get<UObject*>(NetDriver, ReplicationDriverOffset);

				if (ReplicationDriver)
				{
					Defines::ServerReplicateActors(*ReplicationDriver);
				}
			}
		}
	}

	if (Defines::bShouldSpawnFloorLoot) // TODO move this
	{
		static auto SpawnIsland_FloorLoot = FindObject("BlueprintGeneratedClass /Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_Warmup.Tiered_Athena_FloorLoot_Warmup_C");
		static auto BRIsland_FloorLoot = FindObject("BlueprintGeneratedClass /Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C");

		if (!SpawnIsland_FloorLoot || !BRIsland_FloorLoot) // Map has not loaded to the point where there are floor loot actors
		{
			SpawnIsland_FloorLoot = FindObject("BlueprintGeneratedClass /Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_Warmup.Tiered_Athena_FloorLoot_Warmup_C");
			BRIsland_FloorLoot = FindObject("BlueprintGeneratedClass /Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C");
		}
		else
		{
			Defines::bShouldSpawnFloorLoot = false;

			std::cout << "Spawning floor loot!\n";

			std::cout << "SpawnIsland_FloorLoot: " << SpawnIsland_FloorLoot << '\n';
			std::cout << "BRIsland_FloorLoot: " << BRIsland_FloorLoot << '\n';

			auto SpawnFloorLoot = [](UObject* Class) -> int
			{
				auto ClassActors = Helper::GetAllActorsOfClass(Class);

				std::cout << "Size; " << ClassActors.Num() << '\n';

				for (int i = 0; i < ClassActors.Num(); i++)
				{
					auto ClassActor = ClassActors.At(i);

					if (ClassActor)
					{
						constexpr bool bTossPickup = true;
						auto CorrectLocation = Helper::GetActorLocation(ClassActor);
						CorrectLocation.Z += 50;

						static auto Def = FindObject("FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03");

						Helper::SummonPickup(nullptr, Def, CorrectLocation, EFortPickupSourceTypeFlag::FloorLoot, EFortPickupSpawnSource::Unset, 1, true);
					}
				}

				int Num = ClassActors.Num();

				ClassActors.Free();

				return Num;
			};

			SpawnFloorLoot(SpawnIsland_FloorLoot);
			SpawnFloorLoot(BRIsland_FloorLoot);
		}
	}

	return Defines::TickFlush(thisNetDriver, DeltaSeconds);
}

void Server::Hooks::KickPlayer(UObject* GameSession, UObject* Controller, FText a3)
{
	std::cout << "KickPlayer!\n";
	return;
}

char Server::Hooks::ValidationFailure(__int64* a1, __int64 a2)
{
	std::cout << "Validation Failure!\n";
	return false;
}

__int64 Server::Hooks::NoReservation(__int64* a1, __int64 a2, char a3, __int64 a4)
{
	std::cout << "No Reserve!\n";
	return 0;
}