#pragma once

#include "structs.h"

namespace Abilities
{
	inline UObject* GameplayAbilitySpecClass = nullptr;

    void ClientActivateAbilityFailed(UObject* ASC, FGameplayAbilitySpecHandle AbilityToActivate, int16_t PredictionKey);
	void* GenerateNewSpec(UObject* DefaultObject);
    UObject* DoesASCHaveAbility(UObject* ASC, UObject* Ability);
    void* GrantGameplayAbility(UObject* TargetPawn, UObject* GameplayAbilityClass);

    // HOOKS

    bool ServerTryActivateAbility(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters);
    bool ServerTryActivateAbilityWithEventData(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters);
    bool ServerAbilityRPCBatch(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters);
}