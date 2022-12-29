#include "helper.h"
#include "inventory.h"
#include <format>
#include "server.h"

UObject* Helper::Easy::SpawnActor(UObject* Class, FVector Location, FRotator Rotation, UObject* Owner)
{
	bool bUseDouble = Fortnite_Version >= 20.00;
	bool bUseNewSpawnParameters = Engine_Version >= 500;
	auto quat = Rotation.Quaternion();

	if (!bUseDouble)
	{
		void* spawnparms;

		FActorSpawnParameters SpawnParameters{};
		FActorSpawnParametersNew SpawnParametersnew{};

		if (!bUseNewSpawnParameters)
		{
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			SpawnParameters.Owner = Owner;
			spawnparms = &SpawnParameters;
		}
		else
		{
			SpawnParametersnew.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			SpawnParametersnew.Owner = Owner;
			spawnparms = &SpawnParametersnew;
		}

		FTransform transform = FTransform(quat, Location);

		return SpawnActorO ? SpawnActorO(Helper::GetWorld(), Class, &Location, &Rotation, &SpawnParameters)
			: SpawnActorTransform(Helper::GetWorld(), Class, &transform, &SpawnParameters);
	}
	else
	{
		FActorSpawnParametersNew SpawnParameters{};
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParameters.Owner = Owner;

		auto DLocation = Location.ToDouble();
		auto DQuat = quat.ToDouble();

		DTransform transform = DTransform(DQuat, DLocation);

		return SpawnActorO ? SpawnActorO(Helper::GetWorld(), Class, &Location, &Rotation, &SpawnParameters)
			: SpawnActorTransform(Helper::GetWorld(), Class, &transform, &SpawnParameters);
	}
}

UObject* Helper::Easy::SpawnActorDynamic(UObject* Class, BothVector Location, BothRotator Rotation, UObject* Owner)
{
	FActorSpawnParametersNew SpawnParametersNew{};
	SpawnParametersNew.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParametersNew.Owner = Owner;

	FActorSpawnParameters SpawnParameters{};
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParameters.Owner = Owner;

	if (Fortnite_Season >= 20)
	{
		return SpawnActorO(Helper::GetWorld(), Class, &Location.dV, &Rotation.dR, &SpawnParametersNew);
	}
	else
	{
		if (Engine_Version >= 500)
			return SpawnActorO(Helper::GetWorld(), Class, &Location.fV, &Rotation.fR, &SpawnParametersNew);
		else
			return SpawnActorO(Helper::GetWorld(), Class, &Location.fV, &Rotation.fR, &SpawnParameters);
	}
}

UObject* Helper::Easy::SpawnObject(UObject* Class, UObject* Outer)
{
	if (!Class || !Outer)
		return nullptr;

	struct {
		UObject* ObjectClass;
		UObject* Outer;
		UObject* ReturnValue;
	} params{ Class, Outer };

	static auto GSC = FindObject(("/Script/Engine.Default__GameplayStatics"));
	static auto fn = FindObject<UFunction>("/Script/Engine.GameplayStatics.SpawnObject");

	GSC->ProcessEvent(fn, &params);

	return params.ReturnValue;
}

float Helper::GetTimeSeconds()
{
	static auto GSCClass = FindObject(("/Script/Engine.Default__GameplayStatics"));

	struct { UObject* world; float timeseconds; } parms{ GetWorld() };

	static auto GetTimeSeconds = FindObject<UFunction>("/Script/Engine.GameplayStatics.GetTimeSeconds");

	GSCClass->ProcessEvent(GetTimeSeconds, &parms);

	return parms.timeseconds;
}

bool Helper::IsPlayerController(UObject* Object)
{
	static auto PlayerControllerClass = FindObject(("/Game/Athena/Athena_PlayerController.Athena_PlayerController_C"));

	return Object->IsA(PlayerControllerClass);
}

void Helper::ExecuteConsoleCommand(FString Command)
{
	struct { UObject* WorldContextObject; FString Command; UObject* SpecificPlayer; } params{ Helper::GetWorld(), Command, nullptr };

	static auto KSLClass = FindObject("/Script/Engine.Default__KismetSystemLibrary");

	static auto ExecuteConsoleCommandFn = FindObject<UFunction>("/Script/Engine.KismetSystemLibrary.ExecuteConsoleCommand");
	KSLClass->ProcessEvent(ExecuteConsoleCommandFn, &params);
}

std::pair<UObject*, int> Helper::GetAmmoForDefinition(UObject* Definition)
{
	static auto GetAmmoWorldItemDefinition_BP = FindObject<UFunction>("/Script/FortniteGame.FortWorldItemDefinition.GetAmmoWorldItemDefinition_BP");
	UObject* AmmoDef;
	Definition->ProcessEvent(GetAmmoWorldItemDefinition_BP, &AmmoDef);
	
	static auto FortWorldItemDefinitionClass = FindObject("/Script/FortniteGame.FortWorldItemDefinition");

	int DropCount = 0;

	// if (AmmoDef->IsA(FortWorldItemDefinitionClass))
	if (AmmoDef)
	{
		static auto DropCountOffset = AmmoDef->GetOffset("DropCount");

		DropCount = *(int*)(__int64(AmmoDef) + DropCountOffset);
	}

	return std::make_pair(AmmoDef, DropCount);
}

UObject* Helper::GetWorld()
{
	static auto Engine = GetEngine();

	static auto GameViewportOffset = Engine->GetOffset("GameViewport");
	auto GameViewport = *Get<UObject*>(Engine, GameViewportOffset);

	static auto PropertyClass = FindObject("/Script/CoreUObject.Property");

	static auto WorldOffset = GameViewport->GetOffsetSlow("World");
	// std::cout << "WorldOffset: " << WorldOffset << '\n';

	return *Get<UObject*>(GameViewport, WorldOffset);
}

UObject* Helper::GetTransientPackage()
{
	static auto TransientPackage = FindObject("/Engine/Transient");
	return TransientPackage;
}

UObject* Helper::GetEngine()
{
	static auto Engine = FindObjectSlow("FortEngine_");

	return Engine;
}

UObject* Helper::GetGameMode()
{
	auto World = GetWorld();

	static auto AuthorityGameModeOffset = World->GetOffset("AuthorityGameMode");
	auto AuthorityGameMode = *Get<UObject*>(World, AuthorityGameModeOffset);

	return AuthorityGameMode;
}

UObject* Helper::GetGameState()
{
	auto GameMode = Helper::GetGameMode();

	static auto GameStateOffset = GameMode->GetOffset("GameState");
	auto GameState = *Get<UObject*>(GameMode, GameStateOffset);

	return GameState;
}

UObject* Helper::GetLocalPlayerController()
{
	auto Engine = GetEngine();

	static auto GameInstanceOffset = Engine->GetOffset("GameInstance");
	auto GameInstance = *Get<UObject*>(Engine, GameInstanceOffset);

	if (!GameInstance)
		return nullptr;

	static auto LocalPlayersOffset = GameInstance->GetOffset("LocalPlayers");
	auto& LocalPlayers = *Get<TArray<UObject*>>(GameInstance, LocalPlayersOffset);

	auto LocalPlayer = LocalPlayers.At(0);
	
	if (!LocalPlayer)
		return nullptr;

	static auto PlayerControllerOffset = LocalPlayer->GetOffset("PlayerController");

	return *Get<UObject*>(LocalPlayer, PlayerControllerOffset);
}

