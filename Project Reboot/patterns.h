#pragma once

#include <regex>

#include "definitions.h"
#include "mem.h"

inline FString(*GetEngineVersion)();

inline uint64_t InitHostAddress = 0;
inline uint64_t StaticFindObjectAddress = 0;
inline uint64_t StaticLoadObjectAddress = 0;
inline uint64_t ProcessEventAddress = 0;
inline uint64_t SetWorldAddress = 0;
inline uint64_t PauseBeaconRequestsAddress = 0;
inline uint64_t ObjectsAddress = 0;
inline uint64_t InitListenAddress = 0;
inline uint64_t TickFlushAddress = 0;
inline uint64_t KickPlayerAddress = 0;
inline uint64_t ValidationFailureAddress = 0;
inline uint64_t ReallocAddress = 0;
inline uint64_t NoReserveAddress = 0;
inline uint64_t InternalTryActivateAbilityAddress = 0;
inline uint64_t GiveAbilityAddress = 0;
inline uint64_t CantBuildAddress = 0;
inline uint64_t ReplaceBuildingActorAddress = 0;
inline uint64_t WorldGetNetModeAddress = 0;
inline uint64_t NoMCPAddress = 0;
inline uint64_t CanActivateAbilityAddress = 0;
inline uint64_t FreeAddress = 0;
inline uint64_t HandleReloadCostAddress = 0;
inline uint64_t ActorGetNetModeAddress = 0;

