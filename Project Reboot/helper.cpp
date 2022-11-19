#include "helper.h"
#include "inventory.h"
#include <format>
#include "server.h"

UObject* Helper::Easy::SpawnActor(UObject* Class, FVector Location, FRotator Rotation, UObject* Owner)
{
	bool bUseDouble = Fortnite_Version >= 20.00;
	bool bUseNewSpawnParameters = false; // die
	auto quat = Rotation.Quaternion();

	if (!bUseDouble)
	{
		FActorSpawnParameters SpawnParameters{};
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParameters.Owner = Owner;

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

void Helper::ExecuteConsoleCommand(FString& Command)
{
	struct { UObject* WorldContextObject; FString Command; UObject* SpecificPlayer; } params{ Helper::GetWorld(), Command, nullptr };

	static auto KSLClass = FindObject("/Script/Engine.Default__KismetSystemLibrary");

	if (KSLClass)
	{
		// static auto ExecuteConsoleCommandFn = KSLClass->Function(("ExecuteConsoleCommand"));
		static auto ExecuteConsoleCommandFn = FindObject<UFunction>("/Script/Engine.KismetSystemLibrary.ExecuteConsoleCommand");
		KSLClass->ProcessEvent(ExecuteConsoleCommandFn, &params);
	}
	else
		std::cout << ("No KismetSyustemLibrary!\n");
}

std::pair<UObject*, int> Helper::GetAmmoForDefinition(UObject* Definition)
{
	static auto GetAmmoWorldItemDefinition_BP = FindObject<UFunction>("/Script/FortniteGame.FortWorldItemDefinition.GetAmmoWorldItemDefinition_BP");
	UObject* AmmoDef;
	Definition->ProcessEvent(GetAmmoWorldItemDefinition_BP, &AmmoDef);

	static auto DropCountOffset = AmmoDef->GetOffset("DropCount");

	auto DropCount = *(int*)(__int64(AmmoDef) + DropCountOffset);

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

void ApplyCID(UObject* Pawn, UObject* CID)
{
	// CID->ItemVariants
	// CID->DefaultBackpack

	// CID->Hero->Specialization

	if (!CID)
		return;

	static auto CCPClass = FindObject("/Script/FortniteGame.CustomCharacterPart");

	static auto HeroDefinitionOffset = CID->GetOffset("HeroDefinition");

	auto HeroDefinition = *(UObject**)(__int64(CID) + HeroDefinitionOffset);

	if (!HeroDefinition)
		return;

	static auto SpecializationsOffset = HeroDefinition->GetOffset("Specializations");
	auto HeroSpecializations = (TArray<TSoftObjectPtr>*)(__int64(HeroDefinition) + SpecializationsOffset);

	if (!HeroSpecializations)
	{
		std::cout << "No HeroSpecializations!\n";
		return;
	}

	for (int j = 0; j < HeroSpecializations->Num(); j++)
	{
		static auto SpecializationClass = FindObject("/Script/FortniteGame.FortHeroSpecialization");

		auto SpecializationName = HeroSpecializations->At(j).ObjectID.AssetPathName.ToString();

		auto Specialization = StaticLoadObject(SpecializationClass, nullptr, SpecializationName);

		static auto CharacterPartsOffset = Specialization->GetOffset("CharacterParts");
		auto CharacterParts = (TArray<TSoftObjectPtr>*)(__int64(Specialization) + CharacterPartsOffset);

		if (!CharacterParts)
		{
			std::cout << "No CharacterParts!\n";
			return;
		}

		for (int i = 0; i < CharacterParts->Num(); i++)
		{
			auto CharacterPart = StaticLoadObject(CCPClass, nullptr, CharacterParts->At(i).ObjectID.AssetPathName.ToString());

			if (CharacterPart)
			{
				static auto CharacterPartTypeOffset = CharacterPart->GetOffset("CharacterPartType");
				auto PartType = *(TEnumAsByte<EFortCustomPartType>*)(__int64(CharacterPart) + CharacterPartTypeOffset);
				Helper::ChoosePart(Pawn, PartType, CharacterPart);
			}
		}
	}
}

UObject* GetRandomCID()
{
	static auto CIDClass = FindObject("/Script/FortniteGame.AthenaCharacterItemDefinition");

	static auto AllObjects = Helper::GetAllObjectsOfClass(CIDClass);

	UObject* skin = nullptr; // AllObjects.at(random);

	while (!skin || skin->GetFullName().contains("Default") || skin->GetFullName().contains("Test"))
	{
		auto random = rand() % (AllObjects.size());
		random = random <= 0 ? 1 : random; // we love default objects
		skin = AllObjects.at(random);
	}

	return skin;
}

UObject* Helper::SpawnPawn(UObject* Controller, FVector Location, bool bAssignCharacterParts)
{
	static auto PawnClass = FindObject(("/Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C"));

	auto Pawn = Helper::Easy::SpawnActor(PawnClass, Location);

	if (!Pawn)
		return Pawn;

	static auto Possess = FindObject<UFunction>("/Script/Engine.Controller.Possess");

	Controller->ProcessEvent(Possess, &Pawn);

	if (bAssignCharacterParts)
	{
		{
			static auto headPart = FindObject(("/Game/Characters/CharacterParts/Female/Medium/Heads/F_Med_Head1.F_Med_Head1"));
			static auto bodyPart = FindObject(("/Game/Characters/CharacterParts/Female/Medium/Bodies/F_Med_Soldier_01.F_Med_Soldier_01"));

			if (!headPart)
				headPart = FindObject(("/Game/Characters/CharacterParts/Female/Medium/Heads/CP_Head_F_RebirthDefaultA.CP_Head_F_RebirthDefaultA"));

			if (!bodyPart)
				bodyPart = FindObject(("/Game/Athena/Heroes/Meshes/Bodies/CP_Body_Commando_F_RebirthDefaultA.CP_Body_Commando_F_RebirthDefaultA"));

			ChoosePart(Pawn, EFortCustomPartType::Head, headPart);
			ChoosePart(Pawn, EFortCustomPartType::Body, bodyPart);
		}
	}

	static auto ClientOnPawnSpawned = FindObject<UFunction>("/Script/FortniteGame.FortPlayerControllerZone.ClientOnPawnSpawned");
	Controller->ProcessEvent(ClientOnPawnSpawned);

	return Pawn;
}

void Helper::ChoosePart(UObject* Pawn, TEnumAsByte<EFortCustomPartType> Part, UObject* ChosenCharacterPart)
{
	struct { TEnumAsByte<EFortCustomPartType> Part; UObject* ChosenCharacterPart; } SCP_params{ Part, ChosenCharacterPart };

	static auto ServerChoosePart = FindObject<UFunction>("/Script/FortniteGame.FortPlayerPawn.ServerChoosePart");

	Pawn->ProcessEvent(ServerChoosePart, &SCP_params);
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
	return false;
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

UObject* Helper::GetPickaxeDef(UObject* Controller)
{
	static UObject* PickaxeDef = FindObject("/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");

	return PickaxeDef;
}

int* Helper::GetPlayersLeft()
{
	auto GameState = GetGameState();

	static auto PlayersLeftOffset = GameState->GetOffset("PlayersLeft");

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

std::vector<UObject*> Helper::GetAllObjectsOfClass(UObject* Class) // bool bIncludeDefault
{
	std::vector<UObject*> Objects;

	for (int32_t i = 0; i < (NewObjects ? NewObjects->Num() : NewObjects->Num()); i++)
	{
		auto Object = NewObjects ? NewObjects->GetObjectById(i) : NewObjects->GetObjectById(i);

		if (!Object) continue;

		if (Object->IsA(Class))
		{
			Objects.push_back(Object);
		}
	}

	return Objects;
}

UObject* Helper::GetPlayerStart()
{
	static auto WarmupClass = Defines::bIsCreative ? FindObject("/Script/FortniteGame.FortPlayerStartCreative") : FindObject(("/Script/FortniteGame.FortPlayerStartWarmup"));
	
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

UObject* Helper::SummonPickup(UObject* Pawn, UObject* Definition, FVector Location, EFortPickupSourceTypeFlag PickupSource, EFortPickupSpawnSource SpawnSource, int Count, bool bMaxAmmo, int Ammo)
{
	static UObject* PickupClass = FindObject(("/Script/FortniteGame.FortPickupAthena"));

	auto Pickup = Helper::Easy::SpawnActor(PickupClass, Location, FRotator());

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

		static auto TossPickupFn = FindObject<UFunction>("/Script/FortniteGame.FortPickup.TossPickup");

		struct { FVector FinalLocation; UObject* ItemOwner; int OverrideMaxStackCount; bool bToss; EFortPickupSourceTypeFlag InPickupSourceTypeFlags; EFortPickupSpawnSource InPickupSpawnSource; }
		TPParams{ Location, Pawn, 6, true, PickupSource, SpawnSource };

		if (TossPickupFn)
			Pickup->ProcessEvent(TossPickupFn, &TPParams);

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