UObject* Helper::GetPlayerStateFromController(UObject* Controller)
{
	static auto PlayerStateOffset = Controller->GetOffset("PlayerState");

	return *Get<UObject*>(Controller, PlayerStateOffset);
}

UObject* Helper::GetControllerFromPawn(UObject* Pawn)
{
	static auto ControllerOffset = Pawn->GetOffset("Controller");

	return *Get<UObject*>(Pawn, ControllerOffset);
}

UObject* Helper::GetPawnFromController(UObject* Controller)
{
	static auto PawnOffset = Controller->GetOffsetSlow("Pawn");

	return *Get<UObject*>(Controller, PawnOffset);
}

float Helper::GetDistanceTo(UObject* Actor, UObject* OtherActor)
{
	struct { UObject* otherActor; float distance; } GetDistanceTo_Params{ OtherActor };

	static auto GetDistanceTo = FindObject<UFunction>("/Script/Engine.Actor.GetDistanceTo");
	Actor->ProcessEvent(GetDistanceTo, &GetDistanceTo_Params);

	return GetDistanceTo_Params.distance;
}

bool Helper::ApplyCID(UObject* Pawn, UObject* CID)
{
	// CID->ItemVariants
	// CID->DefaultBackpack

	// CID->Hero->Specialization

	if (!CID || !StaticLoadObjectO) 
		return false;

	static auto CCPClass = FindObject("/Script/FortniteGame.CustomCharacterPart");

	static auto HeroDefinitionOffset = CID->GetOffset("HeroDefinition");

	auto HeroDefinition = *(UObject**)(__int64(CID) + HeroDefinitionOffset);

	if (!HeroDefinition)
		return false;

	static auto SpecializationsOffset = HeroDefinition->GetOffset("Specializations");
	auto HeroSpecializations = (TArray<TSoftObjectPtr>*)(__int64(HeroDefinition) + SpecializationsOffset);

	if (!HeroSpecializations)
	{
		std::cout << "No HeroSpecializations!\n";
		return false;
	}

	bool bSuceeded = false;

	for (int j = 0; j < HeroSpecializations->Num(); j++)
	{
		static auto SpecializationClass = FindObject("/Script/FortniteGame.FortHeroSpecialization");

		auto SpecializationName = HeroSpecializations->At(j).ObjectID.AssetPathName.ToString();

		auto Specialization = StaticLoadObject(SpecializationClass, nullptr, SpecializationName);

		if (!Specialization)
			continue;

		static auto CharacterPartsOffset = Specialization->GetOffset("CharacterParts");
		auto CharacterParts = (TArray<TSoftObjectPtr>*)(__int64(Specialization) + CharacterPartsOffset);

		if (!CharacterParts)
		{
			std::cout << "No CharacterParts!\n";
			return false;
		}

		for (int i = 0; i < CharacterParts->Num(); i++)
		{
			auto CharacterPart = StaticLoadObject(CCPClass, nullptr, CharacterParts->At(i).ObjectID.AssetPathName.ToString());

			if (CharacterPart)
			{
				static auto CharacterPartTypeOffset = CharacterPart->GetOffset("CharacterPartType");
				auto PartType = *(TEnumAsByte<EFortCustomPartType>*)(__int64(CharacterPart) + CharacterPartTypeOffset);
				Helper::ChoosePart(Pawn, PartType, CharacterPart);
				bSuceeded = true;
			}
		}
	}

	return bSuceeded;
}

UObject* Helper::GetRandomCID()
{
	static auto CIDClass = FindObject("/Script/FortniteGame.AthenaCharacterItemDefinition");

	static auto AllObjects = Helper::GetAllObjectsOfClass(CIDClass);

	UObject* skin = nullptr; // AllObjects.at(random);

	while (!skin || skin->GetFullName().contains("Default") || skin->GetFullName().contains("Test"))
	{
		auto random = (int)GetRandomFloat(1, AllObjects.size() - 1);
		random = random <= 0 ? 1 : random; // we love default objects
		skin = AllObjects.at(random);
	}

	return skin;
}

float Helper::GetMaxHealth(UObject* BuildingActor)
{
	static auto GetMaxHealth = FindObject<UFunction>("/Script/FortniteGame.BuildingActor.GetMaxHealth");
	float MaxHealth = 0.f;
	BuildingActor->ProcessEvent(GetMaxHealth, &MaxHealth);

	return MaxHealth;
}

UObject* Helper::SpawnPawn(UObject* Controller, BothVector Location, bool bAssignCharacterParts)
{
	static auto PawnClass = FindObject("/Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C");

	auto Pawn = Helper::Easy::SpawnActorDynamic(PawnClass, Location);

	if (!Pawn)
		return Pawn;

	static auto Possess = FindObject<UFunction>("/Script/Engine.Controller.Possess");
	Controller->ProcessEvent(Possess, &Pawn);

	if (bAssignCharacterParts)
	{
		if (false)
		{
			static auto headPart = FindObject("/Game/Characters/CharacterParts/Female/Medium/Heads/F_Med_Head1.F_Med_Head1");
			static auto bodyPart = FindObject("/Game/Characters/CharacterParts/Female/Medium/Bodies/F_Med_Soldier_01.F_Med_Soldier_01");

			if (!headPart)
				headPart = FindObject("/Game/Characters/CharacterParts/Female/Medium/Heads/CP_Head_F_RebirthDefaultA.CP_Head_F_RebirthDefaultA");

			if (!bodyPart)
				bodyPart = FindObject("/Game/Athena/Heroes/Meshes/Bodies/CP_Body_Commando_F_RebirthDefaultA.CP_Body_Commando_F_RebirthDefaultA");

			ChoosePart(Pawn, EFortCustomPartType::Head, headPart);
			ChoosePart(Pawn, EFortCustomPartType::Body, bodyPart);
		}
		else
		{
			ApplyCID(Pawn, Helper::GetRandomCID());
		}
	}

	static auto FortPlayerControllerZoneClass = FindObject("/Script/FortniteGame.FortPlayerControllerZone");

	if (Controller->IsA(FortPlayerControllerZoneClass))
	{
		static auto ClientOnPawnSpawned = FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerZone.ClientOnPawnSpawned");

		if (ClientOnPawnSpawned)
			Controller->ProcessEvent(ClientOnPawnSpawned); // IDK

		SetHealth(Pawn, 100);

		if (Engine_Version <= 420)
		{
			SetMaxHealth(Pawn, 100);
			SetMaxShield(Pawn, 100);
			SetShield(Pawn, 0);
		}

		if (Fortnite_Season >= 16)
		{
			static auto stormeffect = FindObject("/Game/Athena/SafeZone/GE_OutsideSafeZoneDamage.GE_OutsideSafeZoneDamage_C");
			Helper::RemoveGameplayEffect(Pawn, stormeffect);
		}
	}

	return Pawn;
}

