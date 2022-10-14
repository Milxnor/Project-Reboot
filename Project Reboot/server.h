#pragma once

#include "helper.h"
#include "definitions.h"

namespace Server
{
	static bool bUseBeacons = true;
	static bool bListening = false;
	static UObject* BeaconHost = nullptr;
	static UObject* NetDriver = nullptr;

	// Calls BeaconHost->PauseBeaconRequests and then SetWorld
	void PauseBeaconRequests(bool bPause);

	// Calls NetDriver->SetWorld
	void SetWorld(UObject* World);

	/// <summary>
	/// Returns true if NetDriver started listening successfully
	/// </summary>
	/// <param name="Port">The port that the NetDriver will listen on</param>
	/// <returns></returns>
	bool Listen(int Port = 7777);

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

		// void Disable();
	}
}