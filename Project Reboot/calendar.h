// Filename is a bit misleading, this file is for specific things that happen on versions. For example missing pois, waterlevel, etc.

#pragma once

#include "structs.h"

namespace Calendar
{
	void FixLocations();
	void SetWaterLevel(int WaterLevel = 0);
}