void Helper::ChoosePart(UObject* Pawn, TEnumAsByte<EFortCustomPartType> Part, UObject* ChosenCharacterPart)
{
	/* if (Fortnite_Version == 19.10)
	{
		struct FCustomCharacterData
		{
			unsigned char                                      WasPartReplicatedFlags;                                   // 0x0000(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			unsigned char                                      UnknownData00[0x3];                                       // 0x0001(0x0003) MISSED OFFSET
			int                                                RequiredVariantPartFlags;                                 // 0x0004(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			UObject* Parts[0x7];                                               // 0x0008(0x0008) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			UObject* Charms[0x4];                                              // 0x0040(0x0008) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			TArray<UObject*>                VariantRequiredCharacterParts;                            // 0x0060(0x0010) (ZeroConstructor, NativeAccessSpecifierPublic)
			bool                                               bReplicationFailed;                                       // 0x0070(0x0001) (ZeroConstructor, Transient, IsPlainOldData, RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
			unsigned char                                      UnknownData01[0x7];                                       // 0x0071(0x0007) MISSED OFFSET
		};

		auto PlayerState = Helper::GetPlayerStateFromController(Helper::GetControllerFromPawn(Pawn));

		if (PlayerState)
		{
			auto CharacterDataOffset = PlayerState->GetOffset("CharacterData");
			auto CharacterData = Get<FCustomCharacterData>(PlayerState, CharacterDataOffset);

			CharacterData->Parts[(int)Part.Get()] = ChosenCharacterPart;

			static auto OnRep_CharacterData = FindObject<UFunction>("/Script/FortniteGame.FortPlayerState.OnRep_CharacterData");
			PlayerState->ProcessEvent(OnRep_CharacterData);
		}
	}
	else */
	{
		struct { TEnumAsByte<EFortCustomPartType> Part; UObject* ChosenCharacterPart; } SCP_params{ Part, ChosenCharacterPart };

		static auto ServerChoosePart = FindObject<UFunction>("/Script/FortniteGame.FortPlayerPawn.ServerChoosePart");

		Pawn->ProcessEvent(ServerChoosePart, &SCP_params);
	}
}

void Helper::SetOwner(UObject* Actor, UObject* Owner)
{
	static auto SetOwner = FindObject<UFunction>("/Script/Engine.Actor.SetOwner");

	Actor->ProcessEvent(SetOwner, &Owner);
}

UObject* Helper::GetAbilitySystemComponent(UObject* Pawn)
{
	static auto AbilitySystemComponentOffset = Pawn->GetOffset("AbilitySystemComponent");

	return *Get<UObject*>(Pawn, AbilitySystemComponentOffset);
}

UObject* Helper::GetAbilitySystemComponentFromPS(UObject* PlayerState)
{
	static auto AbilitySystemComponentOffset = PlayerState->GetOffset("AbilitySystemComponent");
	return *Get<UObject*>(PlayerState, AbilitySystemComponentOffset);
}

void Helper::InitializeBuildingActor(UObject* Controller, UObject* BuildingActor, bool bUsePlayerBuildAnimations, UObject* ReplacedBuilding)
{
	struct {
		UObject* BuildingOwner; // ABuildingActor
		UObject* SpawningController;
		bool bUsePlayerBuildAnimations; // I think this is not on some versions
		UObject* ReplacedBuilding; // this also not on like below 18.00
	} IBAParams{ BuildingActor, Controller, bUsePlayerBuildAnimations, ReplacedBuilding };

	static auto fn = FindObject<UFunction>("/Script/FortniteGame.BuildingActor.InitializeKismetSpawnedBuildingActor");
	BuildingActor->ProcessEvent(fn, &IBAParams);
}

UObject** Helper::GetPlaylist()
{
	auto GameState = Helper::GetGameState();

	static auto CurrentPlaylistDataOffset = GameState->GetOffset("CurrentPlaylistData", false, false, false);

	// if (Fortnite_Version >= 6.10) // 6.00
	if (CurrentPlaylistDataOffset == 0)
	{
		static auto BasePlaylistOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.PlaylistPropertyArray", ("BasePlaylist"));

		if (BasePlaylistOffset)
		{
			static auto CurrentPlaylistInfoOffset = GameState->GetOffset("CurrentPlaylistInfo");
			auto PlaylistInfo = (void*)(__int64(GameState) + CurrentPlaylistInfoOffset); // gameState->Member<void>(("CurrentPlaylistInfo"));

			auto BasePlaylist = (UObject**)(__int64(PlaylistInfo) + BasePlaylistOffset);// *gameState->Member<UObject>(("CurrentPlaylistInfo"))->Member<UObject*>(("BasePlaylist"), true);

			return BasePlaylist;
		}
	}
	else
	{
		auto PlaylistData = (UObject**)(__int64(GameState) + CurrentPlaylistDataOffset);

		return PlaylistData;
	}

	return nullptr;
}

TArray<UObject*> Helper::GetAllActorsOfClass(UObject* Class)
{
	static auto GetAllActorsOfClass = FindObject<UFunction>("/Script/Engine.GameplayStatics.GetAllActorsOfClass");
	static auto DefaultGameplayStatics = FindObject("/Script/Engine.Default__GameplayStatics");

	struct { UObject* World; UObject* Class; TArray<UObject*> Array; } GetAllActorsOfClass_Params{GetWorld(), Class};

	DefaultGameplayStatics->ProcessEvent(GetAllActorsOfClass, &GetAllActorsOfClass_Params);

	auto Ret = GetAllActorsOfClass_Params.Array; // Array.Data ? Array.ToVector() : std::vector<UObject*>();

	// Array.Free();

	return Ret;
}

bool Helper::IsInAircraft(UObject* Controller)
{
	static auto IsInAircraftFn = FindObject<UFunction>("/Script/FortniteGame.FortPlayerController.IsInAircraft") ? FindObject<UFunction>("/Script/FortniteGame.FortPlayerController.IsInAircraft")
		: FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerAthena.IsInAircraft");

	bool bIsInAircraft = false;
	Controller->ProcessEvent(IsInAircraftFn, &bIsInAircraft);

	return bIsInAircraft;
}

UObject* Helper::GetCurrentWeapon(UObject* Pawn)
{
	static auto CurrentWeaponOffset = Pawn->GetOffset("CurrentWeapon");

	return *Get<UObject*>(Pawn, CurrentWeaponOffset);
}

UObject* Helper::GetWeaponData(UObject* Weapon)
{
	static auto WeaponDataOffset = Weapon->GetOffset("WeaponData");

	return *Get<UObject*>(Weapon, WeaponDataOffset);
}

int* Helper::GetTeamIndex(UObject* PlayerState)
{
	static auto TeamIndexOffset = FindOffsetStruct("Class /Script/FortniteGame.FortPlayerStateAthena", "TeamIndex", true);

	return Get<int>(PlayerState, TeamIndexOffset);
}

FVector Helper::GetActorLocation(UObject* Actor)
{
	static auto K2_GetActorLocationFN = FindObject<UFunction>("/Script/Engine.Actor.K2_GetActorLocation");

	FVector loc;
	Actor->ProcessEvent(K2_GetActorLocationFN, &loc);

	return loc;
}

FRotator Helper::GetActorRotation(UObject* Actor)
{
	static auto K2_GetActorRotation = FindObject<UFunction>("/Script/Engine.Actor.K2_GetActorRotation");

	FRotator loc;
	Actor->ProcessEvent(K2_GetActorRotation, &loc);

	return loc;
}

BothVector Helper::GetActorLocationDynamic(UObject* Actor)
{
	if (Fortnite_Season < 20)
		return BothVector(GetActorLocation(Actor));

	static auto K2_GetActorLocationFN = FindObject<UFunction>("/Script/Engine.Actor.K2_GetActorLocation");

	DVector loc;
	Actor->ProcessEvent(K2_GetActorLocationFN, &loc);

	return BothVector(loc);
}

BothRotator Helper::GetActorRotationDynamic(UObject* Actor)
{
	if (Fortnite_Season < 20)
		return BothRotator(GetActorRotation(Actor));

	static auto K2_GetActorRotation = FindObject<UFunction>("/Script/Engine.Actor.K2_GetActorRotation");

	DRotator loc;
	Actor->ProcessEvent(K2_GetActorRotation, &loc);

	return BothRotator(loc);
}

