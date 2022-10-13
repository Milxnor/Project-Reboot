#include <Windows.h>
#include <iostream>

#include "patterns.h"
#include "server.h"
#include "helper.h"

DWORD WINAPI Input(LPVOID)
{
    while (1)
    {
        if (GetAsyncKeyState(VK_F1) & 1)
        {
            Server::Listen();
        }

        Sleep(1000 / 30);
    }
}

DWORD WINAPI Initialize(LPVOID)
{
    AllocConsole();

    FILE* fptr;
    freopen_s(&fptr, "CONOUT$", "w+", stdout);

    SetConsoleTitleA("Project Reboot V2");

    InitializePatterns();

    std::cout << "Initialized!\n";
    std::cout << "Base Address: " << (uintptr_t)GetModuleHandleW(0) << '\n';

    auto PC = Helper::GetLocalPlayerController();

    static auto SwitchLevel = FindObject<UFunction>("Function /Script/Engine.PlayerController.SwitchLevel");

    FString Level = L"Athena_Terrain";

    PC->ProcessEvent(SwitchLevel, &Level);

    CreateThread(0, 0, Input, 0, 0, 0);

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
        break;
    }
    return TRUE;
}