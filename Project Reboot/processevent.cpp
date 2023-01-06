#include "processevent.h"
#include "helper.h"
#include <MinHook.h>
#include "inventory.h"
#include "abilities.h"
#include "patterns.h"
#include "server.h"
#include "zone.h"
#include "harvesting.h"
#include "calendar.h"
#include "loot.h"
#include "team.h"
#include "events.h"
#include <fstream>
#include "moderation.h"
#include "interaction.h"

#ifdef MILXNOR_H
#include "milxnor.h"
#endif

// bool ReceivedDestroyed(UObject* Effect, UFunction*, void* Parameters) { return true; }

bool ServerAcknowledgePossession(UObject* Object, UFunction* Function, void* Parameters)
{
	struct SAP_Params { UObject* P; };

	auto Params = (SAP_Params*)Parameters;

	if (Params)
	{
		auto Pawn = Params->P;

		static auto AcknowledgedPawnOffset = Object->GetOffset("AcknowledgedPawn");
		*(UObject**)(__int64(Object) + AcknowledgedPawnOffset) = Pawn;
	}

	return false;
}

bool HandleStartingNewPlayer(UObject* Object, UFunction* Function, void* Parameters)
{
	ProcessEventO(Object, Function, Parameters);

	if (!Parameters) // possible?
		return false;

	static int LastTime = 931249515401;

	if (LastTime != Defines::AmountOfRestarts)
	{
		LastTime = Defines::AmountOfRestarts;
		return false;
	}

	static bool bIsFirstClient = false;

	if (!bIsFirstClient)
	{
		bIsFirstClient = true;

		static auto func1 = FindObject("/Game/Athena/SafeZone/SafeZoneIndicator.SafeZoneIndicator_C.OnSafeZoneStateChange");

		AddHook(func1 ? "/Game/Athena/SafeZone/SafeZoneIndicator.SafeZoneIndicator_C.OnSafeZoneStateChange" :
			"/Script/FortniteGame.FortSafeZoneIndicator.OnSafeZoneStateChange", Zone::OnSafeZoneStateChange);

		AddHook("/Script/FortniteGame.FortMatchAnalytics.OnGamePhaseChanged", OnGamePhaseChanged);

		AddHook("/Game/Abilities/Weapons/Ranged/GA_Ranged_GenericDamage.GA_Ranged_GenericDamage_C.K2_CommitExecute", commitExecuteWeapon);

		// AddHook(FindObject("/Script/FortniteGame.FortPhysicsPawn.ServerMove") ? "/Script/FortniteGame.FortPhysicsPawn.ServerMove"
			// : FindObject("/Script/FortniteGame.FortAthenaVehicle.ServerUpdatePhysicsParams") ? "/Script/FortniteGame.FortAthenaVehicle.ServerUpdatePhysicsParams"
			// : "/Script/FortniteGame.FortPhysicsPawn.ServerUpdatePhysicsParams", ServerUpdatePhysicsParams);
 
		// AddHook("/Game/Effects/Fort_Effects/Gameplay/Pickups/B_Pickups_Parent.B_Pickups_Parent_C.ReceiveDestroyed", ReceivedDestroyed);
		AddHook("/Game/Athena/BuildingActors/ConsumableBGAs/CBGA_Parent.CBGA_Parent_C.OnGatherOrInteract", OnGatherOrInteract);

		// AddHook("/Script/FortniteGame.FortPlayerController.ClientForceWorldInventoryUpdate", ClientForceWorldInventoryUpdate);

		if (Engine_Version > 424)
			AddHook("/Script/FortniteGame.BuildingSMActor.BlueprintCanAttemptGenerateResources", Harvesting::BlueprintCanAttemptGenerateResources);
		else
		{
			AddHook(("/Script/FortniteGame.BuildingActor.OnDamageServer"), Harvesting::OnDamageServer);

			if (Fortnite_Version >= 8.00)
				AddHook("/Game/Building/ActorBlueprints/Prop/Car_DEFAULT.Car_DEFAULT_C.OnDamageServer", Harvesting::OnDamageServer);
			else
				AddHook("/Game/Building/ActorBlueprints/Prop/Car_Copper.Car_Copper_C.OnDamageServer", Harvesting::OnDamageServer);
		}

		// *Get<float>(GameState, WarmupCountdownEndTimeOffset) = 1000.f;

		if (Fortnite_Season == 19)
			Helper::SetSnowIndex(0); // Fill snow
		if (Fortnite_Season == 11)
			Helper::SetSnowIndex(100);
		if (Fortnite_Version == 7.2 || Fortnite_Version == 7.3)
			Helper::SetSnowIndex(1000);

		static bool bbb = false;

		if (!bbb)
		{
			bbb = true;

			if (Engine_Version >= 423)
			{
				static auto OnRep_CurrentPlaylistInfo = FindObject<UFunction>("/Script/FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistInfo");
				Helper::GetGameState()->ProcessEvent(OnRep_CurrentPlaylistInfo);
				std::cout << "wtf!\n";
			}
		}

		if (Fortnite_Version == 5.41)
		{
			auto cube = FindObject("/Game/Athena/Maps/Test/Level_CUBE.Level_CUBE.PersistentLevel.CUBE_2");

			if (cube)
			{
				static auto spawncube = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Cube/CUBE.CUBE_C.SpawnCube");
				cube->ProcessEvent(spawncube);
			}
		}

		Defines::bShouldSpawnFloorLoot = Looting::bInitialized && Fortnite_Version < 19.40;// (Fortnite_Version >= 19.40 ? (bool)Defines::ActorChannelClose : true);
	}

	UObject* PlayerController = *(UObject**)Parameters;

	if (PlayerController)
	{
		if (Moderation::Banning::IsBanned(PlayerController))
		{
			std::string KickReason = "You have been banned!";

			std::wstring wstr = std::wstring(KickReason.begin(), KickReason.end());
			FString Reason;
			Reason.Set(wstr.c_str());

			static auto ClientReturnToMainMenu = FindObject<UFunction>("/Script/Engine.PlayerController.ClientReturnToMainMenu");
			PlayerController->ProcessEvent(ClientReturnToMainMenu, &Reason);
			return false;
		}
		
		auto PlayerState = Helper::GetPlayerStateFromController(PlayerController);

		if (!PlayerState)
			return false;

		std::cout << "set!\n";

		if (Fortnite_Version < 7.4)
		{
			static const auto QuickBarsClass = FindObject("/Script/FortniteGame.FortQuickBars");
			static auto QuickBarsOffset = PlayerController->GetOffset("QuickBars");

			*(UObject**)(__int64(PlayerController) + QuickBarsOffset) = Helper::Easy::SpawnActor(QuickBarsClass, FVector(), FRotator(), PlayerController);
		}

		static auto bHasServerFinishedLoadingOffset = PlayerController->GetOffset("bHasServerFinishedLoading");
		*Get<bool>(PlayerController, bHasServerFinishedLoadingOffset) = true;

		static auto bHasStartedPlayingOffset = PlayerState->GetOffset("bHasStartedPlaying"); // BITFIELD
		*Get<bool>(PlayerState, bHasStartedPlayingOffset) = true;
	}

	return true; // should be false
}