__int64* Helper::GetEntryFromPickup(UObject* Pickup)
{
	static auto PrimaryPickupItemEntryOffset = Pickup->GetOffset("PrimaryPickupItemEntry");
	auto PrimaryPickupItemEntry = Get<__int64>(Pickup, PrimaryPickupItemEntryOffset);

	return PrimaryPickupItemEntry;
}

UObject* Helper::GetOwnerOfComponent(UObject* Component)
{
	static auto fn = FindObject<UFunction>("/Script/Engine.ActorComponent.GetOwner");

	UObject* Owner = nullptr;
	Component->ProcessEvent(fn, &Owner);

	return Owner;
}

UObject* Helper::GetOwner(UObject* Actor)
{
	static auto fn = FindObject<UFunction>("/Script/Engine.Actor.GetOwner");

	UObject* Owner = nullptr;
	Actor->ProcessEvent(fn, &Owner);

	return Owner;
}

int Helper::GetMaxBullets(UObject* Definition)
{
	static auto FortWeaponItemDefinitionClass = FindObject("/Script/FortniteGame.FortWeaponItemDefinition");
	auto IsFortWeaponItemDefinition = Definition->IsA(FortWeaponItemDefinitionClass);

	static auto FortGadgetItemDefinitionClass = FindObject("/Script/FortniteGame.FortGadgetItemDefinition");
	auto IsFortGadgetItemDefinition = Definition->IsA(FortGadgetItemDefinitionClass);

	if (!Definition || IsFortGadgetItemDefinition || !IsFortWeaponItemDefinition)
		return 0;

	struct FDataTableRowHandle { UObject* DataTable; FName RowName; };

	static auto WeaponStatHandleOffset = Definition->GetOffset("WeaponStatHandle");
	auto statHandle = (FDataTableRowHandle*)(__int64(Definition) + WeaponStatHandleOffset);

	if (!statHandle || !statHandle->DataTable || !statHandle->RowName.ComparisonIndex)
		return 0;

	auto RangedWeaponsTable = statHandle->DataTable;

	if (!RangedWeaponsTable)
		return 0;

	// auto RangedWeaponRows = GetRowMap(RangedWeaponsTable);

	static auto RowStructOffset = RangedWeaponsTable->GetOffset("RowStruct");
	auto RangedWeaponRows = *(TMap<FName, uint8_t*>*)(__int64(RangedWeaponsTable) + (RowStructOffset + sizeof(UObject*))); // because after rowstruct is rowmap

	static auto ClipSizeOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortBaseWeaponStats", "ClipSize");

	// std::cout << "Number of RangedWeapons: " << RangedWeaponRows.Pairs.Elements.Data.Num() << '\n';

	{
		for (int i = 0; i < RangedWeaponRows.Pairs.Elements.Data.Num(); i++)
		{
			auto Man = RangedWeaponRows.Pairs.Elements.Data.At(i);
			auto& Pair = Man.ElementData.Value;
			auto RowFName = Pair.First;

			if (!RowFName.ComparisonIndex)
				continue;

			// if (RowFName.ToString() == statHandle->RowName.ToString())
			if (RowFName.ComparisonIndex == statHandle->RowName.ComparisonIndex)
			{
				auto data = Pair.Second;
				auto ClipSize = *(int*)(__int64(data) + ClipSizeOffset);
				return ClipSize;
			}
		}
	}

	return 0;
}

UObject* Helper::GetPawnFromPlayerState(UObject* PlayerState)
{
	static auto PawnPrivateOffset = PlayerState->GetOffset("PawnPrivate");
	auto PawnPrivate = *Get<UObject*>(PlayerState, PawnPrivateOffset);

	return PawnPrivate;
}

UObject* Helper::GetPickaxeDef(UObject* Controller, bool bGetNew)
{
	UObject* toRet = nullptr;

	if (bGetNew)
	{
		static auto PickaxeClass = FindObject("/Script/FortniteGame.AthenaPickaxeItemDefinition");
		auto randPickaxeDef = GetRandomObjectOfClass(PickaxeClass);
		
		if (randPickaxeDef)
		{
			static auto WeaponDefinitionOffset = randPickaxeDef->GetOffset("WeaponDefinition");
			toRet = *Get<UObject*>(randPickaxeDef, WeaponDefinitionOffset);
		}
	}
	else
	{
		auto ItemInstances = Inventory::GetItemInstances(Controller);

		if (ItemInstances->Num() >= 6)
		{
			auto PickaxeInstance = ItemInstances->At(1); // cursed probs // loop through all inventoryt and find  first melee
			toRet = IsBadReadPtr(PickaxeInstance) ? nullptr : *UFortItem::GetDefinition(PickaxeInstance);
		}
	}

	return toRet;
}

int* Helper::GetPlayersLeft()
{
	auto GameState = GetGameState();

	static auto PlayersLeftOffset = FindOffsetStruct2("Class /Script/FortniteGame.FortGameStateAthena", "PlayersLeft");

	return Get<int>(GameState, PlayersLeftOffset);
}

void Helper::LoopConnections(std::function<void(UObject* Controller)> fn, bool bPassWithNoPawn)
{
	if (!Server::BeaconHost)
		return;

	auto World = Helper::GetWorld();

	if (World)
	{
		static auto NetDriverOffset = World->GetOffset("NetDriver");
		auto NetDriver = *(UObject**)(__int64(World) + NetDriverOffset);

		if (NetDriver)
		{
			static auto ClientConnectionsOffset = NetDriver->GetOffset("ClientConnections");
			auto ClientConnections = (TArray<UObject*>*)(__int64(NetDriver) + ClientConnectionsOffset);

			if (ClientConnections)
			{
				for (int i = 0; i < ClientConnections->Num(); i++)
				{
					auto Connection = ClientConnections->At(i);

					if (!Connection)
						continue;

					static auto Connection_PlayerControllerOffset = Connection->GetOffset("PlayerController");
					auto aaController = *(UObject**)(__int64(Connection) + Connection_PlayerControllerOffset);

					if (aaController)
					{
						// auto aaPlayerState = Helper::GetPlayerStateFromController(aaController);
						auto aaPawn = Helper::GetPawnFromController(aaController);

						if (aaPawn || bPassWithNoPawn)
						{
							fn(aaController);
						}
					}
				}
			}
		}
	}
}

UObject* Helper::GetGameData()
{
	auto Engine = GetEngine();

	static auto AssetManagerOffset = Engine->GetOffset("AssetManager");
	UObject* AssetManager = *Get<UObject*>(Engine, AssetManagerOffset);

	static auto GameDataOffset = AssetManager->GetOffset("GameData");
	return GameDataOffset == 0 ? nullptr : *Get<UObject*>(AssetManager, GameDataOffset);
}

UObject* Helper::GetGameDataBR()
{
	auto Engine = GetEngine();

	static auto AssetManagerOffset = Engine->GetOffset("AssetManager");
	UObject* AssetManager = *Get<UObject*>(Engine, AssetManagerOffset);

	static auto GameDataBROffset = AssetManager->GetOffset("GameDataBR");
	return GameDataBROffset == 0 ? nullptr : *Get<UObject*>(AssetManager, GameDataBROffset);
}

