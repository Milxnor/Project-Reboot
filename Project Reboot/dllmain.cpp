#include <MinHook.h>
#include <Windows.h>
#include <iostream>
#include <fstream>

#include "patterns.h"
#include "server.h"
#include "helper.h"
#include "processevent.h"
#include "inventory.h"
#include "abilities.h"
#include "build.h"
#include "gui.h"
#include "edit.h"
#include "loot.h"
#include "interaction.h"
#include <intrin.h>

// DEFINE_LOG_CATEGORY_STATIC(LogInit, ELogLevel::All)

__int64 rettrue() { return 1; }
__int64 retfalse() { return 0; }

DWORD WINAPI Initialize(LPVOID)
{
    // if (FALSE && !TRUE)
    {
        AllocConsole();

        FILE* stream;
        freopen_s(&stream, "CONOUT$", "w+", stdout);

        SetConsoleTitleA("Project Reboot V2");
    }

    // Log::Init();

    if (MH_Initialize() != MH_OK)
    {
        MessageBoxA(0, "MinHook failed to initialize", "Project Reboot V2", MB_ICONERROR);
        return 1;
    }

    std::cout << std::format("Base Address 0x{:x}\n", (uintptr_t)GetModuleHandleW(0));

    if (!InitializePatterns())
    {
        MessageBoxA(0, "Failed to setup patterns", "Project Reboot V2", MB_ICONERROR);
        return 1;
    }

    constexpr bool bHookPreLogin = false;

    if constexpr (bHookPreLogin)
    {
        uintptr_t PreLoginAddr = 0;

        if (Engine_Version == 423)
            PreLoginAddr = Memory::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 81 EC ? ? ? ? 48 8B 9C 24 ? ? ? ? 33 FF");

        std::cout << "PreLoginAddr: " << PreLoginAddr << '\n';

        MH_CreateHook((PVOID)PreLoginAddr, rettrue, nullptr);
        MH_EnableHook((PVOID)PreLoginAddr);
    }

    if (Engine_Version <= 422)
    {
        // aStatNetdebugwi
        uintptr_t NetDebugAddr = Memory::FindPattern("40 55 56 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 01"); // 5.41 // tested on 7.3

        std::cout << "NetDebugAddr: " << NetDebugAddr << '\n';

        MH_CreateHook((PVOID)NetDebugAddr, rettrue, nullptr);
        MH_EnableHook((PVOID)NetDebugAddr);
    }

    if (Fortnite_Version == 7.30)
    {
        static auto hookthisaddr = Memory::FindPattern("48 89 5C 24 ? 57 48 83 EC 30 48 8B 01 48 8B F9 FF 90 ? ? ? ? 48 8B D8 48 85 C0 0F 84 ? ? ? ? 48 83 78 ? ?");

        MH_CreateHook((PVOID)hookthisaddr, retfalse, nullptr);
        MH_EnableHook((PVOID)hookthisaddr);
    }

    if (Fortnite_Season >= 17)
    {
        auto sigcrash = Memory::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 48 81 EC ? ? ? ? 65 48 8B 04 25 ? ? ? ? 4C 8B F9");

        std::cout << "sigcrash: " << sigcrash << '\n';

        MH_CreateHook((PVOID)sigcrash, retfalse, nullptr);
        MH_EnableHook((PVOID)sigcrash);
    }

    if (Fortnite_Season >= 22)
    {
        auto afq = Memory::FindPattern("40 55 53 56 57 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B F1 E8 ? ? ? ? 48 8B CE E8 ? ? ? ? 45 33 FF 48 85 C0 74 5A 48");

        std::cout << "afq: " << afq << '\n';

        MH_CreateHook((PVOID)afq, retfalse, nullptr);
        MH_EnableHook((PVOID)afq);
    }

    // if (false)
    {
        bool bIsSettingGIsClient = true;
        bool bSetGIsClientSuccessful = false;

        if (bIsSettingGIsClient)
        {
            uintptr_t GIsClientAddr = 0; // Memory::FindPattern("8A 05 ? ? ? ? F6 D8 1B C0 F7 D8 FF C0 EB 05 B8", true, 2); // 18.40
            uintptr_t GIsServerAddr = 0;

            if (Engine_Version == 421 || Engine_Version == 422) // got on 5.41
            {
                GIsClientAddr = Memory::FindPattern("80 3D ? ? ? ? ? 74 17 48 8D 15 ? ? ? ? 48", true, 2);
                GIsServerAddr = Memory::FindPattern("80 3D ? ? ? ? ? 75 40 80 3D ? ? ? ? ? 48 8D 05", true, 2);
            }

            else if (Engine_Version == 423)
            {
                GIsClientAddr = Memory::FindPattern("C6 05 ? ? ? ? ? 44 88 64 24 ? C6 05", true, 2);
                GIsServerAddr = Memory::FindPattern("80 3D ? ? ? ? ? 75 0D F6 83 ? ? ? ? ? 0F", true, 2); // 8.51
            }

            else if (Engine_Version == 425)
            {
                GIsClientAddr = Memory::FindPattern("80 3D ? ? ? ? ? 41 0F B6 E9 41 0F B6 F8", true, 2);
                GIsServerAddr = Memory::FindPattern("80 3D ? ? ? ? ? 75 0D F6 83 ? ? ? ? ? 0F", true, 2); // 8.51
            }

            else if (Engine_Version == 500)
            {
                GIsClientAddr = Memory::FindPattern("80 3D ? ? ? ? ? 48 8B DA 0F 84 ? ? ? ? 48", true, 2);
                GIsServerAddr = Memory::FindPattern("80 3D ? ? ? ? ? 0F 85 ? ? ? ? F6 83", true, 2);
            }

            std::cout << "GIsClientSig: " << GIsClientAddr << '\n';
            std::cout << "GIsServerAddr: " << GIsServerAddr << '\n';
            // std::cout << "aFTER: " << GIsClientAddr << '\n';

            if (GIsClientAddr)
            {
                std::cout << "BefroreClient: " << *(bool*)(GIsClientAddr) << '\n';
                *(bool*)(GIsClientAddr) = false;
            }

            if (GIsServerAddr)
            {
                std::cout << "BefroreServer: " << *(bool*)(GIsServerAddr) << '\n';
                *(bool*)(GIsServerAddr) = true;
            }

            if (Fortnite_Version == 20.40)
            {
                auto IsGameServerForEvent = __int64(GetModuleHandleW(0)) + 0x5CD2B88;

                std::cout << "IsGameServerForEvent: " << IsGameServerForEvent << '\n';

                MH_CreateHook((PVOID)IsGameServerForEvent, rettrue, nullptr);
                MH_EnableHook((PVOID)IsGameServerForEvent);
            }

            bSetGIsClientSuccessful = GIsClientAddr;
        }
    }

    CreateThread(0, 0, GuiThread, 0, 0, 0);

    MH_CreateHook((PVOID)CanActivateAbilityAddress, rettrue, nullptr); // TODO: Find a better fix
    MH_EnableHook((PVOID)CanActivateAbilityAddress);

    MH_CreateHook((PVOID)HandleReloadCostAddress, Inventory::HandleReloadCost, (PVOID*)&Defines::HandleReloadCost);
    MH_EnableHook((PVOID)HandleReloadCostAddress);

    MH_CreateHook((PVOID)ProcessEventAddress, ProcessEventDetour, (PVOID*)&ProcessEventO);
    MH_EnableHook((PVOID)ProcessEventAddress);

    if (WorldGetNetModeAddress && NoMCPAddress)
    {
        MH_CreateHook((PVOID)WorldGetNetModeAddress, rettrue, nullptr);
        MH_EnableHook((PVOID)WorldGetNetModeAddress);

        MH_CreateHook((PVOID)NoMCPAddress, rettrue, nullptr);
        MH_EnableHook((PVOID)NoMCPAddress);
    }
    else
    {
        std::cout << "Unable to enable GetNetMode fix!\n";
        std::cout << "NoMCPAddress: " << NoMCPAddress << '\n';
        std::cout << "WorldGetNetModeAddress: " << WorldGetNetModeAddress << '\n';
    }

    std::cout << "Initialized\n";
    // LOG(LogInit, All, L"Initialized");
    std::cout << "Fortnite_Season: " << Fortnite_Season << '\n';
    std::cout << "GiveAbilityS14ABOVE: " << Defines::GiveAbilityS14ABOVE << '\n';
    std::cout << "GiveAbilityAddress: " << GiveAbilityAddress << '\n';

    FFortItemEntry::ItemEntryStruct = FindObjectSlow("ScriptStruct /Script/FortniteGame.FortItemEntry", false);
    FastTArray::FastArraySerializerStruct = FindObjectSlow("ScriptStruct /Script/Engine.FastArraySerializer", false);
    Abilities::GameplayAbilitySpecClass = FindObjectSlow("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", false);
    Editing::EditToolDefinition = FindObject("/Game/Items/Weapons/BuildingTools/EditTool.EditTool");

    static auto SizeOfGameplayAbilitySpec = Helper::GetSizeOfClass(Abilities::GameplayAbilitySpecClass);

    std::cout << "SizeOfGameplayAbilitySpec: " << SizeOfGameplayAbilitySpec << '\n';
    std::cout << "SizeOfPredictionKey: " << Helper::GetSizeOfClass(FindObjectSlow("ScriptStruct /Script/GameplayAbilities.PredictionKey", false)) << '\n';
    std::cout << "SizeOfItemEntryStruct: " << Helper::GetSizeOfClass(FFortItemEntry::ItemEntryStruct) << '\n';
    
    // Level.Free();

    std::cout << "Full Version: " << Helper::GetEngineVersion().ToString() << '\n';
    std::cout << std::format("Engine Version {} Version {} CL {}\n", Helper::GetEngineVer(), Helper::GetFortniteVersion(), Helper::GetNetCL());

    auto matchmaking = Memory::FindPattern("83 BD ? ? ? ? 01 7F 18 49 8D 4D D8 48 8B D6 E8 ? ? ? ? 48");

    matchmaking = matchmaking ? matchmaking : Memory::FindPattern("83 7D 88 01 7F 0D 48 8B CE E8");

    Defines::bMatchmakingSupported = matchmaking && Engine_Version >= 420;
    int idx = 0;

    if (Defines::bMatchmakingSupported) // now check if it leads to the right place and where the jg is at
    {
        for (int i = 0; i < 9; i++)
        {
            auto byte = (uint8_t*)(matchmaking + i);

            if (IsBadReadPtr(byte))
                continue;

            // std::cout << std::format("[{}] 0x{:x}\n", i, (int)*byte);

            if (*byte == 0x7F)
            {
                Defines::bMatchmakingSupported = true;
                idx = i;
                break;
            }

            Defines::bMatchmakingSupported = false;
        }
    }

    std::cout << "Matchmaking will " << (Defines::bMatchmakingSupported ? "be supported\n" : "not be supported\n");

    if (Defines::bMatchmakingSupported)
    {
        std::cout << "idx: " << idx << '\n';

        auto before = (uint8_t*)(matchmaking + idx);

        std::cout << "before byte: " << (int)*before << '\n';

        *before = 0x74;
    }

    if (Defines::bIsGoingToPlayMainEvent)
    {
        if (Fortnite_Season == 16)
        {
            Defines::MapName = "Apollo_Terrain_Yogurt";
        }
    }
    else
    {
        if (Defines::bIsCreative)
        {
            if (Fortnite_Season >= 7 && Engine_Version < 424)
            {
                static auto CreativePlaylist = FindObject("/Game/Athena/Playlists/Creative/Playlist_PlaygroundV2.Playlist_PlaygroundV2");

                std::cout << "CreativePlaylist: " << CreativePlaylist << '\n';

                if (CreativePlaylist)
                {
                    static auto AdditionalLevelsOffset = CreativePlaylist->GetOffset("AdditionalLevels");

                    auto AdditionalLevels = Get<TArray<TSoftObjectPtr>>(CreativePlaylist, AdditionalLevelsOffset); // TArray<TSoftObjectPtr<class UWorld>>

                    std::cout << "AdditionalLevels: " << AdditionalLevels->Num() << '\n';

                    for (int i = 0; i < AdditionalLevels->Num(); i++)
                    {
                        auto AdditionalLevel = AdditionalLevels->At(i);

                        auto CurrentLevelName = AdditionalLevel.ObjectID.AssetPathName.ToString();
                        std::cout << std::format("[{}] {}\n", i, CurrentLevelName);
                    }

                    auto LevelToOpen = AdditionalLevels->At(AdditionalLevels->Num() - 1);

                    auto LevelName = LevelToOpen.ObjectID.AssetPathName.ToString();

                    LevelName = LevelName.substr(0, LevelName.find_last_of("."));

                    std::cout << "LevelName: " << LevelName << '\n';

                    // FString levelName = std::wstring(LevelName.begin(), LevelName.end()).c_str();
                    Defines::MapName = LevelName;
                }
            }
            else
            {
                std::cout << "You are on a version that either doesn't have creative or we don't support creative for it!\n";
                Defines::bIsCreative = false;
            }
        }
        else
        {
            Defines::MapName = Engine_Version < 424 ? "Athena_Terrain" : (Engine_Version < 500 ? "Apollo_Terrain" : "Artemis_Terrain");
        }
    }

    std::cout << "skidda!\n";

    while (Defines::SecondsUntilTravel > 0)
    {
        Defines::SecondsUntilTravel -= 1;

        Sleep(1000);
    }

    // Sleep(Defines::SecondsUntilTravel * 1000);

    auto PC = Helper::GetLocalPlayerController();

    std::cout << "PC: " << PC << '\n';

    static auto SwitchLevel = FindObject<UFunction>("/Script/Engine.PlayerController.SwitchLevel");

    std::wstring LevelWStr = std::wstring(Defines::MapName.begin(), Defines::MapName.end());
    const wchar_t* LevelWCSTR = LevelWStr.c_str();
    FString Level = LevelWCSTR;

    PC->ProcessEvent(SwitchLevel, &Level);

    Defines::bTraveled = true;

    // if (Fortnite_Version <= 11.30) // todo test this
        // AddHook("/Script/FortniteGame.FortPlayerControllerAthena.ServerClientIsReadyToRespawn", ServerClientIsReadyToRespawn);

    AddHook("/Script/Engine.GameModeBase.HandleStartingNewPlayer", HandleStartingNewPlayer);
    AddHook("/Script/Engine.GameMode.ReadyToStartMatch", ReadyToStartMatch);
    AddHook("/Script/Engine.PlayerController.ServerAcknowledgePossession", ServerAcknowledgePossession);
    AddHook("/Script/FortniteGame.FortPlayerController.ServerReadyToStartMatch", ServerReadyToStartMatch);

    AddHook("/Script/FortniteGame.FortPlayerControllerAthena.ServerGiveCreativeItem", ServerGiveCreativeItem);

    AddHook("/Script/FortniteGame.FortPlayerController.ServerExecuteInventoryItem", Inventory::ServerExecuteInventoryItem);
    AddHook(Engine_Version >= 420 ? "/Script/FortniteGame.FortPlayerController.ServerAttemptInventoryDrop"
       : "/Script/FortniteGame.FortPlayerController.ServerSpawnInventoryDrop", Inventory::ServerAttemptInventoryDrop);
    AddHook(Fortnite_Season < 13 ? "/Script/FortniteGame.FortPlayerPawn.ServerHandlePickup"
        : "/Script/FortniteGame.FortPlayerPawn.ServerHandlePickupInfo", Inventory::ServerHandlePickup);

    AddHook("/Script/FortniteGame.FortPlayerController.ServerExecuteInventoryWeapon", Inventory::ServerExecuteInventoryWeapon);

    AddHook("/Script/FortniteGame.FortPlayerController.ServerCreateBuildingActor", Build::ServerCreateBuildingActor);
    AddHook("/Script/FortniteGame.FortDecoTool.ServerSpawnDeco", Build::ServerSpawnDeco);

    AddHook(Engine_Version < 423 ? "/Script/FortniteGame.FortPlayerController.ServerAttemptInteract" :
        "/Script/FortniteGame.FortControllerComponent_Interaction.ServerAttemptInteract", Interaction::ServerAttemptInteract);

    AddHook("/Script/FortniteGame.FortPlayerController.ServerLoadingScreenDropped", ServerLoadingScreenDropped);

    if (Fortnite_Season < 20)
    {
        AddHook("/Script/FortniteGame.FortPlayerController.ServerBeginEditingBuildingActor", Editing::ServerBeginEditingBuildingActorHook);
        AddHook("/Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", Editing::ServerEditBuildingActorHook);
        AddHook("/Script/FortniteGame.FortPlayerController.ServerEndEditingBuildingActor", Editing::ServerEndEditingBuildingActorHook);
    }

    AddHook("/Script/FortniteGame.FortPlayerControllerZone.ClientOnPawnDied", ClientOnPawnDied);

    if (InternalTryActivateAbilityAddress && GiveAbilityAddress)
    {
        AddHook("/Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbility", Abilities::ServerTryActivateAbility);
        AddHook("/Script/GameplayAbilities.AbilitySystemComponent.ServerAbilityRPCBatch", Abilities::ServerAbilityRPCBatch);
        AddHook("/Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbilityWithEventData", Abilities::ServerTryActivateAbilityWithEventData);
    }

    AddHook(Fortnite_Version < 9 ? "/Script/FortniteGame.FortPlayerControllerAthena.ServerAttemptAircraftJump"
        : (Engine_Version < 424 ? "/Script/FortniteGame.FortPlayerController.ServerAttemptAircraftJump"
            : "/Script/FortniteGame.FortControllerComponent_Aircraft.ServerAttemptAircraftJump"), ServerAttemptAircraftJump);

    // AddHook("/Script/FortniteGame.BuildingActor.OnDeathServer", OnDeathServer);

    // AddHook("/Script/FortniteGame.FortPlayerController.ServerPlayEmoteItem", ServerPlayEmoteItem);

    // AddHook("/Script/FortniteGame.FortHeldObjectComponent.HandleOwnerAsBuildingActorDestroyed", HandleOwnerAsBuildingActorDestroyed);

    preoffsets::DeathCause = FindOffsetStruct("ScriptStruct /Script/FortniteGame.DeathInfo", "DeathCause");
    preoffsets::FinisherOrDowner = FindOffsetStruct("ScriptStruct /Script/FortniteGame.DeathInfo", "FinisherOrDowner");
    preoffsets::bDBNO = FindOffsetStruct("ScriptStruct /Script/FortniteGame.DeathInfo", "bDBNO");
    preoffsets::Distance = FindOffsetStruct("ScriptStruct /Script/FortniteGame.DeathInfo", "Distance");
    preoffsets::KillScore = FindOffsetStruct("Class /Script/FortniteGame.FortPlayerStateAthena", "KillScore");
    preoffsets::bMarkedAlive = FindOffsetStruct("Class /Script/FortniteGame.FortPlayerControllerAthena", "bMarkedAlive");
    preoffsets::TeamScorePlacement = FindOffsetStruct("Class /Script/FortniteGame.FortPlayerStateAthena", "TeamScorePlacement");
    preoffsets::TeamScore = FindOffsetStruct("Class /Script/FortniteGame.FortPlayerStateAthena", "TeamScore");
    preoffsets::Place = FindOffsetStruct("Class /Script/FortniteGame.FortPlayerStateAthena", "Place");
    preoffsets::AlivePlayers = FindOffsetStruct("Class /Script/FortniteGame.FortGameModeAthena", "AlivePlayers");
    preoffsets::GamePhase = FindOffsetStruct("Class /Script/FortniteGame.FortGameStateAthena", "GamePhase");
    preoffsets::DeathInfo = FindOffsetStruct("Class /Script/FortniteGame.FortPlayerStateAthena", "DeathInfo");
    preoffsets::LastFallDistance = FindOffsetStruct("Class /Script/FortniteGame.FortPlayerPawnAthena", "LastFallDistance");
    preoffsets::Tags = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport", "Tags");
    preoffsets::KillerPawn = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport", "KillerPawn");
    preoffsets::KillerPlayerState = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport", "KillerPlayerState");

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Initialize, 0, 0, 0);
        break;
    case DLL_PROCESS_DETACH:
        MH_DisableHook(MH_ALL_HOOKS);
        // Log::Shutdown();
        break;
    }
    return TRUE;
}