bool ServerReadyToStartMatch(UObject* PlayerController, UFunction* Function, void* Parameters)
{
	auto Pawan = Helper::GetPawnFromController(PlayerController);

	auto GameState = Helper::GetGameState();
	static auto GamePhaseOffset = GameState->GetOffset("GamePhase");

	if (Pawan || *Get<EAthenaGamePhase>(GameState, GamePhaseOffset) > EAthenaGamePhase::Warmup)
		return false;

	auto PlayerState = Helper::GetPlayerStateFromController(PlayerController);

	// static auto WarmupPlayerStartOffset = PlayerController->GetOffset("WarmupPlayerStart");
	// std::cout << "WarmupPlayerStart: " << *Get<UObject*>(PlayerController, WarmupPlayerStartOffset) << '\n';

	auto PlayerStart = Helper::GetPlayerStart(); // *Get<UObject*>(PlayerController, WarmupPlayerStartOffset); // Helper::GetPlayerStart();

	if (!PlayerStart)
	{
		std::cout << "Player joined too early or unable to find playerstart!\n";
		// Helper::KickController(PlayerController, "You joined too early!");
		// return false;
	}

	bool bSpawnIsland = *Get<EAthenaGamePhase>(GameState, GamePhaseOffset) <= EAthenaGamePhase::Warmup; // skunk

	bool bgotafuwyu24tgi32rg = false;
	BothVector suauyr329234ig23;

	if (Defines::bIsGoingToPlayMainEvent)
	{
		suauyr329234ig23 = Events::GetSpawnLocation(&bgotafuwyu24tgi32rg);
		bSpawnIsland = false;
	}

	BothVector SpawnLocation = !PlayerStart || !bSpawnIsland ? (Fortnite_Season >= 20 ? BothVector(DVector{ 1250, 1818, 3284 }) : BothVector(FVector{ 1250, 1818, 3284 }))
		: Helper::GetActorLocationDynamic(PlayerStart);

	if (bgotafuwyu24tgi32rg)
		SpawnLocation = suauyr329234ig23;

	std::cout << "Spawn Loc: " << (Fortnite_Season >= 20 ? std::format("X {} Y {} Z {}\n", SpawnLocation.dV.X, SpawnLocation.dV.Y, SpawnLocation.dV.Z) :
		std::format("X {} Y {} Z {}\n", SpawnLocation.fV.X, SpawnLocation.fV.Y, SpawnLocation.fV.Z)) << '\n';

	if (Engine_Version <= 420)
	{
		static UObject* HeroTypeToUse = FindObject("/Game/Athena/Heroes/HID_058_Athena_Commando_M_SkiDude_GER.HID_058_Athena_Commando_M_SkiDude_GER");

		static auto HeroTypeOffset = PlayerState->GetOffset("HeroType");
		*Get<UObject*>(PlayerState, HeroTypeOffset) = HeroTypeToUse;
	}

	// SETUP LOADOUT

	bool bUpdate = false;

	static auto EditTool = FindObject("/Game/Items/Weapons/BuildingTools/EditTool.EditTool");
	auto EditToolInstance = Inventory::GiveItem(PlayerController, EditTool, EFortQuickBars::Primary, 0, 1, bUpdate);

	static auto BuildingItemData_Wall = FindObject(("/Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall"));
	static auto BuildingItemData_Floor = FindObject(("/Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor"));
	static auto BuildingItemData_Stair_W = FindObject(("/Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W"));
	static auto BuildingItemData_RoofS = FindObject(("/Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS"));

	UObject* PickaxeDef = Helper::GetPickaxeDef(PlayerController, true); // FindObject("/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
	auto PickaxeInstance = Inventory::GiveItem(PlayerController, PickaxeDef, EFortQuickBars::Primary, 0);

	Inventory::GiveItem(PlayerController, BuildingItemData_Wall, EFortQuickBars::Secondary, 0, bUpdate);
	Inventory::GiveItem(PlayerController, BuildingItemData_Floor, EFortQuickBars::Secondary, 1, bUpdate);
	Inventory::GiveItem(PlayerController, BuildingItemData_Stair_W, EFortQuickBars::Secondary, 2, bUpdate);
	Inventory::GiveItem(PlayerController, BuildingItemData_RoofS, EFortQuickBars::Secondary, 3, bUpdate);

	if (Defines::bIsCreative)
	{
		static UObject* PhoneDef = FindObject("/Game/Athena/Items/Weapons/Prototype/WID_CreativeTool.WID_CreativeTool");
		auto CreativeToolInstance = Inventory::GiveItem(PlayerController, PhoneDef, EFortQuickBars::Primary, 1);
	}

	// static auto d = FindObject("/ParallelGameplay/Items/WestSausage/WID_WestSausage_Parallel.WID_WestSausage_Parallel");
	// auto d2 = Inventory::GiveItem(PlayerController, d, EFortQuickBars::Primary, 2);

	// FFortItemEntry::SetLoadedAmmo(UFortItem::GetItemEntry(d2), PlayerController, 100);

	static UObject* Def1 = FindObject("/Game/Athena/Items/Traps/TID_Context_BouncePad_Athena.TID_Context_BouncePad_Athena");
	// auto Def1Instance = Inventory::GiveItem(PlayerController, Def1, EFortQuickBars::Secondary, 0);

	static UObject* Def2 = FindObject("/Game/Athena/Items/Traps/TID_Floor_MountedTurret_Athena.TID_Floor_MountedTurret_Athena");
	// auto Def2Instance = Inventory::GiveItem(PlayerController, Def2, EFortQuickBars::Secondary, 0);

	if (Defines::bIsGoingToPlayMainEvent && Fortnite_Season == 16)
	{
		static auto PortalDeviceDef = FindObject("/Yogurt/Blueprints/WID_Yogurt_PortalDevice.WID_Yogurt_PortalDevice");
		Inventory::GiveItem(PlayerController, PortalDeviceDef, EFortQuickBars::Primary, 1);
	}

	UObject* Pawn = Helper::SpawnPawn(PlayerController, SpawnLocation, true);

	if (!Pawn)
		return false;

	if (Defines::bIsGoingToPlayMainEvent)
	{
		static auto CheatManagerOffset = PlayerController->GetOffset("CheatManager");
		auto CheatManager = (UObject**)(__int64(PlayerController) + CheatManagerOffset);

		static auto CheatManagerClass = FindObject("/Script/Engine.CheatManager");
		*CheatManager = Helper::Easy::SpawnObject(CheatManagerClass, PlayerController);

		static auto God = FindObject<UFunction>("/Script/Engine.CheatManager.God");
		(*CheatManager)->ProcessEvent(God);
	}

	auto CurrentPlaylist = Helper::GetPlaylist();

	static auto FortPlayerControllerZoneClass = FindObject("/Script/FortniteGame.FortPlayerControllerZone");

	if (PlayerController->IsA(FortPlayerControllerZoneClass))
	{
		if (Fortnite_Version < 8.30)
		{
			static auto AbilitySet = FindObject(("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer"));
			GiveFortAbilitySet(Pawn, AbilitySet);
		}
		else
		{
			static auto AbilitySet = FindObject(("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_AthenaPlayer.GAS_AthenaPlayer"));
			GiveFortAbilitySet(Pawn, AbilitySet);
		}

		if (!IsBadReadPtr(CurrentPlaylist) && !IsBadReadPtr(*CurrentPlaylist))
		{
			static auto LTMModifiersOffset = (*CurrentPlaylist)->GetOffset("ModifierList", false, false, false);

			if (LTMModifiersOffset != 0)
			{
				auto LTMModifiers = Get<TArray<TSoftObjectPtr>>(*CurrentPlaylist, LTMModifiersOffset);

				//std::cout << "LTMModifiers->Num(): " << LTMModifiers->Num() << '\n';

				for (int i = 0; i < LTMModifiers->Num(); i++)
				{
					static auto LTMModifierClass = FindObject("/Script/FortniteGame.FortGameplayModifierItemDefinition");
					auto& LTMModifierSoftObject = LTMModifiers->At(i);
					auto LTMModifier = LTMModifierSoftObject.Get(LTMModifierClass);

					// std::cout << std::format("[{}] {}\n", i, CoreGameModeModifier->GetFullName());

					static auto PersistentGameplayEffectsOffset = LTMModifier->GetOffset("PersistentGameplayEffects");
					static auto PersistentAbilitySetsOffset = LTMModifier->GetOffset("PersistentAbilitySets");
					auto PersistentGameplayEffects = Get<TArray<__int64>>(LTMModifier, PersistentGameplayEffectsOffset);
					auto PersistentAbilitySets = Get<TArray<__int64>>(LTMModifier, PersistentAbilitySetsOffset);

					static auto FortGameplayEffectDeliveryInfoSize = Helper::GetSizeOfClass(FindObject("/Script/FortniteGame.FortGameplayEffectDeliveryInfo"));
					static auto FortAbilitySetDeliveryInfoSize = Helper::GetSizeOfClass(FindObject("/Script/FortniteGame.FortAbilitySetDeliveryInfo"));

					if (!IsBadReadPtr(PersistentGameplayEffects))
					{
						for (int o = 0; o < PersistentGameplayEffects->Num(); o++)
						{
							auto DeliveryInfo = PersistentGameplayEffects->AtPtr(o, FortGameplayEffectDeliveryInfoSize);

							static auto DeliveryRequirementsOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortGameplayEffectDeliveryInfo", "DeliveryRequirements");
							void* DeliveryRequirements = Get<void>(DeliveryInfo, DeliveryRequirementsOffset);

							static auto bApplyToPlayerPawnsOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortDeliveryInfoRequirementsFilter", "bApplyToPlayerPawns");
							static auto bApplyToPlayerPawnsFieldMask = GetFieldMask(FindPropStruct2("ScriptStruct /Script/FortniteGame.FortDeliveryInfoRequirementsFilter", "bApplyToPlayerPawns"));

							std::cout << "bApplyToPlayerPawnsFieldMask: " << bApplyToPlayerPawnsFieldMask << '\n';

							if (ReadBitfield(Get<PlaceholderBitfield>(DeliveryRequirements, bApplyToPlayerPawnsOffset), bApplyToPlayerPawnsFieldMask))
							{
								static auto GameplayEffectsOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortGameplayEffectDeliveryInfo", "GameplayEffects");

								if (GameplayEffectsOffset != 0)
								{
									struct FGameplayEffectApplicationInfo
									{
										TSoftObjectPtr GameplayEffect;
										float Level;                                                    // 0x0028(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
										unsigned char                                      UnknownData01[0x4];                                       // 0x002C(0x0004) MISSED OFFSET
									};

									auto GrantedGameplayEffects = Get<TArray<FGameplayEffectApplicationInfo>>(DeliveryInfo, GameplayEffectsOffset);

									for (int n = 0; n < GrantedGameplayEffects->Num(); n++)
									{
										auto& GameplayEffectInfo = GrantedGameplayEffects->At(n);

										static auto BlueprintGeneratedClass = Helper::GetBGAClass();

										auto GameplayEffectBlueprintGeneratedClass = GameplayEffectInfo.GameplayEffect.Get(BlueprintGeneratedClass);

										if (GameplayEffectBlueprintGeneratedClass)
										{
											Helper::ApplyGameplayEffect(Pawn, GameplayEffectBlueprintGeneratedClass, GameplayEffectInfo.Level);
										}
									}
								}
							}
						}
					}

					if (!IsBadReadPtr(PersistentAbilitySets))
					{
						for (int p = 0; p < PersistentAbilitySets->Num(); p++)
						{
							auto DeliveryInfo = PersistentAbilitySets->AtPtr(p, FortAbilitySetDeliveryInfoSize);

							static auto DeliveryRequirementsOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortAbilitySetDeliveryInfo", "DeliveryRequirements");
							void* DeliveryRequirements = Get<void>(DeliveryInfo, DeliveryRequirementsOffset);

							static auto bApplyToPlayerPawnsOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortDeliveryInfoRequirementsFilter", "bApplyToPlayerPawns");
							static auto bApplyToPlayerPawnsFieldMask = GetFieldMask(FindPropStruct2("ScriptStruct /Script/FortniteGame.FortDeliveryInfoRequirementsFilter", "bApplyToPlayerPawns"));

							std::cout << "bApplyToPlayerPawnsFieldMask: " << bApplyToPlayerPawnsFieldMask << '\n';

							if (ReadBitfield(Get<PlaceholderBitfield>(DeliveryRequirements, bApplyToPlayerPawnsOffset), bApplyToPlayerPawnsFieldMask))
							{
								static auto AbilitySetsOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortAbilitySetDeliveryInfo", "AbilitySets");
								auto AbilitySets = Get<TArray<TSoftObjectPtr>>(DeliveryInfo, AbilitySetsOffset);

								for (int z = 0; z < AbilitySets->Num(); z++)
								{
									static auto FortAbilitySetClass = FindObject("/Script/FortniteGame.FortAbilitySet");

									auto& AbilitySetSoft = AbilitySets->At(z);

									auto CurrentAbilitySet = AbilitySetSoft.Get(FortAbilitySetClass);

									std::cout << "CurrentAbilitySet: " << CurrentAbilitySet << " AbilitySetSoft.ObjectID.AssetPathName.ToString(): " << AbilitySetSoft.ObjectID.AssetPathName.ToString() << '\n';

									if (CurrentAbilitySet)
										GiveFortAbilitySet(Pawn, CurrentAbilitySet);
								}
							}
						}
					}
				}
			}
		}
	}

	static bool bFirst = true;

	if (bFirst)
	{
		bFirst = false;

		auto GameMode = Helper::GetGameMode();

		static auto StartMatch = FindObject<UFunction>("/Script/Engine.GameMode.StartMatch");
		GameMode->ProcessEvent(StartMatch);

		bool bGoIntoWarmup = !Defines::bIsGoingToPlayMainEvent || Fortnite_Version == 14.60;

		std::cout << "bGoIntoWarmup: " << bGoIntoWarmup << '\n';

		if (bGoIntoWarmup)
		{
			auto OldPhase = *Get<EAthenaGamePhase>(GameState, GamePhaseOffset);
			*Get<EAthenaGamePhase>(GameState, GamePhaseOffset) = EAthenaGamePhase::Warmup;

			static auto OnRepGamePhase = FindObject<UFunction>("/Script/FortniteGame.FortGameStateAthena.OnRep_GamePhase");
			GameState->ProcessEvent(OnRepGamePhase, &OldPhase);
		}
	}

	if (Engine_Version <= 421 && *Get<EAthenaGamePhase>(GameState, GamePhaseOffset) == EAthenaGamePhase::Warmup)
	{
		static auto bCanBeDamagedOffset = Pawn->GetOffset("bCanBeDamaged");
		static auto bCanBeDamagedFieldMask = GetFieldMask(Pawn->GetProperty("bCanBeDamaged"));
		SetBitfield(Get<PlaceholderBitfield>(Pawn, bCanBeDamagedOffset), bCanBeDamagedFieldMask, true);
	}

	return false;
}

bool ReadyToStartMatch(UObject* GameMode, UFunction* Function, void* Parameters)
{
	/* static auto WarmupClass = Defines::bIsCreative ? FindObject("/Script/FortniteGame.FortPlayerStartCreative") : FindObject("/Script/FortniteGame.FortPlayerStartWarmup");

	if (!WarmupClass)
		return false;

	TArray<UObject*> OutActors = Helper::GetAllActorsOfClass(WarmupClass);

	if (OutActors.Num() == 0)
	{
		return false;
	} */

	if (Defines::bReadyForStartMatch)
	{
		Defines::bReadyForStartMatch = false;

		auto GameState = Helper::GetGameState();

		static auto GamePhaseOffset = GameState->GetOffset("GamePhase");
		auto OldGamePhase = *Get<EAthenaGamePhase>(GameState, GamePhaseOffset);

		*Get<EAthenaGamePhase>(GameState, GamePhaseOffset) = EAthenaGamePhase::None;

		static auto OnRep_GamePhase = FindObject<UFunction>("/Script/FortniteGame.FortGameStateAthena.OnRep_GamePhase");
		GameState->ProcessEvent(OnRep_GamePhase, &OldGamePhase);

		if (Defines::Playlist == "/Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground" && Fortnite_Version > 11.30)
			Defines::Playlist = "/Game/Athena/Playlists/BattleLab/Playlist_BattleLab.Playlist_BattleLab";

		std::string cpyplaylist = Defines::Playlist;

		if (cpyplaylist.find(".") == std::string::npos)
			cpyplaylist = std::format("{}.{}", cpyplaylist, cpyplaylist);

		if (cpyplaylist.find(" ") != std::string::npos)
			cpyplaylist = cpyplaylist.substr(cpyplaylist.find(" ") + 1);

		auto Playlist = FindObjectSlow(cpyplaylist);

		std::cout << "Setting playlist to: " << (Playlist ? Playlist->GetName() : "UNDEFINED") << '\n';

		auto Engine = Helper::GetEngine();

		static auto GameInstanceOffset = Engine->GetOffset("GameInstance");
		auto GameInstance = *Get<UObject*>(Engine, GameInstanceOffset);

		if (!GameInstance)
			return false;

		static auto LocalPlayersOffset = GameInstance->GetOffset("LocalPlayers");
		auto& LocalPlayers = *Get<TArray<UObject*>>(GameInstance, LocalPlayersOffset);

		if (LocalPlayers.size())
			LocalPlayers.RemoveAt(0);

		/* static auto GameplayTagContainerOffset = Playlist->GetOffset("GameplayTagContainer");
		auto GameplayTagContainer = Get<FGameplayTagContainer>(Playlist, GameplayTagContainerOffset);

		std::cout << "playltist tgags; " << GameplayTagContainer->ToStringSimple(true) << '\n'; */

		auto GameStatePlaylist = Helper::GetPlaylist();

		if (GameStatePlaylist)
		{
			*GameStatePlaylist = Playlist;

			if (Fortnite_Version >= 6.10)
			{
				static auto CurrentPlaylistInfoOffset = GameState->GetOffset("CurrentPlaylistInfo");

				static auto PlaylistReplicationKeyOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.PlaylistPropertyArray", "PlaylistReplicationKey");

				auto PlaylistInfo = (void*)(__int64(GameState) + CurrentPlaylistInfoOffset);
				auto PlaylistReplicationKey = (int*)(__int64(PlaylistInfo) + PlaylistReplicationKeyOffset);

				(*(int*)(__int64(PlaylistInfo) + PlaylistReplicationKeyOffset))++;
				FastTArray::MarkArrayDirty(PlaylistInfo);

				static auto OnRep_CurrentPlaylistInfo = FindObject<UFunction>("/Script/FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistInfo");
				GameState->ProcessEvent(OnRep_CurrentPlaylistInfo);
			}
			else
			{
				static auto OnRep_CurrentPlaylistData = FindObject<UFunction>("/Script/FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistData");
				GameState->ProcessEvent(OnRep_CurrentPlaylistData);
			}
		}

		if (!Server::BeaconHost)
		{
			Server::Listen();
			Server::Hooks::Initialize();
		}

		static auto GameSessionOffset = GameMode->GetOffset("GameSession");
		auto GameSession = *Get<UObject*>(GameMode, GameSessionOffset);

		static auto MaxPlayersOffset = GameSession->GetOffset("MaxPlayers");
		*Get<int>(GameSession, MaxPlayersOffset) = 100; // We would get from playlist but playground max is 4 people..

		if (Engine_Version >= 420)
		{
#ifdef TEST_NEW_LOOTING
			Looting::bInitialized = true;
#else
			Looting::Initialize();
#endif
		}

		if (Playlist)
		{
			static auto MinPlayersOffset = Playlist->GetOffset("MinPlayers");
			// *Get<int>(Playlist, MinPlayersOffset) = 1;

			if (Fortnite_Version >= 13)
			{
				static auto LastSafeZoneIndexOffset = Playlist->GetOffset("LastSafeZoneIndex");

				if (LastSafeZoneIndexOffset != -1)
				{
					*(int*)(__int64(Playlist) + LastSafeZoneIndexOffset) = 0;
				}
			}
		}

		auto PlayersLeft = Helper::GetPlayersLeft();

		if (PlayersLeft)
			*PlayersLeft = 0;

		std::cout << "Ready to start match!\n";

		Calendar::FixLocations();

		if (Defines::bIsGoingToPlayMainEvent)
			Events::LoadEvent();

		if (Defines::bIsCreative)
			LoadObject(Helper::GetBGAClass(), "/Game/Playgrounds/Items/BGA_IslandPortal.BGA_IslandPortal_C"); // scuffed

		static auto DefaultGliderRedeployCanRedeployOffset = GameState->GetOffset("DefaultGliderRedeployCanRedeploy", false, false, false);

		if (DefaultGliderRedeployCanRedeployOffset != 0)
			*Get<bool>(GameState, DefaultGliderRedeployCanRedeployOffset) = Defines::bIsPlayground;

		Defines::bIsRestarting = false;

		if (GameMode)
		{
			static auto bWorldIsReadyOffset = GameMode->GetOffset("bWorldIsReady");
			static auto bWorldIsReadyFieldMask = GetFieldMask(GameMode->GetProperty("bWorldIsReady"));
			SetBitfield(Get<PlaceholderBitfield>(GameMode, bWorldIsReadyOffset), bWorldIsReadyFieldMask, true);
		}
	}

	// *(bool*)Parameters = true;

	return true;
	// return false;
}