UObject* Helper::GetGameDataCosmetics()
{
	auto Engine = GetEngine();

	static auto AssetManagerOffset = Engine->GetOffset("AssetManager");
	UObject* AssetManager = *Get<UObject*>(Engine, AssetManagerOffset);

	static auto GameDataCosmeticsOffset = AssetManager->GetOffset("GameDataCosmetics");
	return GameDataCosmeticsOffset == 0 ? nullptr : *Get<UObject*>(AssetManager, GameDataCosmeticsOffset);
}

void Helper::SetSnowIndex(int SnowIndex)
{
	auto GameState = Helper::GetGameState();

	if (Fortnite_Season == 19)
	{
		auto sjt9ase9i = FindObject("/SpecialSurfaceCoverage/Maps/SpecialSurfaceCoverage_Artemis_Terrain_LS_Parent_Overlay.SpecialSurfaceCoverage_Artemis_Terrain_LS_Parent_Overlay.PersistentLevel.BP_Artemis_S19Progression_C_0");

		// std::cout << "sjt9ase9i: " << sjt9ase9i << '\n';

		if (sjt9ase9i)
		{
			auto setprogr = FindObject<UFunction>("/SpecialSurfaceCoverage/Items/BP_Artemis_S19Progression.BP_Artemis_S19Progression_C.SetSnowProgressionPhase");
			sjt9ase9i->ProcessEvent(setprogr, &SnowIndex);

			auto agh = FindObject<UFunction>("/SpecialSurfaceCoverage/Items/BP_Artemis_S19Progression.BP_Artemis_S19Progression_C.UpdateSnowVisualsOnClient");
			sjt9ase9i->ProcessEvent(agh); // idk if this is needed
		}
	}

	if (Fortnite_Season == 11)
	{
		auto agfag = FindObject("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.BP_ApolloSnowSetup_2");

		std::cout << "agfag: " << agfag << '\n';

		if (agfag)
		{
			struct { UObject* GameState; UObject* Playlist; FGameplayTagContainer PlaylistContextTags; } bbparms{ GameState, *Helper::GetPlaylist(),
				FGameplayTagContainer() };

			auto OnReady_E426AA7F4F2319EA06FBA2B9905F0B24 = FindObject<UFunction>("/Game/Athena/Environments/Landscape/Blueprints/BP_SnowSetup.BP_SnowSetup_C.OnReady_E426AA7F4F2319EA06FBA2B9905F0B24");

			if (OnReady_E426AA7F4F2319EA06FBA2B9905F0B24)
				agfag->ProcessEvent(OnReady_E426AA7F4F2319EA06FBA2B9905F0B24, &GameState);

			auto OnReady_0A511B314AE165C51798519FB84738B8 = FindObject<UFunction>("/Game/Athena/Environments/Landscape/Blueprints/BP_SnowSetup.BP_SnowSetup_C.OnReady_0A511B314AE165C51798519FB84738B8");

			if (OnReady_0A511B314AE165C51798519FB84738B8)
				agfag->ProcessEvent(OnReady_0A511B314AE165C51798519FB84738B8, &bbparms);

			auto age = FindObject<UFunction>("/Game/Athena/Apollo/Environments/Blueprints/CalendarEvents/BP_ApolloSnowSetup.BP_ApolloSnowSetup_C.RefreshMapLocations");
			agfag->ProcessEvent(age);
		}
	}

	if (Fortnite_Season == 7)
	{
		auto snowsetup = FindObject("/Game/Athena/Maps/Athena_Terrain.Athena_Terrain.PersistentLevel.BP_SnowSetup_2");

		std::cout << "snowsteup: " << snowsetup << '\n';

		if (snowsetup)
		{
			auto OnReady_347B1F4D45630C357605FCB417D749A3 = FindObject<UFunction>("/Game/Athena/Environments/Landscape/Blueprints/BP_SnowSetup.BP_SnowSetup_C.OnReady_347B1F4D45630C357605FCB417D749A3");
			
			if (OnReady_347B1F4D45630C357605FCB417D749A3)
				snowsetup->ProcessEvent(OnReady_347B1F4D45630C357605FCB417D749A3, &GameState);

			auto afga = FindObject<UFunction>("/Game/Athena/Environments/Landscape/Blueprints/BP_SnowSetup.BP_SnowSetup_C.SetSnow");

			if (afga)
				snowsetup->ProcessEvent(afga, &SnowIndex);
	
			auto RefreshMapLocations = FindObject<UFunction>("/Game/Athena/Environments/Landscape/Blueprints/BP_SnowSetup.BP_SnowSetup_C.RefreshMapLocations");

			if (RefreshMapLocations)
				snowsetup->ProcessEvent(RefreshMapLocations);
		}
	}
}

void Helper::ExportTexture2DToFile(UObject* Texture, FString Path, FString FileName)
{
	struct { UObject* WorldContextObject; UObject*Texture; FString FilePath; FString Filename; } UKismetRenderingLibrary_ExportTexture2D_Params{GetWorld(), Texture, Path, FileName};
	
	static auto fn = FindObject<UFunction>("/Script/Engine.KismetRenderingLibrary.ExportTexture2D");
	static auto krl = FindObject("/Script/Engine.Default__KismetRenderingLibrary");

	krl->ProcessEvent(fn, &UKismetRenderingLibrary_ExportTexture2D_Params);
}

FString Helper::GetEngineVersion()
{
	static auto ksl = FindObject("/Script/Engine.Default__KismetSystemLibrary");
	static auto fn = FindObject<UFunction>("/Script/Engine.KismetSystemLibrary.GetEngineVersion");

	FString EngineVersion;

	ksl->ProcessEvent(fn, &EngineVersion);

	return EngineVersion;
}

std::string Helper::GetNetCL()
{
	auto EngineVer = GetEngineVersion().ToString();
	EngineVer = EngineVer.substr(EngineVer.find_first_of('-') + 1);
	EngineVer = EngineVer.substr(0, EngineVer.find_first_of('+'));
	return EngineVer;
}

std::string Helper::GetEngineVer()
{
	auto EngineVer = GetEngineVersion().ToString();
	EngineVer = EngineVer.substr(0, EngineVer.find_first_of('-'));
	return EngineVer;
}

std::string Helper::GetFortniteVersion()
{
	auto EngineVer = GetEngineVersion().ToString();
	EngineVer = EngineVer.substr(EngineVer.find_last_of('-') + 1);
	return EngineVer;
}

UObject* Helper::GetRootComponent(UObject* Actor)
{
	UObject* RootComp = nullptr;
	static auto GetRootCompFunc = FindObject<UFunction>("/Script/Engine.Actor.K2_GetRootComponent");
	Actor->ProcessEvent(GetRootCompFunc, &RootComp);

	return RootComp;
}

FRotator Helper::GetControlRotation(UObject* Controller)
{
	static auto GetControlRotation = FindObject<UFunction>("/Script/Engine.Controller.GetControlRotation");
	FRotator ControlRotation;
	Controller->ProcessEvent(GetControlRotation, &ControlRotation);
	return ControlRotation;
}

UObject* Helper::GetAbilitySetFromAGID(UObject* AGID)
{
	static auto AbilitySetOffset = AGID->GetOffset("AbilitySet");
	static bool bIsSoftObjectPtr = true;

	std::cout << "bIsSoftObjectPtr: " << bIsSoftObjectPtr << '\n';

	if (bIsSoftObjectPtr)
	{
		static auto FortAbilitySetClass = FindObject("/Script/FortniteGame.FortAbilitySet");

		auto AbilitySetSoft = Get<TSoftObjectPtr>(AGID, AbilitySetOffset);
		auto AbilitySet = AbilitySetSoft->Get(FortAbilitySetClass);

		return AbilitySet;
	}
	else
	{
		static auto AbilitySet = Get<UObject*>(AGID, AbilitySetOffset);
		return *AbilitySet;
	}
}

