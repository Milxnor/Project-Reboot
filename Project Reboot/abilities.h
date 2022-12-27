#pragma once

#include <functional>

#include "structs.h"

void LoopSpecs(UObject* ASC, std::function<void(__int64*)> func);

namespace Abilities
{
	inline UObject* GameplayAbilitySpecClass = nullptr;

    void ClientActivateAbilityFailed(UObject* ASC, FGameplayAbilitySpecHandle AbilityToActivate, int16_t PredictionKey);
	void* GenerateNewSpec(UObject* DefaultObject, UObject* SourceObject = nullptr);
    std::vector<UObject*> DoesASCHaveAbility(UObject* ASC, UObject* Ability);
    void* GrantGameplayAbility(UObject* TargetPawn, UObject* GameplayAbilityClass, UObject* SourceObject = nullptr);
    void GiveAbilityAndActivateOnce(UObject* Pawn, UObject* Class, UObject* SourceObject = nullptr, __int64* EventData = nullptr);

    // HOOKS

    bool ServerTryActivateAbility(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters);
    bool ServerTryActivateAbilityWithEventData(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters);
    bool ServerAbilityRPCBatch(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters);
}

UObject** GetAbilityFromSpec(void* Spec);
__int64* FindAbilitySpecFromHandle(UObject* ASC, FGameplayAbilitySpecHandle Handle);
void InternalServerTryActivateAbility(UObject* ASC, FGameplayAbilitySpecHandle Handle, bool InputPressed, void* PredictionKey, __int64* TriggerEventData);
void GiveFortAbilitySet(UObject* Pawn, UObject* FortAbilitySet);