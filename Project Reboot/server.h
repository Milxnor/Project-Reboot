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

	// Returns true if successful.
	bool Listen(int Port = 7777);
}