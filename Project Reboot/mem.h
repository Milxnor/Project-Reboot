#pragma once

#include <Windows.h>
#include <vector>
#include <string>

namespace Memory
{
	unsigned long long FindPattern(std::string signatureStr, bool bRelative = false, uint32_t offset = 0, bool bIsVar = false);
}