static bool InitializePatterns()
{
	auto SpawnActorAddr = Memory::FindPattern("40 53 56 57 48 83 EC 70 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 0F 28 1D ? ? ? ? 0F 57 D2 48 8B B4 24 ? ? ? ? 0F 28 CB");

	if (!SpawnActorAddr)
		SpawnActorAddr = Memory::FindPattern("40 53 48 83 EC 70 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 0F 28 1D ? ? ? ? 0F 57 D2 48 8B 9C 24 ? ? ? ? 0F 28 CB 0F 54 1D ? ? ? ? 0F 57");

	if (!SpawnActorAddr)
		SpawnActorAddr = Memory::FindPattern("48 89 5C 24 ? 55 56 57 48 8B EC 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 F0 0F 28 05 ? ? ? ? 48 8B FA 0F 28 0D ? ? ? ? 48 8B D9 48 8B 75 40 0F 29 45 C0 0F 28 05 ? ? ? ? 0F 29 45 E0 0F 29 4D D0 4D 85 C0 74 12 F3 41 0F 10 50 ? F2 41 0F 10 18");

	if (!SpawnActorAddr)
		SpawnActorAddr = Memory::FindPattern("48 89 5C 24 ? 55 56 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 2F 0F 28 0D ? ? ? ? 48 8B FA 0F 28 15 ? ? ? ? 48 8B D9 0F"); // 20.00

	SpawnActorO = decltype(SpawnActorO)(SpawnActorAddr);

	/* if (!SpawnActorO)
	{
		SpawnActorAddr = Memory::FindPattern("48 8B C4 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 A8 0F 29 78 98 44 0F 29 40 ? 44 0F 29 88 ? ? ? ? 44 0F 29 90 ? ? ? ? 44 0F 29 98 ? ? ? ? 44 0F 29 A0 ? ? ? ? 44 0F 29 A8 ? ? ? ? 44 0F 29 B0 ? ? ? ? 44 0F 29 B8 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33"); // 20.00

		SpawnActorTransform = decltype(SpawnActorTransform)(SpawnActorAddr);
	} */

	std::string InitHostPattern = "";
	std::string StaticFindObjectPattern = "";
	std::string StaticLoadObjectPattern = "";
	std::string ProcessEventPattern = "";
	std::string SetWorldPattern = "";
	std::string PauseBeaconRequestsPattern = "";
	std::string ObjectsPattern = "";
	std::string InitListenPattern = "";
	std::string TickFlushPattern = "";
	std::string KickPlayerPattern = "";
	std::string ValidationFailurePattern = "";
	std::string ReallocPattern = "";
	std::string NoReservePattern = "";
	std::string InternalTryActivateAbilityPattern = "";
	std::string GiveAbilityPattern = "";
	std::string CantBuildPattern = "";
	std::string ReplaceBuildingActorPattern = "";
	std::string WorldGetNetModePattern = "";
	std::string ActorGetNetModePattern = "";
	std::string NoMCPPattern = "";
	std::string CanActivateAbilityPattern = "";
	std::string FreePattern = "";
	std::string HandleReloadCostPattern = "";

	bool bIsNoMCPRelative = false;
	bool bIsTickFlushRelative = false;

	// TODO REWRITE HERE

	GetEngineVersion = decltype(GetEngineVersion)(Memory::FindPattern(("40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8B C8 41 B8 04 ? ? ? 48 8B D3")));

	std::string FullVersion;
	FString toFree;

	if (!GetEngineVersion)
	{
		auto VerStr = Memory::FindPattern(("2B 2B 46 6F 72 74 6E 69 74 65 2B 52 65 6C 65 61 73 65 2D ? ? ? ?"));

		// if (!VerStr)

		FullVersion = decltype(FullVersion.c_str())(VerStr);
		Engine_Version = 500;
	}

	else
	{
		toFree = GetEngineVersion();
		FullVersion = toFree.ToString();
	}

	std::string FNVer = FullVersion;
	std::string EngineVer = FullVersion;

	if (!FullVersion.contains(("Live")) && !FullVersion.contains(("Next")) && !FullVersion.contains(("Cert")))
	{
		if (GetEngineVersion)
		{
			FNVer.erase(0, FNVer.find_last_of(("-"), FNVer.length() - 1) + 1);
			EngineVer.erase(EngineVer.find_first_of(("-"), FNVer.length() - 1), 40);

			if (EngineVer.find_first_of(".") != EngineVer.find_last_of(".")) // this is for 4.21.0 and itll remove the .0
				EngineVer.erase(EngineVer.find_last_of((".")), 2);

			Engine_Version = std::stod(EngineVer) * 100;
		}

		else
		{
			const std::regex base_regex(("-([0-9.]*)-"));
			std::cmatch base_match;

			std::regex_search(FullVersion.c_str(), base_match, base_regex);

			FNVer = base_match[1];
		}

		Fortnite_Version = std::stod(FNVer);

		if (Fortnite_Version >= 16.00 && Fortnite_Version <= 18.40)
			Engine_Version = 427; // 4.26.1;
	}

	else
	{
		// TODO
		Engine_Version = 419;
		Fortnite_Version = 1.8;
	}

	Fortnite_Season = std::floor(Fortnite_Version);

	// TO HERE

	// Now we have the engine version and fn 

	// patterns

	if (Engine_Version == 416)
	{
		InitHostPattern = "";
		StaticFindObjectPattern = "";
		StaticLoadObjectPattern = "";
		ProcessEventPattern = "";
		SetWorldPattern = "";
		PauseBeaconRequestsPattern = "";
	}

	if (Engine_Version == 419)
	{
		InitHostPattern = "";
		StaticFindObjectPattern = "";
		StaticLoadObjectPattern = "";
		ProcessEventPattern = "";
		SetWorldPattern = "";
		PauseBeaconRequestsPattern = "";
	}

	if (Engine_Version == 420)
	{
		InitHostPattern = "48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ?";
		StaticFindObjectPattern = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6";
		StaticLoadObjectPattern = "4C 89 4C 24 ? 48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 33 D2 48 8D 05 ? ? ? ? 38 15";
		ProcessEventPattern = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 48 63 41 0C 45 33 F6";
		SetWorldPattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B B1 ? ? ? ? 48 8B FA 48 8B D9 48 85 F6 74 5C";
		PauseBeaconRequestsPattern = "40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44";
		ObjectsPattern = "48 8B 05 ? ? ? ? 48 8D 1C C8 81 4B ? ? ? ? ? 49 63 76 30";
		InitListenPattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0";
		TickFlushPattern = "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 48 8D 05 ? ? ? ? 49 89 7B E8 48 8B F9 4D 89 63 E0 45 33 E4";
		KickPlayerPattern = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 49 8B F0 48 8B DA 48 85 D2";
		ValidationFailurePattern = "40 53 56 41 56 48 81 EC ? ? ? ? 45 33 F6 48 8B DA 44 89 B4 24";
		ReallocPattern = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D ? ? ? ?";
		NoReservePattern = "48 89 5C 24 ? 48 89 6C 24 ? 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 01 49 8B E9 45 0F B6 F8";
		InternalTryActivateAbilityPattern = "4C 89 4C 24 20 4C 89 44 24 18 89 54 24 10 55 53 56";
		GiveAbilityPattern = "48 89 5C 24 ? 56 57 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B F0 4C 8B F2 48 8B D9 7E 61";
		CantBuildPattern = "48 89 54 24 ? 55 56 41 56 48 83 EC 50";
		ReplaceBuildingActorPattern = "48 8B C4 4C 89 40 18 55 57 48 8D A8 ? ? ? ? 48 81 EC";
		FreePattern = "48 85 C9 74 1D 4C 8B 05 ? ? ? ? 4D 85 C0 0F 84 ? ? ? ? 49";

		if (Fortnite_Season == 4)
			ObjectsPattern = "48 8B 05 ? ? ? ? 48 8D 14 C8 EB 03 49 8B D6 8B 42 08 C1 E8 1D A8 01 0F 85 ? ? ? ? F7 86 ? ? ? ? ? ? ? ?";
	}

	if (Engine_Version == 421)
	{
		InitHostPattern = "48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ?";
		StaticFindObjectPattern = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6";
		StaticLoadObjectPattern = "4C 89 4C 24 ? 48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 78 45 33 F6 48 8D 05 ? ? ? ?";
		ProcessEventPattern = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? ? ? ? 45 33 F6";
		SetWorldPattern = "48 89 5C 24 ? 57 48 83 EC 20 48 8B FA 48 8B D9 48 8B 91 ? ? ? ? 48 85 D2 74 28 E8 ? ? ? ? 48 8B 8B ? ? ? ?";
		PauseBeaconRequestsPattern = "40 53 48 83 EC 30 48 8B 99 ? ? ? ? 48 85 DB 0F 84 ? ? ? ? 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 41 B9";
		ObjectsPattern = "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1";
		InitListenPattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0";
		TickFlushPattern = "4C 8B DC 55 49 8D AB 98 FE FF FF 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 00 01 00 00 49 89 5B 18 48 8D 05 ? ? ? ?";
		KickPlayerPattern = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 49 8B F0 48 8B DA 48 85 D2";
		ValidationFailurePattern = "40 53 55 41 56 48 81 EC ? ? ? ? 33 ED";
		ReallocPattern = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D ? ? ? ?";
		NoReservePattern = "40 53 41 56 48 81 EC ? ? ? ? 48 8B 01 48 8B DA 4C 8B F1 FF 90";
		InternalTryActivateAbilityPattern = "4C 89 4C 24 20 4C 89 44 24 18 89 54 24 10 55 53 56";
		GiveAbilityPattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9";
		CantBuildPattern = "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC ? 49 8B E9 4D 8B F0";
		ReplaceBuildingActorPattern = "48 8B C4 44 89 48 20 55 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 70 E8 33 FF 40 38 3D ? ? ? ? 48 8B F1 4C 89 60 E0 44 8B E2 4C 89 70 D0 44 8B F7";
		CanActivateAbilityPattern = "4C 89 4C 24 20 55 56 57 41 56 48 8D 6C 24 D1";
		FreePattern = "48 85 C9 74 2E 53 48 83 EC 20 48 8B D9";
		HandleReloadCostPattern = "89 54 24 10 55 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 80 B9 ? ? ? ? ? 4C 8B F1 0F 85";
		ActorGetNetModePattern = "48 89 5C 24 ? 57 48 83 EC 20 48 8B 01 48 8B D9 FF 90 ? ? ? ? 4C 8B 83 ? ? ? ? 48 8B F8 33 C0 48 C7";
		WorldGetNetModePattern = "40 53 48 81 EC ? ? ? ? 48 83 79 ? ? 48 8B D9 74 0E B8 ? ? ? ? 48 81 C4 ? ? ? ? 5B C3"; // 5.41
		NoMCPPattern = "E8 ? ? ? ? 84 C0 75 CE"; // 5.41	

		/* WorldGetNetModePattern = "40 53 48 81 EC ? ? ? ? 48 83 79 ? ? 48 8B D9 74 0E B8 ? ? ? ? 48 81 C4 ? ? ? ? 5B C3 48 8B 89 ? ? ? ? 48 85 C9 74 0D 48 81 C4 ? ? ? ? 5B E9 ? ? ? ? 48 8B 0D ? ? ? ?";
		NoMCPPattern = "E8 ? ? ? ? 84 C0 75 CE";
					
		bIsNoMCPRelative = true; */

		if (Fortnite_Version <= 6.02)
			ObjectsPattern = "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1";

		if (Fortnite_Version >= 6.3)
			PauseBeaconRequestsPattern = "40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44";
	}

	if (Engine_Version == 422)
	{
		InitHostPattern = "48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ?";
		StaticFindObjectPattern = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6";
		StaticLoadObjectPattern = "4C 89 4C 24 ? 48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 78 45 33 F6 48 8D 05 ? ? ? ?";
		ProcessEventPattern = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? ? ? ? 45 33 F6";
		SetWorldPattern = "48 89 74 24 ? 57 48 83 EC 20 48 8B F2 48 8B F9 48 8B 91";
		PauseBeaconRequestsPattern = "40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 41 B9 ? ? ? ?";
		ObjectsPattern = "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1";
		InitListenPattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0";
		TickFlushPattern = "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 73 F0 48 8B F1 49 89 7B E8 48 8D 0D ? ? ? ? 4D 89 73 D0";
		KickPlayerPattern = "40 53 41 56 48 81 EC ? ? ? ? 48 8B 01 48 8B DA 4C 8B F1";
		ValidationFailurePattern = "40 53 56 41 56 48 81 EC ? ? ? ? 45 33 F6 48 8B DA"; // 2nd string
		ReallocPattern = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D ? ? ? ?";
		NoReservePattern = "48 89 5C 24 ? 48 89 6C 24 ?? 56 41 56 41 57 48 81 EC";
		InternalTryActivateAbilityPattern = "4C 89 4C 24 20 4C 89 44 24 18 89 54 24 10 55 53 56";
		GiveAbilityPattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9";
		CantBuildPattern = "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC ? 49 8B E9 4D 8B F0";
		ReplaceBuildingActorPattern = "4C 8B DC 55 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 85 ? ? ? ? 33 FF 40 38 3D ? ? ? ?";
		FreePattern = "48 85 C9 74 2E 53 48 83 EC 20 48 8B D9";
		HandleReloadCostPattern = "89 54 24 10 55 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 80 B9 ? ? ? ? ? 4C 8B F1 0F 85"; // got on 7.10
		CanActivateAbilityPattern = "4C 89 4C 24 20 55 56 57 41 56 48 8D 6C 24 D1";
		WorldGetNetModePattern = "40 53 48 81 EC ? ? ? ? 48 83 79 ? ? 48 8B D9 74 0E B8 ? ? ? ? 48 81 C4"; // got on 7.3
		NoMCPPattern = "E8 ? ? ? ? 84 C0 75 CE";

		if (Fortnite_Version == 7.10)
		{
			TickFlushPattern = "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 73 F0 49 89 7B E8 48 8B F9 4D 89 73 D0";
			SetWorldPattern = "48 89 5C 24 ? 57 48 83 EC 20 48 8B FA 48 8B D9 48 8B 91 ? ? ? ? 48 85 D2 74 28 E8 ? ? ? ? 48 8B 8B";
		}

		if (Fortnite_Version == 7.30)
		{
			TickFlushPattern = "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 73 F0 49 89 7B E8 48 8B";
			SetWorldPattern = "48 89 5C 24 ? 57 48 83 EC 20 48 8B FA 48 8B D9 48 8B 91 ? ? ? ? 48 85 D2 74 28 E8 ? ? ? ? 48";
		}
	}

	if (Engine_Version == 423)
	{
		InitHostPattern = "48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ?";
		StaticFindObjectPattern = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6";
		StaticLoadObjectPattern = "4C 89 4C 24 ? 48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 78 45 33 F6 48 8D 05 ? ? ? ?";
		ProcessEventPattern = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? ? ? ? 45 33 F6";
		SetWorldPattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F2 48 8B F9 48 85 DB 0F 84 ? ? ? ? 48 8B 97";
		PauseBeaconRequestsPattern = "40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 41 B9 ? ? ? ?";
		ObjectsPattern = "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1";
		InitListenPattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0";
		TickFlushPattern = "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 73 F0 48 8B F1 49 89 7B E8 48 8D 0D ? ? ? ? 4D 89 73 D0";
		KickPlayerPattern = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 49 8B F0 48 8B DA 48 85 D2";
		ValidationFailurePattern = "40 55 53 41 54 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 45 33 E4 48 8B DA 44 89 65 50";
		ReallocPattern = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D ? ? ? ?";
		NoReservePattern = "48 89 5C 24 ? 48 89 6C 24 ?? 56 41 56 41 57 48 81 EC";
		InternalTryActivateAbilityPattern = "4C 89 4C 24 20 4C 89 44 24 18 89 54 24 10 55 53 56";
		GiveAbilityPattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9";
		CantBuildPattern = "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC ? 49 8B E9 4D 8B F0";
		ReplaceBuildingActorPattern = "4C 8B DC 55 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 85 ? ? ? ? 33 FF 40 38 3D ? ? ? ?";
		FreePattern = "48 85 C9 74 2E 53 48 83 EC 20 48 8B D9";
		CanActivateAbilityPattern = "48 89 5C 24 ? 4C 89 4C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 49 8B F0 8B DA 48 8B F9 4D 85 C0 0F 84 ? ? ? ? 49 8D 48 10 E8 ? ? ? ? 48 85";
		WorldGetNetModePattern = "40 53 48 81 EC ? ? ? ? 48 83 79 ? ? 48 8B D9 74 0E B8 ? ? ? ? 48 81 C4 ? ? ? ? 5B C3 48 8B 89 ? ? ? ? 48"; // 8.51
		NoMCPPattern = "E8 ? ? ? ? 84 C0 75 C0"; // 8.51
		HandleReloadCostPattern = "89 54 24 10 55 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 80 B9 ? ? ? ? ? 4C 8B F1 0F 85"; // untested

		if (Fortnite_Season == 10)
			ValidationFailurePattern = "40 53 41 56 48 81 EC ? ? ? ? 48 8B 01 48 8B DA 4C 8B F1 FF 90 ? ? ? ? 48 8B 0D";
	}

	if (Engine_Version == 424)
	{
		InitHostPattern = "48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ?";
		StaticFindObjectPattern = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6";
		StaticLoadObjectPattern = "4C 89 4C 24 ? 48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 78 45 33 F6 48 8D 05 ? ? ? ?";
		ProcessEventPattern = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? ? ? ? 45 33 F6";
		SetWorldPattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F2 48 8B F9 48 85 DB 0F 84 ? ? ? ? 48 8B 97";
		PauseBeaconRequestsPattern = "40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 41 B9 ? ? ? ?";
		ObjectsPattern = "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1";
		InitListenPattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0";
		TickFlushPattern = "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 73 F0 48 8B F1 49 89 7B E8 48 8D 0D ? ? ? ? 4D 89 73 D0";
		KickPlayerPattern = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 49 8B F0 48 8B DA 48 85 D2";
		ValidationFailurePattern = "40 55 53 41 54 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 45 33 E4 48 8B DA 44 89 65 50";
		ReallocPattern = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D ? ? ? ?";
		NoReservePattern = "48 89 5C 24 ? 48 89 6C 24 ?? 56 41 56 41 57 48 81 EC";
		InternalTryActivateAbilityPattern = "4C 89 4C 24 20 4C 89 44 24 18 89 54 24 10 55 53 56";
		GiveAbilityPattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9";
		CantBuildPattern = "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC ? 49 8B E9 4D 8B F0";
		FreePattern = "48 85 C9 74 2E 53 48 83 EC 20 48 8B D9";
		CanActivateAbilityPattern = "48 89 5C 24 ? 4C 89 4C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 49 8B F0 8B DA";
	}

	if (Engine_Version == 425)
	{
		InitHostPattern = "48 8B C4 48 81 EC ? ? ? ? 48 89 58 10 4C 8D 05";
		// StaticFindObjectPattern = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC ? 80 3D ? ? ? ? ? 45 0F B6";
		StaticFindObjectPattern = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6";
		StaticLoadObjectPattern = "4C 89 4C 24 ? 48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 68 45 33 F6 48 8D 05";
		ProcessEventPattern = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? ? ? ? 45 33 F6";
		SetWorldPattern = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8B EC 48 83 EC 30 48 8B 99";
		PauseBeaconRequestsPattern = "40 53 48 83 EC 30 48 8B D9 84 D2 74 5E 80 3D ? ? ? ? ? 72";
		ObjectsPattern = "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1";
		InitListenPattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0";
		TickFlushPattern = "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 73 F0 49 89 7B E8 48 8B F9 4D";
		KickPlayerPattern = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 49 8B F0 48 8B DA 48 85 D2";
		ValidationFailurePattern = "48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 DB 48 8B F2";
		ReallocPattern = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D ? ? ? ?";
		NoReservePattern = "48 89 5C 24 ? 48 89 6C 24 ?? 56 41 56 41 57 48 81 EC";
		InternalTryActivateAbilityPattern = "4C 89 4C 24 20 4C 89 44 24 18 89 54 24 10 55 53 56";
		GiveAbilityPattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9";
		CantBuildPattern = "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC ? 49 8B E9 4D 8B F0";
		ReplaceBuildingActorPattern = "4C 8B DC 55 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 85 ? ? ? ? 33 FF 40 38 3D ? ? ? ? 49";
		FreePattern = "48 85 C9 74 2E 53 48 83 EC 20 48 8B D9";
		HandleReloadCostPattern = "89 54 24 10 55 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 80";
		CanActivateAbilityPattern = "48 89 5C 24 ? 4C 89 4C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 49";
		WorldGetNetModePattern = "40 53 48 81 EC ? ? ? ? 48 83 79 ? ? 48 8B D9 74 0E B8 ? ? ? ? 48 81 C4 ? ? ? ? 5B C3"; // 12.61
		NoMCPPattern = "E8 ? ? ? ? 84 C0 75 C1";

		if (Fortnite_Version == 12.00)
		{
			SetWorldPattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F2 48 8B F9 48 85 DB 0F 84 ? ? ? ? 48 8B 97";
		}
	}

	if (Engine_Version == 426)
	{
		InitHostPattern = "48 8B C4 48 81 EC ? ? ? ? 48 89 58 10 4C 8D 05";
		StaticFindObjectPattern = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6";
		StaticLoadObjectPattern = "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 8B 85 ? ? ? ? 48 8B FA"; // 14.60
		ProcessEventPattern = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? ? ? ? 45 33 F6";
		PauseBeaconRequestsPattern = "40 57 48 83 EC 30 48 8B F9 84 D2 74 62 80 3D ? ? ? ? ? 72 22 48 8D 05"; // 14.60
		ObjectsPattern = "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1";
		InitListenPattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0";
		TickFlushPattern = "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 73 F0 48 8B F1 49 89 7B E8 4D 89 7B"; // 14.60
		KickPlayerPattern = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 49 8B F0 48 8B DA 48 85 D2";
		ValidationFailurePattern = "48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 DB 48 8B F2 89 9D ? ? ? ? 4C 8B E9 E8 ? ? ? ? "; // 14.60
		ReallocPattern = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D ? ? ? ?";
		// NoReservePattern = "48 89 5C 24 ? 48 89 6C 24 ?? 56 41 56 41 57 48 81 EC";
		InternalTryActivateAbilityPattern = "4C 89 4C 24 20 4C 89 44 24 18 89 54 24 10 55 53 56";
		GiveAbilityPattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 8B 81 ? ? ? ? 49"; // 14.60
		CantBuildPattern = "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC ? 49 8B E9 4D 8B F0";
		ReplaceBuildingActorPattern = "4C 8B DC 55 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 85 ? ? ? ? 33 FF 40 38 3D ? ? ? ? 49 89 5B";
		WorldGetNetModePattern = "40 53 48 81 EC ? ? ? ? 48 83 79 ? ? 48 8B D9 74 0E B8 ? ? ? ? 48 81 C4 ? ? ? ? 5B C3 48 8B 89 ? ? ? ? 48 85 C9 74 0D 48 81 C4 ? ? ? ? 5B E9 ? ? ? ? 48 8B 0D ? ? ? ?";
		CanActivateAbilityPattern = "48 89 5C 24 ? 4C 89 4C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 49 8B F0 8B DA 48";
		HandleReloadCostPattern = "89 54 24 10 55 53 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 80 B9 ? ? ? ? ? 8B DA";
		FreePattern = "48 85 C9 74 2E 53 48 83 EC 20 48 8B D9";
		// ActorGetNetModePattern = "48 89 5C 24 ? 57 48 83 EC 20 48 8B 01 48 8B D9 FF 90 ? ? ? ? 48 8B 9B ? ? ? ? BA ? ? ? ? 8B";

		if (Fortnite_Season == 13)
			GiveAbilityPattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49";

		if (Fortnite_Version == 15.30)
			TickFlushPattern = "4C 8B DC 55 49 8D AB 78 FE FF FF 48 81 EC 80 02 ? ? 48 8B 05 AF B7 51 04 48 33 C4 48 89 85 ? 01 ? ? 49 89 5B 18 49 89 73 F0 48 8B F1 49 89 7B E8";

		if (Fortnite_Version == 15.10)
		{
			TickFlushPattern = "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 73 F0 48 8B F1 49";
			NoMCPPattern = "48 83 EC 28 65 48 8B 04 25 ? ? ? ? 8B 0D ? ? ? ? BA ? ? ? ? 48 8B 0C C8 8B 04 0A 39 05 ? ? ? ? 7F 0C 0F B6 05 ? ? ? ? 48 83 C4 28 C3 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 DF E8 ? ? ? ? 48 8B C8 48 8D 15 ? ? ? ? E8 ? ? ? ? 48 8D 0D";
		}

		if (Fortnite_Season == 13)
			TickFlushPattern = "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 73 F0 48 8B F1 49 89 7B E8 48 8D 0D ? ? ? ? 4D 89 63 E0 45 33 E4 4D 89 6B D8 45";

		if (Fortnite_Version == 14.60)
			NoMCPPattern = "48 83 EC 28 65 48 8B 04 25 ? ? ? ? 8B 0D ? ? ? ? BA ? ? ? ? 48 8B 0C C8 8B 04 0A 39 05 ? ? ? ? 7F 0C 0F B6 05 ? ? ? ? 48 83 C4 28 C3 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 DF E8 ? ? ? ? 48 8B C8 48 8D 15 ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? 88 05 ? ? ? ? E8 ? ? ? ? EB B7";
	}

	if (Engine_Version == 427) // 4.26.1
	{
		InitHostPattern = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B F1 4C 8D 05";
		StaticFindObjectPattern = "40 55 53 57 41 54 41 55 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05";
		StaticLoadObjectPattern = "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 8B 85 ? ? ? ? 48 8B FA"; // 14.60
		ProcessEventPattern = "40 55 53 56 57 41 54 41 56 41 57 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 8B 05 ? ? ? ? 48 33";
		PauseBeaconRequestsPattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 30 33 F6 48 8B F9 84 D2 74 42 80 3D";
		ObjectsPattern = "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1";
		InitListenPattern = "4C 8B DC 49 89 5B 08 49 89 73 10 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0";
		TickFlushPattern = "48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B F9 48";
		KickPlayerPattern = "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 60 48 83 65 ? ? 4C 8B F2";
		ValidationFailurePattern = "48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 DB 48 8B F2 89 9D ? ? ? ? 4C 8B E9 E8 ? ? ? ?";
		ReallocPattern = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D ? ? ? ?";
		InternalTryActivateAbilityPattern = "4C 89 4C 24 20 4C 89 44 24 18 89 54 24 10 55 53 56";
		GiveAbilityPattern = "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 83 EC 20 8B 81 ? ? ? ? 49 8B E8 4C";
		CantBuildPattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 60 4D 8B F1 4D";
		ReplaceBuildingActorPattern = "48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 85 ? ? ? ? 48";
		WorldGetNetModePattern = "40 53 48 81 EC ? ? ? ? 48 83 79 ? ? 48 8B D9 74 0E B8 ? ? ? ? 48 81 C4 ? ? ? ? 5B C3 48 8B 89 ? ? ? ? 48 85 C9 74 0D 48 81 C4 ? ? ? ? 5B E9 ? ? ? ? 48 8B 0D ? ? ? ?";
		CanActivateAbilityPattern = "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 20 55 41 54 41 55 41 56 41 57 48 8D 68 B1 48 81 EC ? ? ? ? 4C 8B F1 B9"; // got on 18.40
		FreePattern = "48 85 C9 0F 84 ? ? ? ? 48 89 5C 24 ? 57 48 83 EC 20 48 8B 3D ? ? ? ? 48 8B D9 48";
		NoReservePattern = "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 60 49 8B D9 45";
		HandleReloadCostPattern = "89 54 24 10 55 53 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 78 41 BD";

		if (Fortnite_Season == 16)
		{
			WorldGetNetModePattern = "48 83 EC 28 48 83 79 ? ? 75 20 48 8B 91 ? ? ? ? 48 85 D2 74 1E 48 8B 02 48 8B CA FF 90 ? ? ? ? 84 C0";
			NoMCPPattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC 20 65 48 8B 04 25 ? ? ? ? BA ? ? ? ? 48";
		}

		if (Fortnite_Season == 17)
		{
			InternalTryActivateAbilityPattern = "48 8B C4 4C 89 48 20 4C 89 40 18 89 50 10 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48"; // got on 17.30
			TickFlushPattern = "E8 ? ? ? ? 48 8B CB E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 48 8D 4D E7";
			bIsTickFlushRelative = true;
			// TickFlushPattern = "48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 8A"; // 17.10
		
			if (Fortnite_Version >= 17.30 && Fortnite_Version <= 17.50)
			{
				WorldGetNetModePattern = "48 83 EC 28 48 83 79 ? ? 75 20 48 8B 91 ? ? ? ? 48 85 D2 74 1E 48 8B 02 48 8B CA FF 90";
				NoMCPPattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC 20 65 48 8B 04 25 ? ? ? ? BA ? ? ? ? 48 8B 08 8B 04 0A 39 05 ? ? ? ? 7F 23 8A 05 ? ? ? ? 48";
			}
		}	

		if (Fortnite_Season == 18) // 18.40 specifically
		{
			StaticFindObjectPattern = "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 60 45 33 ED 45 8A F9 44 38 2D ? ? ? ? 49 8B F8 48 8B";
			InitHostPattern = "48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 55 41 56 41 57 48 8D 68 A1 48 81 EC ? ? ? ? 48 8B F1 4C 8D 35";
			TickFlushPattern = "48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 44 0F B6 A1";
			InternalTryActivateAbilityPattern = "4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24"; // got on 18.40
		}
	}

	if (Engine_Version == 500)
	{
		InitHostPattern = "48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 55 41 56 41 57 48 8D 68 A1 48 81 EC ? ? ? ? 4C 8B F1 48 8D 3D ? ? ? ?";
		StaticFindObjectPattern = "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 F6 4C 8B E1 45 0F B6 E9 49 8B F8 41 8B C6";
		StaticLoadObjectPattern = "48 8B C4 48 89 58 08 4C 89 48 20 4C 89 40 18 48 89 50 10 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 70 33 FF 48 8D 05 ? ? ? ? 40 38 3D ? ? ? ? 4C 8B E1 49 8B D0 48 8D 4D";
		// ProcessEventPattern = "40 55 53 56 57 41 54 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85";
		ProcessEventPattern = "40 55 53 56 57 41 54 41 56 41 57 48 81 EC ? ? ? ? 48 8D ? 24";
		PauseBeaconRequestsPattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 33 ED 48 8B F1 84 D2 74 27 80 3D";
		ObjectsPattern = "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1";
		InitListenPattern = "4C 8B DC 49 89 5B 10 49 89 73 18 57 48 83 EC 50 48 8B BC 24 ?? ?? ?? ?? 49 8B F0 48 8B";
		TickFlushPattern = "48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 44 8A A1";
		KickPlayerPattern = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8B EC 48 83 EC 50 48 8B DA 4C 8B F1 48 8D 15";
		ValidationFailurePattern = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8B EC 48 83 EC 60 48 8B DA 48 8B F1";
		ReallocPattern = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D ? ? ? ?";
		InternalTryActivateAbilityPattern = "48 8B C4 4C 89 48 20 4C 89 40 18 89 50 10 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 8B DA";
		GiveAbilityPattern = "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 83 EC 20 8B 81 ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 85 C0 0F 8F";
		CantBuildPattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 60 49 8B E9 4D 8B F8 48 8B DA 48 8B F9 BE ? ? ? ? 48";
		ReplaceBuildingActorPattern = "48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 90 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 85 ? ? ? ? 48 8D 3D ? ? ? ? 45 33 E4 44 89 4D D8 44 38 25";
		WorldGetNetModePattern = "48 83 EC 28 48 83 79 ? ? 75 20 48 8B 91 ? ? ? ? 48 85 D2 74 1E 48 8B 02 48 8B CA FF 90 ? ? ? ? 84 C0 0F 85 ? ? ? ? B8";
		CanActivateAbilityPattern = "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 20 55 41 54 41 55 41 56 41 57 48 8D 68 B1 48 81 EC ? ? ? ? 4D";
		FreePattern = "48 85 C9 0F 84 ? ? ? ? 48 89 5C 24 ? 57 48 83 EC 20 48 8B 3D ? ? ? ? 48 8B D9 48";
		NoReservePattern = "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 60 49 8B D9";
		HandleReloadCostPattern = "89 54 24 10 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 40 B6 03 8B DA";
		NoMCPPattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC 20 65 48 8B 04 25 ? ? ? ? BA ? ? ? ? 48 8B 08 8B 04 0A 39 05 ? ? ? ? 7F 23 8A 05 ? ? ? ? 48 8B 5C 24 ? 48 8B 6C 24 ? 48 8B 74 24 ? 48 83 C4 20 41 5F 41 5E 41 5D 41 5C 5F C3 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 C8 E8 ? ? ? ? 45 33 FF 48 8B F8 48 8B D8 66 44 39 38 74 7D 4C 8D 25 ? ? ? ? 4C 8D 2D ? ? ? ? 41 8D 77";

		if (Fortnite_Version == 19.10)
		{
			StaticLoadObjectPattern = "48 8B C4 48 89 58 08 4C 89 48 20 4C 89 40 18 48 89 50 10 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 70 33 FF 48 8D 05 ? ? ? ? 40 38 3D";
			ValidationFailurePattern = "48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 60 48 8B FA 48 8B F1 E8 ? ? ? ? 48 8B D0 E8 ? ? ? ? 85 C0";
			FreePattern = "48 85 C9 0F 84 ? ? ? ? 53 48 83 EC 20 48 89 7C 24 ? 48 8B D9 48 8B 3D ? ? ? ? 48 85"; // 22.3
			CantBuildPattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 60 49 8B E9 4D 8B F8 48 8B DA 48 8B";

			ToStringO = decltype(ToStringO)(Memory::FindPattern("48 89 5C 24 ? 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 83 79 04 00 48 8B DA 0F 85 ? ? ? ? 48 89 BC 24 ? ? ? ? E8 ? ? ? ? 48 8B F8 48 8D 54 24 ? 48 8B C8"));
			std::cout << "ToStringO: " << ToStringO << '\n';
		}
	}

	if (Fortnite_Season == 22)
	{
		TickFlushPattern = "48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8D 91";
		ValidationFailurePattern = "48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 60 48 8B FA 48 8B F1 E8 ? ? ? ? 48 8B D0 E8";
		NoReservePattern = "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 60 49 8B D9 45 8A F8 4C 8B F2 48 8B F9 45 32 E4 E8";
		GiveAbilityPattern = "48 89 5C 24 ? 55 56 57 41 56 41 57 48 8B EC 48 83 EC 30 49 8B 40 10 45 33 F6 4D 8B F8 48 8B F2 48 8B F9";
		StaticFindObjectPattern = "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 33 F6 4C 8B E1 48 83 CB FF";
		StaticLoadObjectPattern = "48 8B C4 48 89 58 08 4C 89 48 20 4C 89 40 18 48 89 50 10 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 81 EC ? ? ? ? 33";
		ProcessEventPattern = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 48 8B FA 4D 8B E0 33 D2 4C 8B F1";
	}

	// todo test tickflush: E8 ? ? ? ? 83 BE ? ? ? ? ? 0F 8E ? ? ? ? 48 8B 86 ? ? ? ?
	// todo test setworld: E8 ? ? ? ? 4C 89 65 AF 48 8D 57 28

	if (Engine_Version == 420)
		ServerReplicateActorsOffset = 0x53;
	else if (Engine_Version == 421)
		ServerReplicateActorsOffset = Fortnite_Season == 5 ? 0x54 : 0x56;
	else if (Engine_Version >= 422 && Engine_Version <= 424)
		ServerReplicateActorsOffset = Fortnite_Version >= 7.40 && Fortnite_Version < 8.40 ? 0x57 :
		Engine_Version == 424 ? (Fortnite_Version >= 11.00 && Fortnite_Version <= 11.01 ? 0x57 : 
			(Fortnite_Version == 11.30 || Fortnite_Version == 11.31 ? 0x59 : 0x5A)) : 0x56;

	// ^ I know this makes no sense, 7.40-8.40 is 0x57, other 7-10 is 0x56, 11.00-11.01 = 0x57, 11.31 = 0x59, other S11 is 0x5A

	else if (Fortnite_Season == 12 || Fortnite_Season == 13)
		ServerReplicateActorsOffset = 0x5D;
	else if (Fortnite_Season == 14 || Fortnite_Version <= 15.2) // never tested 15.2
		ServerReplicateActorsOffset = 0x5E;
	else if (Fortnite_Version >= 15.3 && Engine_Version < 500) // 15.3-18 = 0x5F
		ServerReplicateActorsOffset = 0x5F;
	else if (Fortnite_Season >= 19 && Fortnite_Season <= 20)
		ServerReplicateActorsOffset = 0x66;
	else if (Fortnite_Season >= 21)
		ServerReplicateActorsOffset = 0x67; // checked onb 22.30

	Offset_InternalOffset = Engine_Version >= 425 && Fortnite_Season < 20 ? 0x4C : 0x44;
	SuperStructOffset = Engine_Version >= 422 ? 0x40 : 0x30;
	ChildPropertiesOffset = Engine_Version >= 425 ? 0x50 : SuperStructOffset + 8;
	PropertiesSizeOffset = ChildPropertiesOffset + 8;

	std::cout << "Fortnite_Version: " << Fortnite_Version << '\n';
	std::cout << std::format("Offset_InternalOffset: 0x{:x}\n", Offset_InternalOffset);
	std::cout << std::format("SuperStructOffset: 0x{:x}\n", SuperStructOffset);
	std::cout << std::format("ChildPropertiesOffset: 0x{:x}\n", ChildPropertiesOffset);
	std::cout << std::format("PropertiesSizeOffset: 0x{:x}\n", PropertiesSizeOffset);
	std::cout << std::format("ServerReplicateActorsOffset: 0x{:x}\n", ServerReplicateActorsOffset);

	InitHostAddress = Memory::FindPattern(InitHostPattern);
	StaticFindObjectAddress = Memory::FindPattern(StaticFindObjectPattern);
	StaticLoadObjectAddress = Memory::FindPattern(StaticLoadObjectPattern);
	ProcessEventAddress = Memory::FindPattern(ProcessEventPattern);
	SetWorldAddress = Memory::FindPattern(SetWorldPattern);
	PauseBeaconRequestsAddress = Memory::FindPattern(PauseBeaconRequestsPattern);
	ObjectsAddress = Memory::FindPattern(ObjectsPattern, false, 7, true);
	InitListenAddress = Memory::FindPattern(InitListenPattern);
	TickFlushAddress = bIsTickFlushRelative ? Memory::FindPattern(TickFlushPattern, true, 1) : Memory::FindPattern(TickFlushPattern);
	KickPlayerAddress = Memory::FindPattern(KickPlayerPattern);
	ValidationFailureAddress = Memory::FindPattern(ValidationFailurePattern);
	ReallocAddress = Memory::FindPattern(ReallocPattern);
	NoReserveAddress = Memory::FindPattern(NoReservePattern);
	InternalTryActivateAbilityAddress = Memory::FindPattern(InternalTryActivateAbilityPattern);
	GiveAbilityAddress = Memory::FindPattern(GiveAbilityPattern);
	CantBuildAddress = Memory::FindPattern(CantBuildPattern);
	ReplaceBuildingActorAddress = Memory::FindPattern(ReplaceBuildingActorPattern);
	WorldGetNetModeAddress = Memory::FindPattern(WorldGetNetModePattern);
	NoMCPAddress = Engine_Version < 426 ? Memory::FindPattern(NoMCPPattern, true, 1) : Memory::FindPattern(NoMCPPattern);
	FreeAddress = Memory::FindPattern(FreePattern);
	HandleReloadCostAddress = Memory::FindPattern(HandleReloadCostPattern);
	CanActivateAbilityAddress = Memory::FindPattern(CanActivateAbilityPattern);
	ActorGetNetModeAddress = Memory::FindPattern(ActorGetNetModePattern);

	auto Base = (uintptr_t)GetModuleHandleW(0);

	std::cout << "MCP REL: " << Memory::FindPattern(NoMCPPattern, true, 1) << '\n';
	std::cout << "MCP NOTREL: " << Memory::FindPattern(NoMCPPattern) << '\n';

	if (!TickFlushAddress)
	{
		std::cout << "No tickflush! Testing pattern!\n";
		TickFlushAddress = Memory::FindPattern("E8 ? ? ? ? 83 BE ? ? ? ? ? 0F 8E ? ? ? ? 48 8B 86 ? ? ? ?", true, 1);
	}

	std::cout << std::format("SpawnActorAddress: 0x{:x}\n", (uintptr_t)SpawnActorAddr - Base);
	std::cout << std::format("InitHostAddress: 0x{:x}\n", (uintptr_t)InitHostAddress - Base);
	std::cout << std::format("StaticFindObjectAddress: 0x{:x}\n", (uintptr_t)StaticFindObjectAddress - Base);
	std::cout << std::format("StaticLoadObjectAddress: 0x{:x}\n", (uintptr_t)StaticLoadObjectAddress - Base);
	std::cout << std::format("ProcessEventAddress: 0x{:x}\n", (uintptr_t)ProcessEventAddress - Base);
	std::cout << std::format("SetWorldAddress: 0x{:x}\n", (uintptr_t)SetWorldAddress - Base);
	std::cout << std::format("PauseBeaconRequestsAddress: 0x{:x}\n", (uintptr_t)PauseBeaconRequestsAddress - Base);
	std::cout << std::format("ObjectsAddress: 0x{:x}\n", (uintptr_t)ObjectsAddress - Base);
	std::cout << std::format("InitListenAddress: 0x{:x}\n", (uintptr_t)InitListenAddress - Base);
	std::cout << std::format("TickFlushAddress: 0x{:x}\n", (uintptr_t)TickFlushAddress - Base);
	std::cout << std::format("KickPlayerAddress: 0x{:x}\n", (uintptr_t)KickPlayerAddress - Base);
	std::cout << std::format("ValidationFailureAddress: 0x{:x}\n", (uintptr_t)ValidationFailureAddress - Base);
	std::cout << std::format("ReallocAddress: 0x{:x}\n", (uintptr_t)ReallocAddress - Base);
	std::cout << std::format("NoReserveAddress: 0x{:x}\n", (uintptr_t)NoReserveAddress - Base);
	std::cout << std::format("InternalTryActivateAbilityAddress: 0x{:x}\n", (uintptr_t)InternalTryActivateAbilityAddress - Base);
	std::cout << std::format("GiveAbilityAddress: 0x{:x}\n", (uintptr_t)GiveAbilityAddress - Base);
	std::cout << std::format("CantBuildAddress: 0x{:x}\n", (uintptr_t)CantBuildAddress - Base);
	std::cout << std::format("ReplaceBuildingActorAddress: 0x{:x}\n", (uintptr_t)ReplaceBuildingActorAddress - Base);
	std::cout << std::format("WorldGetNetModeAddress: 0x{:x}\n", (uintptr_t)WorldGetNetModeAddress - Base);
	std::cout << std::format("NoMCPAddress: 0x{:x}\n", (uintptr_t)NoMCPAddress - Base);
	std::cout << std::format("FreeAddress: 0x{:x}\n", (uintptr_t)FreeAddress - Base);
	std::cout << std::format("HandleReloadCostAddress: 0x{:x}\n", (uintptr_t)HandleReloadCostAddress - Base);
	std::cout << std::format("CanActivateAbilityAddress: 0x{:x}\n", (uintptr_t)CanActivateAbilityAddress - Base);
	std::cout << std::format("ActorGetNetModeAddress: 0x{:x}\n", (uintptr_t)ActorGetNetModeAddress - Base);

	if (!InitHostAddress || !ProcessEventAddress || !ObjectsAddress)
		return false;

	Defines::InitHost = decltype(Defines::InitHost)(InitHostAddress);
	StaticFindObjectO = decltype(StaticFindObjectO)(StaticFindObjectAddress);
	StaticLoadObjectO = decltype(StaticLoadObjectO)(StaticLoadObjectAddress);
	ProcessEventO = decltype(ProcessEventO)(ProcessEventAddress);
	Defines::SetWorld = decltype(Defines::SetWorld)(SetWorldAddress);
	Defines::PauseBeaconRequests = decltype(Defines::PauseBeaconRequests)(PauseBeaconRequestsAddress);
	Defines::InitListen = decltype(Defines::InitListen)(InitListenAddress);
	Defines::TickFlush = decltype(Defines::TickFlush)(TickFlushAddress);
	Defines::KickPlayer = decltype(Defines::KickPlayer)(KickPlayerAddress);
	Defines::ValidationFailure = decltype(Defines::ValidationFailure)(ValidationFailureAddress);
	FMemory::Realloc = decltype(FMemory::Realloc)(ReallocAddress);
	Defines::NoReservation = decltype(Defines::NoReservation)(NoReserveAddress);

	if (Engine_Version == 426 && Fortnite_Season < 17)
		Defines::InternalTryActivateAbilityFTS = decltype(Defines::InternalTryActivateAbilityFTS)(InternalTryActivateAbilityAddress);
	else
		Defines::InternalTryActivateAbility = decltype(Defines::InternalTryActivateAbility)(InternalTryActivateAbilityAddress);

	if (Fortnite_Season == 13)
		Defines::GiveAbilityS13 = decltype(Defines::GiveAbilityS13)(GiveAbilityAddress);
	else if (Fortnite_Season >= 14 && Fortnite_Season < 17)
		Defines::GiveAbilityS14ABOVE = decltype(Defines::GiveAbilityS14ABOVE)(GiveAbilityAddress);
	else if (Fortnite_Season >= 17)
		Defines::GiveAbilityS17ABOVE = decltype(Defines::GiveAbilityS17ABOVE)(GiveAbilityAddress);
	else
		Defines::GiveAbility = decltype(Defines::GiveAbility)(GiveAbilityAddress);

	Defines::CantBuild = decltype(Defines::CantBuild)(CantBuildAddress);
	Defines::ReplaceBuildingActor = decltype(Defines::ReplaceBuildingActor)(ReplaceBuildingActorAddress);
	FMemory::Free = decltype(FMemory::Free)(FreeAddress);

	if (Engine_Version >= 421)
		NewObjects = decltype(NewObjects)(ObjectsAddress);
	else
		OldObjects = decltype(OldObjects)(ObjectsAddress);

	// toFree.Free();

	return true;
}
