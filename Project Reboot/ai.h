#pragma once

#include "definitions.h"

namespace AI
{
    inline FVector* (__fastcall* GetRandomLocationSafeToReachO)(UObject* AIBotController, FVector* outVec, __int64* a3);
    FVector* __fastcall GetRandomLocationSafeToReach(UObject* AIBotController, FVector* outVec, __int64* a3);
}