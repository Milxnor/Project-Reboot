#pragma once

#include "structs.h"

namespace Teams
{
	static inline constexpr int StartingTeamIndex = 4;
	static inline int NextTeamIndex = StartingTeamIndex;
	static inline int CurrentNumPlayersOnTeam = 0; // Scuffed

	bool AssignTeam(UObject* Controller);
}