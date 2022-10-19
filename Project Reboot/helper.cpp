#include "helper.h"
#include "inventory.h"
#include <format>

UObject* Helper::Easy::SpawnActor(UObject* Class, FVector Location, FRotator Rotation, UObject* Owner)
{
	FActorSpawnParameters SpawnParameters{};
	SpawnParameters.Owner = Owner;
	return SpawnActorO(Helper::GetWorld(), Class, &Location, &Rotation, SpawnParameters);
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

	static auto GSC = FindObject(("GameplayStatics /Script/Engine.Default__GameplayStatics"));
	static auto fn = FindObject<UFunction>("Function /Script/Engine.GameplayStatics.SpawnObject");

	GSC->ProcessEvent(fn, &params);

	return params.ReturnValue;
}

float Helper::GetTimeSeconds()
{
	static auto GSCClass = FindObject(("GameplayStatics /Script/Engine.Default__GameplayStatics"));

	struct { UObject* world; float timeseconds; } parms{ GetWorld() };

	static auto GetTimeSeconds = FindObject<UFunction>("Function /Script/Engine.GameplayStatics.GetTimeSeconds");

	GSCClass->ProcessEvent(GetTimeSeconds, &parms);

	return parms.timeseconds;
}

bool Helper::IsPlayerController(UObject* Object)
{
	static auto PlayerControllerClass = FindObject(("BlueprintGeneratedClass /Game/Athena/Athena_PlayerController.Athena_PlayerController_C"));

	return Object->IsA(PlayerControllerClass);
}

UObject* Helper::GetWorld()
{
	auto Engine = GetEngine();

	static auto GameViewportOffset = Engine->GetOffset("GameViewport");
	auto GameViewport = *Get<UObject*>(Engine, GameViewportOffset);

	static auto PropertyClass = FindObject("Class /Script/CoreUObject.Property");

	static auto WorldOffset = GameViewport->GetOffsetSlow("World");
	// std::cout << "WorldOffset: " << WorldOffset << '\n';

	auto World = *Get<UObject*>(GameViewport, WorldOffset);

	return World;
}

UObject* Helper::GetTransientPackage()
{
	static auto TransientPackage = FindObject("Package /Engine/Transient");
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

	static auto GetDistanceTo = FindObject<UFunction>("Function /Script/Engine.Actor.GetDistanceTo");
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

	static auto CCPClass = FindObject("Class /Script/FortniteGame.CustomCharacterPart");

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
		static auto SpecializationClass = FindObject("Class /Script/FortniteGame.FortHeroSpecialization");

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
	static auto CIDClass = FindObject("Class /Script/FortniteGame.AthenaCharacterItemDefinition");

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
	static auto PawnClass = FindObject(("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C"));

	auto Pawn = Helper::Easy::SpawnActor(PawnClass, Location);

	if (!Pawn)
		return Pawn;

	static auto Possess = FindObject<UFunction>("Function /Script/Engine.Controller.Possess");

	Controller->ProcessEvent(Possess, &Pawn);

	if (bAssignCharacterParts)
	{
		{
			static auto headPart = FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/F_Med_Head1.F_Med_Head1"));
			static auto bodyPart = FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Bodies/F_Med_Soldier_01.F_Med_Soldier_01"));

			if (!headPart)
				headPart = FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/CP_Head_F_RebirthDefaultA.CP_Head_F_RebirthDefaultA"));

			if (!bodyPart)
				bodyPart = FindObject(("CustomCharacterPart /Game/Athena/Heroes/Meshes/Bodies/CP_Body_Commando_F_RebirthDefaultA.CP_Body_Commando_F_RebirthDefaultA"));

			ChoosePart(Pawn, EFortCustomPartType::Head, headPart);
			ChoosePart(Pawn, EFortCustomPartType::Body, bodyPart);
		}
	}

	return Pawn;
}

void Helper::ChoosePart(UObject* Pawn, TEnumAsByte<EFortCustomPartType> Part, UObject* ChosenCharacterPart)
{
	struct { TEnumAsByte<EFortCustomPartType> Part; UObject* ChosenCharacterPart; } SCP_params{ Part, ChosenCharacterPart };

	static auto ServerChoosePart = FindObject<UFunction>("Function /Script/FortniteGame.FortPlayerPawn.ServerChoosePart");

	Pawn->ProcessEvent(ServerChoosePart, &SCP_params);
}

void Helper::SetOwner(UObject* Actor, UObject* Owner)
{
	static auto SetOwner = FindObject<UFunction>("Function /Script/Engine.Actor.SetOwner");

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

	static auto fn = FindObject<UFunction>("Function /Script/FortniteGame.BuildingActor.InitializeKismetSpawnedBuildingActor");
	BuildingActor->ProcessEvent(fn, &IBAParams);
}

