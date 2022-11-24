#pragma once

#include "structs.h"

UObject* GetEventLoader();
UObject* GetEventScripting();

namespace Events
{
	inline bool bHasBeenLoaded = false;

	bool HasEvent(); // Does this version have a event?
	bool IsEventSupported(); // First checks HasEvent and then checks if we support it
	void LoadEvent();
	void StartEvent();
	void StartNewYears();
	std::string GetEventPlaylistName();
	BothVector GetSpawnLocation(bool* outSuccess = nullptr);
	// std::string GetEventCodename();

	// Stuff for specific events

	void Unvault(FName ItemToUnvault);
}