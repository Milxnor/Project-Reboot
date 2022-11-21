#pragma once

#include "structs.h"
#include "log.h"

namespace Defines
{
	inline bool bLogProcessEvent = false;
	inline bool bReadyForStartMatch = true;
	inline bool bIsPlayground = false;
	inline bool bIsLateGame = false;
	inline bool bRandomSkin = true;
	inline bool bRandomPickaxe = true;
	inline bool bIsCreative = false;
	inline bool bIsGoingToPlayMainEvent = false;
	inline bool bTraveled = false;
	inline bool bWipeInventoryOnAircraft = true;
	inline bool bInfiniteAmmo = false;
	inline bool bInfiniteMats = false;

	inline int SecondsUntilTravel = 5;

	inline std::string MapName;

	inline std::string Playlist = Defines::bIsCreative ? ("/Game/Athena/Playlists/Creative/Playlist_PlaygroundV2.Playlist_PlaygroundV2") :
		Defines::bIsPlayground ? ("/Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground") :
			("/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo");

	inline std::string urlForPortal = "https://images-ext-2.discordapp.net/external/fX-M8zr0lV9X4eU6cCKGbkbNhyLpSpSgLcUHrQX5BZw/https/i.ibb.co/F7VPqsW/image.png?width=1012&height=676";

	// DON'T CHANGE BELOW THIS

	inline bool bMatchmakingSupported = false;
	inline bool bShouldSpawnFloorLoot = false;
	inline bool bShouldSpawnVehicles = false;
	inline bool bShouldSpawnForagedItems = false;
	inline bool bIsRestarting = false;
	inline UObject* Portal = nullptr;

	inline int AmountOfRestarts = 0;

	inline std::vector<std::pair<UObject*, std::string>> ObjectsToLoad;

	inline bool (*InitHost)(UObject* Beacon);
	inline void (*PauseBeaconRequests)(UObject* Beacon, bool bPause);
	inline void* (*SetWorld)(UObject* NetDriver, UObject* World);
	inline bool (*InitListen)(UObject* Driver, void* InNotify, FURL& LocalURL, bool bReuseAddressAndPort, FString& Error);
	inline void (*TickFlush)(UObject* NetDriver, float DeltaSeconds);
	inline void (*ServerReplicateActors)(UObject* ReplicationDriver);
	inline char (*KickPlayer)(UObject* GameSession, UObject* Controller, FText a3);
	inline char (*ValidationFailure)(__int64* a1, __int64 a2);
	inline __int64 (*NoReservation)(__int64* a1, __int64 a2, char a3, __int64 a4);
	inline __int64 (*CantBuild)(UObject*, UObject*, FVector, FRotator, char, void*, char*);
	inline void (*HandleReloadCost)(UObject* Weapon, int AmountToRemove);
	inline UObject* (*ReplaceBuildingActor)(UObject* BuildingSMActor, unsigned int a2, UObject* a3, unsigned int a4, int a5, unsigned __int8 bMirrored, UObject* Controller);

	inline bool (*InternalTryActivateAbility)(UObject* comp, FGameplayAbilitySpecHandle Handle, PadHex18 InPredictionKey, UObject** /* UGameplayAbility** */ OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, __int64* TriggerEventData); // // https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L1327
	inline bool (*InternalTryActivateAbilityFTS)(UObject* comp, FGameplayAbilitySpecHandle Handle, PadHex10 InPredictionKey, UObject** /* UGameplayAbility** */ OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, __int64* TriggerEventData); // // https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L1327
	// inline bool (*InternalTryActivateAbilityTest)(UObject* comp, FGameplayAbilitySpecHandle Handle, void* InPredictionKey,
		// UObject** OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, __int64* TriggerEventData);

	inline bool (*InternalTryActivateAbilityTest)(UObject* comp, FGameplayAbilitySpecHandle Handle, PadHex18 InPredictionKey, ...);

	inline FGameplayAbilitySpecHandle* (*GiveAbility)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, PadHexC8 inSpec); // 4.20-4.25 etc.
	inline FGameplayAbilitySpecHandle* (*GiveAbilityS13)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, PadHexC0 inSpec);
	inline FGameplayAbilitySpecHandle* (*GiveAbilityS14ABOVE)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, PadHexE0 inSpec);
	inline FGameplayAbilitySpecHandle* (*GiveAbilityS17ABOVE)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, PadHexE8 inSpec);
}