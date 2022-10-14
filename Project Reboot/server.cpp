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
		int SetWorldVTableIndex = Fortnite_Version < 19.00 ? 0x72 : 0x7A;

		if (Fortnite_Version >= 20.00)
			SetWorldVTableIndex = 0x7B;

		if (Fortnite_Version >= 21.00)
			SetWorldVTableIndex = 0x7C;

		Defines::SetWorld = decltype(Defines::SetWorld)(NetDriver->VFTable[SetWorldVTableIndex]);
		Defines::SetWorld(NetDriver, World);
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

	static auto ReplicationDriverOffset = NetDriver->GetOffset("ReplicationDriver");
	auto ReplicationDriver = *Get<UObject*>(NetDriver, ReplicationDriverOffset);

	Defines::ServerReplicateActors = decltype(Defines::ServerReplicateActors)(ReplicationDriver->VFTable[Defines::ServerReplicateActorsOffset]);

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
				auto ReplicationDriver = *Get<UObject*>(NetDriver, ReplicationDriverOffset);

				if (ReplicationDriver && Defines::ServerReplicateActors)
				{
					Defines::ServerReplicateActors(ReplicationDriver);
				}
				else
					std::cout << "skidda; " << Defines::ServerReplicateActors << '\n';
			}
		}
		else
			std::cout << "No World netDriver??\n";
	}
	else
		std::cout << "no world?!?!\n";

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