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
	*Get<FName>(NetDriver, NetDriverNameOffset) = Helper::Conversion::StringToName(L"GameNetDriver");

	auto InitListenResult = Defines::InitListen(NetDriver, World, InURL, false, Error);

	// end setup

	PauseBeaconRequests(World);

	static auto ReplicationDriverOffset = NetDriver->GetOffset("ReplicationDriver");
	auto ReplicationDriver = *Get<UObject*>(NetDriver, ReplicationDriverOffset);

	Defines::ServerReplicateActors = decltype(Defines::ServerReplicateActors)(ReplicationDriver->VFTable[Defines::ServerReplicateActorsOffset]);

	static auto LevelCollectionsOffset = World->GetOffset("LevelCollections");
	auto LevelCollections = Get<TArray<UObject*>>(World, LevelCollectionsOffset);

	if (LevelCollections && LevelCollections->Data)
	{
		static auto LevelCollectionSize = 0x78;

		auto FirstLevelCollection = LevelCollections->At(0, LevelCollectionSize);
		static auto LC_NetDriverOffset = 0x10; // FirstLevelCollection->GetOffset("NetDriver");
		
		std::cout << "LC_NetDriverOffset: " << LC_NetDriverOffset << '\n';

		*Get<UObject*>(FirstLevelCollection, LC_NetDriverOffset) = NetDriver;
		*Get<UObject*>(LevelCollections->At(1, LevelCollectionSize), LC_NetDriverOffset) = NetDriver;
	}

	if (false)
	{
		static auto World_NetDriverOffset = World->GetOffset("NetDriver");
		std::cout << "World_NetDriverOffset: " << World_NetDriverOffset << '\n';

		if (World_NetDriverOffset != 0)
			*Get<UObject*>(World, World_NetDriverOffset) = NetDriver;
	}

	std::cout << "Listening on port: " << Port << '\n';

	return true;
}