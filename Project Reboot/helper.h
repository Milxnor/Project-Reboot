#pragma once

#include <vector>

#include "definitions.h"
#include "enums.h"
#include <functional>

namespace Helper
{
	namespace Easy
	{
		UObject* SpawnActor(UObject* Class, FVector Location, FRotator Rotation = FRotator(), UObject* Owner = nullptr);
		UObject* SpawnActorDynamic(UObject* Class, BothVector Location, BothRotator Rotation = BothRotator(), UObject* Owner = nullptr);
		UObject* SpawnObject(UObject* Class, UObject* Outer);
	}

	static int GetSizeOfClass(UObject* Class) { return Class ? *(int*)(__int64(Class) + PropertiesSizeOffset) : 0; }
	static UObject* GetBGAClass() { static auto BGAClass = FindObject("/Script/Engine.BlueprintGeneratedClass"); return BGAClass; }
	static void DestroyActor(UObject* Actor) { static auto fn = FindObject<UFunction>("/Script/Engine.Actor.K2_DestroyActor"); Actor->ProcessEvent(fn); }

	float GetTimeSeconds();
	bool IsPlayerController(UObject* Object);
	void ExecuteConsoleCommand(FString Command);
	std::pair<UObject*, int> GetAmmoForDefinition(UObject* Definition);
	UObject* GetWorld();
	UObject* GetTransientPackage();
	UObject* GetEngine();
	UObject* GetGameMode();
	UObject* GetGameState();
	UObject* GetLocalPlayerController();
	UObject* GetPlayerStateFromController(UObject* Controller);
	UObject* GetControllerFromPawn(UObject* Pawn);
	UObject* GetPawnFromController(UObject* Controller);
	float GetDistanceTo(UObject* Actor, UObject* OtherActor);
	bool ApplyCID(UObject* Pawn, UObject* CID);
	UObject* GetRandomCID();
	float GetMaxHealth(UObject* BuildingActor);
	UObject* SpawnPawn(UObject* Controller, BothVector Location, bool bAssignCharacterParts = false);
	void ChoosePart(UObject* Pawn, TEnumAsByte<EFortCustomPartType> Part, UObject* ChosenCharacterPart);
	void SetOwner(UObject* Actor, UObject* Owner);
	UObject* GetAbilitySystemComponent(UObject* Pawn);
	UObject* GetAbilitySystemComponentFromPS(UObject* PlayerState);
	void InitializeBuildingActor(UObject* Controller, UObject* BuildingActor, bool bUsePlayerBuildAnimations = false, UObject* ReplacedBuilding = nullptr);
	UObject** GetPlaylist();
	TArray<UObject*> GetAllActorsOfClass(UObject* Class);
	bool IsInAircraft(UObject* Controller);
	UObject* GetCurrentWeapon(UObject* Pawn);
	UObject* GetWeaponData(UObject* Weapon);
	int* GetTeamIndex(UObject* PlayerState);
	FVector GetActorLocation(UObject* Actor);
	FRotator GetActorRotation(UObject* Actor);
	BothVector GetActorLocationDynamic(UObject* Actor);
	BothRotator GetActorRotationDynamic(UObject* Actor);
	__int64* GetEntryFromPickup(UObject* Pickup);
	UObject* GetOwnerOfComponent(UObject* Component);
	UObject* GetOwner(UObject* Actor);
	int GetMaxBullets(UObject* Definition);
	UObject* GetPawnFromPlayerState(UObject* PlayerState);
	UObject* GetPickaxeDef(UObject* Controller, bool bGetNew = false);
	int* GetPlayersLeft();
	void LoopConnections(std::function<void(UObject* Controller)> fn, bool bPassWithNoPawn = false);
	UObject* GetGameData();
	UObject* GetGameDataBR();
	UObject* GetGameDataCosmetics();
	void SetSnowIndex(int SnowIndex);
	void ExportTexture2DToFile(UObject* Texture, FString Path, FString FileName);
	FString GetEngineVersion();
	std::string GetNetCL();
	std::string GetEngineVer();
	std::string GetFortniteVersion();
	UObject* GetRootComponent(UObject* Actor);
	FRotator GetControlRotation(UObject* Controller);
	UObject* GetAbilitySetFromAGID(UObject* AGID);
	FString GetIPf(UObject* PlayerState);
	std::string GetPlayerName(UObject* Controller);
	FActiveGameplayEffectHandle ApplyGameplayEffect(UObject* Pawn, UObject* GEClass, float Level);
	void RemoveGameplayEffect(UObject* Pawn, UObject* GEClass, int Stacks = 1);
	UObject* GetRandomObjectOfClass(UObject* Class, bool bUseCache = true, bool bSaveToCache = true);
	void ForceNetUpdate(UObject* Actor);
	UObject* GetAnimInstance(UObject* Mesh);
	UObject* GetMesh(UObject* Character);

	void SetHealth(UObject* Pawn, float Health);
	void SetMaxHealth(UObject* Pawn, float MaxHealth);
	void SetShield(UObject* Pawn, float Shield);
	void SetMaxShield(UObject* Pawn, float MaxShield);

	FVector GetActorForwardVector(UObject* Actor);
	FVector GetActorRightVector(UObject* Actor);
	FVector GetCorrectLocation(UObject* Actor);

	BothVector GetActorForwardVectorDynamic(UObject* Actor);
	BothVector GetActorRightVectorDynamic(UObject* Actor);
	BothVector GetCorrectLocationDynamic(UObject* Actor);

	void* GetCosmeticLoadoutForPC(UObject* PC);
	void* GetCosmeticLoadoutForPawn(UObject* Pawn);

	std::vector<UObject*> GetAllObjectsOfClass(UObject* Class);
	UObject* GetPlayerStart();
	UObject* SummonPickup(UObject* Pawn, UObject* Definition, BothVector Location, EFortPickupSourceTypeFlag PickupSource, EFortPickupSpawnSource SpawnSource, int Count = 1, bool bMaxAmmo = false, int Ammo = 0);

	namespace Conversion
	{
		FName StringToName(FString& String); // We don't want the fstring to never get free'd
		FString TextToString(FText Text);
	}
}