FString Helper::GetIPf(UObject* PlayerState)
{
	static auto SavedNetworkAddressOffset = PlayerState->GetOffset("SavedNetworkAddress");
	auto SavedNetworkAddress = Get<FString>(PlayerState, SavedNetworkAddressOffset);

	return *SavedNetworkAddress;
}

std::string Helper::GetPlayerName(UObject* Controller)
{
	// A controller without a player has no "owner"
	// return (Player != NULL) ? NetConnection : NULL;

	static auto NetConnectionOffset = Controller->GetOffset("NetConnection");

	auto Connection = *Get<UObject*>(Controller, NetConnectionOffset);

	if (!Connection)
		return "NO_CONNECTION";

	auto RequestURL = *GetRequestURL(Connection);

	if (RequestURL.Data.Data)
	{
		auto RequestURLStr = RequestURL.ToString();

		std::size_t pos = RequestURLStr.find("Name=");

		if (pos != std::string::npos) {
			std::size_t end_pos = RequestURLStr.find('?', pos);

			if (end_pos != std::string::npos)
				RequestURLStr = RequestURLStr.substr(pos + 5, end_pos - pos - 5);
		}

		return RequestURLStr;
	}

	return "INVALID_REQUEST_URL";
}

FActiveGameplayEffectHandle Helper::ApplyGameplayEffect(UObject* Pawn, UObject* GEClass, float Level)
{
	static auto BP_ApplyGameplayEffectToSelf = FindObject<UFunction>("/Script/GameplayAbilities.AbilitySystemComponent.BP_ApplyGameplayEffectToSelf");

	struct
	{
		UObject* GameplayEffect;
		float Level;
		FGameplayEffectContextHandle EffectContext;
		FActiveGameplayEffectHandle Return;
	} BP_ApplyGameplayEffectToSelf_Params{GEClass, Level, FGameplayEffectContextHandle()};

	auto ASC = Helper::GetAbilitySystemComponent(Pawn);
	ASC->ProcessEvent(BP_ApplyGameplayEffectToSelf, &BP_ApplyGameplayEffectToSelf_Params);

	return BP_ApplyGameplayEffectToSelf_Params.Return;
}

void Helper::RemoveGameplayEffect(UObject* Pawn, UObject* GEClass, int Stacks)
{
	static auto fn = FindObject<UFunction>("/Script/GameplayAbilities.AbilitySystemComponent.RemoveActiveGameplayEffectBySourceEffect");
	auto ASC = Helper::GetAbilitySystemComponent(Pawn);

	if (!ASC)
		return;

	struct { UObject* GameplayEffect; UObject* InstigatorAbilitySystemComponent; int StacksToRemove; } UAbilitySystemComponent_RemoveActiveGameplayEffectBySourceEffect_Params{GEClass, ASC, Stacks};

	ASC->ProcessEvent(fn, &UAbilitySystemComponent_RemoveActiveGameplayEffectBySourceEffect_Params);
}

UObject* Helper::GetRandomObjectOfClass(UObject* Class, bool bUseCache, bool bSaveToCache)
{
	std::vector<UObject*> AllObjects;
	static std::unordered_map<UObject*, std::vector<UObject*>> objectLists;

	if (bUseCache)
	{
		auto pos = objectLists.find(Class);

		if (pos != objectLists.end())
			AllObjects = objectLists.at(Class);
	}

	if (AllObjects.empty())
	{
		AllObjects = GetAllObjectsOfClass(Class);

		if (bSaveToCache)
			objectLists.emplace(Class, AllObjects);
	}

	UObject* RandObject = nullptr;

	while (!RandObject || RandObject->GetFullName().contains("Default"))
	{
		auto random = (int)GetRandomFloat(1, AllObjects.size() - 1); // idk if the -1 is needed
		random = random <= 0 ? 1 : random; // we love default objects
		RandObject = AllObjects.at(random);
	}

	return RandObject;
}

void Helper::ForceNetUpdate(UObject* Actor)
{
	static auto ForceNetUpdateFn = FindObject<UFunction>("/Script/Engine.Actor.ForceNetUpdate");
	Actor->ProcessEvent(ForceNetUpdateFn);
}

UObject* Helper::GetAnimInstance(UObject* Mesh)
{
	static auto GetAnimInstance = FindObject<UFunction>("/Script/Engine.SkeletalMeshComponent.GetAnimInstance");
	UObject* AnimInstance = nullptr;
	Mesh->ProcessEvent(GetAnimInstance, &AnimInstance);
	return AnimInstance;
}

UObject* Helper::GetMesh(UObject* Character)
{
	static auto MeshOffset = Character->GetOffsetSlow("Mesh");
	return *Get<UObject*>(Character, MeshOffset);
}

UObject* GetHealthSet(UObject* Pawn)
{
	static auto HealthSetOffset = FindOffsetStruct2("Class /Script/FortniteGame.FortPawn", "HealthSet");

	std::cout << "HealthSetOffset: " << HealthSetOffset << '\n';

	auto HealthSet = *Get<UObject*>(Pawn, HealthSetOffset);

	return HealthSet;
}

void Helper::SetHealth(UObject* Pawn, float Health)
{
	auto PawnsController = Helper::GetControllerFromPawn(Pawn);

	if (!PawnsController)
		return;

	UObject* PlayerState = Helper::GetPlayerStateFromController(PawnsController);

	if (!PlayerState)
		return;

	static auto PS_CurrentHealthOffset = PlayerState->GetOffset("CurrentHealth", false, false, false);

	if (PS_CurrentHealthOffset != 0)
		*(float*)(__int64(PlayerState) + PS_CurrentHealthOffset) = Health;

	auto HealthSet = GetHealthSet(Pawn);

	static auto CurrentValueOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAttributeData", "CurrentValue");

	static auto HealthOffset = FindOffsetStruct("Class /Script/FortniteGame.FortHealthSet", "Health");

	if (HealthOffset != 0)
	{
		auto HealthData = (__int64*)(__int64(HealthSet) + HealthOffset);

		*(float*)(__int64(HealthData) + CurrentValueOffset) = Health;
	}

	/* static UFunction* OnRep_Health = FindObject<UFunction>("/Script/FortniteGame.FortHealthSet.OnRep_Health");

	if (OnRep_Health)
		HealthSet->ProcessEvent(OnRep_Health); */
}

void Helper::SetMaxHealth(UObject* Pawn, float MaxHealth)
{
	UObject* PlayerState = Helper::GetPlayerStateFromController(Helper::GetControllerFromPawn(Pawn));

	static auto PS_MaxHealthOffset = PlayerState->GetOffset("MaxHealth", false, false, false);

	if (PS_MaxHealthOffset != 0)
		*(float*)(__int64(PlayerState) + PS_MaxHealthOffset) = MaxHealth;

	static auto MaximumOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortGameplayAttributeData", "Maximum");

	auto HealthSet = GetHealthSet(Pawn);

	static auto MaxHealthOffset = FindOffsetStruct("Class /Script/FortniteGame.FortHealthSet", "MaxHealth");

	if (MaxHealthOffset != 0)
	{
		auto MaxHealthData = (__int64*)(__int64(HealthSet) + MaxHealthOffset);

		*(float*)(__int64(MaxHealthData) + MaximumOffset) = MaxHealth;
	}

	static UFunction* OnRep_MaxHealth = FindObject<UFunction>("/Script/FortniteGame.FortHealthSet.OnRep_MaxHealth");

	if (OnRep_MaxHealth)
		HealthSet->ProcessEvent(OnRep_MaxHealth);
}

