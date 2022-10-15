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

static void InitializePatterns()
{
	auto SpawnActorAddr = Memory::FindPattern("40 53 56 57 48 83 EC 70 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 0F 28 1D ? ? ? ? 0F 57 D2 48 8B B4 24 ? ? ? ? 0F 28 CB");

	if (!SpawnActorAddr)
		SpawnActorAddr = Memory::FindPattern("40 53 48 83 EC 70 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 0F 28 1D ? ? ? ? 0F 57 D2 48 8B 9C 24 ? ? ? ? 0F 28 CB 0F 54 1D ? ? ? ? 0F 57");

	if (!SpawnActorAddr)
		SpawnActorAddr = Memory::FindPattern("48 89 5C 24 ? 55 56 57 48 8B EC 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 F0 0F 28 05 ? ? ? ? 48 8B FA 0F 28 0D ? ? ? ? 48 8B D9 48 8B 75 40 0F 29 45 C0 0F 28 05 ? ? ? ? 0F 29 45 E0 0F 29 4D D0 4D 85 C0 74 12 F3 41 0F 10 50 ? F2 41 0F 10 18");

	SpawnActorO = decltype(SpawnActorO)(SpawnActorAddr);

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

		if (Fortnite_Version >= 16.00 && Fortnite_Version < 18.40)
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

	// offsets

	if (Engine_Version == 421)
	{
		Offset_InternalOffset = 0x44;
		SuperStructOffset = sizeof(UObject) + 8;
		ChildPropertiesOffset = SuperStructOffset + 8;
		PropertiesSizeOffset = ChildPropertiesOffset + 8;
		Defines::ServerReplicateActorsOffset = Fortnite_Season == 5 ? 0x54 : 0x56;
	}

	// pattners

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
		InitHostPattern = "";
		StaticFindObjectPattern = "";
		StaticLoadObjectPattern = "";
		ProcessEventPattern = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 48 63 41 0C 45 33 F6";
		SetWorldPattern = "";
		PauseBeaconRequestsPattern = "";
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
		NoReservePattern = "48 89 5C 24 ?? 48 89 6C 24 ?? 56 41 56 41 57 48 81 EC";
		InternalTryActivateAbilityPattern = "4C 89 4C 24 20 4C 89 44 24 18 89 54 24 10 55 53 56";
		GiveAbilityPattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9";
		CantBuildPattern = "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC ? 49 8B E9 4D 8B F0";
	}

	if (Engine_Version == 422)
	{
		InitHostPattern = "";
		StaticFindObjectPattern = "";
		StaticLoadObjectPattern = "";
		ProcessEventPattern = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? ? ? ? 45 33 F6";
		SetWorldPattern = "";
		PauseBeaconRequestsPattern = "";
	}

	if (Engine_Version == 423)
	{
		InitHostPattern = "";
		StaticFindObjectPattern = "";
		StaticLoadObjectPattern = "";
		ProcessEventPattern = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? ? ? ? 45 33 F6";
		SetWorldPattern = "";
		PauseBeaconRequestsPattern = "";
	}

	if (Engine_Version == 424)
	{
		InitHostPattern = "";
		StaticFindObjectPattern = "";
		StaticLoadObjectPattern = "";
		ProcessEventPattern = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? ? ? ? 45 33 F6";
		SetWorldPattern = "";
		PauseBeaconRequestsPattern = "";
	}

	if (Engine_Version == 425)
	{
		InitHostPattern = "";
		StaticFindObjectPattern = "";
		StaticLoadObjectPattern = "";
		ProcessEventPattern = "";
		SetWorldPattern = "";
		PauseBeaconRequestsPattern = "";
	}

	if (Engine_Version == 426)
	{
		InitHostPattern = "";
		StaticFindObjectPattern = "";
		StaticLoadObjectPattern = "";
		ProcessEventPattern = "";
		SetWorldPattern = "";
		PauseBeaconRequestsPattern = "";
	}

	if (Engine_Version == 427) // 4.26.1
	{
		InitHostPattern = "";
		StaticFindObjectPattern = "";
		StaticLoadObjectPattern = "";
		ProcessEventPattern = "";
		SetWorldPattern = "";
		PauseBeaconRequestsPattern = "";
	}

	if (Engine_Version == 500)
	{
		InitHostPattern = "";
		StaticFindObjectPattern = "";
		StaticLoadObjectPattern = "";
		ProcessEventPattern = "";
		SetWorldPattern = "";
		PauseBeaconRequestsPattern = "";
	}

	InitHostAddress = Memory::FindPattern(InitHostPattern);
	StaticFindObjectAddress = Memory::FindPattern(StaticFindObjectPattern);
	StaticLoadObjectAddress = Memory::FindPattern(StaticLoadObjectPattern);
	ProcessEventAddress = Memory::FindPattern(ProcessEventPattern);
	SetWorldAddress = Memory::FindPattern(SetWorldPattern);
	PauseBeaconRequestsAddress = Memory::FindPattern(PauseBeaconRequestsPattern);
	ObjectsAddress = Memory::FindPattern(ObjectsPattern, false, 7, true);
	InitListenAddress = Memory::FindPattern(InitListenPattern);
	TickFlushAddress = Memory::FindPattern(TickFlushPattern);
	KickPlayerAddress = Memory::FindPattern(KickPlayerPattern);
	ValidationFailureAddress = Memory::FindPattern(ValidationFailurePattern);
	ReallocAddress = Memory::FindPattern(ReallocPattern);
	NoReserveAddress = Memory::FindPattern(NoReservePattern);
	InternalTryActivateAbilityAddress = Memory::FindPattern(InternalTryActivateAbilityPattern);
	GiveAbilityAddress = Memory::FindPattern(GiveAbilityPattern);
	CantBuildAddress = Memory::FindPattern(CantBuildPattern);

	std::cout << "InitHostAddress: " << InitHostAddress << '\n';
	std::cout << "StaticFindObjectAddress: " << StaticFindObjectAddress << '\n';
	std::cout << "StaticLoadObjectAddress: " << StaticLoadObjectAddress << '\n';
	std::cout << "ProcessEventAddress: " << ProcessEventAddress << '\n';
	std::cout << "SetWorldAddress: " << SetWorldAddress << '\n';
	std::cout << "PauseBeaconRequestsAddress: " << PauseBeaconRequestsAddress << '\n';
	std::cout << "ObjectsAddress: " << ObjectsAddress << '\n';
	std::cout << "InitListenAddress: " << InitListenAddress << '\n';
	std::cout << "TickFlushAddress: " << TickFlushAddress << '\n';
	std::cout << "KickPlayerAddress: " << KickPlayerAddress << '\n';
	std::cout << "ValidationFailureAddress: " << ValidationFailureAddress << '\n';
	std::cout << "ReallocAddress: " << ReallocAddress << '\n';
	std::cout << "NoReserveAddress: " << NoReserveAddress << '\n';
	std::cout << "InternalTryActivateAbilityAddress: " << InternalTryActivateAbilityAddress << '\n';
	std::cout << "GiveAbilityAddress: " << GiveAbilityAddress << '\n';
	std::cout << "CantBuildAddress: " << CantBuildAddress << '\n';

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
	Defines::InternalTryActivateAbility = decltype(Defines::InternalTryActivateAbility)(InternalTryActivateAbilityAddress);
	Defines::GiveAbility = decltype(Defines::GiveAbility)(GiveAbilityAddress);
	Defines::CantBuild = decltype(Defines::CantBuild)(CantBuildAddress);

	if (Engine_Version >= 421)
		NewObjects = decltype(NewObjects)(ObjectsAddress);
	else
		OldObjects = decltype(OldObjects)(ObjectsAddress);

	std::cout << NewObjects->Num() << '\n';

	// toFree.Free();
}