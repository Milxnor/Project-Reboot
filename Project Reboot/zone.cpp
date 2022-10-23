#include "zone.h"
#include "definitions.h"
#include "helper.h"

bool Zone::OnSafeZoneStateChange(UObject* Indicator, UFunction* Function, void* Parameters)
{
	if (Indicator && Parameters && Defines::bIsLateGame)
	{
		struct ASafeZoneIndicator_C_OnSafeZoneStateChange_Params { uint8_t NewState; bool bInitial; };

		auto Params = (ASafeZoneIndicator_C_OnSafeZoneStateChange_Params*)Parameters;

		static auto NextCenterOffset = FindOffsetStruct2("/Script/FortniteGame.FortSafeZoneIndicator", "NextCenter", false, true);
		auto NextCenter = (FVector*)(__int64(Indicator) + NextCenterOffset);

		auto AuthGameMode = Helper::GetGameMode();

		static auto SafeZonePhaseOffset = AuthGameMode->GetOffset("SafeZonePhase");
		auto SafeZonePhase = *(int*)(__int64(AuthGameMode) + SafeZonePhaseOffset);

		bool bIsStartZone = SafeZonePhase == 0;

		auto GameState = Helper::GetGameState();

		static auto AircraftsOffset = GameState->GetOffset("Aircrafts");
		auto Aircrafts = (TArray<UObject*>*)(__int64(GameState) + AircraftsOffset);

		auto Aircraft = Aircrafts->At(0);

		if (!Aircraft)
			return false;

		static FVector AircraftLocation;

		static int LastResetNum = 824524899135;

		if (LastResetNum != Defines::AmountOfRestarts)
		{
			LastResetNum = Defines::AmountOfRestarts;
			AircraftLocation = Helper::GetActorLocation(Aircraft);
		}

		FString StartShrinkSafeZone;
		StartShrinkSafeZone.Set(L"startshrinksafezone");

		FString SkipShrinkSafeZone;
		SkipShrinkSafeZone.Set(L"skipshrinksafezone");

		static auto NextRadiusOffset = Indicator->GetOffset("NextRadius");
		auto NextRadius = (float*)(__int64(Indicator) + NextRadiusOffset);

		std::cout << "SafeZonePhase: " << SafeZonePhase << '\n';

		auto nextCenter = AircraftLocation;

		if (SafeZonePhase == 0) {
			Helper::ExecuteConsoleCommand(SkipShrinkSafeZone);
			static auto RadiusOffset = Indicator->GetOffset("Radius");
			*(float*)(__int64(Indicator) + RadiusOffset) = 20000;

			*NextRadius = 20000;
			*NextCenter = nextCenter;
		}
		if (SafeZonePhase == 1) {
			Helper::ExecuteConsoleCommand(SkipShrinkSafeZone);
			*NextRadius = 20000;
			*NextCenter = nextCenter;
		}
		if (SafeZonePhase == 2) {
			Helper::ExecuteConsoleCommand(SkipShrinkSafeZone);
			*NextRadius = 20000;
			*NextCenter = nextCenter;
		}
		if (SafeZonePhase == 3) {
			Helper::ExecuteConsoleCommand(SkipShrinkSafeZone);
			*NextRadius = 20000;
			*NextCenter = nextCenter;
		}
		if (SafeZonePhase == 4) {
			Helper::ExecuteConsoleCommand(StartShrinkSafeZone);
			*NextRadius = 9500;
			*NextCenter = nextCenter;
		}
		if (SafeZonePhase == 5) {
			Helper::ExecuteConsoleCommand(StartShrinkSafeZone);
			*NextRadius = 4000;
			*NextCenter = nextCenter;
		}
		if (SafeZonePhase == 6) {
			Helper::ExecuteConsoleCommand(StartShrinkSafeZone);
			*NextRadius = 1000;
			*NextCenter = nextCenter;
		}
		if (SafeZonePhase == 7) {
			Helper::ExecuteConsoleCommand(StartShrinkSafeZone);
			*NextRadius = 0;
			*NextCenter = nextCenter;
		}
		if (SafeZonePhase == 8) {
			Helper::ExecuteConsoleCommand(StartShrinkSafeZone);
			*NextRadius = 0;
			*NextCenter = nextCenter;
		}
		if (SafeZonePhase == 9) {
			Helper::ExecuteConsoleCommand(StartShrinkSafeZone);
			*NextRadius = 0;
			*NextCenter = nextCenter;
		}

		StartShrinkSafeZone.Free();
		SkipShrinkSafeZone.Free();
	}

	return false;
}