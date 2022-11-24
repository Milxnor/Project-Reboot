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
#include "interaction.h"

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

		Defines::bShouldSpawnFloorLoot = Looting::bInitialized && Fortnite_Version < 19.40;

		static auto func1 = FindObject("/Game/Athena/SafeZone/SafeZoneIndicator.SafeZoneIndicator_C.OnSafeZoneStateChange");

		AddHook(func1 ? "/Game/Athena/SafeZone/SafeZoneIndicator.SafeZoneIndicator_C.OnSafeZoneStateChange" :
			"/Script/FortniteGame.FortSafeZoneIndicator.OnSafeZoneStateChange", Zone::OnSafeZoneStateChange);

		AddHook("/Script/FortniteGame.FortMatchAnalytics.OnGamePhaseChanged", OnGamePhaseChanged);

		// AddHook("/Game/Abilities/Weapons/Ranged/GA_Ranged_GenericDamage.GA_Ranged_GenericDamage_C.K2_CommitExecute", commitExecuteWeapon);

		AddHook(Engine_Version >= 424 ? "/Script/FortniteGame.FortPhysicsPawn.ServerMove" 
			: "/Script/FortniteGame.FortAthenaVehicle.ServerUpdatePhysicsParams", ServerUpdatePhysicsParams);
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
	}

	UObject* PlayerController = *(UObject**)Parameters;

	if (PlayerController)
	{
		auto PlayerState = Helper::GetPlayerStateFromController(PlayerController);

		if (!PlayerState)
			return PlayerController;

		/* static auto InventoryNetworkManagementComponentOffset = PlayerController->GetOffset("InventoryNetworkManagementComponent");

		std::cout << "InventoryNetworkManagementComponent: " << *Get<UObject*>(PlayerController, InventoryNetworkManagementComponentOffset) << '\n';

		*Get<UObject*>(PlayerController, InventoryNetworkManagementComponentOffset) = nullptr;

		auto WorldInventory = Inventory::GetWorldInventory(PlayerController);

		static auto ForceNetUpdate = FindObject<UFunction>("/Script/Engine.Actor.ForceNetUpdate");
		WorldInventory->ProcessEvent(ForceNetUpdate);

		static auto NetUpdateFrequencyOffset = WorldInventory->GetOffset("NetUpdateFrequency");
		*Get<float>(WorldInventory, NetUpdateFrequencyOffset) = 100.f;

		static auto MinNetUpdateFrequencyOffset = WorldInventory->GetOffset("MinNetUpdateFrequency");
		*Get<float>(WorldInventory, MinNetUpdateFrequencyOffset) = 100.f;

		static auto NetPriorityOffset = WorldInventory->GetOffset("NetPriority");
		*Get<float>(WorldInventory, NetPriorityOffset) = 3.f; */

		/* static auto AttributeSetsOffset = PlayerState->GetOffset("AttributeSets");
		auto AttributeSets = Get<__int64>(PlayerState, AttributeSetsOffset);
		auto HomebaseSet = *(UObject**)(__int64(AttributeSets) + 0x48);

		// *(UObject**)(__int64(AttributeSets) + 0x48) = nullptr;

		static auto WorldInventorySizeBonusOffset = HomebaseSet->GetOffset("WorldInventorySizeBonus");

		auto WorldInventorySizeBonus = Get<FFortGameplayAttributeData>(HomebaseSet, WorldInventorySizeBonusOffset);

		*WorldInventorySizeBonus = FFortGameplayAttributeData(); */

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

	return true;
}