uint8_t GetDeathCause(UObject* PlayerState, FGameplayTagContainer Tags, bool* OutWasDBNO = nullptr)
{
	// UFortDeathCauseFromTagMapping
	// FortDeathCauseFromTagMapping

	uint8_t DeathCause = 0;

	std::cout << "Tags: " << Tags.ToStringSimple(true) << '\n';

	if (Fortnite_Version >= 6.21) // might have been 6.2
	{
		static auto FortPlayerStateAthenaDefault = FindObject("/Script/FortniteGame.Default__FortPlayerStateAthena");

		struct
		{
			FGameplayTagContainer                       InTags;                                                   // (ConstParm, Parm, OutParm, ReferenceParm, NativeAccessSpecifierPublic)
			bool                                               bWasDBNO;                                                 // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			uint8_t                                        ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		} AFortPlayerStateAthena_ToDeathCause_Params{ Tags, false };

		static auto ToDeathCause = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateAthena.ToDeathCause");
		FortPlayerStateAthenaDefault->ProcessEvent(ToDeathCause, &AFortPlayerStateAthena_ToDeathCause_Params);

		DeathCause = AFortPlayerStateAthena_ToDeathCause_Params.ReturnValue;
	}
	else
	{
		static auto DeathCauseEnum = FindObject("/Script/FortniteGame.EDeathCause");

		for (int i = 0; i < Tags.GameplayTags.Num(); i++) // SKUNK
		{
			auto Tag = Tags.GameplayTags.At(i);

			if (Tag.TagName.ComparisonIndex)
			{
				auto TagNameStr = Tag.TagName.ToString(); // tbh we should compre fnames instead

				if (TagNameStr.contains("Gameplay.Damage.Environment.Falling"))
					return GetEnumValue(DeathCauseEnum, "FallDamage");

				else if (TagNameStr.contains("Weapon.Ranged.Shotgun"))
					return GetEnumValue(DeathCauseEnum, "Shotgun");

				else if (TagNameStr.contains("weapon.ranged.heavy.rocket_launcher"))
					return GetEnumValue(DeathCauseEnum, "RocketLauncher");

				else if (TagNameStr.contains("weapon.ranged.assault."))
					return GetEnumValue(DeathCauseEnum, "Rifle");

				else if (TagNameStr.contains("DeathCause.LoggedOut"))
					return GetEnumValue(DeathCauseEnum, "LoggedOut");

				else if (TagNameStr.contains("Weapon.Ranged.SMG"))
					return GetEnumValue(DeathCauseEnum, "SMG");

				else if (TagNameStr.contains("weapon.ranged.sniper."))
					return GetEnumValue(DeathCauseEnum, "Sniper");

				else if (TagNameStr.contains("Weapon.Ranged.Pistol."))
					return GetEnumValue(DeathCauseEnum, "Pistol");

				else if (TagNameStr.contains("Weapon.Ranged.Grenade.Gas"))
					return GetEnumValue(DeathCauseEnum, "GasGrenade");

				else if (TagNameStr.contains("weapon.ranged.heavy.grenade_launcher"))
					return GetEnumValue(DeathCauseEnum, "GrenadeLauncher");

				else if (TagNameStr.contains("Weapon.Ranged.Minigun"))
					return GetEnumValue(DeathCauseEnum, "Minigun");

				else if (TagNameStr.contains("trap."))
					return GetEnumValue(DeathCauseEnum, "Trap");

				else if (TagNameStr.contains("Gameplay.Damage.TeamSwitchSuicide"))
					return GetEnumValue(DeathCauseEnum, "TeamSwitchSuicide");
			}
		}
	}

	if (OutWasDBNO)
	{

	}

	return DeathCause;
}


bool ClientOnPawnDied(UObject* DeadController, UFunction* fn, void* Parameters)
{
	if (!Parameters)
		return false;

	ProcessEventO(DeadController, fn, Parameters);

	std::cout << "Mf!\n";

	auto GameMode = Helper::GetGameMode();
	auto DeathReport = (__int64*)Parameters;

	auto DeadPlayerState = Helper::GetPlayerStateFromController(DeadController);
	auto DeadPawn = Helper::GetPawnFromController(DeadController);

	static auto KillerPawnOffset = preoffsets::KillerPawn; // FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport", "KillerPawn");
	static auto KillerPlayerStateOffset = preoffsets::KillerPlayerState; // FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport", "KillerPlayerState");

	std::cout << "KillerPlayerStateOffset: " << KillerPlayerStateOffset << '\n';

	auto KillerPawn = *(UObject**)(__int64(DeathReport) + KillerPawnOffset);
	auto KillerPlayerState = *(UObject**)(__int64(DeathReport) + KillerPlayerStateOffset);

	static auto TagsOffset = preoffsets::Tags; // FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport", ("Tags"));
	FGameplayTagContainer* Tags = (FGameplayTagContainer*)(__int64(DeathReport) + TagsOffset);
	bool bWasDBNO = false;
	auto DeathCause = Tags ? GetDeathCause(DeadPlayerState, *Tags, &bWasDBNO) : 0;

	std::cout << "DeathCause: " << DeathCause << '\n';

	// normal death stuff

	auto DeathInfoOffset = preoffsets::DeathInfo; // DeadPlayerState->GetOffset("DeathInfo");

	if (DeathInfoOffset == 0) // iirc if u rejoin and die this is invalid idfk why
		return true;

	auto DeathInfo = Get<__int64>(DeadPlayerState, DeathInfoOffset);

	static auto DeathCauseEnum = FindObject("ScriptStruct /Script/FortniteGame.EDeathCause");

	*(uint8_t*)(__int64(DeathInfo) + preoffsets::DeathCause) = DeathCause;
	*(UObject**)(__int64(DeathInfo) + preoffsets::FinisherOrDowner) = KillerPlayerState ? KillerPlayerState : DeadPlayerState;
	*(bool*)(__int64(DeathInfo) + preoffsets::bDBNO) = bWasDBNO;

	if (preoffsets::bInitialized != 0)
		*(bool*)(__int64(DeathInfo) + preoffsets::bInitialized) = true;

	static auto FallDamageEnumValue = GetEnumValue(DeathCauseEnum, "FallDamage");

	// if (preoffsets::DeathTags != 0)
		// *Get<FGameplayTagContainer>(DeathInfo, preoffsets::DeathTags) = *Tags;

	if (preoffsets::Distance != 0)
	{
		if (DeathCause != FallDamageEnumValue)
		{
			*(float*)(__int64(DeathInfo) + preoffsets::Distance) = KillerPawn ? Helper::GetDistanceTo(KillerPawn, DeadPawn) : 0.f;
		}
		else
		{
			static auto LastFallDistanceOffset = preoffsets::LastFallDistance; // DeadPawn->GetOffsetSlow("LastFallDistance");

			if (LastFallDistanceOffset != -1)
				*(float*)(__int64(DeathInfo) + preoffsets::Distance) = *(float*)(__int64(DeadPawn) + LastFallDistanceOffset);
		}
	}

	std::cout << "aa!\n";

	auto PlayersLeftPtr = Helper::GetPlayersLeft();

	int beforePlayersLeft = 0;

	auto GameState = Helper::GetGameState();

	auto TeamsLeftOffset = preoffsets::TeamsLeft;
	std::cout << "TeamsLeftOffset: " << TeamsLeftOffset << '\n';
	int* TeamsLeft = Get<int>(GameState, TeamsLeftOffset);

	static auto GamePhaseOffset = preoffsets::GamePhase;
	auto GamePhase = *Get<EAthenaGamePhase>(GameState, GamePhaseOffset);

	static auto AlivePlayersOffset = preoffsets::AlivePlayers;
	static auto bMarkedAliveOffset = preoffsets::bMarkedAlive;

	auto AlivePlayers = (TArray<UObject*>*)(__int64(GameMode) + AlivePlayersOffset);

	*Get<bool>(DeadController, bMarkedAliveOffset) = false;

	auto TeamsLeftBefore = *TeamsLeft;

	if (!Defines::bIsPlayground) // && OldPhase > EAthenaGamePhase::Warmup)
	{
		beforePlayersLeft = *PlayersLeftPtr;
		(*PlayersLeftPtr)--;

		static auto OnRep_PlayersLeft = FindObject<UFunction>("/Script/FortniteGame.FortGameStateAthena:OnRep_PlayersLeft");
		GameState->ProcessEvent(OnRep_PlayersLeft);

		auto PlayerState = Helper::GetPlayerStateFromController(DeadController);

		static auto PlayerTeamOffset = PlayerState->GetOffset("PlayerTeam");
		auto PlayerTeam = Get<UObject*>(PlayerState, PlayerTeamOffset);

		if (TeamsLeftOffset != 0)
		{
			static auto TeamMembersOffset = (*PlayerTeam)->GetOffset("TeamMembers");
			auto TeamMembers = Get<TArray<UObject*>>(*PlayerTeam, TeamMembersOffset);

			bool bAllIsDead = true;

			for (int i = 0; i < TeamMembers->Num(); i++)
			{
				auto TeamMember = TeamMembers->At(i);

				auto bAlive = *Get<bool>(TeamMember, bMarkedAliveOffset);
				std::cout << std::format("[{}] bAlive: {}", i, bAlive);

				if (TeamMember != DeadController)
				{
					if (bAlive)
					{
						bAllIsDead = false;
						break;
					}
				}
			}

			std::cout << "bAllIsDead: " << bAllIsDead << '\n';

			if (bAllIsDead)
			{
				(*TeamsLeft)--;
			}
		}
	}

	std::cout << "beforePlayersLeft: " << beforePlayersLeft << '\n';

	auto KillerController = KillerPawn ? Helper::GetControllerFromPawn(KillerPawn) : nullptr;

	static auto PlaceOffset = preoffsets::Place;
	auto DeadPS_Place = Get<int>(DeadPlayerState, PlaceOffset);
	*DeadPS_Place = TeamsLeftBefore;

	if (GamePhase > EAthenaGamePhase::Warmup)
	{
		// if (*TeamsLeft <= 1) // && (int)Playlist->WinCondition <= 1
		if (*PlayersLeftPtr <= 1 && !Defines::bIsPlayground)
		{
			static auto ClientNotifyWon = FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerAthena.ClientNotifyTeamWon");

			UObject* FinishingWeaponDefinition = nullptr;

			static auto DamageCauserOffset = preoffsets::DamageCauser;

			auto DamageCauser = *(UObject**)(__int64(DeathReport) + DamageCauserOffset);

			if (DamageCauser)
			{
				static auto ProjectileClass = FindObject("/Script/FortniteGame.FortProjectileBase");
				static auto FortWeaponClass = FindObject("/Script/FortniteGame.FortWeapon");

				if (DamageCauser->IsA(ProjectileClass))
					FinishingWeaponDefinition = Helper::GetWeaponData(Helper::GetOwner(DamageCauser));
				else if (DamageCauser->IsA(FortWeaponClass))
					FinishingWeaponDefinition = Helper::GetWeaponData(DamageCauser);
			}

			struct { UObject* FinisherPawn; UObject* FinishingWeapon; uint8_t DeathCause; } AFortPlayerControllerAthena_ClientNotifyWon_Params{ KillerPawn, FinishingWeaponDefinition, DeathCause };

			for (int i = 0; i < AlivePlayers->Num(); i++)
			{
				auto AlivePlayer = AlivePlayers->At(i);

				if (!AlivePlayer)
					continue;

				if (AlivePlayer != DeadController)
				{
					AlivePlayer->ProcessEvent(ClientNotifyWon, &AFortPlayerControllerAthena_ClientNotifyWon_Params);

					auto AlivePlayerPS = Helper::GetPlayerStateFromController(AlivePlayer);

					if (!AlivePlayerPS)
						continue;

					auto apPlace = Get<int>(AlivePlayerPS, PlaceOffset);

					*apPlace = 1;
				}
			}

			if (preoffsets::WinningPlayerState != 0)
			{
				*(UObject**)(__int64(GameState) + preoffsets::WinningPlayerState) = KillerPlayerState;
			}

			static auto EndMatch = FindObject<UFunction>("/Script/Engine.GameMode.EndMatch");
			GameMode->ProcessEvent(EndMatch);
		}

	}

	/* static auto TeamScoreOffset = preoffsets::TeamScore; // DeadPlayerState->GetOffsetSlow("TeamScore");

	if (TeamScoreOffset != 0)
	{
		auto TeamScore = Get<int>(DeadPlayerState, TeamScoreOffset);

		*TeamScore = beforePlayersLeft; // IDK

		static auto OnRep_TeamScore = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateAthena.OnRep_TeamScore");
		DeadPlayerState->ProcessEvent(OnRep_TeamScore);
	} */

	static auto TeamScorePlacementOffset = preoffsets::TeamScorePlacement; // DeadPlayerState->GetOffsetSlow("TeamScorePlacement");

	if (TeamScorePlacementOffset != 0)
	{
		auto TeamScorePlacement = Get<int>(DeadPlayerState, TeamScorePlacementOffset);

		*TeamScorePlacement = TeamsLeftBefore; // IDK

		static auto OnRep_TeamScorePlacement = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateAthena.OnRep_TeamScorePlacement");
		DeadPlayerState->ProcessEvent(OnRep_TeamScorePlacement);
	}

	struct FAthenaRewardResult
	{
		int                                                LevelsGained;                                             // 0x0000(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		int                                                BookLevelsGained;                                         // 0x0004(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		int                                                TotalSeasonXpGained;                                      // 0x0008(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		int                                                TotalBookXpGained;                                        // 0x000C(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		int                                                PrePenaltySeasonXpGained;                                 // 0x0010(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		unsigned char                                      UnknownData00[0x4];                                       // 0x0014(0x0004) MISSED OFFSET
		TArray<__int64>       XpMultipliers;                                            // 0x0018(0x0010) (ZeroConstructor, NativeAccessSpecifierPublic)
		TArray<__int64>                   Rewards;                                                  // 0x0028(0x0010) (ZeroConstructor, NativeAccessSpecifierPublic)
		float                                              AntiAddictionMultiplier;                                  // 0x0038(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		unsigned char                                      UnknownData01[0x4];                                       // 0x003C(0x0004) MISSED OFFSET
	};

	// 	void ClientSendEndBattleRoyaleMatchForPlayer(bool bSuccess, const struct FAthenaRewardResult& Result);

	static auto ClientSendEndBattleRoyaleMatchForPlayer = FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerAthena.ClientSendEndBattleRoyaleMatchForPlayer");
	int TotalSeasonXpGained = INT32_MAX; // This is the only one that u can see
	struct { bool bSuccess; FAthenaRewardResult res; } parm{ true, FAthenaRewardResult(1500, 1200, TotalSeasonXpGained, 1400) }; // MatchReport->EndOfMatchResults

	DeadController->ProcessEvent(ClientSendEndBattleRoyaleMatchForPlayer, &parm); // lil xp thingy

	struct FAthenaMatchStats
	{
		FString                                     StatBucket;                                               // 0x0000(0x0010) (ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
		FString                                     MatchID;                                                  // 0x0010(0x0010) (ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
		FString                                     MatchEndTime;                                             // 0x0020(0x0010) (ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
		FString                                     MatchPlatform;                                            // 0x0030(0x0010) (ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
		int                                                Stats[0x14];                                              // 0x0040(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
		TArray<__int64>                  WeaponStats;                                              // 0x0090(0x0010) (ZeroConstructor, NativeAccessSpecifierPrivate)
	};

	FAthenaMatchStats Stats{};
	Stats.Stats[3] = 100; // Elimations I think

	auto teamStats = FAthenaMatchTeamStats();
	teamStats.Place = *DeadPS_Place;
	teamStats.TotalPlayers = PlayersLeftPtr ? (*PlayersLeftPtr) + 1 : 100; // i believe this is supposed to be how many players were at aircraft

	if (false) // 7.40
	{
		static auto MatchReportOffset = DeadController->GetOffset("MatchReport");
		auto MatchReport = Get<UObject*>(DeadController, MatchReportOffset);

		std::cout << "MatchReport: " << *MatchReport << '\n';

		if (*MatchReport) // null yay!!
		{
			static auto MatchStatsOffset = (*MatchReport)->GetOffsetSlow("MatchStats");
			auto MatchStats = *Get<FAthenaMatchStats>(*MatchReport, MatchStatsOffset);

			static auto TeamStatsOffset = (*MatchReport)->GetOffsetSlow("TeamStats");
			auto TeamStats = Get<FAthenaMatchTeamStats>(*MatchReport, TeamStatsOffset);

			static auto bHasTeamStatsOffset = (*MatchReport)->GetOffsetSlow("bHasTeamStats");
			auto bHasTeamStats = Get<bool>(*MatchReport, bHasTeamStatsOffset);

			*bHasTeamStats = true;

			std::cout << "MatchStats.Stats[2]: " << MatchStats.Stats[2] << '\n';

			Stats = MatchStats;
			teamStats = *TeamStats;
		}
	}

	static auto ClientSendMatchStatsForPlayer = FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerAthena.ClientSendMatchStatsForPlayer");
	// DeadController->ProcessEvent(ClientSendMatchStatsForPlayer, &Stats); // For now, because the size of the struct changes and im too lazy to allocate it

	static auto ClientSendTeamStatsForPlayer = FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerAthena.ClientSendTeamStatsForPlayer");
	DeadController->ProcessEvent(ClientSendTeamStatsForPlayer, &teamStats); // "You came x out of y Players"

	// if (DeadPawn->IsKillPending() // IDK
	auto DeathLocation = Helper::GetActorLocation(DeadPawn);

	DeathLocation.Describe();

	if (Fortnite_Version >= 8.30) // && Fortnite_Version != 14.40
	{
		if (false)
		{
			static auto ChipClass = FindObject(("/Game/Athena/Items/EnvironmentalItems/SCMachine/BGA_Athena_SCMachine_Pickup.BGA_Athena_SCMachine_Pickup_C"));

			auto Chip = Helper::Easy::SpawnActor(ChipClass, DeathLocation);

			if (Chip && DeadPlayerState)
			{
				std::cout << "Chip: " << Chip->GetFullName() << '\n';

				static auto PS_SquadIdOffset = FindOffsetStruct("Class /Script/FortniteGame.FortPlayerStateAthena", "SquadId", true);
				auto PS_SquadId = Get<int>(DeadPlayerState, PS_SquadIdOffset);

				static auto UnHideOffset = 0x7E0; // Chip->GetOffset("UnHide");

				if (UnHideOffset != 0)
					*Get<int>(Chip, UnHideOffset) = 0;

				static auto OnRep_UnHide = FindObject<UFunction>("/Game/Athena/Items/EnvironmentalItems/SCMachine/BGA_Athena_SCMachine_Pickup.BGA_Athena_SCMachine_Pickup_C.OnRep_UnHide");

				std::cout << "OnRep_UnHide: " << OnRep_UnHide << '\n';

				struct
				{
				public:
					bool                                         CallFunc_Conv_IntToBool_ReturnValue;               // 0x0(0x1)(ZeroConstructor, IsPlainOldData, NoDestructor)
					// uint8                                        Pad_630C[0x3];                                     // Fixing Size After Last Property  [ Dumper-7 ]
					int32_t                                        Temp_int_Array_Index_Variable;                     // 0x4(0x4)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash)
					float                                        CallFunc_GetGameTimeInSeconds_ReturnValue;         // 0x8(0x4)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash)
					// uint8                                        Pad_630D[0x4];                                     // Fixing Size After Last Property  [ Dumper-7 ]
					TArray<UObject*>         CallFunc_GetLocalFortPlayerControllers_ReturnValue; // 0x10(0x10)(ZeroConstructor, ReferenceParm)
					int32_t                                        Temp_int_Loop_Counter_Variable;                    // 0x20(0x4)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash)
					// uint8                                        Pad_630E[0x4];                                     // Fixing Size After Last Property  [ Dumper-7 ]
					UObject* CallFunc_Array_Get_Item;                           // 0x28(0x8)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash)
					uint8_t                                        CallFunc_GetActorTeam_ReturnValue;                 // 0x30(0x1)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash)
					// uint8                                        Pad_630F[0x3];                                     // Fixing Size After Last Property  [ Dumper-7 ]
					int32_t                                        CallFunc_Add_IntInt_ReturnValue;                   // 0x34(0x4)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash)
					bool                                         CallFunc_EqualEqual_ByteByte_ReturnValue;          // 0x38(0x1)(ZeroConstructor, IsPlainOldData, NoDestructor)
					// uint8                                        Pad_6310[0x3];                                     // Fixing Size After Last Property  [ Dumper-7 ]
					int32_t                                        CallFunc_Array_Length_ReturnValue;                 // 0x3C(0x4)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash)
					bool                                         CallFunc_Less_IntInt_ReturnValue;                  // 0x40(0x1)(ZeroConstructor, IsPlainOldData, NoDestructor)
					bool                                         CallFunc_Conv_IntToBool_ReturnValue_1;             // 0x41(0x1)(ZeroConstructor, IsPlainOldData, NoDestructor)
				} ABGA_Athena_SCMachine_Pickup_C_OnRep_UnHide_Params{};

				Chip->ProcessEvent(OnRep_UnHide, &ABGA_Athena_SCMachine_Pickup_C_OnRep_UnHide_Params);

				static UFunction* SetActorHiddenInGame = FindObject<UFunction>("/Script/Engine.Actor.SetActorHiddenInGame");
				bool bNewHidden = false;
				Chip->ProcessEvent(SetActorHiddenInGame, &bNewHidden);

				static auto SquadIdOffset = FindOffsetStruct("Class /Script/FortniteGame.BuildingGameplayActorSpawnChip", "SquadId", true);
				*Get<uint8_t>(Chip, SquadIdOffset) = *PS_SquadId;

				static auto OwnerPlayerControllerOffset = FindOffsetStruct("Class /Script/FortniteGame.BuildingGameplayActorSpawnChip", "OwnerPlayerController", true);
				*Get<UObject*>(Chip, OwnerPlayerControllerOffset) = DeadController;

				static auto OwnerPlayerIdOffset = FindOffsetStruct("Class /Script/FortniteGame.BuildingGameplayActorSpawnChip", "OwnerPlayerId", true);
				static auto UniqueIdOffset = DeadPlayerState->GetOffset("UniqueId");
				*Get<FUniqueNetIdRepl>(Chip, OwnerPlayerIdOffset) = *Get<FUniqueNetIdRepl>(DeadPlayerState, UniqueIdOffset);

				struct FRebootCardReplicatedState
				{
				public:
					float                                        ChipExpirationServerStartTime;                     // 0x0(0x4)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					uint8_t                                        Pad_269F[0x4];                                     // Fixing Size After Last Property  [ Dumper-7 ]
					UObject* PlayerState;                                       // 0x8(0x8)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				};

				struct FFortResurrectionData
				{
				public:
					bool                                         bResurrectionChipAvailable;                        // 0x0(0x1)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					uint8_t                                        Pad_2933[0x3];                                     // Fixing Size After Last Property  [ Dumper-7 ]
					float                                        ResurrectionExpirationTime;                        // 0x4(0x4)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					float                                        ResurrectionExpirationLength;                      // 0x8(0x4)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					FVector                               WorldLocation;                                     // 0xC(0xC)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				};

				static auto bAutoAcquireSpawnChipOffset = Chip->GetOffset("bAutoAcquireSpawnChip");
				*Get<bool>(Chip, bAutoAcquireSpawnChipOffset) = false;

				static auto IsPendingKillOffset = Chip->GetOffset("IsPendingKill");
				*Get<bool>(Chip, IsPendingKillOffset) = false;

				static auto OwnerPlayerStateOffset = FindOffsetStruct("Class /Script/FortniteGame.BuildingGameplayActorSpawnChip", "OwnerPlayerState", true);
				*Get<UObject*>(Chip, OwnerPlayerStateOffset) = DeadPlayerState;

				static auto OwnerTeamOffset = Chip->GetOffset("OwnerTeam");
				*Get<uint8_t>(Chip, OwnerTeamOffset) = *Helper::GetTeamIndex(DeadPlayerState);

				static auto ChipExpirationLengthOffset = Chip->GetOffset("ChipExpirationLength");
				Get<FScalableFloat>(Chip, ChipExpirationLengthOffset)->Value = 100;

				static auto Row_PickupLifeOffset = Chip->GetOffset("Row_PickupLife");
				Get<FScalableFloat>(Chip, Row_PickupLifeOffset)->Value = 100;

				static auto SpawnSoundPlayedOffset = Chip->GetOffset("SpawnSoundPlayed");
				*Get<bool>(Chip, SpawnSoundPlayedOffset) = true;

				static auto ResurrectionChipAvailableOffset = DeadPlayerState->GetOffset("ResurrectionChipAvailable");
				auto ResurrectionChipAvailable = Get<FFortResurrectionData>(DeadPlayerState, ResurrectionChipAvailableOffset);
				ResurrectionChipAvailable->WorldLocation = DeathLocation;
				ResurrectionChipAvailable->bResurrectionChipAvailable = true;
				ResurrectionChipAvailable->ResurrectionExpirationTime = Helper::GetTimeSeconds() + 70;
				ResurrectionChipAvailable->ResurrectionExpirationLength = 70;

				static auto RebootCardReplicatedStateOffset = Chip->GetOffset("RebootCardReplicatedState");
				Get<FRebootCardReplicatedState>(Chip, RebootCardReplicatedStateOffset)->PlayerState = DeadPlayerState;
				Get<FRebootCardReplicatedState>(Chip, RebootCardReplicatedStateOffset)->ChipExpirationServerStartTime = Helper::GetTimeSeconds();

				static auto OnRep_RebootCardReplicatedState = FindObject<UFunction>("/Script/FortniteGame.BuildingGameplayActorSpawnChip.OnRep_RebootCardReplicatedState");
				Chip->ProcessEvent(OnRep_RebootCardReplicatedState);

				static auto SetReplicateMovementFn = FindObject<UFunction>("/Script/Engine.Actor.SetReplicateMovement");
				bool bTrue = true;
				Chip->ProcessEvent(SetReplicateMovementFn, &bTrue);

				Helper::InitializeBuildingActor(DeadController, Chip);

				*Get<bool>(Chip, IsPendingKillOffset) = false;
			}
		}
	}

	if (!Defines::bIsPlayground)
	{
		auto ItemInstances = Inventory::GetItemInstances(DeadController);

		if (!IsBadReadPtr(ItemInstances))
		{
			auto NumItemInstances = ItemInstances->Num();

			for (int i = 6; i < NumItemInstances; i++) // dont ask
			{
				std::cout << std::format("{}/{}\n", i, NumItemInstances);

				auto ItemInstance = ItemInstances->At(i);

				if (IsBadReadPtr(ItemInstance))
					continue;

				// bDropOnDeath

				auto ItemEntry = UFortItem::GetItemEntry(ItemInstance);

				if (IsBadReadPtr(ItemEntry))
					continue;

				auto ItemGuidPtr = UFortItem::GetGuid(ItemInstance);
				auto CountPtr = UFortItem::GetCount(ItemInstance);
				auto ItemDefinitionPtr = UFortItem::GetDefinition(ItemInstance);
				auto LoadedAmmoPtr = FFortItemEntry::GetLoadedAmmo(UFortItem::GetItemEntry(ItemInstance));

				if (IsBadReadPtr(ItemGuidPtr) || IsBadReadPtr(CountPtr) || IsBadReadPtr(ItemDefinitionPtr) || IsBadReadPtr(LoadedAmmoPtr))
					continue;

				auto ItemGuid = *ItemGuidPtr;
				auto Count = *CountPtr;
				auto ItemDefinition = *ItemDefinitionPtr;
				auto LoadedAmmo = *LoadedAmmoPtr;

				if (IsBadReadPtr(ItemDefinition))
					continue;

				struct { FGuid ItemGuid; int Count; } drop_parms{ ItemGuid, Count };
				// Inventory::ServerAttemptInventoryDrop(DeadController, nullptr, &drop_parms);

				Inventory::TakeItem(DeadController, ItemGuid, Count, true);
				Helper::SummonPickup(DeadPawn, ItemDefinition, DeathLocation, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, Count, false, LoadedAmmo);
			}
		}
	}
	else
	{
		// if (Fortnite_Version > 11.30)
		{
			auto RespawnedPawn = Helper::SpawnPawn(DeadController, DeathLocation);

			static auto TeleportToSkyDive = FindObject<UFunction>("/Script/FortniteGame.FortPlayerPawnAthena.TeleportToSkyDive");
			float HeightAboveGround = 15000;

			if (TeleportToSkyDive)
				RespawnedPawn->ProcessEvent(TeleportToSkyDive, &HeightAboveGround);
		}
	}

	if (KillerPawn && KillerPlayerState != DeadPlayerState)
	{
		static auto KillScoreOffset = preoffsets::KillScore; // KillerPlayerState->GetOffset("KillScore");
		(*Get<int>(KillerPlayerState, KillScoreOffset))++;

		if (KillerController)
		{
			static auto ClientReceiveKillNotification = FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerPvP.ClientReceiveKillNotification") ?
				FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerPvP.ClientReceiveKillNotification") : FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerAthena.ClientReceiveKillNotification");

			std::cout << "ClientReceiveKillNotification: " << ClientReceiveKillNotification << '\n';

			struct {
				// Both playerstates
				UObject* Killer;
				UObject* Killed;
			} ClientReceiveKillNotification_Params{ KillerPlayerState, DeadPlayerState };

			KillerController->ProcessEvent(ClientReceiveKillNotification, &ClientReceiveKillNotification_Params);
		}

		std::cout << "areu su!\n";

		static auto ClientReportKill = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateAthena.ClientReportKill");
		KillerPlayerState->ProcessEvent(ClientReportKill, &DeadPlayerState);
	}

	static auto OnRep_DeathInfo = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateAthena.OnRep_DeathInfo");
	DeadPlayerState->ProcessEvent(OnRep_DeathInfo);

	return true;
}

bool ServerAttemptAircraftJump(UObject* Controller, UFunction*, void* Parameters)
{
	if (!Parameters)
		return false;

	auto o = Controller;

	if (Engine_Version >= 424)
		Controller = Helper::GetOwnerOfComponent(Controller); // CurrentAircraft

	BothRotator Rotation = Fortnite_Season >= 20 ? BothRotator(*(DRotator*)Parameters) : BothRotator(*(FRotator*)Parameters);

	UObject* Aircraft = nullptr;

	if (Engine_Version >= 424)
	{
		static auto CurrentAircraftOffset = o->GetOffset("CurrentAircraft");

		if (CurrentAircraftOffset != 0)
		{
			Aircraft = *Get<UObject*>(o, CurrentAircraftOffset);
		}
	}

	if (!Aircraft)
	{
		auto GameState = Helper::GetGameState();

		static auto AircraftsOffset = GameState->GetOffset("Aircrafts", false, false, false);

		if (AircraftsOffset != 0)
		{
			auto Aircrafts = (TArray<UObject*>*)(__int64(GameState) + AircraftsOffset);

			if (IsBadReadPtr(Aircrafts))
				return false;

			Aircraft = Aircrafts->At(0);
		}
		else
		{
			static auto AircraftOffset = GameState->GetOffset("Aircraft");
			Aircraft = *Get<UObject*>(GameState, AircraftOffset);
		}
	}

	if (IsBadReadPtr(Aircraft))
		return false;

	BothVector ExitLocation = Helper::GetActorLocationDynamic(Aircraft);

	if (Defines::bWipeInventoryOnAircraft)
		Inventory::WipeInventory(Controller, false);

	auto Pawn = Helper::SpawnPawn(Controller, ExitLocation, false);

	if (Defines::bIsLateGame)
	{	
		if (!Defines::bWipeInventoryOnAircraft)
			Inventory::WipeInventory(Controller, false);

		if (Pawn)
			Helper::SetShield(Pawn, 100);

		std::cout << "A!\n";

		static UObject* AthenaAmmoDataRockets = FindObject(("/Game/Athena/Items/Ammo/AmmoDataRockets.AmmoDataRockets"));
		static UObject* AthenaAmmoDataShells = FindObject(("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells"));
		static UObject* AthenaAmmoDataBulletsMedium = FindObject(("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium"));
		static UObject* AthenaAmmoDataBulletsLight = FindObject(("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight"));
		static UObject* AthenaAmmoDataBulletsHeavy = FindObject(("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy"));

		if (!AthenaAmmoDataRockets || !AthenaAmmoDataShells || !AthenaAmmoDataBulletsMedium || !AthenaAmmoDataBulletsLight || !AthenaAmmoDataBulletsHeavy)
			std::cout << "Some ammo is invalid!\n";

		Inventory::GiveItem(Controller, AthenaAmmoDataRockets, EFortQuickBars::Secondary, 0, 15);
		Inventory::GiveItem(Controller, AthenaAmmoDataShells, EFortQuickBars::Secondary, 0, 50);
		Inventory::GiveItem(Controller, AthenaAmmoDataBulletsMedium, EFortQuickBars::Secondary, 0, 350);
		Inventory::GiveItem(Controller, AthenaAmmoDataBulletsLight, EFortQuickBars::Secondary, 0, 300);
		Inventory::GiveItem(Controller, AthenaAmmoDataBulletsHeavy, EFortQuickBars::Secondary, 0, 50);

		static auto WoodItemData = FindObject(("/Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
		static auto StoneItemData = FindObject(("/Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
		static auto MetalItemData = FindObject(("/Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

		Inventory::GiveItem(Controller, WoodItemData, EFortQuickBars::Secondary, 0, 500);
		Inventory::GiveItem(Controller, StoneItemData, EFortQuickBars::Secondary, 0, 500);
		Inventory::GiveItem(Controller, MetalItemData, EFortQuickBars::Secondary, 0, 500);

		std::cout << "C!\n";

		auto AR = Looting::GetRandomItem(ItemType::Weapon);

		while (!AR.Definition || (!AR.Definition->GetFullName().contains("Assault") && !AR.Definition->GetFullName().contains("LMG")))
		{
			AR = Looting::GetRandomItem(ItemType::Weapon);
		}

		std::cout << "B!\n";

		auto Shotgun = Looting::GetRandomItem(ItemType::Weapon);

		while (!Shotgun.Definition || !Shotgun.Definition->GetFullName().contains("Shotgun"))
		{
			Shotgun = Looting::GetRandomItem(ItemType::Weapon);
		}

		std::cout << "D!\n";

		Inventory::GiveItem(Controller, AR.Definition, EFortQuickBars::Primary, 1, 1, true, Helper::GetMaxBullets(AR.Definition));
		Inventory::GiveItem(Controller, Shotgun.Definition, EFortQuickBars::Primary, 2, 1, true, Helper::GetMaxBullets(Shotgun.Definition));

		std::cout << "CC!\n";

		std::random_device rd; // obtain a random number from hardware
		std::mt19937 gen(rd()); // seed the generator

		std::uniform_int_distribution<> distr(0, 10);

		int slotForFirstConsumable = 3;
		int slotForSecondConsumable = 4;
		int slotForThirdConsumable = 5;

		if (Fortnite_Version < 9)
		{
			if (distr(gen) > 6) // 2 heals
			{
				if (distr(gen) >= 4) // 40/60 sniper or smg
				{
					auto SMG = Looting::GetRandomItem(ItemType::Weapon);

					while (!SMG.Definition || IsBadReadPtr(SMG.Definition) || !SMG.Definition->GetFullName().contains("PDW")) // bad
					{
						SMG = Looting::GetRandomItem(ItemType::Weapon);
					}

					Inventory::GiveItem(Controller, SMG.Definition, EFortQuickBars::Primary, 3, 1, true, Helper::GetMaxBullets(SMG.Definition));
				}
				else
				{
					auto Sniper = Looting::GetRandomItem(ItemType::Weapon);

					while (!Sniper.Definition || IsBadReadPtr(Sniper.Definition) || !Sniper.Definition->GetFullName().contains("Sniper"))
					{
						Sniper = Looting::GetRandomItem(ItemType::Weapon);
					}

					Inventory::GiveItem(Controller, Sniper.Definition, EFortQuickBars::Primary, 3, 1, true, Helper::GetMaxBullets(Sniper.Definition));
				}

				std::cout << "E!\n";

				slotForFirstConsumable = 4;
				slotForSecondConsumable = 5;
				slotForThirdConsumable = -1;
			}
			else // 1 heal
			{
				{
					auto SMG = Looting::GetRandomItem(ItemType::Weapon);

					while (!SMG.Definition || IsBadReadPtr(SMG.Definition) || !SMG.Definition->GetFullName().contains("PDW")) // bad
					{
						SMG = Looting::GetRandomItem(ItemType::Weapon);
					}

					Inventory::GiveItem(Controller, SMG.Definition, EFortQuickBars::Primary, 3, 1, true, Helper::GetMaxBullets(SMG.Definition));
				}

				{
					auto Sniper = Looting::GetRandomItem(ItemType::Weapon);

					while (!Sniper.Definition || IsBadReadPtr(Sniper.Definition) || !Sniper.Definition->GetFullName().contains("Sniper"))
					{
						Sniper = Looting::GetRandomItem(ItemType::Weapon);
					}

					Inventory::GiveItem(Controller, Sniper.Definition, EFortQuickBars::Primary, 4, 1, true, Helper::GetMaxBullets(Sniper.Definition));
				}

				std::cout << "G!\n";

				slotForFirstConsumable = 5;
				slotForSecondConsumable = -1;
				slotForThirdConsumable = -1;
			}
		}

		if (slotForFirstConsumable != -1)
		{
			auto Consumable1 = Looting::GetRandomItem(ItemType::Consumable);

			/* while (!Consumable1.Definition || !Consumable1.Definition->GetFullName().contains("Shield") || !Consumable1.Definition->GetFullName().contains("Med"))
			{
				Consumable1 = Looting::GetRandomItem(ItemType::Consumable);
			} */

			Inventory::GiveItem(Controller, Consumable1.Definition, EFortQuickBars::Primary, slotForFirstConsumable, Consumable1.DropCount);
		}

		if (slotForSecondConsumable != -1)
		{
			auto Consumable2 = Looting::GetRandomItem(ItemType::Consumable);

			/* while (!Consumable2.Definition || !Consumable2.Definition->GetFullName().contains("Shield") || !Consumable2.Definition->GetFullName().contains("Med"))
			{
				Consumable2 = Looting::GetRandomItem(ItemType::Consumable);
			} */

			Inventory::GiveItem(Controller, Consumable2.Definition, EFortQuickBars::Primary, slotForSecondConsumable, Consumable2.DropCount);
		}

		if (slotForThirdConsumable != -1)
		{
			auto Consumable3 = Looting::GetRandomItem(ItemType::Consumable);

			/* while (!Consumable3.Definition || !Consumable3.Definition->GetFullName().contains("Shield") || !Consumable3.Definition->GetFullName().contains("Med"))
			{
				Consumable3 = Looting::GetRandomItem(ItemType::Consumable);
			} */

			Inventory::GiveItem(Controller, Consumable3.Definition, EFortQuickBars::Primary, slotForThirdConsumable, Consumable3.DropCount);
		}
	}

	// ASC->RemoveActiveGameplayEffectBySourceEffect(SlurpEffect);

	return false;
}

bool commitExecuteWeapon(UObject* Ability, UFunction*, void* Parameters)
{
	// std::cout << "execute\n";

	if (Ability)
	{
		UObject* Pawn; // Helper::GetOwner(ability);
		static auto Func = FindObject<UFunction>("/Script/FortniteGame.FortGameplayAbility.GetActivatingPawn");
		Ability->ProcessEvent(Func, &Pawn);

		if (Pawn)
		{
			// std::cout << "pawn: " << Pawn->GetFullName() << '\n';
			auto currentWeapon = Helper::GetCurrentWeapon(Pawn);

			if (currentWeapon)
			{
				auto Controller = Helper::GetControllerFromPawn(Pawn);
				auto entry = Inventory::GetEntryFromWeapon(Controller, currentWeapon);

				auto loadedammo = FFortItemEntry::GetLoadedAmmo(entry);

				static auto AmmoCountOffset = currentWeapon->GetOffset("AmmoCount");
				FFortItemEntry::SetLoadedAmmo(entry, Controller, *Get<int>(currentWeapon, AmmoCountOffset));
			}
			else
				std::cout << "No CurrentWeapon!\n";
		}
	}

	return false;
}


bool OnGamePhaseChanged(UObject* MatchAnaylitics, UFunction*, void* Parameters)
{
	auto Phase = *(EAthenaGamePhase*)Parameters;

	std::cout << "Phase: " << (int)Phase << '\n';

	/*
	if ((int)Phase == 3 && Defines::bIsLateGame)
	{
		std::cout << "Nice!\n";

		auto GameState = Helper::GetGameState();

		static auto AircraftsOffset = GameState->GetOffset("Aircrafts");
		auto Aircrafts = (TArray<UObject*>*)(__int64(GameState) + AircraftsOffset);

		auto Aircraft = Aircrafts->At(0);

		if (!Aircraft)
		{
			std::cout << "No aircraft!\n";
			return false;
		}

		FString StartSafeZone = L"startsafezone";
		Helper::ExecuteConsoleCommand(StartSafeZone);

		static auto SafeZonesStartTimeOffset = GameState->GetOffset("SafeZonesStartTime");
		*Get<float>(GameState, SafeZonesStartTimeOffset) = 0.f;
	}
	*/

	return false;
}

bool ServerUpdatePhysicsParams(UObject* Vehicle, UFunction* Function, void* Parameters) // FortAthenaVehicle
{
	if (Vehicle && Parameters)
	{
		struct parms { __int64 InState; };
		auto Params = (parms*)Parameters;

		std::string StateName = FindObject("/Script/FortniteGame.ReplicatedPhysicsPawnState") ? "ScriptStruct /Script/FortniteGame.ReplicatedPhysicsPawnState" :
			"ScriptStruct /Script/FortniteGame.ReplicatedAthenaVehiclePhysicsState";

		static auto TranslationOffset = FindOffsetStruct(StateName, ("Translation"));
		auto Translation = (FVector*)(__int64(&Params->InState) + TranslationOffset);

		static auto RotationOffset = FindOffsetStruct(StateName, ("Rotation"));
		auto Rotation = (FQuat*)(__int64(&Params->InState) + RotationOffset);

		static auto LinearVelocityOffset = FindOffsetStruct(StateName, ("LinearVelocity"));
		auto LinearVelocity = (FVector*)(__int64(&Params->InState) + LinearVelocityOffset);

		static auto AngularVelocityOffset = FindOffsetStruct(StateName, ("AngularVelocity"));
		auto AngularVelocity = (FVector*)(__int64(&Params->InState) + AngularVelocityOffset);

		if (Translation && Rotation)
		{
			UObject* RootComp = nullptr;
			static auto GetRootCompFunc = FindObject<UFunction>("/Script/Engine.Actor.K2_GetRootComponent");

			if (GetRootCompFunc)
				Vehicle->ProcessEvent(GetRootCompFunc, &RootComp);

			if (RootComp)
			{
				static auto SetWorldTransform = FindObject<UFunction>("/Script/Engine.SceneComponent.K2_SetWorldTransform");
				static auto SetRelativeLocation = FindObject<UFunction>("/Script/Engine.SceneComponent.K2_SetRelativeLocation");

				static auto SizeOfSetWorldTransform = Helper::GetSizeOfClass(SetWorldTransform);
				static auto SizeOfSetRelativeLocation = Helper::GetSizeOfClass(SetRelativeLocation);

				// std::cout << "SizeOfSetWorldTransform: " << SizeOfSetWorldTransform << '\n';

				auto SetWorldTransform_Params = Alloc(SizeOfSetWorldTransform);
				auto SetRelativeLocation_Params = Alloc(SizeOfSetRelativeLocation);

				if (SetWorldTransform_Params && SetRelativeLocation_Params)
				{
					static auto NewTransformOffset = FindOffsetStruct2("/Script/Engine.SceneComponent.K2_SetWorldTransform", "NewTransform", false, true);
					auto NewTransform = (FTransform*)(__int64(SetWorldTransform_Params) + NewTransformOffset);

					static auto NewLocationOffset = FindOffsetStruct2("/Script/Engine.SceneComponent.K2_SetRelativeLocation", "NewLocation", false, true);
					*(FVector*)(__int64(SetRelativeLocation_Params) + NewLocationOffset) = *Translation;

					auto rot = Rotation->Rotator();
					rot.Pitch = Helper::GetActorRotation(Vehicle).Pitch;

					auto Quaternion = rot.Quaternion();

					// std::cout << "Quat: ";
					// Quaternion.Describe();

					NewTransform->Translation = *Translation;
					NewTransform->Rotation = Quaternion; // *Rotation;
					NewTransform->Scale3D = { 1, 1, 1 };

					RootComp->ProcessEvent(SetRelativeLocation, SetRelativeLocation_Params);
					RootComp->ProcessEvent(SetWorldTransform, SetWorldTransform_Params);

					static auto bComponentToWorldUpdatedOffset = RootComp->GetOffset("bComponentToWorldUpdated");
					static auto bComponentToWorldUpdatedFieldMask = GetFieldMask(RootComp->GetProperty("bComponentToWorldUpdated"));

					SetBitfield(Get<PlaceholderBitfield>(RootComp, bComponentToWorldUpdatedOffset), bComponentToWorldUpdatedFieldMask, true);

					static auto SetPhysicsLinearVelocity = FindObject<UFunction>("/Script/Engine.PrimitiveComponent.SetPhysicsLinearVelocity");

					struct {
						FVector                                     NewVel;                                                   // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
						bool                                        bAddToCurrent;                                            // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
						FName                                       BoneName;
					} SetPhysicsLinearVelocity_Params{ *LinearVelocity, false, FName() };

					RootComp->ProcessEvent(SetPhysicsLinearVelocity, &SetPhysicsLinearVelocity);

					static auto SetPhysicsAngularVelocity = FindObject<UFunction>("/Script/Engine.PrimitiveComponent.SetPhysicsAngularVelocity");

					struct {
						FVector                                     NewAngVel;                                                // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
						bool                                               bAddToCurrent;                                            // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
						FName                                       BoneName;
					} SetPhysicsAngularVelocity_Params{ *AngularVelocity, false, FName() };

					RootComp->ProcessEvent(SetPhysicsAngularVelocity, &SetPhysicsAngularVelocity_Params);
				}
			}

			// Vehicle->ProcessEvent("OnRep_ServerCorrection");
		}
	}

	return false;
}

bool ServerGiveCreativeItem(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (!Parameters || !Defines::bIsCreative) // We should do more checks than this..
		return false;

	auto CreativeItemEntry = (__int64*)Parameters;

	auto Def = *FFortItemEntry::GetItemDefinition(CreativeItemEntry);

	Inventory::GiveItem(Controller, Def, Inventory::WhatQuickBars(Def), 1);

	return false;
}

bool ServerLoadingScreenDropped(UObject* Controller, UFunction* Function, void* Parameters)
{
	// skunked

	/* auto Pawn = Helper::GetPawnFromController(Controller);

	static auto PawnClass = FindObject("/Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C");

	if (Pawn && Pawn->IsA(PawnClass))
	{
		if (Fortnite_Version < 8.30)
		{
			static auto AbilitySet = FindObject(("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer"));
			GiveFortAbilitySet(Pawn, AbilitySet);
		}
		else
		{
			static auto AbilitySet = FindObject(("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_AthenaPlayer.GAS_AthenaPlayer"));
			GiveFortAbilitySet(Pawn, AbilitySet);
		}
	}

	*/

	/*
	auto TeamIDX = Helper::GetTeamIndex(Helper::GetPlayerStateFromController(Controller));

	std::cout << "TeamIDX: " << *TeamIDX << '\n';

	static auto SquadIdOffset = FindOffsetStruct("Class /Script/FortniteGame.FortPlayerStateAthena", "SquadId", true);

	auto PlayerState = Helper::GetPlayerStateFromController(Controller);

	auto TeamIndexPtr = Helper::GetTeamIndex(Helper::GetPlayerStateFromController(Controller));
	auto SquadIdPtr = Get<int>(PlayerState, SquadIdOffset);

	std::cout << "SquadIdPtr: " << *SquadIdPtr << '\n';

	// if (*TeamIDX <= 3)

	Teams::AssignTeam(Controller);

	*/

	static bool bbbb = false;

	if (!bbbb)
	{
		bbbb = true;

		if (Defines::bIsLateGame)
		{
			auto GameState = Helper::GetGameState();

			static auto MapInfoOffset = GameState->GetOffsetSlow("MapInfo");
			auto MapInfo = *Get<UObject*>(GameState, MapInfoOffset);

			static auto FlightInfosOffset = MapInfo->GetOffset("FlightInfos");
			auto FlightInfos = Get<TArray<__int64>>(MapInfo, FlightInfosOffset);

			auto FlightInfo = FlightInfos->AtPtr(0);

			static auto BuildingFoundationClass = FindObject("/Script/FortniteGame.BuildingFoundation");

			auto AllBuildingFoundations = Helper::GetAllActorsOfClass(BuildingFoundationClass);

			UObject* Foundation = nullptr;

			while (!Foundation)
			{
				auto random = rand();

				if (random >= 1)
					Foundation = AllBuildingFoundations.At(random % (AllBuildingFoundations.Num()));
			}

			static auto FlightStartLocationOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.AircraftFlightInfo", "FlightStartLocation");
			*(FVector*)(__int64(FlightInfo) + FlightStartLocationOffset) = Helper::GetActorLocation(Foundation) + FVector{ 0, 0, 10000 };

			static auto FlightSpeedOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.AircraftFlightInfo", "FlightSpeed");
			*(float*)(__int64(FlightInfo) + FlightSpeedOffset) = 0;
		}
	}

	return false;
}

bool OnGatherOrInteract(UObject* CBGAParent, UFunction* Function, void* Parameters)
{
	if (!Parameters)
		return false;

	// CBGAParent->Gather

	auto InteractingPawn = *(UObject**)Parameters;

	if (!InteractingPawn)
		return false;

	static auto GatheredLootTierOffset = CBGAParent->GetOffset("GatheredLootTier");
	auto GatheredLootTier = *Get<FName>(CBGAParent, GatheredLootTierOffset);

	/* static auto Gather = FindObject<UFunction>("/Game/Athena/BuildingActors/ConsumableBGAs/CBGA_Parent.CBGA_Parent_C.Gather");
	UObject* Controller = Helper::GetControllerFromPawn(InteractingPawn);

	CBGAParent->ProcessEvent(Gather, &Controller); */

	return false;
}

bool ServerCheat(UObject* PlayerController, UFunction* Function, void* Parameters)
{
#ifdef MILXNOR_H
	return ServerCheatMilxnor(PlayerController, Function, Parameters);
#else
	return true;
#endif
}

struct FFortRespawnData
{
	bool                                               bRespawnDataAvailable;                                    // 0x0000(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                               bClientIsReady;                                           // 0x0001(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                               bServerIsReady;                                           // 0x0002(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	unsigned char                                      UnknownData00[0x1];                                       // 0x0003(0x0001) MISSED OFFSET
	struct FVector                                     RespawnLocation;                                          // 0x0004(0x000C) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FRotator                                    RespawnRotation;                                          // 0x0010(0x000C) (ZeroConstructor, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
	float                                              RespawnCameraDistance;                                    // 0x001C(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

bool ServerClientIsReadyToRespawn(UObject* Controller, UFunction*, void* Parameters)
{
	auto Pawn = Helper::GetPawnFromController(Controller);

	if (Pawn || (!Defines::bIsPlayground && !Defines::bIsCreative))
		return false;

	std::cout << "ready!\n";

	auto PlayerState = Helper::GetPlayerStateFromController(Controller);

	static auto RespawnDataOffset = PlayerState->GetOffset("RespawnData");
	auto RespawnData = Get<FFortRespawnData>(PlayerState, RespawnDataOffset);

	std::cout << "bServerIsReady: " << RespawnData->bServerIsReady << '\n';

	// if (!RespawnData->bServerIsReady) // UNTESTED
		// return false;

	RespawnData->bClientIsReady = true;

	Helper::SpawnPawn(Controller, RespawnData->RespawnLocation);

	return false;
}

bool ServerPlayEmoteItem(UObject* Controller, UFunction*, void* Parameters)
{
	if (!Parameters)
		return false;

	struct SPEIParams { UObject* EmoteAsset; }; // UFortMontageItemDefinitionBase
	auto EmoteParams = (SPEIParams*)Parameters;

	auto EmoteAsset = EmoteParams->EmoteAsset;

	if (!EmoteAsset)
		return false;

	// std::cout << "EmoteAsset: " << EmoteAsset->GetFullName() << '\n';

	auto Pawn = Helper::GetPawnFromController(Controller);

	if (!Pawn)
		return false;

	auto ASC = Helper::GetAbilitySystemComponent(Pawn);

	/* static auto AnimationOffset = EmoteAsset->GetOffset("Animation");
	static auto AnimMontageClass = FindObject("/Script/Engine.AnimMontage");
	auto AnimationSoft = Get<TSoftObjectPtr>(EmoteAsset, AnimationOffset);
	auto Animation = AnimationSoft->Get(AnimMontageClass); */

	if (!GiveAbilityAndActivateOnceAddress)
	{
		std::cout << "No GiveAbilityAndActivateOnceAddress!\n";
		return false;
	}

	UObject* AbilityToUse = nullptr;

	static auto AthenaSprayItemDefinitionClass = FindObject("/Script/FortniteGame.AthenaSprayItemDefinition");
	static auto AthenaToyItemDefinitionClass = FindObject("/Script/FortniteGame.AthenaToyItemDefinition");

	if (EmoteAsset->IsA(AthenaSprayItemDefinitionClass))
	{
		static auto SprayGameplayAbilityDefault = FindObject("/Game/Abilities/Sprays/GAB_Spray_Generic.Default__GAB_Spray_Generic_C");
		AbilityToUse = SprayGameplayAbilityDefault;
	}
	else
	{
		static auto EmoteGameplayAbilityDefault = FindObject("/Game/Abilities/Emotes/GAB_Emote_Generic.Default__GAB_Emote_Generic_C");
		AbilityToUse = EmoteGameplayAbilityDefault;
	}

	if (EmoteAsset->IsA(AthenaToyItemDefinitionClass))
	{
		static auto ToySpawnAbilityOffset = EmoteAsset->GetOffset("ToySpawnAbility");
		auto ToySpawnAbility = Get<TSoftObjectPtr>(EmoteAsset, ToySpawnAbilityOffset);

		AbilityToUse = GetDefaultObject(ToySpawnAbility->Get(Helper::GetBGAClass()));
	}

	if (!AbilityToUse)
		return false;

	// std::cout << "AbilityToUse: " << AbilityToUse->GetFullName() << '\n';

	/* auto newSpec = Abilities::GenerateNewSpec(AbilityToUse, EmoteAsset);

	unsigned int* (*GiveAbilityAndActivateOnce)(UObject* ASC, int* outHandle, PadHexE8 Spec) = decltype(GiveAbilityAndActivateOnce)(GiveAbilityAndActivateOnceAddress);
	unsigned int* (*GiveAbilityAndActivateOnceNew)(UObject* ASC, int* outHandle, PadHexE8 Spec, __int64* GameplayEventData) = decltype(GiveAbilityAndActivateOnceNew)(GiveAbilityAndActivateOnceAddress);

	int outHandle = 0;

	if (Engine_Version < 500)
		GiveAbilityAndActivateOnce(ASC, &outHandle, *(PadHexE8*)newSpec);
	else
		GiveAbilityAndActivateOnceNew(ASC, &outHandle, *(PadHexE8*)newSpec, nullptr); */


	static auto Emote_bMovingEmoteOffset = EmoteAsset->GetOffset("bMovingEmote");
	static auto Emote_bMovingEmoteFieldMask = GetFieldMask(EmoteAsset->GetProperty("bMovingEmote"));

	bool Emote_bMovingEmote = ReadBitfield(Get<PlaceholderBitfield>(EmoteAsset, Emote_bMovingEmoteOffset), Emote_bMovingEmoteFieldMask);

	static auto Emote_bMoveForwardOnlyOffset = EmoteAsset->GetOffset("bMoveForwardOnly", false, false, false);
	static auto Emote_bMoveForwardOnlyFieldMask = Emote_bMoveForwardOnlyOffset ? GetFieldMask(EmoteAsset->GetProperty("bMoveForwardOnly")) : 0;

	bool Emote_bMoveForwardOnly = Emote_bMoveForwardOnlyOffset ? ReadBitfield(Get<PlaceholderBitfield>(EmoteAsset, Emote_bMoveForwardOnlyOffset), Emote_bMoveForwardOnlyFieldMask) : true;

	// std::cout << "Emote_bMovingEmote: " << Emote_bMovingEmote << '\n';

	static auto Pawn_bMovingEmoteOffset = Pawn->GetOffset("bMovingEmote");
	static auto Pawn_bMovingEmoteFieldMask = GetFieldMask(Pawn->GetProperty("bMovingEmote"));
	PlaceholderBitfield* Pawn_bMovingEmote = Get<PlaceholderBitfield>(Pawn, Pawn_bMovingEmoteOffset);

	static auto Pawn_bMovingEmoteForwardOnlyOffset = Pawn->GetOffset("bMovingEmoteForwardOnly");

	if (Pawn_bMovingEmoteForwardOnlyOffset)
	{
		PlaceholderBitfield* Pawn_bMovingEmoteForwardOnly = Get<PlaceholderBitfield>(Pawn, Pawn_bMovingEmoteForwardOnlyOffset);
		static auto Pawn_bMovingEmoteForwardOnlyFieldMask = GetFieldMask(Pawn->GetProperty("bMovingEmoteForwardOnly"));

		SetBitfield(Pawn_bMovingEmoteForwardOnly, Pawn_bMovingEmoteForwardOnlyFieldMask, Emote_bMoveForwardOnly);
	}

	// std::cout << "Pawn_bMovingEmoteForwardOnlyFieldMask: " << (int)Pawn_bMovingEmoteFieldMask << '\n';
	// std::cout << "Emote_bMoveForwardOnly: " << Emote_bMoveForwardOnly << '\n';
	// std::cout << "Emote_bMovingEmote: " << Emote_bMovingEmote << '\n';

	SetBitfield(Pawn_bMovingEmote, Pawn_bMovingEmoteFieldMask, Emote_bMovingEmote);

	if (Emote_bMovingEmote)
	{
		static auto WalkForwardSpeedOffset = EmoteAsset->GetOffset("WalkForwardSpeed");
		auto WalkForwardSpeed = Get<float>(EmoteAsset, WalkForwardSpeedOffset);

		static auto EmoteWalkSpeedOffset = Pawn->GetOffset("EmoteWalkSpeed");
		auto EmoteWalkSpeed = Get<float>(Pawn, EmoteWalkSpeedOffset);

		*EmoteWalkSpeed = *WalkForwardSpeed;
	}

	Abilities::GiveAbilityAndActivateOnce(ASC, AbilityToUse, EmoteAsset);

	/* static auto Pawn_bIsEmoteLeaderOffset = Pawn->GetOffset("bIsEmoteLeader");

	if (Pawn_bIsEmoteLeaderOffset)
	{
		static auto Pawn_bIsEmoteLeaderyFieldMask = GetFieldMask(Pawn->GetProperty("bIsEmoteLeader"));
		auto Pawn_bIsEmoteLeader = Get<PlaceholderBitfield>(Pawn, Pawn_bIsEmoteLeaderOffset);

		static auto Emote_bGroupEmoteOffset = EmoteAsset->GetOffset("bGroupEmote");

		if (Emote_bGroupEmoteOffset)
		{
			static auto Emote_bGroupEmoteFieldMask = GetFieldMask(EmoteAsset->GetProperty("bGroupEmote"));
			PlaceholderBitfield* Emote_bGroupEmote = Get<PlaceholderBitfield>(EmoteAsset, Emote_bGroupEmoteOffset);

			SetBitfield(Pawn_bIsEmoteLeader, Pawn_bIsEmoteLeaderyFieldMask, ReadBitfield(Emote_bGroupEmote, Emote_bGroupEmoteFieldMask));
		}
	} */

	return false;
}

bool SpawnDefaultPawnFor(UObject* GameMode, UFunction*, void* Parameters)
{
	struct parmas { UObject* PlayerController; UObject* StartSpot; UObject* Pawn;  };

	auto Params = (parmas*)Parameters;

	std::cout << "cal!\n";

	// if (Helper::GetPawnFromController(Params->PlayerController))
		// return false;

	if (Helper::GetLocalPlayerController() == Params->PlayerController)
	{
		std::cout << "bruh!\n";
		return true;
	}

	FTransform Transform{};
	Transform.Translation = Helper::GetActorLocation(Helper::GetPlayerStart()); // Helper::GetActorLocation(Params->StartSpot); // IDK WHY THIS doesnt work
	Transform.Scale3D = { 1, 1, 1 };

	/* struct { UObject* NewPlayer; FTransform SpawnTransform; UObject* Pawn; } SpawnDefaultPawnAtTransform_Params{Params->PlayerController, Transform};

	static auto SpawnDefaultPawnAtTransform = FindObject<UFunction>("/Script/Engine.GameModeBase.SpawnDefaultPawnAtTransform");
	GameMode->ProcessEvent(SpawnDefaultPawnAtTransform, &SpawnDefaultPawnAtTransform_Params); */ // IDK WHY CRASH

	static auto PawnClass = FindObject("/Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C");

	Params->Pawn = Helper::Easy::SpawnActor(PawnClass, Transform.Translation, Transform.Rotation.Rotator()); // THIS IS ALL THE FUNC DOES anyway // SpawnDefaultPawnAtTransform_Params.Pawn;

	return true;
}

bool ServerSendZiplineState(UObject* Pawn, UFunction*, void* Parameters)
{
	/* if (ZiplineState.Zipline)
	{
		ZiplineState.TimeZipliningBegan = Helper::GetTimeSeconds();
	}
	else
	{
		ZiplineState.TimeZipliningEndedFromJump = Helper::GetTimeSeconds(); // ??
	} */

	static auto ZiplineStateOffset = Pawn->GetOffset("ZiplineState");

	auto PawnZiplineState = Get<void>(Pawn, ZiplineStateOffset);

	static auto AuthoritativeValueOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.ZiplinePawnState", "AuthoritativeValue");
	
	if (*(int*)(__int64(Parameters) + AuthoritativeValueOffset) > *(int*)(__int64(PawnZiplineState) + AuthoritativeValueOffset))
	{
		static auto ZiplinePawnStateSize = Helper::GetSizeOfClass(FindObject("/Script/FortniteGame.ZiplinePawnState"));

		memcpy_s(PawnZiplineState, ZiplinePawnStateSize, Parameters, ZiplinePawnStateSize);

		PawnZiplineState = Get<void>(Pawn, ZiplineStateOffset); // definitely how this works

		static auto bIsZipliningOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.ZiplinePawnState", "bIsZiplining");
		static auto bJumpedOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.ZiplinePawnState", "bJumped");

		if (!*(bool*)(__int64(PawnZiplineState) + bIsZipliningOffset))
		{
			if (*(bool*)(__int64(PawnZiplineState) + bJumpedOffset))
			{
				FVector LaunchVelocity;
				float ZiplineJumpDampening = 0.f;
				float ZiplineJumpStrength = 0.f;
				float PlayerVelocity = 0.f;
				float PlayerVelocity2 = 0.f;

				static auto CharacterMovementOffset = Pawn->GetOffset("CharacterMovement");
				auto CharacterMovement = *(UObject**)(__int64(Pawn) + CharacterMovementOffset);

				static auto JumpZVelocityOffset = CharacterMovement->GetOffset("JumpZVelocity");
				auto JumpZVelocity = Get<float>(CharacterMovement, JumpZVelocityOffset);

				PlayerVelocity = *JumpZVelocity;
				PlayerVelocity2 = *JumpZVelocity;

				static auto ZiplineJumpDampeningCurveOffset = Pawn->GetOffset("ZiplineJumpDampening");
				auto ZiplineJumpDampeningCurve = Get<FCurveTableRowHandle>(Pawn, ZiplineJumpDampeningCurveOffset);

				static auto ZiplineJumpStrengthCurveOffset = Pawn->GetOffset("ZiplineJumpStrength");
				auto ZiplineJumpStrengthCurve = Get<FCurveTableRowHandle>(Pawn, ZiplineJumpStrengthCurveOffset);

				// ZiplineJumpDampeningCurve->Eval(0, &ZiplineJumpDampening);
				// ZiplineJumpStrengthCurve->Eval(0, &ZiplineJumpStrength);

				// ZiplineJumpDampening = 1500;
				// ZiplineJumpStrength = -0.5;

				if ((float)(ZiplineJumpDampening * PlayerVelocity) >= -750.0)
					LaunchVelocity.X = fminf(ZiplineJumpDampening * PlayerVelocity, 750.0);
				else
					LaunchVelocity.X = -1002733568;
				if ((float)(ZiplineJumpDampening * PlayerVelocity2) >= -750.0)
					LaunchVelocity.Y = fminf(ZiplineJumpDampening * PlayerVelocity2, 750.0);
				else
					LaunchVelocity.Y = -1002733568;

				static auto LaunchCharacter = FindObject<UFunction>("/Script/Engine.Character.LaunchCharacter");

				struct { FVector LaunchVelocity; bool bXYOverride; bool bZOverride; } ACharacter_LaunchCharacter_Params{LaunchVelocity, false, false};
				Pawn->ProcessEvent(LaunchCharacter, &ACharacter_LaunchCharacter_Params);
			}
		}
	}

	static bool bFoundFunc = false;

	static void (*OnRep_ZiplineState)(UObject* Pawn);

	if (!bFoundFunc)
	{
		bFoundFunc = true;

		static auto Addrr = Memcury::Scanner::FindStringRef(L"ZIPLINES!! Role(%s)  AFortPlayerPawn::OnRep_ZiplineState ZiplineState.bIsZiplining=%d").Get();

		std::cout << "Addrr: " << Addrr << '\n';

		if (Addrr)
		{
			for (int i = 600; i >= 0; i--)
			{
				// LOG("[{}] 0x{:x} 0x{:x}", i, (int)*(uint8_t*)Addr - i, (int)*(uint8_t*)(Addr - i), (int)*(uint8_t*)(Addr - i + 1));

				if (*(uint8_t*)(uint8_t*)(Addrr - i) == 0x40 && *(uint8_t*)(uint8_t*)(Addrr - i + 1) == 0x53)
				{
					OnRep_ZiplineState = decltype(OnRep_ZiplineState)(Addrr - i);
				}
			}
		}

		std::cout << std::format("OnRep_ZiplineState: 0x{:x}\n", (uintptr_t)OnRep_ZiplineState - __int64(GetModuleHandleW(0)));
	}

	if (OnRep_ZiplineState)
		OnRep_ZiplineState(Pawn);

	// Helper::ForceNetUpdate(Pawn);

	return false;
}

bool onendabilitydance(UObject* Ability, UFunction*, void* Parameters)
{
	UObject* Pawn; // Helper::GetOwner(ability);
	static auto Func = FindObject<UFunction>("/Script/FortniteGame.FortGameplayAbility.GetActivatingPawn");
	Ability->ProcessEvent(Func, &Pawn);

	if (Pawn)
	{
		// std::cout << "got pawn!\n";

		static auto Pawn_bMovingEmoteOffset = Pawn->GetOffset("bMovingEmote");
		PlaceholderBitfield* Pawn_bMovingEmote = Get<PlaceholderBitfield>(Pawn, Pawn_bMovingEmoteOffset);

		static auto Pawn_bMovingEmoteForwardOnlyOffset = Pawn->GetOffset("bMovingEmoteForwardOnly");

		if (Pawn_bMovingEmoteForwardOnlyOffset)
		{
			PlaceholderBitfield* Pawn_bMovingEmoteForwardOnly = Get<PlaceholderBitfield>(Pawn, Pawn_bMovingEmoteForwardOnlyOffset);
			static auto Pawn_bMovingEmoteForwardOnlyFieldMask = GetFieldMask(Pawn->GetProperty("bMovingEmoteForwardOnly"));

			SetBitfield(Pawn_bMovingEmoteForwardOnly, Pawn_bMovingEmoteForwardOnlyFieldMask, false);
		}

		static auto Pawn_bMovingEmoteFieldMask = GetFieldMask(Pawn->GetProperty("bMovingEmote"));

		// std::cout << "bMovingEmoteFieldMask: " << (int)Pawn_bMovingEmoteFieldMask << '\n';

		SetBitfield(Pawn_bMovingEmote, Pawn_bMovingEmoteFieldMask, false);

		/* static auto Pawn_bIsEmoteLeaderOffset = Pawn->GetOffset("bIsEmoteLeader");

		if (Pawn_bIsEmoteLeaderOffset)
		{
			static auto Pawn_bIsEmoteLeaderyFieldMask = GetFieldMask(Pawn->GetProperty("bIsEmoteLeader"));
			auto Pawn_bIsEmoteLeader = Get<PlaceholderBitfield>(Pawn, Pawn_bIsEmoteLeaderOffset);

			SetBitfield(Pawn_bIsEmoteLeader, Pawn_bIsEmoteLeaderyFieldMask, false);
		} */
	}

	return false;
}

bool HandleOwnerAsBuildingActorDestroyed(UObject* ObjectComponent, UFunction* func, void* Parameters)
{
	auto InstigatedByOffset = 0x10; // func->GetOffset("InstigatedBy", true);
	auto InstigatedBy = *Get<UObject*>(func, InstigatedByOffset); // AController;

	if (!InstigatedBy)
		return false;

	std::cout << "InstigatedBy Name: " << InstigatedBy->GetFullName() << '\n';

	static auto pcl = FindObject("/Script/FortniteGame.FortPlayerController");

	if (!InstigatedBy->IsA(pcl))
		return false;

	static auto EquippedWeaponItemDefinitionOffset = ObjectComponent->GetOffset("EquippedWeaponItemDefinition");
	TSoftObjectPtr EquippedWeaponItemDefinitionSoft = *Get<TSoftObjectPtr>(ObjectComponent, EquippedWeaponItemDefinitionOffset);

	auto EquippedWeaponItemDefinition = EquippedWeaponItemDefinitionSoft.Get(nullptr, true);

	std::cout << "EquippedWeaponItemDefinition: " << EquippedWeaponItemDefinition << '\n';

	if (EquippedWeaponItemDefinition)
		Inventory::GiveItem(InstigatedBy, EquippedWeaponItemDefinition, EFortQuickBars::Primary, 1);

	return false;
}

bool OnDeathServer(UObject* BuildingActor, UFunction* func, void* Parameters)
{
	/* static auto InstigatedByOffset = FindOffsetStruct("Function /Script/FortniteGame.BuildingActor.OnDeathServer", "InstigatedBy");
	auto InstigatedBy = *Get<UObject*>(BuildingActor, InstigatedByOffset);

	if (!Defines::bIsRestarting)
	{
		if (Engine_Version >= 424)
		{
			static auto BuildingContainerClass = FindObject("/Script/FortniteGame.BuildingContainer");

			if (BuildingActor->IsA(BuildingContainerClass))
			{
				static auto InteractionComponentOffset = InstigatedBy->GetOffset("InteractionComponent");
				auto InteractionComponent = *Get<UObject*>(InstigatedBy, InteractionComponentOffset);

				Interaction::ServerAttemptInteract(InteractionComponent, nullptr, &BuildingActor);
			}
		}
	} */

	return false;
}

bool OnAircraftExitedDropZone(UObject* GameMode, UFunction*, void* Parameters)
{
	auto aa = [](UObject* Controller) {
		// static auto FortPlayerControllerClass = FindObjectSlow("Class /Script/FortniteGame.FortPlayerController", false);
		// std::cout << "FortPlayerControllerClass: " << FortPlayerControllerClass << '\n';

		// if (Controller->IsA(FortPlayerControllerClass) && Helper::IsInAircraft(Controller))
		if (Helper::IsInAircraft(Controller))
		{
			if (Fortnite_Season < 20)
			{
				FRotator rot{ 0, 0, 0 };
				ServerAttemptAircraftJump(Controller, nullptr, &rot);
			}
			else
			{
				DRotator rot{ 0, 0, 0 };
				ServerAttemptAircraftJump(Controller, nullptr, &rot);
			}
		}
	};

	Helper::LoopConnections(aa, true);

	return false;
}

bool PlayerCanRestart(UObject* GameMode, UFunction*, void* Parameters)
{
	/* std::cout << "PlayerCanRestart!\n";

	struct aa { UObject* Controller; bool ret; };

	auto Params = (aa*)Parameters;

	Params->ret = true;

	static auto bWorldIsReadyOffset = GameMode->GetOffset("bWorldIsReady");
	std::cout << "Get<PlaceholderBitfield>(GameMode, bWorldIsReadyOffset)->First : " << Get<PlaceholderBitfield>(GameMode, bWorldIsReadyOffset)->First << '\n';
	Get<PlaceholderBitfield>(GameMode, bWorldIsReadyOffset)->First = true;

	return true; */

	return false;
}

bool ServerChoosePart(UObject* Pawn, UFunction*, void* Parameters)
{
	struct SCP_Params { TEnumAsByte<EFortCustomPartType> Part; UObject* ChosenCharacterPart; };

	auto Params = (SCP_Params*)Parameters;

	if (Params && (!Params->ChosenCharacterPart && Params->Part.Get() != EFortCustomPartType::Backpack))
		return true;

	return false;
}

bool ServerUpdateStateSync(UObject* Vehicle, UFunction*, void* Parameters)
{
	auto StateSyncData = (TArray<unsigned char>*)Parameters;

	std::cout << "StateSyncData Num: " << StateSyncData->Num() << '\n';

	for (int i = 0; i < StateSyncData->Num(); i++)
	{
		auto StateSyncDataCurrent = StateSyncData->At(i);

		std::cout << "StateSyncData: " << StateSyncData << '\n';
		std::cout << "StateSyncData Int: " << (int)StateSyncData << '\n';
	}

	return false;
}

void AddHook(const std::string& str, std::function<bool(UObject*, UFunction*, void*)> func)
{
	auto funcObject = FindObject<UFunction>(str);

	if (!funcObject)
		std::cout << "Unable to find Function: " << str << '\n';
	else
		FunctionsToHook.insert({ funcObject, func });
}

void ProcessEventDetour(UObject* Object, UFunction* Function, void* Parameters)
{
	if (!Object || !Function)
		return;

	if (Defines::bLogProcessEvent)
	{
		auto FunctionName = Function->GetPathName();

		if (!strstr(FunctionName.c_str(), ("EvaluateGraphExposedInputs")) &&
			!strstr(FunctionName.c_str(), ("Tick")) &&
			!strstr(FunctionName.c_str(), ("OnSubmixEnvelope")) &&
			!strstr(FunctionName.c_str(), ("OnSubmixSpectralAnalysis")) &&
			!strstr(FunctionName.c_str(), ("OnMouse")) &&
			!strstr(FunctionName.c_str(), ("Pulse")) &&
			!strstr(FunctionName.c_str(), ("BlueprintUpdateAnimation")) &&
			!strstr(FunctionName.c_str(), ("BlueprintPostEvaluateAnimation")) &&
			!strstr(FunctionName.c_str(), ("BlueprintModifyCamera")) &&
			!strstr(FunctionName.c_str(), ("BlueprintModifyPostProcess")) &&
			!strstr(FunctionName.c_str(), ("Loop Animation Curve")) &&
			!strstr(FunctionName.c_str(), ("UpdateTime")) &&
			!strstr(FunctionName.c_str(), ("GetMutatorByClass")) &&
			!strstr(FunctionName.c_str(), ("UpdatePreviousPositionAndVelocity")) &&
			!strstr(FunctionName.c_str(), ("IsCachedIsProjectileWeapon")) &&
			!strstr(FunctionName.c_str(), ("LockOn")) &&
			!strstr(FunctionName.c_str(), ("GetAbilityTargetingLevel")) &&
			!strstr(FunctionName.c_str(), ("ReadyToEndMatch")) &&
			!strstr(FunctionName.c_str(), ("ReceiveDrawHUD")) &&
			!strstr(FunctionName.c_str(), ("OnUpdateDirectionalLightForTimeOfDay")) &&
			!strstr(FunctionName.c_str(), ("GetSubtitleVisibility")) &&
			!strstr(FunctionName.c_str(), ("GetValue")) &&
			!strstr(FunctionName.c_str(), ("InputAxisKeyEvent")) &&
			!strstr(FunctionName.c_str(), ("ServerTouchActiveTime")) &&
			!strstr(FunctionName.c_str(), ("SM_IceCube_Blueprint_C")) &&
			!strstr(FunctionName.c_str(), ("OnHovered")) &&
			!strstr(FunctionName.c_str(), ("OnCurrentTextStyleChanged")) &&
			!strstr(FunctionName.c_str(), ("OnButtonHovered")) &&
			!strstr(FunctionName.c_str(), ("ExecuteUbergraph_ThreatPostProcessManagerAndParticleBlueprint")) &&
			!strstr(FunctionName.c_str(), ("UpdateCamera")) &&
			!strstr(FunctionName.c_str(), ("GetMutatorContext")) &&
			!strstr(FunctionName.c_str(), ("CanJumpInternal")) &&
			!strstr(FunctionName.c_str(), ("OnDayPhaseChanged")) &&
			!strstr(FunctionName.c_str(), ("Chime")) &&
			!strstr(FunctionName.c_str(), ("ServerMove")) &&
			!strstr(FunctionName.c_str(), ("OnVisibilitySetEvent")) &&
			!strstr(FunctionName.c_str(), "ReceiveHit") &&
			!strstr(FunctionName.c_str(), "ReadyToStartMatch") &&
			!strstr(FunctionName.c_str(), "ClientAckGoodMove") &&
			!strstr(FunctionName.c_str(), "Prop_WildWest_WoodenWindmill_01") &&
			!strstr(FunctionName.c_str(), "ContrailCheck") &&
			!strstr(FunctionName.c_str(), "B_StockBattleBus_C") &&
			!strstr(FunctionName.c_str(), "Subtitles.Subtitles_C.") &&
			!strstr(FunctionName.c_str(), "/PinkOatmeal/PinkOatmeal_") &&
			!strstr(FunctionName.c_str(), "BP_SpectatorPawn_C") &&
			!strstr(FunctionName.c_str(), "FastSharedReplication") &&
			!strstr(FunctionName.c_str(), "OnCollisionHitEffects") &&
			!strstr(FunctionName.c_str(), "BndEvt__SkeletalMesh") &&
			!strstr(FunctionName.c_str(), ".FortAnimInstance.AnimNotify_") &&
			!strstr(FunctionName.c_str(), "OnBounceAnimationUpdate") &&
			!strstr(FunctionName.c_str(), "ShouldShowSoundIndicator") &&
			!strstr(FunctionName.c_str(), "Primitive_Structure_AmbAudioComponent_C") &&
			!strstr(FunctionName.c_str(), "PlayStoppedIdleRotationAudio") &&
			!strstr(FunctionName.c_str(), "UpdateOverheatCosmetics") &&
			!strstr(FunctionName.c_str(), "StormFadeTimeline__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "BindVolumeEvents") &&
			!strstr(FunctionName.c_str(), "UpdateStateEvent") &&
			!strstr(FunctionName.c_str(), "VISUALS__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "Flash__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "SetCollisionEnabled") &&
			!strstr(FunctionName.c_str(), "SetIntensity") &&
			!strstr(FunctionName.c_str(), "Storm__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "CloudsTimeline__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "SetRenderCustomDepth") &&
			!strstr(FunctionName.c_str(), "K2_UpdateCustomMovement") &&
			!strstr(FunctionName.c_str(), "AthenaHitPointBar_C.Update") &&
			!strstr(FunctionName.c_str(), "ExecuteUbergraph_Farm_WeatherVane_01") &&
			!strstr(FunctionName.c_str(), "HandleOnHUDElementVisibilityChanged") &&
			!strstr(FunctionName.c_str(), "ExecuteUbergraph_Fog_Machine") &&
			!strstr(FunctionName.c_str(), "ReceiveBeginPlay") &&
			!strstr(FunctionName.c_str(), "OnMatchStarted") &&
			!strstr(FunctionName.c_str(), "CustomStateChanged") &&
			!strstr(FunctionName.c_str(), "OnBuildingActorInitialized") &&
			!strstr(FunctionName.c_str(), "OnWorldReady") &&
			!strstr(FunctionName.c_str(), "OnAttachToBuilding") &&
			!strstr(FunctionName.c_str(), "Clown Spinner") &&
			!strstr(FunctionName.c_str(), "K2_GetActorLocation") &&
			!strstr(FunctionName.c_str(), "GetViewTarget") &&
			!strstr(FunctionName.c_str(), "GetAllActorsOfClass") &&
			!strstr(FunctionName.c_str(), "OnUpdateMusic") &&
			!strstr(FunctionName.c_str(), "Check Closest Point") &&
			!strstr(FunctionName.c_str(), "OnSubtitleChanged__DelegateSignature") &&
			!strstr(FunctionName.c_str(), "OnServerBounceCallback") &&
			!strstr(FunctionName.c_str(), "BlueprintGetInteractionTime") &&
			!strstr(FunctionName.c_str(), "OnServerStopCallback") &&
			!strstr(FunctionName.c_str(), "Light Flash Timeline__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "MainFlightPath__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "PlayStartedIdleRotationAudio") &&
			!strstr(FunctionName.c_str(), "BGA_Athena_FlopperSpawn_") &&
			!strstr(FunctionName.c_str(), "CheckShouldDisplayUI") &&
			!strstr(FunctionName.c_str(), "Timeline_0__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "ClientMoveResponsePacked") &&
			!strstr(FunctionName.c_str(), "ExecuteUbergraph_B_Athena_FlopperSpawnWorld_Placement") &&
			!strstr(FunctionName.c_str(), "Countdown__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "OnParachuteTrailUpdated") &&
			!strstr(FunctionName.c_str(), "Moto FadeOut__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "ExecuteUbergraph_Apollo_GasPump_Valet") &&
			!strstr(FunctionName.c_str(), "GetOverrideMeshMaterial") &&
			!strstr(FunctionName.c_str(), "VendWobble__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "WaitForPawn") &&
			!strstr(FunctionName.c_str(), "FragmentMovement__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "TrySetup") &&
			!strstr(FunctionName.c_str(), "Fade Doused Smoke__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "SetPlayerToSkydive") &&
			!strstr(FunctionName.c_str(), "BounceCar__UpdateFunc") &&
			!strstr(FunctionName.c_str(), "BP_CalendarDynamicPOISelect") &&
			!strstr(FunctionName.c_str(), "OnComponentHit_Event_0") &&
			!strstr(FunctionName.c_str(), "HandleSimulatingComponentHit") &&
			!strstr(FunctionName.c_str(), "CBGA_GreenGlop_WithGrav_C") &&
			!strstr(FunctionName.c_str(), "WarmupCountdownEndTimeUpdated") &&
			!strstr(FunctionName.c_str(), "BP_CanInteract") &&
			!strstr(FunctionName.c_str(), "AthenaHitPointBar_C") &&
			!strstr(FunctionName.c_str(), "ServerFireAIDirectorEvent") &&
			!strstr(FunctionName.c_str(), "BlueprintThreadSafeUpdateAnimation") &&
			!strstr(FunctionName.c_str(), "On Amb Zap Spawn") &&
			!strstr(FunctionName.c_str(), "ServerSetPlayerCanDBNORevive") &&
			!strstr(FunctionName.c_str(), "BGA_Petrol_Pickup_C"))
		{
			std::cout << ("Function called: ") << FunctionName << '\n';
		}
	}

	if (Defines::bLogRPCs)
	{
		auto FunctionName = Function->GetName();

		if (FunctionName.starts_with("Server"))
		{
			std::ofstream aaaaa("rpcs.log", std::ios_base::app);
			aaaaa << FunctionName << '\n';
			aaaaa.close();
		}
	}

	for (auto& Func : FunctionsToHook)
	{
		if (Function == Func.first)
		{
			if (Func.second(Object, Function, Parameters)) // If the returned true, then cancel default execution.
			{
				return;
			}

			break; // There shouldn't be repeat.
		}
	}

	return ProcessEventO(Object, Function, Parameters);
}
