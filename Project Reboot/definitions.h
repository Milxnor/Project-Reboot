#pragma once

#include "structs.h"
#include "log.h"

namespace Defines
{
	inline bool bLogProcessEvent = false;
	inline bool bReadyForStartMatch = true;
	inline bool bIsPlayground = false;
	inline bool bShouldSpawnFloorLoot = false;
	inline bool bRandomSkin = true;
	inline bool bRandomPickaxe = true;
	inline bool bIsLateGame = true;

	inline int AmountOfRestarts = 0;

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

	inline FGameplayAbilitySpecHandle* (*GiveAbility)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, PadHexC8 inSpec); // 4.20-4.25 etc.
	inline FGameplayAbilitySpecHandle* (*GiveAbilityS14ANDS15)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, PadHexE0 inSpec);
	/* inline FGameplayAbilitySpecHandle* (*GiveAbilityOLDDD)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, FGameplayAbilitySpec<FGameplayAbilityActivationInfo, 0> inSpec);
	inline FGameplayAbilitySpecHandle* (*GiveAbilityFTS)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS, 0x50> inSpec);
	inline FGameplayAbilitySpecHandle* (*GiveAbilityNewer)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, FGameplayAbilitySpecNewer inSpec);
	inline FGameplayAbilitySpecHandle* (*GiveAbilityS16)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, PaddingDec232 inSpec); */

}