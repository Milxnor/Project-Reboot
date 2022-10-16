#include "helper.h"
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

UObject* Helper::GetPawnFromController(UObject* Controller)
{
	static auto PawnOffset = Controller->GetOffsetSlow("Pawn");

	return *Get<UObject*>(Controller, PawnOffset);
}

UObject* Helper::SpawnPawn(UObject* Controller, FVector Location, bool bAssignCharacterParts)
{
	static auto PawnClass = FindObject(("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C"));

	auto Pawn = Helper::Easy::SpawnActor(PawnClass, Location);

	if (!Pawn)
		return Pawn;

	// static auto SetReplicateMovementFn = FindObject<UFunction>("Function /Script/Engine.Actor.SetReplicateMovement");
	// bool bReplicateMovement = true;
	// Pawn->ProcessEvent(SetReplicateMovementFn, &bReplicateMovement);

	static auto Possess = FindObject<UFunction>("Function /Script/Engine.Controller.Possess");

	Controller->ProcessEvent(Possess, &Pawn);

	if (bAssignCharacterParts)
	{
		static auto headPart = FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/F_Med_Head1.F_Med_Head1"));
		static auto bodyPart = FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Bodies/F_Med_Soldier_01.F_Med_Soldier_01"));

		ChoosePart(Pawn, EFortCustomPartType::Head, headPart);
		ChoosePart(Pawn, EFortCustomPartType::Body, bodyPart);
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

	TArray<UObject*> Array;

	struct { UObject* World; UObject* Class; TArray<UObject*> Array; } GetAllActorsOfClass_Params{GetWorld(), Class, Array};

	DefaultGameplayStatics->ProcessEvent(GetAllActorsOfClass, &GetAllActorsOfClass_Params);

	auto Ret = Array; // Array.Data ? Array.ToVector() : std::vector<UObject*>();

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
	return nullptr;
}

FName Helper::Conversion::StringToName(FString& String)
{
	static auto Conv_StringToName = FindObject<UFunction>(("Function /Script/Engine.KismetStringLibrary.Conv_StringToName"));
	static auto Default__KismetStringLibrary = FindObject(("KismetStringLibrary /Script/Engine.Default__KismetStringLibrary"));

	struct { FString InString; FName ReturnValue; } Conv_StringToName_Params{ String };

	Default__KismetStringLibrary->ProcessEvent(Conv_StringToName, &Conv_StringToName_Params);

	return Conv_StringToName_Params.ReturnValue;
}