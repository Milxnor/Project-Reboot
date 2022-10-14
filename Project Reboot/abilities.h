#pragma once

#include "structs.h"

struct FGameplayAbilitySpecHandle
{
    int Handle;

    void GenerateNewHandle()
    {
        // Must be in C++ to avoid duplicate statics across execution units
        static int32_t GHandle = 1;
        Handle = GHandle++;
    }
};

namespace Abilities
{
	inline UObject* GameplayAbilitySpecClass = nullptr;

	void* GenerateNewSpec(UObject* DefaultObject);
}