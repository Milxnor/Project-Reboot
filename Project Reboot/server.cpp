#include <MinHook.h>

#include "patterns.h"
#include "server.h"
#include "loot.h"
#include <intrin.h>
#include "team.h"
#include "replication.h"

void Server::PauseBeaconRequests(bool bPause)
{
	Defines::PauseBeaconRequests(BeaconHost, bPause);
	std::cout << "cc!\n";
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
			/* int SetWorldIndex = Fortnite_Version < 19.00 ? Fortnite_Season < 15 ? 0x71
				: Fortnite_Season == 14 ? 0x72 : Fortnite_Season == 13 ? 0x70
				: Fortnite_Version >= 20.00 ? (Fortnite_Version >= 21 ? 0x7C : 0x7B) : 0x7A : ; // s13-14 = 0x71 s15-s18 = 0x72 s19 = 0x7A s20 = 7B s21 = 7c 

			*/

			int SetWorldIndex = 0;

			if (Fortnite_Season == 13)
				SetWorldIndex = 0x70;
			else if (Fortnite_Season == 14 || Fortnite_Version <= 15.2)
				SetWorldIndex = 0x71;
			else if (Fortnite_Version >= 15.3 && Fortnite_Season < 18) // i havent tested 15.2
				SetWorldIndex = 0x72;
			else if (Fortnite_Season == 18)
				SetWorldIndex = 0x73;
			else if (Fortnite_Season >= 19 && Fortnite_Season < 21)
				SetWorldIndex = 0x7A; // 20.00
			else if (Fortnite_Season == 21)
				SetWorldIndex = 0x7C; // 21
			else if (Fortnite_Season == 22)
				SetWorldIndex = 0x7B;

			if (Fortnite_Version == 20.40)
				SetWorldIndex = 0x7B;

			std::cout << "SetWorldIndex: " << SetWorldIndex << '\n';

			Defines::SetWorld = decltype(Defines::SetWorld)(NetDriver->VFTable[SetWorldIndex]);
		}

		if (Defines::SetWorld)
			Defines::SetWorld(NetDriver, World);
		else
			std::cout << "Invalid SetWorld!\n";
	}
}

DWORD WINAPI PauseThread(LPVOID)
{
	Sleep(4000);

	auto World = Helper::GetWorld();

	static auto StreamingLevelsOffset = World->GetOffset("StreamingLevels");
	auto StreamingLevels = Get<TArray<UObject*>>(World, StreamingLevelsOffset);

	std::cout << "StreamingLevels->Num(): " << StreamingLevels->Num() << '\n';

	bool bAllLevelsLoaded = false;

	while (!bAllLevelsLoaded)
	{
		for (int i = 0; i < StreamingLevels->Num(); i++)
		{
			auto StreamingLevel = StreamingLevels->At(i);

			if (IsBadReadPtr(StreamingLevel))
				continue;

			static auto LoadedLevelOffset = StreamingLevel->GetOffsetSlow("LoadedLevel");
			auto LoadedLevel = *Get<UObject*>(StreamingLevel, LoadedLevelOffset);

			bool bShouldBeLoaded = false;

			static auto ShouldBeLoadedFn = FindObject<UFunction>("/Script/Engine.LevelStreaming.ShouldBeLoaded");
			StreamingLevel->ProcessEvent(ShouldBeLoadedFn, &bShouldBeLoaded);

			std::cout << std::format("[{}] {} {:x}\n", i, bShouldBeLoaded, __int64(LoadedLevel));

			if (!LoadedLevel)
			{
				Sleep(1000);

				if (bShouldBeLoaded)
				{
					bAllLevelsLoaded = false;
					break; // restart loop
				}
			}

			bAllLevelsLoaded = true;
		}
	}

	Server::PauseBeaconRequests(false);

	std::cout << "Players may join now!\n";

	return 0;
}

