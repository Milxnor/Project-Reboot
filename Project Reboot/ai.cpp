#include "ai.h"

FVector* __fastcall AI::GetRandomLocationSafeToReach(UObject* AIBotController, FVector* outVec, __int64* a3)
{
	// std::cout << "aa!\n";
	// FVector aa = FVector{ 1250, 1818, 3284 };
	// return &aa;

	std::cout << "default outVec: " << outVec->Describe() << '\n';
	std::cout << "a3: " << a3 << '\n';

	auto vec = GetRandomLocationSafeToReachO(AIBotController, outVec, a3);
	std::cout << "og ret: " << vec->Describe() << '\n';

	static auto ReachLocationValidationModeOffset = AIBotController->GetOffset("ReachLocationValidationMode");
	auto ReachLocationValidationMode = Get<EReachLocationValidationMode>(AIBotController, ReachLocationValidationModeOffset);
	std::cout << "ReachLocationValidationMode beofre: " << (int)(*ReachLocationValidationMode) << '\n';

	// *ReachLocationValidationMode = EReachLocationValidationMode::None;

	// auto aftervec = GetRandomLocationSafeToReachO(AIBotController, outVec, a3);
	// std::cout << "after vec: " << aftervec->Describe() << '\n';

	*outVec = FVector{ 1250, 1818, 3284 };
	return outVec; // outVec;
}