bool ServerReadyToStartMatch(UObject* PlayerController, UFunction* Function, void* Parameters)
{
	auto Pawan = Helper::GetPawnFromController(PlayerController);

	if (Pawan)
		return false;

	auto GameState = Helper::GetGameState();
	static auto GamePhaseOffset = GameState->GetOffset("GamePhase");

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
		suauyr329234ig23 =Events::GetSpawnLocation(&bgotafuwyu24tgi32rg);
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

	if (Defines::bRandomCosmetics)
	{
		static auto GliderClass = FindObject("/Script/FortniteGame.AthenaGliderItemDefinition");
		static auto ContrailClass = FindObject("/Script/FortniteGame.AthenaSkyDiveContrailItemDefinition");

		auto CosmeticLoadoutPC = Helper::GetCosmeticLoadoutForPC(PlayerController);

		static auto CharacterOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortAthenaLoadout", "Character");
		*Get<UObject*>(CosmeticLoadoutPC, CharacterOffset) = Helper::GetRandomCID();

		static auto GliderOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortAthenaLoadout", "Glider");
		*Get<UObject*>(CosmeticLoadoutPC, GliderOffset) = Helper::GetRandomObjectOfClass(GliderClass);

		static auto SkyDiveContrailOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortAthenaLoadout", "SkyDiveContrail", false, false, false);

		if (SkyDiveContrailOffset != 0)
			*Get<UObject*>(CosmeticLoadoutPC, SkyDiveContrailOffset) = Helper::GetRandomObjectOfClass(ContrailClass);
	}

	bool bUpdate = false;

	static auto EditTool = FindObject("/Game/Items/Weapons/BuildingTools/EditTool.EditTool");
	auto EditToolInstance = Inventory::GiveItem(PlayerController, EditTool, EFortQuickBars::Primary, 0, 1, bUpdate);

	static auto BuildingItemData_Wall = FindObject(("/Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall"));
	static auto BuildingItemData_Floor = FindObject(("/Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor"));
	static auto BuildingItemData_Stair_W = FindObject(("/Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W"));
	static auto BuildingItemData_RoofS = FindObject(("/Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS"));

	Inventory::GiveItem(PlayerController, BuildingItemData_Wall, EFortQuickBars::Secondary, 0, bUpdate);
	Inventory::GiveItem(PlayerController, BuildingItemData_Floor, EFortQuickBars::Secondary, 1, bUpdate);
	Inventory::GiveItem(PlayerController, BuildingItemData_Stair_W, EFortQuickBars::Secondary, 2, bUpdate);
	Inventory::GiveItem(PlayerController, BuildingItemData_RoofS, EFortQuickBars::Secondary, 3, bUpdate);

	UObject* PickaxeDef = Defines::bRandomCosmetics ? Helper::GetPickaxeDef(PlayerController, true) : FindObject("/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
	auto PickaxeInstance = Inventory::GiveItem(PlayerController, PickaxeDef, EFortQuickBars::Primary, 0);

	if (Defines::bIsCreative)
	{
		static UObject* PhoneDef = FindObject("/Game/Athena/Items/Weapons/Prototype/WID_CreativeTool.WID_CreativeTool");
		auto CreativeToolInstance = Inventory::GiveItem(PlayerController, PhoneDef, EFortQuickBars::Primary, 1);
	}

	// static auto d = FindObject("/ParallelGameplay/Items/WestSausage/WID_WestSausage_Parallel.WID_WestSausage_Parallel");
	// auto d2 = Inventory::GiveItem(PlayerController, d, EFortQuickBars::Primary, 2);

	// FFortItemEntry::SetLoadedAmmo(UFortItem::GetItemEntry(d2), PlayerController, 100);

	//

	/* static UObject* Def1 = FindObject("/HighTower/Items/HoneyDew/Fist/Abilities/WID_HighTower_HoneyDew_Fists.WID_HighTower_HoneyDew_Fists");
	std::cout << "Def1: " << Def1 << '\n';
	auto Def1Instance = Inventory::GiveItem(PlayerController, Def1, EFortQuickBars::Primary, 1);

	static UObject* Def2 = FindObject("/HighTower/Items/Wasabi/Claws/CoreBR/WID_HighTower_Wasabi_Claws_CoreBR.WID_HighTower_Wasabi_Claws_CoreBR");
	auto Def2Instance = Inventory::GiveItem(PlayerController, Def2, EFortQuickBars::Primary, 2);

	static UObject* Def3 = FindObject("/HighTower/Items/Tomato/RepulsorCannon/CoreBR/WID_HighTower_Tomato_RepulsorCannon_CoreBR.WID_HighTower_Tomato_RepulsorCannon_CoreBR");
	auto Def3Instance = Inventory::GiveItem(PlayerController, Def3, EFortQuickBars::Primary, 3);

	static UObject* Def4 = FindObject("/HighTower/Items/Tapas/SkyStrike/CoreBR/WID_HighTower_Tapas_SkyStrike_CoreBR.WID_HighTower_Tapas_SkyStrike_CoreBR");
	auto Def4Instance = Inventory::GiveItem(PlayerController, Def4, EFortQuickBars::Primary, 4);

	static UObject* Def5 = FindObject("/HighTower/Items/Grape/BrambleShield/CoreBR/WID_HighTower_Grape_BrambleShield_CoreBR.WID_HighTower_Grape_BrambleShield_CoreBR");
	auto Def5Instance = Inventory::GiveItem(PlayerController, Def5, EFortQuickBars::Primary, 5); */
	
	/*

	static UObject* Def1 = FindObject("/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_Tomato.AGID_Athena_Keycard_Tomato");
	std::cout << "Def1: " << Def1 << '\n';
	auto Def1Instance = Inventory::GiveItem(PlayerController, Def1, EFortQuickBars::Primary, 1);

	static UObject* Def2 = FindObject("/Game/Athena/Items/Consumables/RiftItem/Athena_Rift_Item.Athena_Rift_Item");
	auto Def2Instance = Inventory::GiveItem(PlayerController, Def2, EFortQuickBars::Primary, 2);

	*/

	static UObject* Def1 = FindObject("/Game/Athena/Items/Traps/TID_Context_BouncePad_Athena.TID_Context_BouncePad_Athena");
	// auto Def1Instance = Inventory::GiveItem(PlayerController, Def1, EFortQuickBars::Secondary, 0);

	static UObject* Def2 = FindObject("/Game/Athena/Items/Traps/TID_Floor_MountedTurret_Athena.TID_Floor_MountedTurret_Athena");
	auto Def2Instance = Inventory::GiveItem(PlayerController, Def2, EFortQuickBars::Secondary, 0);

	if (Defines::bIsGoingToPlayMainEvent && Fortnite_Season == 16)
	{
		static auto PortalDeviceDef = FindObject("/Yogurt/Blueprints/WID_Yogurt_PortalDevice.WID_Yogurt_PortalDevice");
		Inventory::GiveItem(PlayerController, PortalDeviceDef, EFortQuickBars::Primary, 1);
	}

	//

	UObject* Pawn = Helper::SpawnPawn(PlayerController, SpawnLocation, true);

	auto AbilitySystemComponent = Helper::GetAbilitySystemComponent(Pawn);

	// if ((Engine_Version < 426 || Fortnite_Season >= 14) && Fortnite_Season < 17)
	{
		if (Fortnite_Version < 8.30)
		{
			static auto AbilitySet = FindObject(("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer"));

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
			static auto AbilitySet = FindObject(("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_AthenaPlayer.GAS_AthenaPlayer"));

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

	/* auto Boss = LoadObject(Helper::GetBGAClass(), "/Game/Athena/AI/MANG/BP_MangPlayerPawn_Boss_Meowscles_Jr.BP_MangPlayerPawn_Boss_Meowscles_Jr_C");

	std::cout << "Boss: " << Boss << '\n';

	Helper::Easy::SpawnActor(Boss, Helper::GetActorLocation(Pawn)); */

	if (Defines::bIsGoingToPlayMainEvent)
	{
		static auto CheatManagerOffset = PlayerController->GetOffset("CheatManager");
		auto CheatManager = (UObject**)(__int64(PlayerController) + CheatManagerOffset);

		static auto CheatManagerClass = FindObject("/Script/Engine.CheatManager");
		*CheatManager = Helper::Easy::SpawnObject(CheatManagerClass, PlayerController);

		static auto God = FindObject<UFunction>("/Script/Engine.CheatManager.God");
		(*CheatManager)->ProcessEvent(God);
	}

	if (Defines::bIsCreative)
	{
		static auto OtherRiftClass = FindObject("/Game/Playgrounds/Items/BGA_IslandPortal.BGA_IslandPortal_C"); // LoadObject(Helper::GetBGAClass(), "/Game/Playgrounds/Items/BGA_IslandPortal.BGA_IslandPortal_C");

		UObject* NewPortal = nullptr;

		std::cout << "OtherRiftClass: " << OtherRiftClass << '\n';

		const wchar_t* url = L"https://media.discordapp.net/attachments/993197214744715284/1038297667463291001/45F01C49-1220-426C-973B-110583AC1B4F.png"; // std::wstring(Defines::urlForPortal.begin(), Defines::urlForPortal.end()).c_str();

		if (OtherRiftClass)
		{
			NewPortal = Helper::Easy::SpawnActor(OtherRiftClass, Helper::GetActorLocation(Helper::GetPlayerStart()));

			static auto DestinationActorOffset = NewPortal->GetOffset("DestinationActor");
			*Get<UObject*>(NewPortal, DestinationActorOffset) = Pawn;

			static auto CreatorNameOffset = NewPortal->GetOffset("CreatorName");
			*Get<FString>(NewPortal, CreatorNameOffset) = L"MILXNORDEV";

			static auto UserDescriptionOffset = NewPortal->GetOffset("UserDescription");
			*Get<FString>(NewPortal, UserDescriptionOffset) = L"Project Reboot bad tbh";

			static auto ImageURLOffset = NewPortal->GetOffset("ImageURL");
			*Get<FString>(NewPortal, ImageURLOffset) = url;

			static auto OnThumbnailTextureReady = FindObject<UFunction>("/Script/FortniteGame.FortAthenaCreativePortal.OnThumbnailTextureReady");
			NewPortal->ProcessEvent(OnThumbnailTextureReady);

			static auto OnRep_ImageURLChanged = FindObject<UFunction>("/Script/FortniteGame.FortAthenaCreativePortal.OnRep_ImageURLChanged");
			NewPortal->ProcessEvent(OnRep_ImageURLChanged);

			static auto UniqueIdOffset = PlayerState->GetOffset("UniqueId");

			static auto OwningPlayerOffset = NewPortal->GetOffset("OwningPlayer");
			*Get<FUniqueNetIdRepl>(NewPortal, OwningPlayerOffset) = *(FUniqueNetIdRepl*)(__int64(PlayerState) + UniqueIdOffset);

			static auto bIsPublishedPortalOffset = NewPortal->GetOffset("bIsPublishedPortal");
			*Get<bool>(NewPortal, bIsPublishedPortalOffset) = false;

			static auto OnRep_PublishedPortal = FindObject<UFunction>("/Script/FortniteGame.FortAthenaCreativePortal.OnRep_PublishedPortal");
			NewPortal->ProcessEvent(OnRep_PublishedPortal);

			static auto OnRep_OwningPlayer = FindObject<UFunction>("/Script/FortniteGame.FortAthenaCreativePortal.OnRep_OwningPlayer");
			NewPortal->ProcessEvent(OnRep_OwningPlayer);

			static auto bPortalOpenOffset = NewPortal->GetOffset("bPortalOpen");
			*Get<bool>(NewPortal, bPortalOpenOffset) = true;

			static auto OnRep_PortalOpen = FindObject<UFunction>("/Script/FortniteGame.FortAthenaCreativePortal.OnRep_PortalOpen");
			NewPortal->ProcessEvent(OnRep_PortalOpen);
		}

		Helper::InitializeBuildingActor(PlayerController, NewPortal);

		static int OwnedPortalOffset = PlayerController->GetOffset("OwnedPortal");
		*Get<UObject*>(PlayerController, OwnedPortalOffset) = NewPortal;

		Defines::Portal = NewPortal;

		auto GameState = Helper::GetGameState();
		static auto VolumeManagerOffset = GameState->GetOffset("VolumeManager");

		auto VolumeManager = Get<UObject*>(GameState, VolumeManagerOffset);
		std::cout << "VolumeManager: " << *VolumeManager << '\n';

		if (!*VolumeManager)
		{
			static auto VolumeManagerClass = FindObject("/Game/Athena/BuildingActors/FortVolumeManager_BP.FortVolumeManager_BP_C");
			*VolumeManager = Helper::Easy::SpawnActor(VolumeManagerClass, Helper::GetActorLocation(Pawn));
		}

		std::cout << "VolumeManager: " << *VolumeManager << '\n';

		static auto VolumeClass = FindObject("/Script/FortniteGame.FortVolume");
		auto NewVolume = Helper::Easy::SpawnActor(VolumeClass, Helper::GetActorLocation(Pawn));
		Helper::InitializeBuildingActor(PlayerController, NewVolume);

		if (*VolumeManager)
		{
			struct FVolumePlayerStateInfo : public FFastArraySerializerItem
			{
				unsigned char                                      UnknownData00[0x4];                                       // 0x000C(0x0004) MISSED OFFSET
				UObject* PlayerState;                                              // 0x0010(0x0008) (ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				UObject* Volume;                                                   // 0x0018(0x0008) (ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			};

			struct FFortVolumeActiveUsers : public FastTArray::FFastArraySerializerOL
			{
				TArray<FVolumePlayerStateInfo>              Items;                                                    // 0x00B0(0x0010) (ZeroConstructor, Transient, NativeAccessSpecifierPrivate)
				UObject* Manager;                                                  // 0x00C0(0x0008) (ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
			};

			static auto VolumeObjectsOffset = (*VolumeManager)->GetOffset("VolumeObjects");
			Get<TArray<UObject*>>(*VolumeManager, VolumeObjectsOffset)->Add(NewVolume);

			static auto VolumeActivePlayersOffset = (*VolumeManager)->GetOffset("VolumeActivePlayers");
			auto VolumeActivePlayers = Get<FFortVolumeActiveUsers>(*VolumeManager, VolumeActivePlayersOffset);

			VolumeActivePlayers->Manager = *VolumeManager;
			FVolumePlayerStateInfo newinfo;
			newinfo.PlayerState = PlayerState;
			newinfo.Volume = NewVolume;
			VolumeActivePlayers->Items.Add(newinfo);
			FastTArray::MarkArrayDirty(VolumeActivePlayers);
		}

		/* static auto UpdateSize = FindObject<UFunction>("/Script/FortniteGame.FortVolume.UpdateSize");
		FVector NewSize = FVector{ 10000, 10000, 10000 };
		NewVolume->ProcessEvent(UpdateSize, &NewSize); */
	}

	bool bGoIntoWarmup = !Defines::bIsGoingToPlayMainEvent || Fortnite_Version == 14.60;

	std::cout << "bGoIntoWarmup: " << bGoIntoWarmup << '\n';

	if (bGoIntoWarmup)
	{
		auto OldPhase = *Get<EAthenaGamePhase>(GameState, GamePhaseOffset);

		*Get<EAthenaGamePhase>(GameState, GamePhaseOffset) = EAthenaGamePhase::Warmup;

		static auto OnRepGamePhase = FindObject<UFunction>("/Script/FortniteGame.FortGameStateAthena.OnRep_GamePhase");

		GameState->ProcessEvent(OnRepGamePhase, &OldPhase);
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
		auto OldGamePhase = *Get<EAthenaGamePhase>(GameState, GamePhaseOffset);

		*Get<EAthenaGamePhase>(GameState, GamePhaseOffset) = EAthenaGamePhase::None;

		static auto OnRep_GamePhase = FindObject<UFunction>("/Script/FortniteGame.FortGameStateAthena.OnRep_GamePhase");
		GameState->ProcessEvent(OnRep_GamePhase, &OldGamePhase);

		if (Defines::Playlist == "/Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground" && Fortnite_Version > 11.30)
			Defines::Playlist = "/Game/Athena/Playlists/BattleLab/Playlist_BattleLab.Playlist_BattleLab";

		UObject* Playlist = FindObject(Defines::Playlist);

		std::cout << "Setting playlist to: " << (Playlist ? Playlist->GetName() : "UNDEFINED") << '\n';

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
			Looting::Initialize();

		if (Playlist)
		{
			static auto MinPlayersOffset = Playlist->GetOffset("MinPlayers");
			// *Get<int>(Playlist, MinPlayersOffset) = 1;

			if (Fortnite_Version >= 13)
			{
				static auto LastSafeZoneIndexOffset = Playlist->GetOffset("LastSafeZoneIndex");

				if (LastSafeZoneIndexOffset != -1)
					*(int*)(__int64(Playlist) + LastSafeZoneIndexOffset) = 0;
			}
		}

		auto PlayersLeft = Helper::GetPlayersLeft();

		if (PlayersLeft)
			*PlayersLeft = 0;

		std::cout << "Ready to start match!\n";

		Calendar::FixLocations();

		Events::LoadEvent();

		if (Defines::bIsCreative)
			LoadObject(Helper::GetBGAClass(), "/Game/Playgrounds/Items/BGA_IslandPortal.BGA_IslandPortal_C"); // scuffed

		static auto DefaultGliderRedeployCanRedeployOffset = GameState->GetOffset("DefaultGliderRedeployCanRedeploy", false, false, false);

		if (DefaultGliderRedeployCanRedeployOffset != 0)
			*Get<bool>(GameState, DefaultGliderRedeployCanRedeployOffset) = Defines::bIsPlayground;

		Defines::bIsRestarting = false;
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

	auto DeathReport = (__int64*)Parameters;

	auto DeadPlayerState = Helper::GetPlayerStateFromController(DeadController);
	auto DeadPawn = Helper::GetPawnFromController(DeadController);

	static auto KillerPawnOffset = preoffsets::KillerPawn; // FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport", "KillerPawn");
	static auto KillerPlayerStateOffset = preoffsets::KillerPlayerState; // FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport", "KillerPlayerState");

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

	static auto FallDamageEnumValue = GetEnumValue(DeathCauseEnum, "FallDamage");
	
	// if (preoffsets::DeathTags != 0)
		// *Get<FGameplayTagContainer>(DeathInfo, preoffsets::DeathTags) = *Tags;

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

	static auto OnRep_DeathInfo = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateAthena.OnRep_DeathInfo");
	DeadPlayerState->ProcessEvent(OnRep_DeathInfo);

	auto PlayersLeftPtr = Helper::GetPlayersLeft();

	int beforePlayersLeft = 0;

	auto GameState = Helper::GetGameState();

	// auto TeamsLeftOffset = GameState->GetOffset("TeamsLeft");

	// auto TeamsLeft = *PlayersLeftPtr; // *Get<int>(GameState, TeamsLeftOffset);

	// std::cout << "TeamsLeft: " << TeamsLeft << '\n';

	static auto GamePhaseOffset = preoffsets::GamePhase; // GameState->GetOffset("GamePhase");
	auto OldPhase = *Get<EAthenaGamePhase>(GameState, GamePhaseOffset);

	auto GameMode = Helper::GetGameMode();
	static auto AlivePlayersOffset = preoffsets::AlivePlayers;

	auto AlivePlayers = (TArray<UObject*>*)(__int64(GameMode) + AlivePlayersOffset);

	if (IsBadReadPtr(PlayersLeftPtr))
	{
		if (!Defines::bIsPlayground && OldPhase > EAthenaGamePhase::Warmup)
		{
			beforePlayersLeft = *PlayersLeftPtr;
			(*PlayersLeftPtr)--;
		}
	}
	else
	{
		beforePlayersLeft = AlivePlayers->Num();
	}

	std::cout << "beforePlayersLeft: " << beforePlayersLeft << '\n';

	static auto PlaceOffset = preoffsets::Place; // DeadPlayerState->GetOffsetSlow("Place

	auto Place = Get<int>(DeadPlayerState, PlaceOffset);

	if (beforePlayersLeft - 1 <= 1) // && (int)Playlist->WinCondition <= 1
	{
		static auto ClientNotifyWon = FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerAthena.ClientNotifyTeamWon");

		UObject* FinishingWeaponDefinition = nullptr;

		static auto DamageCauserOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport"), ("DamageCauser"));

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

				static auto OnRep_Place = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateAthena.OnRep_Place");
				AlivePlayerPS->ProcessEvent(OnRep_Place);
			}
		}

		/* *Get<EAthenaGamePhase>(GameState, GamePhaseOffset) = EAthenaGamePhase::EndGame;

		static auto OnRepGamePhase = FindObject<UFunction>("/Script/FortniteGame.FortGameStateAthena.OnRep_GamePhase");
		GameState->ProcessEvent(OnRepGamePhase, &OldPhase);

		static auto EndGamePhaseStarted = FindObject<UFunction>("/Script/FortniteGame.FortGameModeAthena.EndGamePhaseStarted");

		if (EndGamePhaseStarted)
			Helper::GetGameMode()->ProcessEvent(EndGamePhaseStarted); */
	}

	*Place = beforePlayersLeft; // SKUNKED for teams

	static auto OnRep_Place = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateAthena.OnRep_Place");
	DeadPlayerState->ProcessEvent(OnRep_Place);

	// TODO Loop through teammembers and if they are all dead then remove 1 from TeamsLeft

	static auto TeamScoreOffset = preoffsets::TeamScore; // DeadPlayerState->GetOffsetSlow("TeamScore");

	if (TeamScoreOffset != 0)
	{
		auto TeamScore = Get<int>(DeadPlayerState, TeamScoreOffset);

		*TeamScore = beforePlayersLeft; // IDK

		static auto OnRep_TeamScore = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateAthena.OnRep_TeamScore");
		DeadPlayerState->ProcessEvent(OnRep_TeamScore);
	}

	static auto TeamScorePlacementOffset = preoffsets::TeamScorePlacement; // DeadPlayerState->GetOffsetSlow("TeamScorePlacement");

	if (TeamScorePlacementOffset != 0)
	{
		auto TeamScorePlacement = Get<int>(DeadPlayerState, TeamScorePlacementOffset);

		*TeamScorePlacement = beforePlayersLeft; // IDK

		static auto OnRep_TeamScorePlacement = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateAthena.OnRep_TeamScorePlacement");
		DeadPlayerState->ProcessEvent(OnRep_TeamScorePlacement);
	}

	static auto bMarkedAliveOffset = preoffsets::bMarkedAlive; // DeadController->GetOffset("bMarkedAlive");
	*Get<bool>(DeadController, bMarkedAliveOffset) = false;

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
	struct { bool bSuccess; FAthenaRewardResult res; } parm { true, FAthenaRewardResult(1500, 1200, TotalSeasonXpGained, 1400)}; // MatchReport->EndOfMatchResults

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
	teamStats.Place = *Place;
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

	auto DeathLocation = Helper::GetActorLocation(DeadPawn);

	DeathLocation.Describe();

	if (false)
	{
		static auto ChipClass = FindObject(("/Game/Athena/Items/EnvironmentalItems/SCMachine/BGA_Athena_SCMachine_Pickup.BGA_Athena_SCMachine_Pickup_C"));

		auto Chip = Helper::Easy::SpawnActor(ChipClass, DeathLocation, Helper::GetActorRotation(DeadPawn));

		if (Chip && DeadPlayerState)
		{
			Helper::InitializeBuildingActor(DeadController, Chip);

			static auto PS_SquadIdOffset = FindOffsetStruct("Class /Script/FortniteGame.FortPlayerStateAthena", "SquadId", true);
			auto PS_SquadId = Get<int>(DeadPlayerState, PS_SquadIdOffset);

			static auto SquadIdOffset = FindOffsetStruct("Class /Script/FortniteGame.BuildingGameplayActorSpawnChip", "SquadId", true); // Chip->GetOffset("SquadId");
			*Get<uint8_t>(Chip, SquadIdOffset) = *PS_SquadId;

			static auto OwnerTeamOffset = Chip->GetOffset("OwnerTeam");
			*Get<uint8_t>(Chip, OwnerTeamOffset) = *PS_SquadId;

			static auto ChipExpirationLengthOffset = Chip->GetOffset("ChipExpirationLength");
			Get<FScalableFloat>(Chip, ChipExpirationLengthOffset)->Value = 100;

			static auto SetReplicateMovementFn = FindObject<UFunction>("/Script/Engine.Actor.SetReplicateMovement");
			bool bTrue = true;
			Chip->ProcessEvent(SetReplicateMovementFn, &bTrue);

			static auto ProjectileMovementComponentClass = FindObject("/Script/Engine.ProjectileMovementComponent"); // UFortProjectileMovementComponent

			static auto ProjectileMovementOffset = Chip->GetOffset("ProjectileMovement");
			auto ProjectileMovement = Get<UObject*>(Chip, ProjectileMovementOffset);
			std::cout << "bef: " << *ProjectileMovement << '\n';
			*ProjectileMovement = Helper::Easy::SpawnObject(ProjectileMovementComponentClass, Chip);
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
		auto KillerController = Helper::GetControllerFromPawn(KillerPawn);

		static auto KillScoreOffset = preoffsets::KillScore; // KillerPlayerState->GetOffset("KillScore");
		(*Get<int>(KillerPlayerState, KillScoreOffset))++;

		static auto ClientReceiveKillNotification = FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerPvP.ClientReceiveKillNotification") ?
			FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerPvP.ClientReceiveKillNotification") : FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerAthena.ClientReceiveKillNotification");

		std::cout << "ClientReceiveKillNotification: " << ClientReceiveKillNotification << '\n';

		struct {
			// Both playerstates
			UObject* Killer;
			UObject* Killed;
		} ClientReceiveKillNotification_Params{ KillerPlayerState, DeadPlayerState };
		KillerController->ProcessEvent(ClientReceiveKillNotification, &ClientReceiveKillNotification_Params);

		static auto ClientReportKill = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateAthena.ClientReportKill");
		KillerPlayerState->ProcessEvent(ClientReportKill, &DeadPlayerState);
	}

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

	if ((int)Phase == 3 && Defines::bIsLateGame)
	{
		std::cout << "Nice!\n";

		static auto BuildingFoundationClass = FindObject("/Script/FortniteGame.BuildingFoundation");

		auto AllBuildingFoundations = Helper::GetAllActorsOfClass(BuildingFoundationClass);

		UObject* Foundation = nullptr;

		while (!Foundation)
		{
			auto random = rand();

			if (random >= 1)
				Foundation = AllBuildingFoundations.At(random % (AllBuildingFoundations.Num()));
		}

		auto GameState = Helper::GetGameState();

		static auto AircraftsOffset = GameState->GetOffset("Aircrafts");
		auto Aircrafts = (TArray<UObject*>*)(__int64(GameState) + AircraftsOffset);

		auto Aircraft = Aircrafts->At(0);

		if (!Aircraft)
		{
			std::cout << "No aircraft!\n";
			return false;
		}

		static auto FlightInfoOffset = Aircraft->GetOffset("FlightInfo");
		auto FlightInfo = Get<__int64>(Aircraft, FlightInfoOffset);

		static auto FlightStartLocationOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.AircraftFlightInfo", "FlightStartLocation");
		*(FVector*)(__int64(FlightInfo) + FlightStartLocationOffset) = Helper::GetActorLocation(Foundation) + FVector{ 0, 0, 10000 };

		static auto FlightSpeedOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.AircraftFlightInfo", "FlightSpeed");
		*(float*)(__int64(FlightInfo) + FlightSpeedOffset) = 0;

		FString StartSafeZone = L"startsafezone";
		Helper::ExecuteConsoleCommand(StartSafeZone);

		static auto SafeZonesStartTimeOffset = GameState->GetOffset("SafeZonesStartTime");
		*Get<float>(GameState, SafeZonesStartTimeOffset) = 0.f;
	}

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

				static auto SizeOfSetWorldTransform = Helper::GetSizeOfClass(SetWorldTransform);

				// std::cout << "SizeOfSetWorldTransform: " << SizeOfSetWorldTransform << '\n';

				auto params = malloc(SizeOfSetWorldTransform);

				if (params)
				{
					/*
					
						FTransform                                  NewTransform;                                             // (ConstParm, Parm, OutParm, ReferenceParm, IsPlainOldData)
						bool                                               bSweep;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
						struct FHitResult                                  SweepHitResult;                                           // (Parm, OutParm, IsPlainOldData)
						bool                                               bTeleport;

					*/

					static auto NewTransformOffset = FindOffsetStruct2("/Script/Engine.SceneComponent.K2_SetWorldTransform", "NewTransform", false, true);
					auto NewTransform = (FTransform*)(__int64(params) + NewTransformOffset);
					
					// auto Quaternion = *Rotation; // Helper::GetActorRotation(Vehicle);
					// auto Rotator = Quaternion.Rotator();

					// auto Rotator = Helper::GetActorRotation(Vehicle);
					// auto Quaternion = Rotator.Quaternion();

					/* auto wrongRot = *Rotation;
					auto Rotator = wrongRot.Rotator();
					std::cout << "Before: ";
					Rotator.Describe();
					Rotator = { Rotator.Pitch, Rotator.Roll, Rotator.Yaw }; */
					auto Rotator = Helper::GetActorRotation(Vehicle);
					// std::cout << "After Rot: ";
					// Rotator.Describe();
					auto Quaternion = Rotator.Quaternion();

					// std::cout << "Quat: ";
					// Quaternion.Describe();

					NewTransform->Translation = *Translation;
					NewTransform->Rotation = Quaternion; // *Rotation;
					NewTransform->Scale3D = { 1, 1, 1 };

					bool bShouldTeleport = false;

					static auto bTeleportOffset = FindOffsetStruct2("/Script/Engine.SceneComponent.K2_SetWorldTransform", "bTeleport", false, true);
					auto bTeleport = (bool*)(__int64(params) + bTeleportOffset);
					*bTeleport = bShouldTeleport;

					/* static auto bSweepOffset = FindOffsetStruct2("/Script/Engine.SceneComponent.K2_SetWorldTransform", "bSweep", false, true);
					auto bSweep = (bool*)(__int64(params) + bSweepOffset);
					*bSweep = true; // col;lision stuff */

					/* struct FVehicleSafeTeleportInfo
					{
						FVector                                     Location;                                                 // 0x0000(0x000C) (ZeroConstructor, IsPlainOldData)
						FRotator                                    Rotation;                                                 // 0x000C(0x000C) (ZeroConstructor, IsPlainOldData)
					};

					static auto SafeTeleportInfoOffset = Vehicle->GetOffset("SafeTeleportInfo");
					auto SafeTeleportInfo = Get<FVehicleSafeTeleportInfo>(Vehicle, SafeTeleportInfoOffset);
					SafeTeleportInfo->Location = NewTransform->Translation;
					SafeTeleportInfo->Rotation = Rotator;

					static auto OnRep_SafeTeleportInfo = FindObject<UFunction>("/Script/FortniteGame.FortPhysicsPawn:OnRep_SafeTeleportInfo");
					Vehicle->ProcessEvent(OnRep_SafeTeleportInfo); */
				}

				/* static auto SetPhysicsLinearVelocity = FindObject<UFunction>("/Script/Engine.PrimitiveComponent.SetPhysicsLinearVelocity");

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

				RootComp->ProcessEvent(SetPhysicsAngularVelocity, &SetPhysicsAngularVelocity_Params); */
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

	auto TeamIDX = Helper::GetTeamIndex(Helper::GetPlayerStateFromController(Controller));

	std::cout << "TeamIDX: " << *TeamIDX << '\n';

	static auto SquadIdOffset = FindOffsetStruct("Class /Script/FortniteGame.FortPlayerStateAthena", "SquadId", true);

	auto PlayerState = Helper::GetPlayerStateFromController(Controller);

	auto TeamIndexPtr = Helper::GetTeamIndex(Helper::GetPlayerStateFromController(Controller));
	auto SquadIdPtr = Get<int>(PlayerState, SquadIdOffset);

	std::cout << "SquadIdPtr: " << *SquadIdPtr << '\n';

	// if (*TeamIDX <= 3)

	Teams::AssignTeam(Controller);

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

	auto Pawn = Helper::GetPawnFromController(Controller);

	if (!Pawn)
		return false;

	static auto fn = FindObject<UFunction>("/Script/FortniteGame.FortMontageItemDefinitionBase.GetAnimationHardReference");

	void* parmaqi2 = nullptr;

	struct
	{
	public:
		EFortCustomBodyType               BodyType;                                          // 0x0(0x1)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		EFortCustomGender                 Gender;                                            // 0x1(0x1)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		uint8_t                                        Pad_2129[0x6];                                     // Fixing Size After Last Property  [ Dumper-7 ]
		UObject* PawnContext;                                       // 0x8(0x8)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		UObject* ReturnValue;                                       // 0x10(0x8)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	} ada{ EFortCustomBodyType::All, EFortCustomGender::Both };

	struct {
		TEnumAsByte<EFortCustomBodyType> BodyType;
		TEnumAsByte<EFortCustomGender> Gender;
		UObject* AnimMontage; // UAnimMontage
	} GAHRParams{ EFortCustomBodyType::All, EFortCustomGender::Both }; // (CurrentPawn->CharacterBodyType, CurrentPawn->CharacterGender)

	if (Fortnite_Version == 14.60)
	{
		ada.PawnContext = Pawn;

		parmaqi2 = &ada;
	}
	else
	{
		parmaqi2 = &GAHRParams;
	}

	EmoteAsset->ProcessEvent(fn, parmaqi2);

	auto Montage = Fortnite_Version == 14.60 ? ada.ReturnValue : GAHRParams.AnimMontage;

	std::cout << "Montage: " << Montage << '\n';

	if (!Montage)
		return false;

	static auto PlayAnimMontage = FindObject<UFunction>("/Script/Engine.Character.PlayAnimMontage");

	struct
	{
		UObject* NewAnimMontage;                                           // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		float                                              InPlayRate;                                               // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		FName                                       StartSectionName;                                         // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		float                                              ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	} ACharacter_PlayAnimMontage_Params{GAHRParams.AnimMontage, 1.0f, FName(0)};

	Pawn->ProcessEvent(PlayAnimMontage, &ACharacter_PlayAnimMontage_Params);

	std::cout << "skidder: " << ACharacter_PlayAnimMontage_Params.ReturnValue << '\n';

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
			!strstr(FunctionName.c_str(), "BlueprintThreadSafeUpdateAnimation"))
		{
			std::cout << ("Function called: ") << FunctionName << '\n';
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