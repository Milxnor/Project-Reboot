#include "processevent.h"
#include "helper.h"
#include <MinHook.h>
#include "inventory.h"
#include "abilities.h"
#include "patterns.h"
#include "server.h"
#include "harvesting.h"
#include "calendar.h"

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
	if (!Parameters)
		return false;

	static bool bSpawnedFloorLoot = false;

	if (!bSpawnedFloorLoot)
	{
		bSpawnedFloorLoot = true;

		Defines::bShouldSpawnFloorLoot = true;

		static auto OnRep_CurrentPlaylistInfo = FindObject<UFunction>("Function /Script/FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistInfo");
		Helper::GetGameState()->ProcessEvent(OnRep_CurrentPlaylistInfo);

		if (Engine_Version > 424)
			AddHook("Function /Script/FortniteGame.BuildingSMActor.BlueprintCanAttemptGenerateResources", Harvesting::BlueprintCanAttemptGenerateResources);
		else
		{
			AddHook(("Function /Script/FortniteGame.BuildingActor.OnDamageServer"), Harvesting::OnDamageServer);

			if (Fortnite_Version >= 8.00)
				AddHook("Function /Game/Building/ActorBlueprints/Prop/Car_DEFAULT.Car_DEFAULT_C.OnDamageServer", Harvesting::OnDamageServer);
			else
				AddHook("Function /Game/Building/ActorBlueprints/Prop/Car_Copper.Car_Copper_C.OnDamageServer", Harvesting::OnDamageServer);
		}
	}

	UObject* PlayerController = *(UObject**)Parameters;

	if (PlayerController)
	{
		if (Fortnite_Version < 7.4)
		{
			static const auto QuickBarsClass = FindObject("Class /Script/FortniteGame.FortQuickBars");
			static auto QuickBarsOffset = PlayerController->GetOffset("QuickBars");

			*(UObject**)(__int64(PlayerController) + QuickBarsOffset) = Helper::Easy::SpawnActor(QuickBarsClass, FVector(), FRotator(), PlayerController);
		}

		auto PlayerState = Helper::GetPlayerStateFromController(PlayerController);

		static auto bHasServerFinishedLoadingOffset = PlayerController->GetOffset("bHasServerFinishedLoading");
		*Get<bool>(PlayerController, bHasServerFinishedLoadingOffset) = true;

		static auto bHasStartedPlayingOffset = PlayerState->GetOffset("bHasStartedPlaying");
		*Get<bool>(PlayerState, bHasStartedPlayingOffset) = true;

		auto PlayerStart = Helper::GetPlayerStart();
		
		if (!PlayerStart)
		{
			std::cout << "Player joined too early or unable to find playerstart!\n";
			// Helper::KickController(PlayerController, "You joined too early!");
			// return false;
		}

		bool bSpawnIsland = Engine_Version >= 424; // or else u die

		auto SpawnLocation = !PlayerStart || !bSpawnIsland ? FVector{ 1250, 1818, 3284 } : Helper::GetActorLocation(PlayerStart);

		auto Pawn = Helper::SpawnPawn(PlayerController, SpawnLocation, true);

		bool bUpdate = false;

		static auto EditTool = FindObject(("FortEditToolItemDefinition /Game/Items/Weapons/BuildingTools/EditTool.EditTool"));
		auto EditToolInstance = Inventory::GiveItem(PlayerController, EditTool, EFortQuickBars::Primary, 0, 1, bUpdate);

		static auto BuildingItemData_Wall = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall"));
		static auto BuildingItemData_Floor = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor"));
		static auto BuildingItemData_Stair_W = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W"));
		static auto BuildingItemData_RoofS = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS"));

		Inventory::GiveItem(PlayerController, BuildingItemData_Wall, EFortQuickBars::Secondary, 0, bUpdate);
		Inventory::GiveItem(PlayerController, BuildingItemData_Floor, EFortQuickBars::Secondary, 1, bUpdate);
		Inventory::GiveItem(PlayerController, BuildingItemData_Stair_W, EFortQuickBars::Secondary, 2, bUpdate);
		Inventory::GiveItem(PlayerController, BuildingItemData_RoofS, EFortQuickBars::Secondary, 3, bUpdate);

		static auto PIDClass = FindObject("Class /Script/FortniteGame.AthenaPickaxeItemDefinition");

		static auto AllObjects = Helper::GetAllObjectsOfClass(PIDClass);

		auto random = rand() % (AllObjects.size());
		random = random <= 0 ? 1 : random;

		auto pick = AllObjects.at(random);
		static auto WeaponDefinitionOffset = pick->GetOffset("WeaponDefinition");

		auto PickaxeDef = *(UObject**)(__int64(pick) + WeaponDefinitionOffset);

		// static auto PickaxeDef = FindObject("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
		auto PickaxeInstance = Inventory::GiveItem(PlayerController, PickaxeDef, EFortQuickBars::Primary, 0);

		static auto WoodItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
		Inventory::GiveItem(PlayerController, WoodItemData, EFortQuickBars::Secondary, 0, 999);

		auto AbilitySystemComponent = Helper::GetAbilitySystemComponent(Pawn);

		if (Engine_Version == 421 || Fortnite_Season == 14)
		{
			if (Fortnite_Version < 8.30)
			{
				static auto AbilitySet = FindObject(("FortAbilitySet /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer"));

				if (AbilitySet)
				{
					static auto GameplayAbilitiesOffset = AbilitySet->GetOffset("GameplayAbilities");
					auto Abilities = (TArray<UObject*>*)(__int64(AbilitySet) + GameplayAbilitiesOffset);

					if (Abilities)
					{
						for (int i = 0; i < Abilities->Num(); i++)
						{
							auto Ability = Abilities->At(i);

							if (!Ability)
								continue;

							Abilities::GrantGameplayAbility(Pawn, Ability);
						}
					}
				}
			}
			else
			{
				static auto AbilitySet = FindObject(("FortAbilitySet /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_AthenaPlayer.GAS_AthenaPlayer"));

				if (AbilitySet)
				{
					static auto GameplayAbilitiesOffset = AbilitySet->GetOffset("GameplayAbilities");
					auto Abilities = Get<TArray<UObject*>>(AbilitySet, GameplayAbilitiesOffset);

					if (Abilities)
					{
						for (int i = 0; i < Abilities->Num(); i++)
						{
							auto Ability = Abilities->At(i);

							if (!Ability)
								continue;

							Abilities::GrantGameplayAbility(Pawn, Ability);
						}
					}
				}
			}
		}
	}

	return false;
}

