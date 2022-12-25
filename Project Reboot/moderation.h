#pragma once

#include "structs.h"

namespace Moderation
{
	namespace Banning
	{
		std::string GetFilePath();
		void Ban(UObject* PlayerController);
		void Unban(UObject* PlayerController);
		bool IsBanned(UObject* PlayerController);
	}
}