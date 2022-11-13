#pragma once

#include "structs.h"
#include "log.h"

namespace Defines
{
	inline bool bLogProcessEvent = false;
	inline bool bReadyForStartMatch = true;
	inline bool bIsPlayground = true;
	inline bool bIsLateGame = false;
	inline bool bRandomSkin = true;
	inline bool bRandomPickaxe = true;
	inline bool bIsCreative = false;
	inline bool bIsGoingToPlayMainEvent = false;
	inline std::string urlForPortal = "https://images-ext-2.discordapp.net/external/fX-M8zr0lV9X4eU6cCKGbkbNhyLpSpSgLcUHrQX5BZw/https/i.ibb.co/F7VPqsW/image.png?width=1012&height=676";

	// DON'T CHANGE HERE

	inline bool bMatchmakingSupported = false;
	inline bool bShouldSpawnFloorLoot = false;
	inline bool bShouldSpawnVehicles = false;
	inline bool bShouldSpawnForagedItems = false;
	inline UObject* Portal = nullptr;

	inline int AmountOfRestarts = 0;

	// TO HERE

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
	inline FGameplayAbilitySpecHandle* (*GiveAbilityS14ABOVE)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, PadHexE0 inSpec);
	inline FGameplayAbilitySpecHandle* (*GiveAbilityS17ABOVE)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, PadHexE8 inSpec);

	static FString GetMapName()
	{
		if (bIsGoingToPlayMainEvent)
		{
			if (Fortnite_Season == 16)
			{
				return L"Apollo_Terrain_Yogurt";
			}
		}

		if (bIsCreative)
		{
			if (Fortnite_Season >= 7 && Engine_Version < 424)
			{
				static auto CreativePlaylist = FindObject("/Game/Athena/Playlists/Creative/Playlist_PlaygroundV2.Playlist_PlaygroundV2");

				std::cout << "CreativePlaylist: " << CreativePlaylist << '\n';

				if (CreativePlaylist)
				{
					static auto AdditionalLevelsOffset = CreativePlaylist->GetOffset("AdditionalLevels");

					auto AdditionalLevels = Get<TArray<TSoftObjectPtr>>(CreativePlaylist, AdditionalLevelsOffset); // TArray<TSoftObjectPtr<class UWorld>>

					std::cout << "AdditionalLevels: " << AdditionalLevels->Num() << '\n';

					for (int i = 0; i < AdditionalLevels->Num(); i++)
					{
						auto AdditionalLevel = AdditionalLevels->At(i);

						auto CurrentLevelName = AdditionalLevel.ObjectID.AssetPathName.ToString();
						std::cout << std::format("[{}] {}\n", i, CurrentLevelName);
					}

					auto LevelToOpen = AdditionalLevels->At(AdditionalLevels->Num() - 1);

					auto LevelName = LevelToOpen.ObjectID.AssetPathName.ToString();

					LevelName = LevelName.substr(0, LevelName.find_last_of("."));

					std::cout << "LevelName: " << LevelName << '\n';

					FString levelName = std::wstring(LevelName.begin(), LevelName.end()).c_str();
					return levelName;
				}
			}
			else
			{
				std::cout << "You are on a version that either doesn't have creative or we don't support creative for it!\n";
				bIsCreative = false;
			}
		}

		return Engine_Version < 424 ? L"Athena_Terrain" : (Engine_Version < 500 ? L"Apollo_Terrain" : L"Artemis_Terrain");
	}
}