void Helper::SetShield(UObject* Pawn, float Shield)
{
	UObject* PlayerState = Helper::GetPlayerStateFromController(Helper::GetControllerFromPawn(Pawn));

	if (Engine_Version < 421)
	{
		static auto PS_CurrentShieldOffset = PlayerState->GetOffset("CurrentShield", false, false, false);

		if (PS_CurrentShieldOffset != 0)
			*(float*)(__int64(PlayerState) + PS_CurrentShieldOffset) = Shield;

		static auto CurrentValueOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAttributeData", "CurrentValue");
		static auto MinimumOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortGameplayAttributeData", "Minimum");
		static auto BaseValueOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAttributeData", "BaseValue");

		auto HealthSet = GetHealthSet(Pawn);

		static auto ShieldOffset = FindOffsetStruct("Class /Script/FortniteGame.FortHealthSet", "Shield");
		static auto CurrentShieldOffset = FindOffsetStruct("Class /Script/FortniteGame.FortHealthSet", "CurrentShield");

		if (ShieldOffset != 0)
		{
			auto ShieldData = (__int64*)(__int64(HealthSet) + ShieldOffset);
			*(float*)(__int64(ShieldData) + CurrentValueOffset) = Shield;
			*(float*)(__int64(ShieldData) + BaseValueOffset) = Shield;
			*(float*)(__int64(ShieldData) + MinimumOffset) = Shield;
		}

		if (CurrentShieldOffset != 0)
		{
			auto CurrentShieldData = (__int64*)(__int64(HealthSet) + CurrentShieldOffset);
			*(float*)(__int64(CurrentShieldData) + CurrentValueOffset) = Shield;
			*(float*)(__int64(CurrentShieldData) + BaseValueOffset) = Shield;
			*(float*)(__int64(CurrentShieldData) + MinimumOffset) = Shield;
		}

		static UFunction* OnRep_Shield = FindObject<UFunction>("/Script/FortniteGame.FortHealthSet.OnRep_Shield");

		if (OnRep_Shield)
			HealthSet->ProcessEvent(OnRep_Shield);

		static UFunction* OnRep_CurrentShield = FindObject<UFunction>("/Script/FortniteGame.FortHealthSet.OnRep_CurrentShield");

		if (OnRep_CurrentShield)
			HealthSet->ProcessEvent(OnRep_CurrentShield);
	}
	else
	{
		static auto setShield = FindObject<UFunction>("/Script/FortniteGame.FortPawn.SetShield");

		if (setShield)
			Pawn->ProcessEvent(setShield, &Shield);
	}
}

void Helper::SetMaxShield(UObject* Pawn, float MaxShield)
{
	UObject* PlayerState = Helper::GetPlayerStateFromController(Helper::GetControllerFromPawn(Pawn));

	static auto MaxShieldOffset = PlayerState->GetOffset("MaxShield", false, false, false);

	if (MaxShieldOffset != 0)
		*(float*)(__int64(PlayerState) + MaxShieldOffset) = MaxShield;

	static auto MaximumOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortGameplayAttributeData", "Maximum");

	auto HealthSet = GetHealthSet(Pawn);

	static auto ShieldOffset = FindOffsetStruct("Class /Script/FortniteGame.FortHealthSet", "Shield");
	static auto CurrentShieldOffset = FindOffsetStruct("Class /Script/FortniteGame.FortHealthSet", "CurrentShield");

	if (ShieldOffset != 0)
	{
		auto ShieldData = (__int64*)(__int64(HealthSet) + ShieldOffset);
		*(float*)(__int64(ShieldData) + MaximumOffset) = MaxShield;
	}

	if (CurrentShieldOffset != 0)
	{
		auto CurrentShieldData = (__int64*)(__int64(HealthSet) + ShieldOffset);
		*(float*)(__int64(CurrentShieldData) + MaximumOffset) = MaxShield;
	}

	static UFunction* OnRep_Shield = FindObject<UFunction>("/Script/FortniteGame.FortHealthSet.OnRep_Shield");

	if (OnRep_Shield)
		HealthSet->ProcessEvent(OnRep_Shield);

	static UFunction* OnRep_CurrentShield = FindObject<UFunction>("/Script/FortniteGame.FortHealthSet.OnRep_CurrentShield");

	if (OnRep_CurrentShield)
		HealthSet->ProcessEvent(OnRep_CurrentShield);
}

FVector Helper::GetActorForwardVector(UObject* Actor)
{
	static auto GetActorForwardVectorFN = FindObject<UFunction>("/Script/Engine.Actor.GetActorForwardVector");

	FVector loc;
	Actor->ProcessEvent(GetActorForwardVectorFN, &loc);
	return loc;
}

FVector Helper::GetActorRightVector(UObject* Actor)
{
	static auto GetActorRightVectorFN = FindObject<UFunction>("/Script/Engine.Actor.GetActorRightVector");

	FVector loc;
	Actor->ProcessEvent(GetActorRightVectorFN, &loc);
	return loc;
}

FVector Helper::GetCorrectLocation(UObject* Actor)
{
	auto Location = Helper::GetActorLocation(Actor);
	auto RightVector = Helper::GetActorRightVector(Actor);

	return Location + RightVector * 70.0f + FVector{ 0, 0, 50 };
}

BothVector Helper::GetActorForwardVectorDynamic(UObject* Actor)
{
	if (Fortnite_Season < 20)
		return BothVector(GetActorForwardVector(Actor));

	static auto GetActorForwardVectorFN = FindObject<UFunction>("/Script/Engine.Actor.GetActorForwardVector");

	DVector loc;
	Actor->ProcessEvent(GetActorForwardVectorFN, &loc);

	return BothVector(loc);
}

BothVector Helper::GetActorRightVectorDynamic(UObject* Actor)
{
	if (Fortnite_Season < 20)
		return BothVector(GetActorRightVector(Actor));

	static auto GetActorRightVectorFN = FindObject<UFunction>("/Script/Engine.Actor.GetActorRightVector");

	DVector loc;
	Actor->ProcessEvent(GetActorRightVectorFN, &loc);

	return BothVector(loc);
}

BothVector Helper::GetCorrectLocationDynamic(UObject* Actor)
{
	auto Location = Helper::GetActorLocationDynamic(Actor);
	auto RightVector = Helper::GetActorRightVectorDynamic(Actor);

	return Fortnite_Season < 20 ? BothVector(Location.fV + RightVector.fV * 70.0f + FVector{ 0, 0, 50 }) :
		BothVector(Location.dV + RightVector.dV * 70.0f + DVector{ 0, 0, 50 });
}

void* Helper::GetCosmeticLoadoutForPC(UObject* PC)
{
	static auto CosmeticLoadoutPCOffset = PC->GetOffset("CosmeticLoadoutPC", false, false, false);

	if (CosmeticLoadoutPCOffset != 0)
	{
		return Get<void>(PC, CosmeticLoadoutPCOffset);
	}
	else
	{
		static auto CustomizationLoadoutOffset = PC->GetOffset("CustomizationLoadout");
		return Get<void>(PC, CustomizationLoadoutOffset);
	}
}