bool Server::Listen(int Port)
{
	if (Defines::bIsRestarting)
		Port -= Defines::AmountOfRestarts;

	if (bUseBeacons)
	{
		static UObject* BeaconClass = FindObject("/Script/FortniteGame.FortOnlineBeaconHost"); // We use the Fort one because then FindObject will not mistake for the BeaconHostObject.

		BeaconHost = Helper::Easy::SpawnActorDynamic(BeaconClass, BothVector());

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
		std::cout << "Not using beacons isn't supported yet!\n";
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

	// static auto ReplicationDriverClassOffset = NetDriver->GetOffset("ReplicationDriverClass");
	// static auto FortReplicationGraphClass = FindObject("/Script/FortniteGame.FortReplicationGraph");

	// *Get<UObject*>(NetDriver, ReplicationDriverClassOffset) = FortReplicationGraphClass;

	auto InitListenResult = Defines::InitListen(NetDriver, World, InURL, false, Error);

	// end setup

	static auto LevelCollectionStruct = FindObject("/Script/Engine.LevelCollection");
	static auto LevelCollectionsOffset = World->GetOffset("LevelCollections");
	auto LevelCollections = Get<TArray<UObject>>(World, LevelCollectionsOffset);

	if (LevelCollections && LevelCollections->Data)
	{
		static auto LevelCollectionSize = Helper::GetSizeOfClass(LevelCollectionStruct);

		std::cout << "LevelCollectionSize: " << LevelCollectionSize << '\n';

		auto FirstLevelCollection = LevelCollections->AtPtr(0, LevelCollectionSize);

		static auto LC_NetDriverOffset = 0x10; // LevelCollectionStruct->GetOffset("NetDriver", true);

		std::cout << "LC_NetDriverOffset: " << LC_NetDriverOffset << '\n';

		*Get<UObject*>(FirstLevelCollection, LC_NetDriverOffset) = NetDriver;
		*Get<UObject*>(LevelCollections->AtPtr(1, LevelCollectionSize), LC_NetDriverOffset) = NetDriver;
	}
	else
		std::cout << "Unable to find LevelCollections!\n";

	std::cout << "aa!\n";

	std::cout << "bb!\n";

	if (Fortnite_Version >= 3.3)
	{
		static auto ReplicationDriverOffset = NetDriver->GetOffset("ReplicationDriver");
		auto ReplicationDriver = *Get<UObject*>(NetDriver, ReplicationDriverOffset);

		if (ReplicationDriver)
			Defines::ServerReplicateActors = decltype(Defines::ServerReplicateActors)(ReplicationDriver->VFTable[ServerReplicateActorsOffset]);
		else
			std::cout << "ReplicationDriver was not created!\n";
	}

	std::cout << "dd!\n";

	static auto World_NetDriverOffset = World->GetOffsetSlow("NetDriver");

	if (World_NetDriverOffset != 0)
		*Get<UObject*>(World, World_NetDriverOffset) = NetDriver;

	std::cout << "Listening on port: " << Port << '\n';
	
	bool bVersionSupportsNoPrejoin = false && Engine_Version >= 422 && Engine_Version < 500;
	
	std::cout << "bVersionSupportsNoPrejoin: " << bVersionSupportsNoPrejoin << '\n';

	if (bVersionSupportsNoPrejoin)
	{
		CreateThread(0, 0, PauseThread, 0, 0, 0);
	}
	else
	{
		Server::PauseBeaconRequests(false);

		std::cout << "Players may join now!\n";
	}

	return true;
}

void Server::Restart()
{
	Defines::AmountOfRestarts++;
	Defines::bIsRestarting = true;
	
	std::cout << MH_StatusToString(MH_DisableHook((PVOID)TickFlushAddress)) << '\n';
	std::cout << MH_StatusToString(MH_DisableHook((PVOID)KickPlayerAddress)) << '\n';
	std::cout << MH_StatusToString(MH_DisableHook((PVOID)ValidationFailureAddress)) << '\n';

	if (Engine_Version < 424)
		std::cout << MH_StatusToString(MH_DisableHook((PVOID)NoReserveAddress)) << '\n';

	if (BeaconHost)
		Helper::DestroyActor(BeaconHost);

	BeaconHost = nullptr;

	// Sleep(5000);

	static auto SwitchLevel = FindObject<UFunction>("/Script/Engine.PlayerController.SwitchLevel");

	std::wstring LevelWStr = std::wstring(Defines::MapName.begin(), Defines::MapName.end());
	const wchar_t* LevelWCSTR = LevelWStr.c_str();
	FString Level = LevelWCSTR;

	auto PC = Helper::GetLocalPlayerController();

	PC->ProcessEvent(SwitchLevel, &Level);

	Teams::NextTeamIndex = Teams::StartingTeamIndex;
	Teams::CurrentNumPlayersOnTeam = 0;

	Defines::bReadyForStartMatch = true;
}

static __int64 rettruae() { return 1; }

__int64 Server::Hooks::NoReservation(__int64* a1, __int64 a2, char a3, __int64 a4)
{
	std::cout << "No Reserve!\n";
	return 0;
}

struct FNetViewer
{
	UObject** GetConnection()
	{
		static auto Viewer_ConnectionOffset = FindOffsetStruct("ScriptStruct /Script/Engine.NetViewer", "Connection");
		return (UObject**)(__int64(this) + Viewer_ConnectionOffset);
	}

	UObject** GetInViewer()
	{
		static auto Viewer_InViewerOffset = FindOffsetStruct("ScriptStruct /Script/Engine.NetViewer", "InViewer");
		return (UObject**)(__int64(this) + Viewer_InViewerOffset);
	}

	UObject** GetViewTarget()
	{
		static auto Viewer_ViewTargetOffset = FindOffsetStruct("ScriptStruct /Script/Engine.NetViewer", "ViewTarget");
		return (UObject**)(__int64(this) + Viewer_ViewTargetOffset);
	}

	FVector* GetViewLocation()
	{
		static auto Viewer_ViewLocationOffset = FindOffsetStruct("ScriptStruct /Script/Engine.NetViewer", "ViewLocation");
		return (FVector*)(__int64(this) + Viewer_ViewLocationOffset);
	}

	FVector* GetViewDir()
	{
		static auto Viewer_ViewDirOffset = FindOffsetStruct("ScriptStruct /Script/Engine.NetViewer", "ViewDir");
		return (FVector*)(__int64(this) + Viewer_ViewDirOffset);
	}
};

FNetViewer* __fastcall NetViewerConstructorDetour2(FNetViewer* NetViewer, UObject* InController)
{
	if (InController)
	{
		static auto PC_GetViewTarget = FindObject<UFunction>("/Script/Engine.Controller.GetViewTarget");
		UObject* PCViewTarget = nullptr;
		InController->ProcessEvent(PC_GetViewTarget, &PCViewTarget);

		*NetViewer->GetViewTarget() = PCViewTarget;

		if (*NetViewer->GetViewTarget())
		{
			*NetViewer->GetViewLocation() = Helper::GetActorLocation(*NetViewer->GetViewTarget());
		}

		FRotator ViewRotation = Helper::GetControlRotation(InController);
	
		// GetPlayerViewpoint

		if (PCViewTarget)
		{
			*NetViewer->GetViewLocation() = Helper::GetActorLocation(PCViewTarget);
			ViewRotation = Helper::GetActorRotation(PCViewTarget);
		}

		*NetViewer->GetViewDir() = ViewRotation.Vector();
	}

	return NetViewer;
}

UObject* GetViewTargetCameraManager(UObject* PlayerCameraManager)
{
	return nullptr;
}

UObject* GetViewTargetPlayerController(UObject* PC)
{
	static auto PlayerCameraManagerOffset = PC->GetOffset("PlayerCameraManager");
	auto PlayerCameraManager = *Get<UObject*>(PC, PlayerCameraManagerOffset);

	UObject* CameraManagerViewTarget = PlayerCameraManager ? GetViewTargetCameraManager(PlayerCameraManager) : nullptr;

	return CameraManagerViewTarget ? CameraManagerViewTarget : PC;
}

void __fastcall GetPlayerViewPointDetour(UObject* pc, FVector* a2, FRotator* a3)
{
	/* static auto PlayerCameraManagerOffset = pc->GetOffset("PlayerCameraManager");
	auto PlayerCameraManager = *Get<UObject*>(pc, PlayerCameraManagerOffset);

	if (PlayerCameraManager
		// PlayerCameraManager->GetCameraCacheTime() > 0.f // Whether camera was updated at least once)
		)
	{
		GetCameraViewpoint(PlayerCameraManager, *a2, *a3);
	}
	else */
	{
		static auto PC_GetViewTarget = FindObject<UFunction>("/Script/Engine.Controller.GetViewTarget");
		UObject* ViewTarget = nullptr;
		pc->ProcessEvent(PC_GetViewTarget, &ViewTarget);

		if (ViewTarget)
		{
			*a2 = Helper::GetActorLocation(ViewTarget);
			*a3 = Helper::GetActorRotation(ViewTarget);
		}
	}
}

__int64 (__fastcall* NetViewerConstructorO)(__int64 NetViewer, UObject* Connection);

FNetViewer* __fastcall NetViewerConstructorDetour(FNetViewer* NetViewer, UObject* Connection)
{
	// dude 17.50 Connection != UNetConnection or something
	
	static auto Connection_ViewTargetOffset = FindOffsetStruct("Class /Script/Engine.NetConnection", "ViewTarget");
	static auto Connection_PlayerControllerOffset = FindOffsetStruct("Class /Script/Engine.Player", "PlayerController");
	static auto Connection_OwningActorOffset = FindOffsetStruct("Class /Script/Engine.NetConnection", "OwningActor");

	auto Connection_ViewTarget = *(UObject**)(__int64(Connection) + Connection_ViewTargetOffset);
	auto ViewingController = *(UObject**)(__int64(Connection) + Connection_PlayerControllerOffset);
	auto Connection_OwningActor = *(UObject**)(__int64(Connection) + Connection_OwningActorOffset);

	static auto Viewer_ConnectionOffset = FindOffsetStruct("ScriptStruct /Script/Engine.NetViewer", "Connection");
	*(UObject**)(__int64(NetViewer) + Viewer_ConnectionOffset) = Connection;

	static auto Viewer_InViewerOffset = FindOffsetStruct("ScriptStruct /Script/Engine.NetViewer", "InViewer");
	*(UObject**)(__int64(NetViewer) + Viewer_InViewerOffset) = ViewingController ? ViewingController : Connection_OwningActor;

	static auto Viewer_ViewLocationOffset = FindOffsetStruct("ScriptStruct /Script/Engine.NetViewer", "ViewLocation");
	*(FVector*)(__int64(NetViewer) + Viewer_ViewLocationOffset) = Helper::GetActorLocation(ViewingController);

	static auto Viewer_ViewTargetOffset = FindOffsetStruct("ScriptStruct /Script/Engine.NetViewer", "ViewTarget");
	auto Viewer_ViewTarget = (UObject**)(__int64(NetViewer) + Viewer_ViewTargetOffset);
	*Viewer_ViewTarget = Connection_ViewTarget;

	if (!Connection_OwningActor)
		return NetViewer;

	if (!(!ViewingController || (ViewingController == Connection_OwningActor)))
		return NetViewer;

	*(FVector*)(__int64(NetViewer) + Viewer_ViewLocationOffset) = Helper::GetActorLocation(*Viewer_ViewTarget);

	if (ViewingController)
	{
		FRotator ViewRotation = Helper::GetControlRotation(ViewingController);

		GetPlayerViewPointDetour(ViewingController, (FVector*)(__int64(NetViewer) + Viewer_ViewLocationOffset), &ViewRotation);

		static auto Viewer_ViewDirOffset = FindOffsetStruct("ScriptStruct /Script/Engine.NetViewer", "ViewDir");
		*(FVector*)(__int64(NetViewer) + Viewer_ViewDirOffset) = ViewRotation.Vector();
	}

	return NetViewer;
}

struct FPostProcessSettings
{
	char pad[0x04E0];
};

struct FMinimalViewInfo
{
	char pad[0x0530];
};

struct FViewTargetTransitionParams
{
	float                                              BlendTime;                                                // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8_t              BlendFunction;                                            // 0x0004(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	unsigned char                                      UnknownData00[0x3];                                       // 0x0005(0x0003) MISSED OFFSET
	float                                              BlendExp;                                                 // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	unsigned char                                      bLockOutgoing : 1;                                        // 0x000C(0x0001) (Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	unsigned char                                      UnknownData01[0x3];                                       // 0x000D(0x0003) MISSED OFFSET
};

struct FTViewTarget
{
	UObject* Target;                                                   // 0x0000(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	unsigned char                                      UnknownData00[0x8];                                       // 0x0008(0x0008) MISSED OFFSET
	FMinimalViewInfo                            POV;                                                      // 0x0010(0x0530) (Edit, BlueprintVisible, NativeAccessSpecifierPublic)
	UObject* PlayerState;                                              // 0x0540(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	unsigned char                                      UnknownData01[0x8];                                       // 0x0548(0x0008) MISSED OFFSET

	UObject** GetPlayerState()
	{
		return &PlayerState;
	}
};

void (*skidd)(UObject* PlayerCameraManager, UObject* NewTarget, FTViewTarget& VT, FViewTargetTransitionParams TransitionParams);

void AssignViewTarget(UObject* PlayerCameraManager, UObject* NewTarget, FTViewTarget& VT, FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams())
{
	static auto aa = Memory::FindPattern("48 85 D2 0F 84 ? ? ? ? 48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 30 49 8B D8 48 8B F9 49 8B F1 4C 8B C2"); // PlayerCameraManager->VFTable[0xCA];

	skidd = decltype(skidd)(aa);

	skidd(PlayerCameraManager, NewTarget, VT, TransitionParams);
}

void CheckViewTarget(FTViewTarget viewTarget, UObject* OwningController)
{
	if (!OwningController)
		return;

	auto ViewTarget = &viewTarget;

	// static auto ViewTargetOffset = PlayerCameraManager->GetOffset("ViewTarget");
	// auto ViewTarget = Get<FTViewTarget>(PlayerCameraManager, ViewTargetOffset);

	static auto ControllerClass = FindObject("/Script/Engine.Controller");
	static auto PawnClass = FindObject("/Script/Engine.Pawn");
	static auto PlayerStateClass = FindObject("/Script/Engine.PlayerState");

	static auto PlayerCameraManagerOffset = OwningController->GetOffset("PlayerCameraManager");
	auto PlayerCameraManager = *Get<UObject*>(OwningController, PlayerCameraManagerOffset);

	if (!ViewTarget->Target)
	{
		ViewTarget->Target = OwningController;
	}

	if (ViewTarget->Target == OwningController)
	{
		*ViewTarget->GetPlayerState() = nullptr;
	}
	else if (ViewTarget->Target->IsA(ControllerClass))
	{
		*ViewTarget->GetPlayerState() = Helper::GetPlayerStateFromController(ViewTarget->Target);
	}
	else if (ViewTarget->Target->IsA(PawnClass))
	{
		*ViewTarget->GetPlayerState() = Helper::GetPlayerStateFromController(Helper::GetControllerFromPawn(ViewTarget->Target));
	}
	else if (ViewTarget->Target->IsA(PlayerStateClass))
	{
		*ViewTarget->GetPlayerState() = ViewTarget->Target;
	}
	else
	{
		*ViewTarget->GetPlayerState() = nullptr;
	}

	if (*ViewTarget->GetPlayerState())
	{
		auto PawnPlayerState = ViewTarget->Target->IsA(PawnClass) ? Helper::GetPlayerStateFromController(Helper::GetControllerFromPawn(ViewTarget->Target)) : nullptr;

		if (!ViewTarget->Target->IsA(PawnClass) || PawnPlayerState != *ViewTarget->GetPlayerState())
		{
			ViewTarget->Target = nullptr;

			auto PlayerStateOwner = Helper::GetOwner(*ViewTarget->GetPlayerState());

			if (!PlayerStateOwner)
			{
				*ViewTarget->GetPlayerState() = nullptr;
			}
			else
			{
				if (PlayerStateOwner->IsA(ControllerClass))
				{
					auto PlayerStateViewTarget = Helper::GetPawnFromController(PlayerStateOwner);

					if (PlayerStateViewTarget)
					{
						AssignViewTarget(PlayerCameraManager, PlayerStateViewTarget, *ViewTarget);
					}
					else
					{
						ViewTarget->Target = *ViewTarget->GetPlayerState();
					}
				}
				else
				{
					*ViewTarget->GetPlayerState() = nullptr;
				}
			}
		}
	}

	if (!ViewTarget->Target)
	{
		auto OwningPCPawn = Helper::GetPawnFromController(OwningController);

		if (OwningPCPawn) // && OwningPCPawn->IsPendingKillPending
		{
			AssignViewTarget(PlayerCameraManager, OwningPCPawn, *ViewTarget);
		}
		else
		{
			AssignViewTarget(PlayerCameraManager, OwningController, *ViewTarget);
		}
	}
}

struct FMinimalViewInfoUD
{
public:
	FVector                               Location;                                          // 0x0(0xC)(Edit, BlueprintVisible, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	FRotator                              Rotation;
};

FMinimalViewInfoUD GetCameraCacheView(UObject* PlayerCameraManager)
{
	static auto CameraCachePrivateOffset = PlayerCameraManager->GetOffset("CameraCachePrivate");
	auto CameraCachePrivate = Get<void>(PlayerCameraManager, CameraCachePrivateOffset);

	static auto POVOffset = FindOffsetStruct2("ScriptStruct /Script/Engine.CameraCacheEntry", "POV");
	return *(FMinimalViewInfoUD*)(__int64(CameraCachePrivate) + POVOffset);
}

void GetCameraViewpoint(UObject* PlayerCameraManager, FVector& OutCamLoc, FRotator& OutCamRot)
{
	const FMinimalViewInfoUD& CurrentPOV = GetCameraCacheView(PlayerCameraManager);
	OutCamLoc = CurrentPOV.Location;
	OutCamRot = CurrentPOV.Rotation;
}

void Server::Hooks::Initialize()
{
	// if (false)
	{
		std::cout << MH_StatusToString(MH_CreateHook((PVOID)TickFlushAddress, Server::Hooks::TickFlush, (PVOID*)&Defines::TickFlush)) << '\n';
		std::cout << MH_StatusToString(MH_EnableHook((PVOID)TickFlushAddress)) << '\n';
	}

	std::cout << MH_StatusToString(MH_CreateHook((PVOID)KickPlayerAddress, Server::Hooks::KickPlayer, (PVOID*)&Defines::KickPlayer)) << '\n';
	std::cout << MH_StatusToString(MH_EnableHook((PVOID)KickPlayerAddress)) << '\n';

	std::cout << MH_StatusToString(MH_CreateHook((PVOID)ValidationFailureAddress, Server::Hooks::ValidationFailure, (PVOID*)&Defines::ValidationFailure)) << '\n';
	std::cout << MH_StatusToString(MH_EnableHook((PVOID)ValidationFailureAddress)) << '\n';

	if (Engine_Version < 424)
	{
		std::cout << MH_StatusToString(MH_CreateHook((PVOID)NoReserveAddress, Server::Hooks::NoReservation, (PVOID*)&Defines::NoReservation)) << '\n';
		std::cout << MH_StatusToString(MH_EnableHook((PVOID)NoReserveAddress)) << '\n';
	}

	// Maybe: 48 83 EC 28 48 8B 01 FF 90 ? ? ? ? 84 C0

	// if (false)
	{
		if (false)
		{
			auto sig = Memory::FindPattern("40 57 48 83 EC 20 48 8B B9 ? ? ? ? 48 85 FF 74 53");

			std::cout << "GetViewTarget: " << sig << '\n';

			std::cout << MH_StatusToString(MH_CreateHook((PVOID)sig, Server::Hooks::GetViewTarget, nullptr)) << '\n';
			std::cout << MH_StatusToString(MH_EnableHook((PVOID)sig)) << '\n';
		}
		else if (true) // if (Fortnite_Version < 17.50)
		{
			auto sig = Memory::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 40 48 89 11 48 8B D9 48 8B 42 30 48 85 C0 75 07 48 8B 82 ? ? ? ? 48");

			if (!sig)
				sig = Memory::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 40 48 89 11");

			if (Fortnite_Version >= 17.50)
				sig = Memory::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 40 48 89 11 45");

			if (!sig)
				sig = Memory::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 89 11 45 33 C0 48 8B 42 30 48 8B D9 48 85 C0 75 07 48 8B 82 ? ? ? ? 48 89 41 08 48 8D 79 18 48"); // 20.40

			std::cout << "sig: " << sig << '\n';

			std::cout << MH_StatusToString(MH_CreateHook((PVOID)sig, NetViewerConstructorDetour, (PVOID*)&NetViewerConstructorO)) << '\n';
			std::cout << MH_StatusToString(MH_EnableHook((PVOID)sig)) << '\n';
		}
		else if (false)
		{
			auto sig = Memory::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 55 41 56 41 57 48 8B EC 48 83 EC 40 48 8B F2 48 C7 45 ? ? ? ? ? 48 8B 55 38 4D 8B F0 48 8B D9 45 33 FF E8 ? ? ? ? 84");

			MH_CreateHook((PVOID)sig, GetPlayerViewPointDetour, nullptr);
			MH_EnableHook((PVOID)sig);
		}
	}
}

void Server::Hooks::TickFlush(UObject* thisNetDriver, float DeltaSeconds)
{
	auto load = [](UObject* Class, const std::string& Name) -> UObject* {
		UObject* Object = FindObject(Name);

		if (!Object)
			Object = StaticLoadObject(Class, nullptr, Name);

		return Object;
	};

	auto World = Helper::GetWorld();

	if (World)
	{
		static auto NetDriverOffset = World->GetOffsetSlow("NetDriver");
		auto NetDriver = *Get<UObject*>(World, NetDriverOffset);

		if (NetDriver)
		{
			if (Fortnite_Version <= 3.3 || Fortnite_Version >= 19.40)
			{
				ServerReplicateActors(NetDriver);
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

	if (Defines::ObjectsToLoad.size() > 0)
	{
		for (int i = 0; i < Defines::ObjectsToLoad.size(); i++)
		{
			auto& Object = Defines::ObjectsToLoad.at(i);

			auto loaded = StaticLoadObject(Object.first, nullptr, Object.second);
			// std::cout << "Loaded " << Object.second << " at " << loaded << '\n';

			if (Defines::bIsCreative)
				Helper::Easy::SpawnActor(loaded, FVector()); // scuffed

			Defines::ObjectsToLoad.erase(Defines::ObjectsToLoad.begin() + i);
		}
	}

	if (Defines::ActorsToSpawn.size() > 0)
	{
		for (int i = 0; i < Defines::ActorsToSpawn.size(); i++)
		{
			auto& Actor = Defines::ActorsToSpawn.at(i);

			auto loaded = StaticLoadObject(Actor.ClassOfClass, nullptr, Actor.ClassToSpawn);

			if (loaded)
			{
				// std::cout << "Loaded " << Object.second << " at " << loaded << '\n';
				Helper::Easy::SpawnActorDynamic(loaded, Actor.SpawnLocation); // scuffed
			}
			else
			{
				std::cout << "Failed to find: " << Actor.ClassToSpawn << '\n';
			}

			Defines::ActorsToSpawn.erase(Defines::ActorsToSpawn.begin() + i);
		}
	}

	/*
	if (Defines::bShouldRestart)
	{
		Defines::bShouldRestart = false;

		Defines::AmountOfRestarts++;
		Defines::bIsRestarting = true;

		std::cout << MH_StatusToString(MH_DisableHook((PVOID)TickFlushAddress)) << '\n';
		std::cout << MH_StatusToString(MH_DisableHook((PVOID)KickPlayerAddress)) << '\n';
		std::cout << MH_StatusToString(MH_DisableHook((PVOID)ValidationFailureAddress)) << '\n';

		if (Engine_Version < 424)
			std::cout << MH_StatusToString(MH_DisableHook((PVOID)NoReserveAddress)) << '\n';

		if (BeaconHost)
			Helper::DestroyActor(BeaconHost);

		BeaconHost = nullptr;

		// Sleep(5000);

		static auto RestartGame = FindObject<UFunction>("/Script/Engine.GameMode.RestartGame");
		Helper::GetGameMode()->ProcessEvent(RestartGame);

		Teams::NextTeamIndex = Teams::StartingTeamIndex;
		Teams::CurrentNumPlayersOnTeam = 0;

		Defines::bReadyForStartMatch = true;
	}
	*/

	if (Defines::bShouldStartBus)
	{
		Defines::bShouldStartBus = false;
		FString cmd = L"startaircraft";

		Helper::ExecuteConsoleCommand(cmd);
	}

	if (Defines::bShouldSpawnForagedItems)
	{
		Defines::bShouldSpawnForagedItems = false;

		static auto BGAConsumableSpawnerClass = FindObject("/Script/FortniteGame.BGAConsumableSpawner");

		if (BGAConsumableSpawnerClass)
		{
			auto AllActors = Helper::GetAllActorsOfClass(BGAConsumableSpawnerClass);

			std::cout << "Spawning: " << AllActors.Num() << " foraged items!\n";

			for (int i = 0; i < AllActors.Num(); i++)
			{
				auto AllActor = AllActors.At(i);

				if (AllActor)
				{
					static auto SpawnLootTierGroupOffset = AllActor->GetOffset("SpawnLootTierGroup");
					auto SpawnLootTierGroupFName = Get<FName>(AllActor, SpawnLootTierGroupOffset);

					auto SpawnLootTierGroup = SpawnLootTierGroupFName->ToString();

					std::cout << "SpawnLootTierGroup: " << SpawnLootTierGroup << '\n';

					auto Location = Helper::GetCorrectLocation(AllActor);
					auto Rotation = Helper::GetActorRotation(AllActor);

					// static auto BGACWID = FindObject("/Script/FortniteGame.BGAConsumableWrapperItemDefinition");

#ifdef TEST_NEW_LOOTING
					auto LootDrops = Looting::PickLootDrops(SpawnLootTierGroup);

					std::cout << "LootDrops: " << LootDrops.size() << '\n';

					for (auto& LootDrop : LootDrops)
					{
						static auto bgafrift = FindObject("/Game/Athena/Items/ForagedItems/Rift/ConsumableVersion/Athena_Foraged_Rift.Athena_Foraged_Rift");

						UObject* ConsumableClass = nullptr;

						static auto ConsumableClassOffset = LootDrop.first->GetOffset("ConsumableClass");
						auto ConsumableClassSoft = Get<TSoftObjectPtr>(LootDrop.first, ConsumableClassOffset);

						if (LootDrop.first == bgafrift) // we love rifts
						{
							static auto riftportal = load(Helper::GetBGAClass(), "/Game/Athena/Items/ForagedItems/Rift/BGA_RiftPortal_Athena.BGA_RiftPortal_Athena_C");
							ConsumableClass = riftportal;
						}
						else
						{
							ConsumableClass = ConsumableClassSoft->Get(Helper::GetBGAClass());
						}

						if (ConsumableClass)
						{
							std::cout << "class: " << ConsumableClass->GetFullName() << '\n';

							Helper::Easy::SpawnActor(ConsumableClass, Location, Rotation);
						}
						else
						{
							std::cout << "unable to find: " << ConsumableClassSoft->ObjectID.AssetPathName.ToString() << '\n';
						}
					}
#endif
				}
			}
		}
	}

	if (Defines::bShouldSpawnVehicles)
	{
		Defines::bShouldSpawnVehicles = false;

		static auto FortVehicleSpawnerClass = FindObject("/Script/FortniteGame.FortAthenaVehicleSpawner"); // FindObject("/Game/Athena/DrivableVehicles/Athena_VehicleSpawner.Athena_VehicleSpawner_C");

		auto spawnerClass = FortVehicleSpawnerClass;

		auto Spawners = Helper::GetAllActorsOfClass(spawnerClass);

		std::cout << "Spawning: " << Spawners.Num() << " vehicles\n";

		for (int i = 0; i < Spawners.Num(); i++)
		{
			auto Spawner = Spawners.At(i);

			if (!Spawner)
				continue;

			// std::cout << std::format("[{}] {}\n", i, Spawner->GetFullName());

			static auto FortVehicleItemDefVariantsOffset = Spawner->GetOffset("FortVehicleItemDefVariants");

			bool aa = true;

			static auto VIDClass = FindObject("/Script/FortniteGame.FortVehicleItemDefinition");

			if (FortVehicleItemDefVariantsOffset != 0)
			{
				struct FVehicleWeightedDef
				{
					TSoftObjectPtr VehicleItemDef;
					FScalableFloat Weight;                                                   // 0x0028(0x0020) (Edit, BlueprintVisible, BlueprintReadOnly)
				};

				auto FortVehicleItemDefVariants = Get<TArray<FVehicleWeightedDef>>(Spawner, FortVehicleItemDefVariantsOffset);

				// std::cout << "FortVehicleItemDefVariants: " << FortVehicleItemDefVariants->Num() << '\n';

				if (FortVehicleItemDefVariants->Num() > 0)
				{
					aa = false;
					auto first = FortVehicleItemDefVariants->At(0);

					auto AssetPathName = first.VehicleItemDef.ObjectID.AssetPathName;

					// std::cout << "AssetPathName: " << AssetPathName.ComparisonIndex << '\n';

					if (!AssetPathName.ComparisonIndex)
						continue;

					auto VehicleItemDef = load(VIDClass, AssetPathName.ToString());

					// std::cout << "VehicleItemDef: " << VehicleItemDef << '\n';

					if (VehicleItemDef)
					{
						static auto VehicleActorClassOffset = VehicleItemDef->GetOffset("VehicleActorClass");

						auto VehicleActorClassSoft = Get<TSoftObjectPtr>(VehicleItemDef, VehicleActorClassOffset);

						auto assetpathname = VehicleActorClassSoft->ObjectID.AssetPathName;

						// std::cout << "assetpathname sof: " << assetpathname.ComparisonIndex << '\n';

						if (!assetpathname.ComparisonIndex)
							continue;

						auto VehicleActorClass = load(Helper::GetBGAClass(), assetpathname.ToString());

						// std::cout << "VehicleActorClass: " << VehicleActorClass << '\n';

						if (!VehicleActorClass)
							continue;

						auto SpawnerLoc = Helper::GetActorLocation(Spawner);

						Helper::Easy::SpawnActor(VehicleActorClass, SpawnerLoc, Helper::GetActorRotation(Spawner));
					}
				}
			}
			
			if (aa)
			{
				static auto FortVehicleItemDefOffset = Spawner->GetOffset("FortVehicleItemDef");

				auto FortVehicleItemDefSoft = Get<TSoftObjectPtr>(Spawner, FortVehicleItemDefOffset);

				auto assstpaht = FortVehicleItemDefSoft->ObjectID.AssetPathName;

				// std::cout << "assstpaht: " << assstpaht.ComparisonIndex << '\n';

				if (!assstpaht.ComparisonIndex)
					continue;

				auto FortVehicleItemDef = load(VIDClass, assstpaht.ToString());

				if (!FortVehicleItemDef)
					continue;

				static auto VehicleActorClassOffset = FortVehicleItemDef->GetOffset("VehicleActorClass");

				auto VehicleActorClassSoft = Get<TSoftObjectPtr>(FortVehicleItemDef, VehicleActorClassOffset);

				auto assetpathname = VehicleActorClassSoft->ObjectID.AssetPathName;

				// std::cout << "assetpathname sof: " << assetpathname.ComparisonIndex << '\n';

				if (!assetpathname.ComparisonIndex)
					continue;

				auto VehicleActorClass = load(Helper::GetBGAClass(), assetpathname.ToString());

				// std::cout << "VehicleActorClass: " << VehicleActorClass << '\n';

				if (!VehicleActorClass)
					continue;

				auto SpawnerLoc = Helper::GetActorLocation(Spawner);

				Helper::Easy::SpawnActor(VehicleActorClass, SpawnerLoc, Helper::GetActorRotation(Spawner));
			}
		}

		Spawners.Free();
	}

	if (Defines::bShouldSpawnFloorLoot && Engine_Version >= 421) // TODO move this
	{
		static auto SpawnIsland_FloorLoot = FindObject("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_Warmup.Tiered_Athena_FloorLoot_Warmup_C");
		static auto BRIsland_FloorLoot = FindObject("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C");

		if (!SpawnIsland_FloorLoot || !BRIsland_FloorLoot) // Map has not loaded to the point where there are floor loot actors
		{
			SpawnIsland_FloorLoot = FindObject("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_Warmup.Tiered_Athena_FloorLoot_Warmup_C");
			BRIsland_FloorLoot = FindObject("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C");
		}
		else
		{
			std::cout << "Spawning floor loot!\n";

			std::cout << "SpawnIsland_FloorLoot: " << SpawnIsland_FloorLoot << '\n';
			std::cout << "BRIsland_FloorLoot: " << BRIsland_FloorLoot << '\n';

			auto SpawnFloorLoot = [](UObject* Class, const std::string& TierGroup) -> int
			{
				auto ClassActors = Helper::GetAllActorsOfClass(Class);

				std::cout << "Size: " << ClassActors.Num() << '\n';

				for (int i = 0; i < ClassActors.Num(); i++)
				{
					auto ClassActor = ClassActors.At(i);

					if (ClassActor)
					{
						BothVector CorrectLocation = Helper::GetActorLocationDynamic(ClassActor);
						
						if (Fortnite_Season < 20)
							CorrectLocation.fV.Z += 50;
						else
							CorrectLocation.dV.Z += 50;

						EFortPickupSourceTypeFlag SourceTypeFlag = EFortPickupSourceTypeFlag::FloorLoot;

#ifdef TEST_NEW_LOOTING

						auto LootDrops = Looting::PickLootDrops(TierGroup);

						for (auto& LootDrop : LootDrops)
						{
							Helper::SummonPickup(nullptr, LootDrop.first, CorrectLocation, SourceTypeFlag, EFortPickupSpawnSource::Unset, LootDrop.second, true);
						}

						continue;

#endif

						bool ShouldSpawn = RandomBoolWithWeight(0.3f);

						if (ShouldSpawn)
						{
							UObject* MainPickup = nullptr;

							if (RandomBoolWithWeight(6, 1, 100))
							{
								auto Ammo = Looting::GetRandomItem(ItemType::Ammo);

								MainPickup = Helper::SummonPickup(nullptr, Ammo.Definition, CorrectLocation, SourceTypeFlag,
									EFortPickupSpawnSource::Unset, Ammo.DropCount);
							}

							else if (RandomBoolWithWeight(5, 1, 100))
							{
								auto Trap = Looting::GetRandomItem(ItemType::Trap);

								MainPickup = Helper::SummonPickup(nullptr, Trap.Definition, CorrectLocation, SourceTypeFlag,
									EFortPickupSpawnSource::Unset, Trap.DropCount);
							}

							else if (RandomBoolWithWeight(26, 1, 100))
							{
								auto Consumable = Looting::GetRandomItem(ItemType::Consumable);

								MainPickup = Helper::SummonPickup(nullptr, Consumable.Definition, CorrectLocation, SourceTypeFlag,
									EFortPickupSpawnSource::Unset, Consumable.DropCount);
							}

							else
							{
								auto Weapon = Looting::GetRandomItem(ItemType::Weapon);

								MainPickup = Helper::SummonPickup(nullptr, Weapon.Definition, CorrectLocation, SourceTypeFlag, EFortPickupSpawnSource::Unset, 1, true);

								if (MainPickup)
								{
									auto AmmoDef = Helper::GetAmmoForDefinition(Weapon.Definition);

									Helper::SummonPickup(nullptr, AmmoDef.first, CorrectLocation, SourceTypeFlag,
										EFortPickupSpawnSource::Unset, AmmoDef.second);
								}
							}
						}
					}
				}

				int Num = ClassActors.Num();

				ClassActors.Free();

				return Num;
			};

			if (SpawnFloorLoot(SpawnIsland_FloorLoot, "Loot_AthenaFloorLoot_Warmup") != 0)
			{
				if (SpawnFloorLoot(BRIsland_FloorLoot, "Loot_AthenaFloorLoot") != 0)
				{
					Defines::bShouldSpawnFloorLoot = false;
					std::cout << "Finished spawning floor loot!\n";
				}
			}
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

/* UObject* GetViewTargetCameraManager(UObject* NonConstThis)
{
	return nullptr;

	static auto PendingViewTargetOffset = NonConstThis->GetOffset("PendingViewTarget");
	auto PendingViewTarget = Get<FTViewTarget>(NonConstThis, PendingViewTargetOffset);

	static auto PCOwnerOffset = NonConstThis->GetOffset("PCOwner");
	auto PCOwner = *Get<UObject*>(NonConstThis, PCOwnerOffset);

	if (PendingViewTarget->Target)
	{
		CheckViewTarget(PendingViewTarget, PCOwner);

		if (PendingViewTarget->Target)
			return PendingViewTarget->Target;
	}

	static auto ViewTargetOffset = NonConstThis->GetOffset("ViewTarget");
	auto ViewTarget = Get<FTViewTarget>(NonConstThis, ViewTargetOffset);

	CheckViewTarget(ViewTarget, PCOwner);
	return ViewTarget->Target; 
} */

UObject* Server::Hooks::GetViewTarget(UObject* PC, __int64 Unused, __int64 a3)
{
	auto Pawn = Helper::GetPawnFromController(PC);

	return Pawn ? Pawn : PC;

	// skunk buit work
	UObject* PlayerCameraManager = nullptr;

	UObject* CameraManagerViewTarget = PlayerCameraManager ? GetViewTargetCameraManager(PlayerCameraManager) : nullptr;

	// https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Source/Runtime/Engine/Private/PlayerCameraManager.cpp#L235
	// https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Source/Runtime/Engine/Private/PlayerCameraManager.cpp#L1603
	// https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Source/Runtime/Engine/Private/PlayerCameraManager.cpp#L195

	return CameraManagerViewTarget ? CameraManagerViewTarget : PC;
}
