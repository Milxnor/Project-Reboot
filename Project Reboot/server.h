#pragma once

#include "helper.h"
#include "definitions.h"

namespace Server
{
	inline bool bUseBeacons = true;
	inline bool bListening = false;
	inline UObject* BeaconHost = nullptr;
	inline UObject* NetDriver = nullptr;

	// Calls BeaconHost->PauseBeaconRequests and then SetWorld
	void PauseBeaconRequests(bool bPause);

	// Calls NetDriver->SetWorld
	void SetWorld(UObject* World);

	/// Returns true if NetDriver started listening successfully
	bool Listen(int Port = 7777);
	
	void Restart();

	namespace Hooks
	{
		// Creates and enables the hooks using MinHook
		void Initialize();

		// Call ServerReplicateActors
		void TickFlush(UObject* thisNetDriver, float DeltaSeconds);

		// Returns or else it will kick (TODO Not hook this)
		void KickPlayer(UObject* GameSession, UObject* Controller, FText a3);

		// Returns or else it will kick (TODO Not hook this)
		char ValidationFailure(__int64* a1, __int64 a2);

		// Returns or else it will kick (TODO Not hook this)
		__int64 NoReservation(__int64* a1, __int64 a2, char a3, __int64 a4);

		UObject* GetViewTarget(UObject* PC, __int64 Unused, __int64 a3);

		// void Disable();
	}
}