void* Helper::GetCosmeticLoadoutForPawn(UObject* Pawn)
{
	static auto CosmeticLoadoutOffset = Pawn->GetOffset("CosmeticLoadout", false, false, false);

	if (CosmeticLoadoutOffset != 0)
	{
		return Get<void>(Pawn, CosmeticLoadoutOffset);
	}
	else
	{
		static auto CustomizationLoadoutOffset = Pawn->GetOffset("CustomizationLoadout");
		return Get<void>(Pawn, CustomizationLoadoutOffset);
	}
}

std::vector<UObject*> Helper::GetAllObjectsOfClass(UObject* Class) // bool bIncludeDefault
{
	std::vector<UObject*> Objects;

	for (int32_t i = 0; i < (NewObjects ? NewObjects->Num() : OldObjects->Num()); i++)
	{
		auto Object = NewObjects ? NewObjects->GetObjectById(i) : OldObjects->GetObjectById(i);

		if (!Object) 
			continue;

		if (Object->IsA(Class))
		{
			Objects.push_back(Object);
		}
	}

	return Objects;
}

UObject* Helper::GetPlayerStart()
{
	static auto WarmupClass = Defines::bIsCreative ? FindObject("/Script/FortniteGame.FortPlayerStartCreative") : FindObject("/Script/FortniteGame.FortPlayerStartWarmup");
	
	if (!WarmupClass)
		return nullptr;

	static TArray<UObject*> OutActors = GetAllActorsOfClass(WarmupClass);

	if (OutActors.Num() == 0)
	{
		OutActors = GetAllActorsOfClass(WarmupClass);

		if (OutActors.Num() == 0)
			return nullptr;
	}

	int ActorToUseNum = std::floor(GetRandomFloat(2, OutActors.Num() - 1));
	auto ActorToUse = OutActors.At(ActorToUseNum);

	while (!ActorToUse)
	{
		ActorToUseNum = std::floor(GetRandomFloat(2, OutActors.Num() - 1));
		ActorToUse = OutActors.At(ActorToUseNum);
	}

	return ActorToUse;
}

UObject* Helper::SummonPickup(UObject* Pawn, UObject* Definition, BothVector Location, EFortPickupSourceTypeFlag PickupSource, EFortPickupSpawnSource SpawnSource, int Count, bool bMaxAmmo, int Ammo)
{
	if (!Definition)
		return nullptr;

	static UObject* PickupClass = FindObject("/Script/FortniteGame.FortPickupAthena");

	auto Pickup = Helper::Easy::SpawnActorDynamic(PickupClass, Location);

	if (Pickup)
	{
		auto PickupEntry = Helper::GetEntryFromPickup(Pickup);

		if (!PickupEntry)
			return nullptr;

		static auto FortResourceItemDefinition = FindObject("/Script/FortniteGame.FortResourceItemDefinition");

		if (Definition->IsA(FortResourceItemDefinition))
			bMaxAmmo = false;

		auto LoadedAmmo = FFortItemEntry::GetLoadedAmmo(PickupEntry);

		if (LoadedAmmo)
		{
			if (bMaxAmmo)
			{
				auto MaxAmmo = GetMaxBullets(Definition);
				// std::cout << "MaxAmmo: " << MaxAmmo << '\n';
				*LoadedAmmo = MaxAmmo;
			}
			else
			{
				*LoadedAmmo = Ammo;
			}
		}

		*FFortItemEntry::GetCount(PickupEntry) = Count;
		*FFortItemEntry::GetItemDefinition(PickupEntry) = Definition;

		static auto OnRep_PrimaryPickupItemEntry = FindObject<UFunction>("/Script/FortniteGame.FortPickup.OnRep_PrimaryPickupItemEntry");

		Pickup->ProcessEvent(OnRep_PrimaryPickupItemEntry);

		if (PickupSource == EFortPickupSourceTypeFlag::Container)
		{
			static auto bTossedFromContainerOffset = Pickup->GetOffset("bTossedFromContainer");
			*(bool*)(__int64(Pickup) + bTossedFromContainerOffset) = true;
		}

		static auto PickupLocationDataOffset = Pickup->GetOffset("PickupLocationData");
		auto PickupLocationData = (void*)(__int64(Pickup) + PickupLocationDataOffset);

		static auto TossPickupFn = FindObject<UFunction>("/Script/FortniteGame.FortPickup.TossPickup");

		if (Fortnite_Season < 20)
		{
			if (Fortnite_Version >= 14.60)
			{
				struct
				{
					FVector                                     FinalLocation;                                            // (ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					UObject* ItemOwner;                                                // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					int                                                OverrideMaxStackCount;                                    // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					bool                                               bToss;                                                    // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					bool                                               bShouldCombinePickupsWhenTossCompletes;                   // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					EFortPickupSourceTypeFlag                          InPickupSourceTypeFlags;                                  // (ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					EFortPickupSpawnSource                             InPickupSpawnSource;                                      // (ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				} AFortPickup_TossPickup_Params{ Location.fV, Pawn, 6, true, true, PickupSource, SpawnSource };

				Pickup->ProcessEvent(TossPickupFn, &AFortPickup_TossPickup_Params);
			}
			else
			{
				struct { FVector FinalLocation; UObject* ItemOwner; int OverrideMaxStackCount; bool bToss; EFortPickupSourceTypeFlag InPickupSourceTypeFlags; EFortPickupSpawnSource InPickupSpawnSource; }
				TPParams{ Location.fV, Pawn, 6, true, PickupSource, SpawnSource };
				Pickup->ProcessEvent(TossPickupFn, &TPParams);
			}
		}
		else
		{
			struct { DVector FinalLocation; UObject* ItemOwner; int OverrideMaxStackCount; bool bToss; EFortPickupSourceTypeFlag InPickupSourceTypeFlags; EFortPickupSpawnSource InPickupSpawnSource; }
			TPParams{ Location.dV, Pawn, 6, true, PickupSource, SpawnSource };
			Pickup->ProcessEvent(TossPickupFn, &TPParams);
		}

		// drop physics

		static auto SetReplicateMovementFn = FindObject<UFunction>("/Script/Engine.Actor.SetReplicateMovement");
		bool bTrue = true;
		Pickup->ProcessEvent(SetReplicateMovementFn, &bTrue);

		static auto ProjectileMovementComponentClass = FindObject("/Script/Engine.ProjectileMovementComponent"); // UFortProjectileMovementComponent

		static auto MovementComponentOffset = Pickup->GetOffset("MovementComponent");
		auto MovementComponent = Get<UObject*>(Pickup, MovementComponentOffset);
		*MovementComponent = Easy::SpawnObject(ProjectileMovementComponentClass, Pickup);
	}

	return Pickup;
}

FName Helper::Conversion::StringToName(FString& String)
{
	static auto Conv_StringToName = FindObject<UFunction>(("/Script/Engine.KismetStringLibrary.Conv_StringToName"));
	static auto Default__KismetStringLibrary = FindObject(("/Script/Engine.Default__KismetStringLibrary"));

	struct { FString InString; FName ReturnValue; } Conv_StringToName_Params{ String };

	Default__KismetStringLibrary->ProcessEvent(Conv_StringToName, &Conv_StringToName_Params);

	return Conv_StringToName_Params.ReturnValue;
}

FString Helper::Conversion::TextToString(FText Text)
{
	static auto KTL = FindObject(("/Script/Engine.Default__KismetTextLibrary"));

	FString String;

	static auto fn = FindObject<UFunction>("/Script/Engine.KismetTextLibrary.Conv_TextToString");

	struct { FText InText; FString ReturnValue; } params{ Text };

	KTL->ProcessEvent(fn, &params);

	return params.ReturnValue;
}