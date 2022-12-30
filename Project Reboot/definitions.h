#pragma once

#include "structs.h"
#include "log.h"

// #define TEST_NEW_LOOTING
// #define DEVELOPER_LOGGING
// #define MILXNOR_H

#ifdef DEVELOPER_LOGGING
#define DEV_LOG(...) std::cout << std::format(__VA_ARGS__) << '\n';
#else
#define DEV_LOG(...)
#endif

#define LOG(...) std::cout << std::format(__VA_ARGS__) << '\n';

namespace Defines
{
	inline bool bLogProcessEvent = false;
	inline bool bReadyForStartMatch = true;
	inline bool bIsPlayground = false;
	inline bool bIsLateGame = false;
	inline bool bIsCreative = false;
	inline bool bIsGoingToPlayMainEvent = false;
	inline bool bTraveled = false;
	inline bool bWipeInventoryOnAircraft = true;
	inline bool bInfiniteAmmo = false;
	inline bool bInfiniteMats = false;
	inline bool bIsSTW = false; // not implemeneted
	inline bool bRespawning = false;
	inline bool bLogRPCs = false;
	inline bool bCustomLootpool = false;
	inline bool bShouldRestart = false;

	inline int SecondsUntilTravel = 5;

	inline std::string MapName;

	inline std::string Playlist = Defines::bIsCreative ? ("/Game/Athena/Playlists/Creative/Playlist_PlaygroundV2.Playlist_PlaygroundV2") :
		Defines::bIsPlayground ? ("/Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground") :
		("/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo");
		// ("/Game/Athena/Playlists/Playlist_DefaultDuo.Playlist_DefaultDuo");
		// ("/Game/Athena/Playlists/DefaultBots/Playlist_Bots_DefaultSolo.Playlist_Bots_DefaultSolo");
		// ("Playlist_SolidGold_Solo");

	inline std::string urlForPortal = "";

	// DON'T CHANGE BELOW THIS

	inline bool bMatchmakingSupported = false;
	inline bool bShouldSpawnFloorLoot = false;
	inline bool bShouldSpawnVehicles = false;
	inline bool bShouldSpawnForagedItems = false;
	inline bool bIsRestarting = false;
	inline bool bShouldStartBus = false;
	inline UObject* Portal = nullptr;

	inline float test1 = 0;
	inline bool test2 = false;
	inline int AmountOfRestarts = 0;

	inline std::vector<std::pair<UObject*, std::string>> ObjectsToLoad;
	inline std::vector<ActorSpawnStruct> ActorsToSpawn;

	inline bool (*InitHost)(UObject* Beacon);
	inline void (*PauseBeaconRequests)(UObject* Beacon, bool bPause);
	inline void* (*SetWorld)(UObject* NetDriver, UObject* World);
	inline bool (*InitListen)(UObject* Driver, void* InNotify, FURL& LocalURL, bool bReuseAddressAndPort, FString& Error);
	inline void (*TickFlush)(UObject* NetDriver, float DeltaSeconds);
	inline void (*ServerReplicateActors)(UObject* ReplicationDriver);
	inline char (*KickPlayer)(UObject* GameSession, UObject* Controller, FText a3);
	inline char (*ValidationFailure)(__int64* a1, __int64 a2);
	inline __int64 (*NoReservation)(__int64* a1, __int64 a2, char a3, __int64 a4);
	inline __int64 (*CantBuild)(UObject*, UObject*, FVector, FRotator, char, TArray<UObject*>*, char*);
	inline __int64 (*CantBuildDouble)(UObject*, UObject*, DVector, DRotator, char, TArray<UObject*>*, char*);
	inline void (*HandleReloadCost)(UObject* Weapon, int AmountToRemove);
	inline UObject* (*ReplaceBuildingActor)(UObject* BuildingSMActor, unsigned int a2, UObject* a3, unsigned int a4, int a5, unsigned __int8 bMirrored, UObject* Controller);

	inline bool (*IsNetRelevantFor)(UObject*, UObject*, UObject*, void*); // end var is location
	inline void (*ActorChannelClose)(UObject*, EChannelCloseReason);

	inline void(*CallPreReplication)(UObject* actor, UObject* driver);
	inline char(*ReplicateActor)(UObject* ActorChannel);
	inline UObject* (*CreateChannelByName)(UObject* Connection, FName* ChName, EChannelCreateFlags CreateFlags, int32_t ChannelIndex); // = -1);
	inline void (*SetChannelActor)(UObject* ActorChannel, UObject* InActor, ESetChannelActorFlags Flags);
	inline UObject* (*CreateChannel)(UObject* Connection, EChannelType Type, bool bOpenedLocally, int32_t ChannelIndex);
	inline void (*SendClientAdjustment)(UObject* PlayerController);

	inline bool (*InternalTryActivateAbility)(UObject* comp, FGameplayAbilitySpecHandle Handle, PadHex18 InPredictionKey, UObject** /* UGameplayAbility** */ OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, __int64* TriggerEventData); // // https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L1327
	inline bool (*InternalTryActivateAbilityFTS)(UObject* comp, FGameplayAbilitySpecHandle Handle, PadHex10 InPredictionKey, UObject** /* UGameplayAbility** */ OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, __int64* TriggerEventData); // // https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L1327
	// inline bool (*InternalTryActivateAbilityTest)(UObject* comp, FGameplayAbilitySpecHandle Handle, void* InPredictionKey,
		// UObject** OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, __int64* TriggerEventData);

	inline bool (*InternalTryActivateAbilityTest)(UObject* comp, FGameplayAbilitySpecHandle Handle, PadHex18 InPredictionKey, ...);

	inline FGameplayAbilitySpecHandle* (*GiveAbilityOld)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, PadHex78 inSpec); // 419
	inline FGameplayAbilitySpecHandle* (*GiveAbility)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, PadHexC8 inSpec); // 4.20-4.25 etc.
	inline FGameplayAbilitySpecHandle* (*GiveAbilityS13)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, PadHexC0 inSpec);
	inline FGameplayAbilitySpecHandle* (*GiveAbilityS14ABOVE)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, PadHexE0 inSpec);
	inline FGameplayAbilitySpecHandle* (*GiveAbilityS17ABOVE)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, PadHexE8 inSpec);
}

template <typename T = void>
T* Alloc(size_t Bytes)
{
	return (T*)FMemory::Realloc(nullptr, Bytes, 0);
}

inline FString* GetRequestURL(UObject* Connection)
{
	if (Engine_Version <= 420)
		return (FString*)(__int64(Connection) + 432);
	if (Fortnite_Season >= 5 && Engine_Version < 424)
		return (FString*)(__int64(Connection) + 424);
	else if (Engine_Version >= 424)
		return (FString*)(__int64(Connection) + 440);

	return nullptr;
}