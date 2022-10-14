#include <MinHook.h>
#include <Windows.h>
#include <iostream>

#include "patterns.h"
#include "server.h"
#include "helper.h"
#include "processevent.h"
#include "inventory.h"
#include "abilities.h"

DWORD WINAPI Input(LPVOID)
{
    while (1)
    {
        if (GetAsyncKeyState(VK_F1) & 1)
        {
            Server::Listen();
            Server::Hooks::Initialize();

            std::cout << "Wtf!\n";
            std::cout << "ProcessEventAddress: " << ProcessEventAddress << '\n';
            std::cout << "ProcessEventO: " << ProcessEventO << '\n';

            MH_CreateHook((PVOID)ProcessEventAddress, ProcessEventDetour, (PVOID*)&ProcessEventO);
            MH_EnableHook((PVOID)ProcessEventAddress);
        }

        Sleep(1000 / 30);
    }
}

DWORD WINAPI Initialize(LPVOID)
{
    // if (FALSE && !TRUE)
    {
        AllocConsole();

        FILE* stream;
        freopen_s(&stream, "CONOUT$", "w+", stdout);

        SetConsoleTitleA("Project Reboot V2");
    }

    if (MH_Initialize() != MH_OK)
    {
        MessageBoxA(0, "MinHook failed to initialize", "Project Reboot V2", MB_ICONERROR);
        return 0;
    }

    InitializePatterns();
    
    std::cout << "Initialized\n";
    std::cout << std::format("Base Address 0x{:x}\n", (uintptr_t)GetModuleHandleW(0));

    FFortItemEntry::ItemEntryStruct = FindObjectSlow("ScriptStruct /Script/FortniteGame.FortItemEntry", false);
    FastTArray::FastArraySerializerStruct = FindObjectSlow("ScriptStruct /Script/Engine.FastArraySerializer", false);
    Abilities::GameplayAbilitySpecClass = FindObjectSlow("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", false);

    auto PC = Helper::GetLocalPlayerController();

    static auto SwitchLevel = FindObject<UFunction>("Function /Script/Engine.PlayerController.SwitchLevel");

    FString Level = L"Athena_Terrain"; // to free

    PC->ProcessEvent(SwitchLevel, &Level);

    CreateThread(0, 0, Input, 0, 0, 0);

    AddHook("Function /Script/Engine.GameModeBase.HandleStartingNewPlayer", HandleStartingNewPlayer);
    AddHook(("Function /Script/Engine.GameMode.ReadyToStartMatch"), ReadyToStartMatch);
    AddHook(("Function /Script/Engine.PlayerController.ServerAcknowledgePossession"), ServerAcknowledgePossession);
    AddHook("Function /Script/FortniteGame.FortPlayerController.ServerExecuteInventoryItem", Inventory::ServerExecuteInventoryItem);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Initialize, 0, 0, 0);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        MH_DisableHook(MH_ALL_HOOKS);
        // Log::Shutdown();
        break;
    }
    return TRUE;
}