UObject** Helper::GetPlaylist()
{
	auto GameState = Helper::GetGameState();

	if (Fortnite_Version >= 6.10)
	{
		static auto BasePlaylistOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.PlaylistPropertyArray"), ("BasePlaylist"));

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
		static auto CurrentPlaylistDataOffset = GameState->GetOffset("CurrentPlaylistData");

		auto PlaylistData = (UObject**)(__int64(GameState) + CurrentPlaylistDataOffset);

		return PlaylistData;
	}

	return nullptr;
}

TArray<UObject*> Helper::GetAllActorsOfClass(UObject* Class)
{
	static auto GetAllActorsOfClass = FindObject<UFunction>("Function /Script/Engine.GameplayStatics.GetAllActorsOfClass");
	static auto DefaultGameplayStatics = FindObject("GameplayStatics /Script/Engine.Default__GameplayStatics");

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

}

FVector Helper::GetActorLocation(UObject* Actor)
{
	static auto K2_GetActorLocationFN = FindObject<UFunction>("Function /Script/Engine.Actor.K2_GetActorLocation");

	FVector loc;
	Actor->ProcessEvent(K2_GetActorLocationFN, &loc);

	return loc;
}

FRotator Helper::GetActorRotation(UObject* Actor)
{
	static auto K2_GetActorRotation = FindObject<UFunction>("Function /Script/Engine.Actor.K2_GetActorRotation");

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
	static auto fn = FindObject<UFunction>("Function /Script/Engine.ActorComponent.GetOwner");

	UObject* Owner = nullptr;
	Component->ProcessEvent(fn, &Owner);

	return Owner;
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
	static auto WarmupClass = /* bIsCreative ? FindObject("Class /Script/FortniteGame.FortPlayerStartCreative") : */ FindObject(("Class /Script/FortniteGame.FortPlayerStartWarmup"));
	
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
	static UObject* PickupClass = FindObject(("Class /Script/FortniteGame.FortPickupAthena"));

	auto Pickup = Helper::Easy::SpawnActor(PickupClass, Location, FRotator());

	if (Pickup)
	{
		auto PickupEntry = Helper::GetEntryFromPickup(Pickup);

		if (!PickupEntry)
			return nullptr;

		auto LoadedAmmo = FFortItemEntry::GetLoadedAmmo(PickupEntry);

		if (LoadedAmmo)
		{
			if (bMaxAmmo)
			{
				
			}
			else
			{
				*LoadedAmmo = Ammo;
			}
		}

		*FFortItemEntry::GetCount(PickupEntry) = Count;
		*FFortItemEntry::GetItemDefinition(PickupEntry) = Definition;

		static auto OnRep_PrimaryPickupItemEntry = FindObject<UFunction>("Function /Script/FortniteGame.FortPickup.OnRep_PrimaryPickupItemEntry");

		Pickup->ProcessEvent(OnRep_PrimaryPickupItemEntry);

		if (PickupSource == EFortPickupSourceTypeFlag::Container)
		{
			static auto bTossedFromContainerOffset = Pickup->GetOffset("bTossedFromContainer");
			*(bool*)(__int64(Pickup) + bTossedFromContainerOffset) = true;
		}

		static auto TossPickupFn = FindObject<UFunction>("Function /Script/FortniteGame.FortPickup.TossPickup");

		struct { FVector FinalLocation; UObject* ItemOwner; int OverrideMaxStackCount; bool bToss; EFortPickupSourceTypeFlag InPickupSourceTypeFlags; EFortPickupSpawnSource InPickupSpawnSource; }
		TPParams{ Location, Pawn, 6, true, PickupSource, SpawnSource };

		if (TossPickupFn)
			Pickup->ProcessEvent(TossPickupFn, &TPParams);

		// drop physics

		static auto SetReplicateMovementFn = FindObject<UFunction>("Function /Script/Engine.Actor.SetReplicateMovement");
		bool bTrue = true;
		Pickup->ProcessEvent(SetReplicateMovementFn, &bTrue);

		static auto ProjectileMovementComponentClass = FindObject("Class /Script/Engine.ProjectileMovementComponent"); // UFortProjectileMovementComponent

		static auto MovementComponentOffset = Pickup->GetOffset("MovementComponent");
		auto MovementComponent = Get<UObject*>(Pickup, MovementComponentOffset);
		*MovementComponent = Easy::SpawnObject(ProjectileMovementComponentClass, Pickup);
	}

	return Pickup;
}

FName Helper::Conversion::StringToName(FString& String)
{
	static auto Conv_StringToName = FindObject<UFunction>(("Function /Script/Engine.KismetStringLibrary.Conv_StringToName"));
	static auto Default__KismetStringLibrary = FindObject(("KismetStringLibrary /Script/Engine.Default__KismetStringLibrary"));

	struct { FString InString; FName ReturnValue; } Conv_StringToName_Params{ String };

	Default__KismetStringLibrary->ProcessEvent(Conv_StringToName, &Conv_StringToName_Params);

	return Conv_StringToName_Params.ReturnValue;
}