bool ReadyToStartMatch(UObject* GameMode, UFunction* Function, void* Parameters)
{
	if (GameMode && Defines::bReadyForStartMatch)
	{
		Defines::bReadyForStartMatch = false;

		auto GameState = Helper::GetGameState();

		static auto GamePhaseOffset = GameState->GetOffset("GamePhase");
		*Get<EAthenaGamePhase>(GameState, GamePhaseOffset) = EAthenaGamePhase::None;

		if (!Server::BeaconHost)
		{
			Server::Listen();
			Server::Hooks::Initialize();
		}

		static auto Playlist = FindObject("FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo");

		auto GameStatePlaylist = Helper::GetPlaylist();

		if (GameStatePlaylist)
		{
			*GameStatePlaylist = Playlist;

			if (Fortnite_Version >= 6.10)
			{
				static auto CurrentPlaylistInfoOffset = GameState->GetOffset("CurrentPlaylistInfo");

				static auto PlaylistReplicationKeyOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.PlaylistPropertyArray"), ("PlaylistReplicationKey"));

				auto PlaylistInfo = (void*)(__int64(GameState) + CurrentPlaylistInfoOffset);
				auto PlaylistReplicationKey = (int*)(__int64(PlaylistInfo) + PlaylistReplicationKeyOffset);

				(*(int*)(__int64(PlaylistInfo) + PlaylistReplicationKeyOffset))++;
				FastTArray::MarkArrayDirty(PlaylistInfo);
			}
		}

		static auto GameSessionOffset = GameMode->GetOffset("GameSession");
		auto GameSession = *Get<UObject*>(GameMode, GameSessionOffset);

		static auto MaxPlayersOffset = GameSession->GetOffset("MaxPlayers");
		*Get<int>(GameSession, MaxPlayersOffset) = 100; // We would get from playlist but playground max is 4 people..

		Calendar::FixLocations();

		std::cout << "Ready to start match!\n";
	}

	return false;
}

