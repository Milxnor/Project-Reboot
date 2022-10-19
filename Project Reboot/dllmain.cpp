#include <MinHook.h>
#include <Windows.h>
#include <iostream>

#include "patterns.h"
#include "server.h"
#include "helper.h"
#include "processevent.h"
#include "inventory.h"
#include "abilities.h"
#include "build.h"
#include "edit.h"
#include "loot.h"

DWORD WINAPI Input(LPVOID)
{
    while (1)
    {
        if (GetAsyncKeyState(VK_F1) & 1)
        {
            Server::Listen();
            Server::Hooks::Initialize();

            std::cout << "nice!\n";

            MH_CreateHook((PVOID)ProcessEventAddress, ProcessEventDetour, (PVOID*)&ProcessEventO);
            MH_EnableHook((PVOID)ProcessEventAddress);

            Defines::bLogProcessEvent = true;
        }

        else if (GetAsyncKeyState(VK_F2) & 1)
        {
            Defines::bLogProcessEvent = !Defines::bLogProcessEvent;
            std::cout << "Set Defines::bLogProcessEvent to " << Defines::bLogProcessEvent << '\n';
        }

        else if (GetAsyncKeyState(VK_F3) & 1)
        {
            static auto BuildingItemCollectorClass = FindObject("Class /Script/FortniteGame.BuildingItemCollectorActor");

            std::cout << "BuildingItemCollectorClass: " << BuildingItemCollectorClass << '\n';

            auto BuildingItemCollectorActorActors = Helper::GetAllActorsOfClass(BuildingItemCollectorClass);

            std::cout << "Skid: " << BuildingItemCollectorActorActors.size() << '\n';

            // for (auto BuildingItemCollectorActor : BuildingItemCollectorActorActors)
            for (int i = 0; i < BuildingItemCollectorActorActors.size(); i++)
            {
                auto BuildingItemCollectorActor = BuildingItemCollectorActorActors.At(i);

                std::cout << "A!\n";

                static auto CollectorUnitInfoClassName = FindObject("ScriptStruct /Script/FortniteGame.CollectorUnitInfo") ? "ScriptStruct /Script/FortniteGame.CollectorUnitInfo" :
                    "ScriptStruct /Script/FortniteGame.ColletorUnitInfo";

                static auto CollectorUnitInfoClass = FindObject(CollectorUnitInfoClassName);

                static auto CollectorUnitInfoClassSize = Helper::GetSizeOfClass(CollectorUnitInfoClass);

                static auto ItemCollectionsOffset = BuildingItemCollectorActor->GetOffset("ItemCollections");

                TArray<__int64>* ItemCollections = Get<TArray<__int64>>(BuildingItemCollectorActor, ItemCollectionsOffset); // CollectorUnitInfo

                static auto OutputItemOffset = FindOffsetStruct2(CollectorUnitInfoClassName, "OutputItem");

                static auto Def = FindObject("FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03");

                *Get<UObject*>(ItemCollections->AtPtr(0, CollectorUnitInfoClassSize), OutputItemOffset) = Def;
                *Get<UObject*>(ItemCollections->AtPtr(1, CollectorUnitInfoClassSize), OutputItemOffset) = Def;
                *Get<UObject*>(ItemCollections->AtPtr(2, CollectorUnitInfoClassSize), OutputItemOffset) = Def;
            }

            BuildingItemCollectorActorActors.Free();
        }

        /* else if (GetAsyncKeyState(VK_F4) & 1)
        {
            auto LootDrops = Looting::PickLootDrops("Loot_AthenaFloorLoot");

            std::cout << "LootDrops: " << LootDrops.size() << '\n';

            for (auto& LootDrop : LootDrops)
            {
                auto LootDropDef = LootDrop.first;
            }
        } */

        else if (GetAsyncKeyState(VK_F5) & 1)
        {
            std::cout << MH_StatusToString(MH_CreateHook((PVOID)KickPlayerAddress, Server::Hooks::KickPlayer, (PVOID*)&Defines::KickPlayer)) << '\n';
            std::cout << MH_StatusToString(MH_EnableHook((PVOID)KickPlayerAddress)) << '\n';

            std::cout << MH_StatusToString(MH_CreateHook((PVOID)ValidationFailureAddress, Server::Hooks::ValidationFailure, (PVOID*)&Defines::ValidationFailure)) << '\n';
            std::cout << MH_StatusToString(MH_EnableHook((PVOID)ValidationFailureAddress)) << '\n';

            std::cout << MH_StatusToString(MH_CreateHook((PVOID)NoReserveAddress, Server::Hooks::NoReservation, (PVOID*)&Defines::NoReservation)) << '\n';
            std::cout << MH_StatusToString(MH_EnableHook((PVOID)NoReserveAddress)) << '\n';
        }

        else if (GetAsyncKeyState(VK_F6) & 1)
        {
            auto before = Defines::bShouldSpawnFloorLoot;
            Defines::bShouldSpawnFloorLoot = !Defines::bShouldSpawnFloorLoot;
            std::cout << "Set Defines::bShouldSpawnFloorLoot to " << !before << '\n';
        }

        else if (GetAsyncKeyState(VK_F7) & 1)
        {
            MH_CreateHook((PVOID)ProcessEventAddress, ProcessEventDetour, (PVOID*)&ProcessEventO);
            MH_EnableHook((PVOID)ProcessEventAddress);
        }

        else if (GetAsyncKeyState(VK_F8) & 1)
        {
            auto GameState = Helper::GetGameState();
            static auto GamePhaseOffset = GameState->GetOffset("GamePhase");
            auto OldPhase = *Get<EAthenaGamePhase>(GameState, GamePhaseOffset);
            *Get<EAthenaGamePhase>(GameState, GamePhaseOffset) = EAthenaGamePhase::Warmup;

            auto OnRepGamePhase = FindObject<UFunction>("Function /Script/FortniteGame.FortGameStateAthena.OnRep_GamePhase");

            GameState->ProcessEvent(OnRepGamePhase, &OldPhase);

           /* FString command = L"startaircraft";

            struct {
                UObject* WorldContextObject;                                       // (Parm, ZeroConstructor, IsPlainOldData)
                FString                                     Command;                                                  // (Parm, ZeroConstructor)
                UObject* SpecificPlayer;                                           // (Parm, ZeroConstructor, IsPlainOldData)
            } params{ Helper::GetWorld(), command, nullptr };

            static auto KSLClass = FindObject(("KismetSystemLibrary /Script/Engine.Default__KismetSystemLibrary"));

            if (KSLClass)
            {
                // static auto ExecuteConsoleCommandFn = KSLClass->Function(("ExecuteConsoleCommand"));
                static auto ExecuteConsoleCommandFn = FindObject<UFunction>(("Function /Script/Engine.KismetSystemLibrary.ExecuteConsoleCommand"));

                if (ExecuteConsoleCommandFn)
                    KSLClass->ProcessEvent(ExecuteConsoleCommandFn, &params);
                else
                    std::cout << ("No ExecuteConsoleCommand!\n");
            }
            else
                std::cout << ("No KismetSyustemLibrary!\n"); */
        }

        else if (GetAsyncKeyState(VK_F9) & 1)
        {
            auto riftobject = StaticLoadObjectO(Helper::GetBGAClass(), nullptr, L"/Game/Athena/Items/ForagedItems/Rift/BGA_RiftPortal_Athena.BGA_RiftPortal_Athena_C", nullptr, 0, nullptr, false, nullptr);
            // auto riftobject = StaticLoadObject(Helper::GetBGAClass(), nullptr, "/Game/Athena/Items/ForagedItems/Rift/BGA_RiftPortal_Athena.BGA_RiftPortal_Athena_C");
            std::cout << "riftobject: " << riftobject << '\n';
        }

        else if (GetAsyncKeyState(VK_F10) & 1)
        {
            auto GameState = Helper::GetGameState();
            static auto GamePhaseOffset = GameState->GetOffset("GamePhase");
            auto OldPhase = *Get<EAthenaGamePhase>(GameState, GamePhaseOffset);
            *Get<EAthenaGamePhase>(GameState, GamePhaseOffset) = EAthenaGamePhase::None;

            auto OnRepGamePhase = FindObject<UFunction>("Function /Script/FortniteGame.FortGameStateAthena.OnRep_GamePhase");

            GameState->ProcessEvent(OnRepGamePhase, &OldPhase);
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
        return 1;
    }

    if (!InitializePatterns())
    {
        MessageBoxA(0, "Failed to setup patterns", "Project Reboot V2", MB_ICONERROR);
        return 1;
    }
    
    std::cout << "Initialized\n";
    std::cout << "Fortnite_Season: " << Fortnite_Season << '\n';
    std::cout << "GiveAbilityS14ANDS15: " << Defines::GiveAbilityS14ANDS15 << '\n';
    std::cout << "GiveAbilityAddress: " << GiveAbilityAddress << '\n';
    std::cout << std::format("Base Address 0x{:x}\n", (uintptr_t)GetModuleHandleW(0));

    FFortItemEntry::ItemEntryStruct = FindObjectSlow("ScriptStruct /Script/FortniteGame.FortItemEntry", false);
    FastTArray::FastArraySerializerStruct = FindObjectSlow("ScriptStruct /Script/Engine.FastArraySerializer", false);
    Abilities::GameplayAbilitySpecClass = FindObjectSlow("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", false);
    Editing::EditToolDefinition = FindObject("FortEditToolItemDefinition /Game/Items/Weapons/BuildingTools/EditTool.EditTool");

    auto PC = Helper::GetLocalPlayerController();

    std::cout << "PC: " << PC << '\n';

    static auto SwitchLevel = FindObject<UFunction>("Function /Script/Engine.PlayerController.SwitchLevel");

    FString Level = Engine_Version < 424 ? L"Athena_Terrain" : (Engine_Version < 500 ? L"Apollo_Terrain" : L"Artemis_Terrain");

    PC->ProcessEvent(SwitchLevel, &Level);
    
    CreateThread(0, 0, Input, 0, 0, 0);

    AddHook("Function /Script/Engine.GameModeBase.HandleStartingNewPlayer", HandleStartingNewPlayer);
    AddHook("Function /Script/Engine.GameMode.ReadyToStartMatch", ReadyToStartMatch);
    AddHook("Function /Script/Engine.PlayerController.ServerAcknowledgePossession", ServerAcknowledgePossession);

    AddHook("Function /Script/FortniteGame.FortPlayerController.ServerExecuteInventoryItem", Inventory::ServerExecuteInventoryItem);
    AddHook("Function /Script/FortniteGame.FortPlayerPawn.ServerHandlePickup", Inventory::ServerHandlePickup);

    AddHook("Function /Script/FortniteGame.FortPlayerController.ServerCreateBuildingActor", Build::ServerCreateBuildingActor);

    if (false)
    {
        AddHook("Function /Script/FortniteGame.FortPlayerController.ServerBeginEditingBuildingActor", Editing::ServerBeginEditingBuildingActorHook);
        AddHook("Function /Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", Editing::ServerEditBuildingActorHook);
        AddHook("Function /Script/FortniteGame.FortPlayerController.ServerEndEditingBuildingActor", Editing::ServerEndEditingBuildingActorHook);
    }
    
    AddHook("Function /Script/FortniteGame.FortPlayerControllerZone.ClientOnPawnDied", ClientOnPawnDied);

    if (InternalTryActivateAbilityAddress)
    {
        AddHook("Function /Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbility", Abilities::ServerTryActivateAbility);
        AddHook("Function /Script/GameplayAbilities.AbilitySystemComponent.ServerAbilityRPCBatch", Abilities::ServerAbilityRPCBatch);
        // AddHook("Function /Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbilityWithEventData", Abilities::ServerTryActivateAbilityWithEventData);
    }

    if (Fortnite_Version < 9) // idk if right
        AddHook(("Function /Script/FortniteGame.FortPlayerControllerAthena.ServerAttemptAircraftJump"), ServerAttemptAircraftJump);
    else if (Engine_Version < 424)
        AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerAttemptAircraftJump"), ServerAttemptAircraftJump);
    else
        AddHook(("Function /Script/FortniteGame.FortControllerComponent_Aircraft.ServerAttemptAircraftJump"), ServerAttemptAircraftJump);

    Level.Free();

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