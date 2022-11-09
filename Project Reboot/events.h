#pragma once

#include "structs.h"

UObject* GetEventLoader();
UObject* GetEventScripting();

namespace Events
{
	inline bool bHasBeenLoaded = false;

	bool HasEvent(); // Does this version have a event?
	void LoadEvent();
	void StartEvent();

	// Stuff for specific events
}