uint8_t GetDeathCause(UObject* PlayerState, FGameplayTagContainer Tags, bool* OutWasDBNO = nullptr)
{
	// UFortDeathCauseFromTagMapping
	// FortDeathCauseFromTagMapping

	uint8_t DeathCause = 0;

	std::cout << "Tags: " << Tags.ToStringSimple(true) << '\n';

	if (Fortnite_Version >= 6.21) // might have been 6.2
	{
		static auto FortPlayerStateAthenaDefault = FindObject("FortPlayerStateAthena /Script/FortniteGame.Default__FortPlayerStateAthena");

		struct
		{
			FGameplayTagContainer                       InTags;                                                   // (ConstParm, Parm, OutParm, ReferenceParm, NativeAccessSpecifierPublic)
			bool                                               bWasDBNO;                                                 // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			uint8_t                                        ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		} AFortPlayerStateAthena_ToDeathCause_Params{ Tags, false };

		static auto ToDeathCause = FindObject<UFunction>("Function /Script/FortniteGame.FortPlayerStateAthena.ToDeathCause");
		FortPlayerStateAthenaDefault->ProcessEvent(ToDeathCause, &AFortPlayerStateAthena_ToDeathCause_Params);

		DeathCause = AFortPlayerStateAthena_ToDeathCause_Params.ReturnValue;
	}
	else
	{
		static auto DeathCauseEnum = FindObject("Enum /Script/FortniteGame.EDeathCause");

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


bool ClientOnPawnDied(UObject* DeadController, UFunction*, void* Parameters)
{
	if (!Parameters)
		return false;

	std::cout << "Mf!\n";

	auto DeathReport = (__int64*)Parameters;

	auto DeadPlayerState = Helper::GetPlayerStateFromController(DeadController);
	auto DeadPawn = Helper::GetPawnFromController(DeadController);

	static auto KillerPawnOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport", "KillerPawn");
	static auto KillerPlayerStateOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport", "KillerPlayerState");

	auto KillerPawn = *(UObject**)(__int64(DeathReport) + KillerPawnOffset);
	auto KillerPlayerState = *(UObject**)(__int64(DeathReport) + KillerPlayerStateOffset);

	static auto TagsOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport"), ("Tags"));
	FGameplayTagContainer* Tags = (FGameplayTagContainer*)(__int64(DeathReport) + TagsOffset);
	bool bWasDBNO = false;
	auto DeathCause = Tags ? GetDeathCause(DeadPlayerState, *Tags, &bWasDBNO) : 0;

	// normal death stuff

	auto DeathInfoOffset = DeadPlayerState->GetOffset("DeathInfo");

	if (DeathInfoOffset == 0) // iirc if u rejoin and die this is invalid idfk why
		return false;

	auto DeathInfo = Get<__int64>(DeadPlayerState, DeathInfoOffset);

	static auto DeathCauseOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.DeathInfo"), ("DeathCause"));
	static auto FinisherOrDownerOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.DeathInfo"), ("FinisherOrDowner"));
	static auto bDBNOOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.DeathInfo"), ("bDBNO"));
	static auto DistanceOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.DeathInfo"), ("Distance"));
	static auto DeathCauseEnum = FindObject("Enum /Script/FortniteGame.EDeathCause");

	*(uint8_t*)(__int64(DeathInfo) + DeathCauseOffset) = DeathCause;
	*(UObject**)(__int64(DeathInfo) + FinisherOrDownerOffset) = KillerPlayerState ? KillerPlayerState : DeadPlayerState;
	*(bool*)(__int64(DeathInfo) + bDBNOOffset) = bWasDBNO;

	static auto FallDamageEnumValue = GetEnumValue(DeathCauseEnum, "FallDamage");

	if (DeathCause != FallDamageEnumValue)
	{
		*(float*)(__int64(DeathInfo) + DistanceOffset) = KillerPawn ? Helper::GetDistanceTo(KillerPawn, DeadPawn) : 0.f;
	}
	else
	{
		static auto LastFallDistanceOffset = DeadPawn->GetOffset("LastFallDistance");

		if (LastFallDistanceOffset != -1)
			*(float*)(__int64(DeathInfo) + DistanceOffset) = *(float*)(__int64(DeadPawn) + LastFallDistanceOffset);
	}

	return false;
}

bool ServerAttemptAircraftJump(UObject* Controller, UFunction*, void* Parameters)
{
	if (Engine_Version >= 424)
		Controller = Helper::GetOwnerOfComponent(Controller); // CurrentAircraft

	auto Rotation = Parameters ? *(FRotator*)Parameters : FRotator();

	auto GameState = Helper::GetGameState();

	static auto AircraftsOffset = GameState->GetOffset("Aircrafts");
	auto Aircrafts = (TArray<UObject*>*)(__int64(GameState) + AircraftsOffset);

	if (!Aircrafts)
		return false;

	auto Aircraft = Aircrafts->At(0);

	if (!Aircraft)
		return false;

	auto ExitLocation = Helper::GetActorLocation(Aircraft);

	auto Pawn = Helper::SpawnPawn(Controller, ExitLocation, false);

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

	auto FunctionName = Function->GetFullName();

	if (Defines::bLogProcessEvent)
	{
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
			!strstr(FunctionName.c_str(), "OnSubtitleChanged__DelegateSignature"))
		{
			std::cout << ("Function called: ") << FunctionName << '\n';
		}
	}

	for (auto& Func : FunctionsToHook)
	{
		if (Function == Func.first)
		{
			if (Func.second(Object, Function, Parameters)) // If the function returned true, then cancel default execution.
			{
				return;
			}

			break; // There shouldn't be repeat.
		}
	}

	return ProcessEventO(Object, Function, Parameters);
}