#pragma once

#include <string>
#include <vector>
#include <Windows.h>

namespace Memory
{
	unsigned long long FindPattern(std::string signatureStr, bool bRelative = false, uint32_t offset = 0, bool bIsVar = false);
}