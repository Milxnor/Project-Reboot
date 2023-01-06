#pragma once

// TODO: Update ImGUI

#pragma comment(lib, "d3d9.lib")

#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <d3d9.h>

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx9.h>

#include <string>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_stdlib.h>
#include "fontawesome.h"
#include <vector>
#include <format>
#include <imgui/imgui_internal.h>
#include <set>
#include "definitions.h"
#include "loot.h"
#include "events.h"
#include "helper.h"
#include "calendar.h"

#include "moderation.h"

#define GAME_TAB 1
#define PLAYERS_TAB 2
#define GAMEMODE_TAB 3
#define THANOS_TAB 4
#define EVENT_TAB 5
#define LATEGAME_TAB 6
#define DUMP_TAB 7
#define UNBAN_TAB 8
#define SETTINGS_TAB 9
#define CREDITS_TAB 10

#define MAIN_PLAYERTAB 1
#define INVENTORY_PLAYERTAB 2
#define LOADOUT_PLAYERTAB 4
#define FUN_PLAYERTAB 5

// THE BASE CODE IS FROM IMGUI GITHUB

static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::string wstring_to_utf8(const std::wstring& str)
{
	if (str.empty()) return {};
	const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
	std::string str_to(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &str_to[0], size_needed, nullptr, nullptr);
	return str_to;
}

void InitStyle()
{
	ImGui::GetIO().Fonts->AddFontFromFileTTF("Reboot Resources/fonts/ruda-bold.ttf", 17);
	ImGui::GetStyle().FrameRounding = 4.0f;
	ImGui::GetStyle().GrabRounding = 4.0f;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

class Playera
{
public:
	std::string Name;
	int Kills = 0;

	Playera(const std::string& _Name, int _Kills) : Name(_Name), Kills(_Kills) {}

	Playera() {}
};

void TextCentered(std::string text, bool bNewLine = true) {
	if (bNewLine)
		ImGui::NewLine();

	float win_width = ImGui::GetWindowSize().x;
	float text_width = ImGui::CalcTextSize(text.c_str()).x;

	// calculate the indentation that centers the text on one line, relative
	// to window left, regardless of the `ImGuiStyleVar_WindowPadding` value
	float text_indentation = (win_width - text_width) * 0.5f;

	// if text is too long to be drawn on one line, `text_indentation` can
	// become too small or even negative, so we check a minimum indentation
	float min_indentation = 20.0f;
	if (text_indentation <= min_indentation) {
		text_indentation = min_indentation;
	}

	ImGui::SameLine(text_indentation);
	ImGui::PushTextWrapPos(win_width - text_indentation);
	ImGui::TextWrapped(text.c_str());
	ImGui::PopTextWrapPos();
}

bool ButtonCentered(std::string text, bool bNewLine = true) {
	if (bNewLine)
		ImGui::NewLine();

	float win_width = ImGui::GetWindowSize().x;
	float text_width = ImGui::CalcTextSize(text.c_str()).x;

	// calculate the indentation that centers the text on one line, relative
	// to window left, regardless of the `ImGuiStyleVar_WindowPadding` value
	float text_indentation = (win_width - text_width) * 0.5f;

	// if text is too long to be drawn on one line, `text_indentation` can
	// become too small or even negative, so we check a minimum indentation
	float min_indentation = 20.0f;
	if (text_indentation <= min_indentation) {
		text_indentation = min_indentation;
	}

	ImGui::SameLine(text_indentation);
	ImGui::PushTextWrapPos(win_width - text_indentation);
	auto res = ImGui::Button(text.c_str());
	ImGui::PopTextWrapPos();
	return res;
}

void InputVector(std::string baseText, BothVector* vec)
{
	if (Fortnite_Season < 20)
	{
		ImGui::InputFloat((baseText + " X").c_str(), &vec->fV.X);
		ImGui::InputFloat((baseText + " Y").c_str(), &vec->fV.Y);
		ImGui::InputFloat((baseText + " Z").c_str(), &vec->fV.Z);
	}
	else
	{
		ImGui::InputDouble((baseText + " X").c_str(), &vec->dV.X);
		ImGui::InputDouble((baseText + " Y").c_str(), &vec->dV.Y);
		ImGui::InputDouble((baseText + " Z").c_str(), &vec->dV.Z);
	}
}

static int Width = 640;
static int Height = 480;

static int Tab = 1;
static int PlayerTab = -1;
static bool bIsEditingInventory = false;
static bool bInformationTab = false;
static int playerTabTab = MAIN_PLAYERTAB;

void MainTabs()
{
	// std::ofstream bannedStream(Moderation::Banning::GetFilePath());

	if (ImGui::BeginTabBar(""))
	{
		if (ImGui::BeginTabItem(ICON_FA_GAMEPAD " Game"))
		{
			Tab = GAME_TAB;
			PlayerTab = -1;
			bInformationTab = false;
			ImGui::EndTabItem();
		}

		// if (serverStatus == EServerStatus::Up)
		{
			if (ImGui::BeginTabItem(ICON_FA_PEOPLE_CARRY " Players"))
			{
				Tab = PLAYERS_TAB;
				ImGui::EndTabItem();
			}
		}

		if (false && ImGui::BeginTabItem(("Gamemode")))
		{
			Tab = GAMEMODE_TAB;
			PlayerTab = -1;
			bInformationTab = false;
			ImGui::EndTabItem();
		}

		if (false && (std::floor(Fortnite_Version) == 8 || Engine_Version >= 424 || Fortnite_Version == 4.1))
		{
			if (ImGui::BeginTabItem(("Thanos")))
			{
				Tab = THANOS_TAB;
				PlayerTab = -1;
				bInformationTab = false;
				ImGui::EndTabItem();
			}
		}

		// if (Events::HasEvent())
		if (Defines::bIsGoingToPlayMainEvent)
		{
			if (ImGui::BeginTabItem(("Event")))
			{
				Tab = EVENT_TAB;
				PlayerTab = -1;
				bInformationTab = false;
				ImGui::EndTabItem();
			}
		}

		if (false && Defines::bIsLateGame && ImGui::BeginTabItem(("Lategame")))
		{
			Tab = LATEGAME_TAB;
			PlayerTab = -1;
			bInformationTab = false;
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Dump"))
		{
			Tab = DUMP_TAB;
			PlayerTab = -1;
			bInformationTab = false;
			ImGui::EndTabItem();
		}

#if 0
		if (bannedStream.is_open() && ImGui::BeginTabItem("Unban")) // skunked
		{
			Tab = UNBAN_TAB;
			PlayerTab = -1;
			bInformationTab = false;
			ImGui::EndTabItem();
		}
#endif

		if (ImGui::BeginTabItem(("Settings")))
		{
			Tab = SETTINGS_TAB;
			PlayerTab = -1;
			bInformationTab = false;
			ImGui::EndTabItem();
		}

		// maybe a Replication Stats for >3.3?

		if (false && ImGui::BeginTabItem(("Credits")))
		{
			Tab = CREDITS_TAB;
			PlayerTab = -1;
			bInformationTab = false;
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void PlayerTabs()
{
	if (ImGui::BeginTabBar(""))
	{
		if (ImGui::BeginTabItem("Main"))
		{
			playerTabTab = MAIN_PLAYERTAB;
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(("Inventory")))
		{
			playerTabTab = INVENTORY_PLAYERTAB;
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(("Cosmetics")))
		{
			playerTabTab = LOADOUT_PLAYERTAB;
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(("Fun")))
		{
			playerTabTab = FUN_PLAYERTAB;
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void MainUI()
{
	auto bLoaded = Server::BeaconHost && !Defines::bIsRestarting; // Looting::bInitialized;

	std::vector<std::pair<UObject*, UObject*>> AllControllers;

	float* WarmupCountdownEndTime = nullptr;
	UObject* GameState = nullptr;
	float TimeSeconds = 0;

	if (bLoaded)
	{
		GameState = Helper::GetGameState();
		static auto WarmupCountdownEndTimeOffset = GameState->GetOffset("WarmupCountdownEndTime");
		WarmupCountdownEndTime = Get<float>(GameState, WarmupCountdownEndTimeOffset);

		TimeSeconds = Helper::GetTimeSeconds();

		if (*WarmupCountdownEndTime - 10 >= TimeSeconds && *WarmupCountdownEndTime != -1) // IDK
			*WarmupCountdownEndTime = TimeSeconds + 40;	
	}

	if (PlayerTab == -1)
	{
		MainTabs();

		if (Tab == GAME_TAB)
		{
			if (bLoaded)
			{
				ImGui::Checkbox("Log ProcessEvent", &Defines::bLogProcessEvent);
				ImGui::Checkbox("Log RPCS", &Defines::bLogRPCs);
				ImGui::Checkbox("Infinite Mats", &Defines::bInfiniteMats);
				ImGui::Checkbox("Infinite Ammo", &Defines::bInfiniteAmmo);
				// ImGui::Checkbox("Respawning", &Defines::bRespawning);
				// ImGui::Checkbox("Test 2", &Defines::test2);
				// ImGui::SliderFloat("test1", &Defines::test1, 0.f, 1.f);

				/* if (ImGui::Button("Bot Fool"))
				{
					std::cout << "eeer!\n";
					auto GameMode = Helper::GetGameMode();
					static auto AISettingsOffset = GameMode->GetOffset("AISettings");
					auto AISettings = *Get<UObject*>(GameMode, AISettingsOffset);
					static auto AIServicesOffset = AISettings->GetOffset("AIServices");

					static auto funna = FindObject("/Script/FortniteGame.AthenaAIServicePlayerBots");
					auto newplayerbots = Helper::Easy::SpawnObject(funna, AISettings);

					static auto funna1 = FindObject("/Script/FortniteGame.AthenaAIServiceLoot");
					auto newloot = Helper::Easy::SpawnObject(funna1, AISettings);

					static auto funna2 = FindObject("/Script/FFortniteGame.AthenaAIServiceVehicle");
					auto newvehicle = Helper::Easy::SpawnObject(funna2, AISettings);

					Get<TArray<UObject*>>(AISettings, AIServicesOffset)->Add(newplayerbots);
					Get<TArray<UObject*>>(AISettings, AIServicesOffset)->Add(newloot);
					Get<TArray<UObject*>>(AISettings, AIServicesOffset)->Add(newvehicle);

					static auto AIDirectorOffset = GameMode->GetOffset("AIDirector");
					(*Get<UObject*>(GameMode, AIDirectorOffset))->ProcessEvent(FindObject<UFunction>("/Script/FortniteGame.FortAIDirector.Activate"));
				} */

				static std::string ConsoleCommand;

				ImGui::InputText("Console command", &ConsoleCommand);

				if (ImGui::Button("Execute console command"))
				{
					auto wstr = std::wstring(ConsoleCommand.begin(), ConsoleCommand.end());

					auto aa = wstr.c_str();
					FString cmd = aa;

					std::cout << "boi: " << cmd.ToString() << '\n';

					Helper::ExecuteConsoleCommand(cmd);
				}

				if (Defines::bIsCreative)
					ImGui::InputText("URL", &Defines::urlForPortal);

				if (Defines::bIsLateGame)
				{
					if (ImGui::Button("Start safezone"))
					{
						FString StartSafeZone = L"startsafezone";
						Helper::ExecuteConsoleCommand(StartSafeZone);

						static auto SafeZonesStartTimeOffset = GameState->GetOffset("SafeZonesStartTime");
						*Get<float>(GameState, SafeZonesStartTimeOffset) = 0.f;
					}
				}

				if ((Fortnite_Version >= 12.41 && Fortnite_Season < 20) && ImGui::Button("Summon Vehicles"))
				{
					Defines::bShouldSpawnVehicles = true;
				}

				/* if (ImGui::Button("Restart"))
				{
					Defines::bShouldRestart = true;
					Server::Restart();
				} */

				/* if (ImGui::Button("toggle FloorLoot"))
				{
					Defines::bShouldSpawnFloorLoot = !Defines::bShouldSpawnFloorLoot;
				} */

				if (Fortnite_Season == 19)
				{
					static int SnowIndex = 0;
					ImGui::SliderInt("SnowIndex", &SnowIndex, 0, 6);

					if (ImGui::Button("Set Snow Phase"))
					{
						Helper::SetSnowIndex(SnowIndex);
					}
				}

				else if (Fortnite_Season == 13)
				{
					static UObject* WL = FindObject("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.Apollo_WaterSetup_2"); // Is this 13.40 specific?
					static auto last = Defines::AmountOfRestarts;

					if (!WL || Defines::AmountOfRestarts != last)
					{
						last = Defines::AmountOfRestarts;
						WL = FindObject("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.Apollo_WaterSetup_2");
					}

					if (WL)
					{

						static auto MaxWaterLevelOffset = WL->GetOffset("MaxWaterLevel");

						static int MaxWaterLevel = *Get<int>(WL, MaxWaterLevelOffset);
						static int WaterLevel = 0;

						ImGui::SliderInt("WaterLevel", &WaterLevel, 0, MaxWaterLevel);

						if (ImGui::Button("Set Water Level"))
						{
							Calendar::SetWaterLevel(WaterLevel);
							// ApolloSetup->UpdateMinimapData(ApolloSetup->MinimapTextures.At(WaterLevel), ApolloSetup->MinimapDiscoveryMasks.At(WaterLevel));
						}
					}			
				}

				if (ImGui::Button("Fill all vending machines"))
				{
					static auto BuildingItemCollectorClass = FindObject("/Game/Athena/Items/Gameplay/VendingMachine/B_Athena_VendingMachine.B_Athena_VendingMachine_C"); // FindObject("/Script/FortniteGame.BuildingItemCollectorActor");

					std::cout << "BuildingItemCollectorClass: " << BuildingItemCollectorClass << '\n';

					auto BuildingItemCollectorActorActors = Helper::GetAllActorsOfClass(BuildingItemCollectorClass);

					std::cout << "Skid: " << BuildingItemCollectorActorActors.size() << '\n';

					auto GameData = Helper::GetGameData();

					if (GameData)
					{
						struct { UObject* GameState; UObject* Playlist; FGameplayTagContainer PlaylistContextTags; } bbparms{ Helper::GetGameState(), *Helper::GetPlaylist(),
								FGameplayTagContainer() };	

						// for (auto BuildingItemCollectorActor : BuildingItemCollectorActorActors)
						for (int i = 0; i < BuildingItemCollectorActorActors.size(); i++)
						{
							auto BuildingItemCollectorActor = BuildingItemCollectorActorActors.At(i);

							std::cout << "A!\n";

							static auto CollectorUnitInfoClassName = FindObject("/Script/FortniteGame.CollectorUnitInfo") ? "/Script/FortniteGame.CollectorUnitInfo" :
								"/Script/FortniteGame.ColletorUnitInfo";

							static auto CollectorUnitInfoClass = FindObject(CollectorUnitInfoClassName);

							static auto CollectorUnitInfoClassSize = Helper::GetSizeOfClass(CollectorUnitInfoClass);

							static auto ItemCollectionsOffset = BuildingItemCollectorActor->GetOffset("ItemCollections");

							TArray<__int64>* ItemCollections = Get<TArray<__int64>>(BuildingItemCollectorActor, ItemCollectionsOffset); // CollectorUnitInfo

							static auto OutputItemOffset = FindOffsetStruct2(CollectorUnitInfoClassName, "OutputItem", false, true);

							std::cout << "OutputItemOffset: " << OutputItemOffset << '\n';

							int rand = GetRandomInt(3, 5);

							ERarity Rarity = (ERarity)rand;

							std::cout << "Rarity: " << (int)Rarity << '\n';

							auto Def0 = Looting::GetRandomItem(ItemType::Weapon, Looting::LootItems, Rarity).Definition;
							auto Def1 = Looting::GetRandomItem(ItemType::Weapon, Looting::LootItems, Rarity).Definition;
							auto Def2 = Looting::GetRandomItem(ItemType::Weapon, Looting::LootItems, Rarity).Definition;

							std::cout << "Def0: " << Def0 << '\n';
							std::cout << "Def1: " << Def1 << '\n';
							std::cout << "Def2: " << Def2 << '\n';

							*Get<UObject*>(ItemCollections->AtPtr(0, CollectorUnitInfoClassSize), OutputItemOffset) = Def0;
							*Get<UObject*>(ItemCollections->AtPtr(1, CollectorUnitInfoClassSize), OutputItemOffset) = Def1;
							*Get<UObject*>(ItemCollections->AtPtr(2, CollectorUnitInfoClassSize), OutputItemOffset) = Def2;

							// static auto bb = FindObject<UFunction>("/Game/Athena/Items/Gameplay/VendingMachine/B_Athena_VendingMachine.B_Athena_VendingMachine_C.OnReady_21959F7346995C2CA1D67DB9455B627D");
							// BuildingItemCollectorActor->ProcessEvent(bb, &bbparms);

							// continue;

							static auto SetRarityColors = FindObject<UFunction>("/Game/Athena/Items/Gameplay/VendingMachine/B_Athena_VendingMachine.B_Athena_VendingMachine_C.SetRarityColors");

							struct FFortRarityItemData
							{
								char                                      Name[0x0018];                                                     // 0x0000(0x0018) (Edit, BlueprintVisible, BlueprintReadOnly)
								FLinearColor                                Color1;                                                   // 0x0018(0x0010) (Edit, BlueprintVisible, BlueprintReadOnly, IsPlainOldData)
								FLinearColor                                Color2;                                                   // 0x0028(0x0010) (Edit, BlueprintVisible, BlueprintReadOnly, IsPlainOldData)
								FLinearColor                                Color3;                                                   // 0x0038(0x0010) (Edit, BlueprintVisible, BlueprintReadOnly, IsPlainOldData)
								FLinearColor                                Color4;                                                   // 0x0048(0x0010) (Edit, BlueprintVisible, BlueprintReadOnly, IsPlainOldData)
								FLinearColor                                Color5;                                                   // 0x0058(0x0010) (Edit, BlueprintVisible, BlueprintReadOnly, IsPlainOldData)
								float                                              Radius;                                                   // 0x0068(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
								float                                              Falloff;                                                  // 0x006C(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
								float                                              Brightness;                                               // 0x0070(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
								float                                              Roughness;                                                // 0x0074(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
								float                                              Glow;                                                     // 0x0078(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
								unsigned char                                      UnknownData00[0x4];                                       // 0x007C(0x0004) MISSED OFFSET
							};

							FLinearColor Color{};

							constexpr bool bGetColorFromRarityData = false;

							if (bGetColorFromRarityData)
							{
								bool bIsSoftObjectPtr = Engine_Version > 420; // Idk when

								auto RarityDataOffset = GameData->GetOffset("RarityData");
								auto RarityData = bIsSoftObjectPtr ? Get<TSoftObjectPtr>(GameData, RarityDataOffset)->Get(nullptr, true) : *Get<UObject*>(GameData, RarityDataOffset);

								// std::cout << "RarityData: " << RarityData << '\n';

								static auto RarityCollectionOffset = RarityData->GetOffset("RarityCollection");
								// FFortRarityItemData* RarityCollections[0x8] = Get<FFortRarityItemData[0x8]>(RarityData, RarityCollectionOffset); // 0x8-0xA

								FFortRarityItemData* aa = *Get<FFortRarityItemData[10]>(RarityData, RarityCollectionOffset); // [(int)Rarity - 1] ;// RarityCollections[(int)Rarity - 1];

								auto& RarityCollection = aa[rand];

								// FLinearColor Color = Rarity == ERarity::Rare ? RarityCollection->Color3 : 
									// Rarity == ERarity::Epic ? RarityCollection->Color4 : RarityCollection->Color5;

								Color = RarityCollection.Color1;

								std::cout << "Color Before: " << Color.Describe() << '\n';

								/* static auto GetRarityData = FindObject<UFunction>("/Script/FortniteGame.FortRarityData.BPGetRarityData");
								struct { UObject* ItemDef; FFortRarityItemData RarityItemData; } GetRarityDataparms{Def0};
								RarityData->ProcessEvent(GetRarityData, &GetRarityDataparms);

								Color = GetRarityDataparms.RarityItemData.Color1; */
							}
							else
							{
								// got from 3.5 raritydata

								Color = Rarity == ERarity::Rare ? FLinearColor(0, 255, 245.99493, 255) :
									Rarity == ERarity::Epic ? FLinearColor(213.350085, 5.1, 255, 255) : FLinearColor(245.995185, 138.98724, 31.979295, 255);

								// Color = Rarity == ERarity::Rare ? FLinearColor(0, 1, 0.964686, 1) :
									// Rarity == ERarity::Epic ? FLinearColor(0.836667, 0.02, 1, 1) : FLinearColor(0.964687, 0.545048, 0.125409, 1);
							}

							std::cout << "Color After: " << Color.Describe() << '\n';

							BuildingItemCollectorActor->ProcessEvent(SetRarityColors, &Color);
						}
					}

					BuildingItemCollectorActorActors.Free();
				}

				if (ImGui::Button("Start Bus Countdown"))
				{
					if (Defines::bIsLateGame)
						Defines::bShouldStartBus = true;
					else
						*WarmupCountdownEndTime = TimeSeconds + 9;
				}

				/* if (ImGui::Button("test looting"))
				{
					auto LootDrops = Looting::PickLootDrops("Loot_AthenaFloorLoot_Warmup");
					std::cout << "LootDrops Num: " << LootDrops.size() << '\n';

					for (int i = 0; i < LootDrops.size(); i++)
					{
						auto& LootDrop = LootDrops.at(i);

						std::cout << std::format("[{}] {} {}\n", i, LootDrop.first->GetFullName(), LootDrop.second);
					}
				} */

				if (Defines::bIsCreative && ImGui::Button("Apply"))
				{
					auto aa = std::wstring(Defines::urlForPortal.begin(), Defines::urlForPortal.end());
					const wchar_t* url = aa.c_str();

					static auto ImageURLOffset = Defines::Portal->GetOffset("ImageURL");
					*Get<FString>(Defines::Portal, ImageURLOffset) = url;

					static auto OnThumbnailTextureReady = FindObject<UFunction>("/Script/FortniteGame.FortAthenaCreativePortal.OnThumbnailTextureReady");
					Defines::Portal->ProcessEvent(OnThumbnailTextureReady);

					static auto OnRep_ImageURLChanged = FindObject<UFunction>("/Script/FortniteGame.FortAthenaCreativePortal.OnRep_ImageURLChanged");
					Defines::Portal->ProcessEvent(OnRep_ImageURLChanged);
				}

				if (Fortnite_Version == 14.60 && ImGui::Button("show funny flying aircfaft"))
				{
					auto foundation = FindObject("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.Lobby_Foundation3");
					std::cout << "foundation: " << foundation << '\n';

					ShowFoundation(foundation);
				}

				ImGui::Text(("Game has been going on for " + std::to_string(TimeSeconds)).c_str());
				ImGui::SliderFloat("Warmup end", WarmupCountdownEndTime, TimeSeconds + 9, TimeSeconds + 1000);
			}
		}

		else if (Tab == PLAYERS_TAB)
		{
			if (bLoaded)
			{
				auto World = Helper::GetWorld();

				if (World)
				{
					static auto NetDriverOffset = World->GetOffset("NetDriver");
					auto NetDriver = *(UObject**)(__int64(World) + NetDriverOffset);

					if (NetDriver)
					{
						static auto ClientConnectionsOffset = NetDriver->GetOffset("ClientConnections");
						auto ClientConnections = (TArray<UObject*>*)(__int64(NetDriver) + ClientConnectionsOffset);

						if (ClientConnections)
						{
							for (int i = 0; i < ClientConnections->Num(); i++)
							{
								auto Connection = ClientConnections->At(i);

								if (!Connection)
									continue;

								static auto Connection_PlayerControllerOffset = Connection->GetOffset("PlayerController");
								auto CurrentController = *(UObject**)(__int64(Connection) + Connection_PlayerControllerOffset);

								if (CurrentController)
								{
									AllControllers.push_back({ CurrentController, Connection });
								}
							}

							ImGui::Text(("Players Connected: " + std::to_string(AllControllers.size())).c_str());

							for (int i = 0; i < AllControllers.size(); i++)
							{
								auto& CurrentPair = AllControllers.at(i);
								auto CurrentPlayerState = Helper::GetPlayerStateFromController(CurrentPair.first);

								if (!CurrentPlayerState)
								{
									std::cout << "tf!\n";
									continue;
								}

								FString NameFStr;

								/* static auto GetPlayerName = FindObject<UFunction>("/Script/Engine.PlayerState.GetPlayerName");
								// static auto GetPlayerName = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateZone.GetPlayerNameForStreaming");
								CurrentPlayerState->ProcessEvent(GetPlayerName, &NameFStr);

								const wchar_t* NameWCStr = NameFStr.Data.Data;
								std::wstring NameWStr = std::wstring(NameWCStr);
								std::string Name = NameFStr.ToString(); // std::string(NameWStr.begin(), NameWStr.end());

								auto NameCStr = Name.c_str(); */

								auto Connection = CurrentPair.second;
								auto RequestURL = *GetRequestURL(Connection);

								if (RequestURL.Data.Data && RequestURL.Data.ArrayNum)
								{
									auto RequestURLStr = RequestURL.ToString();

									std::size_t pos = RequestURLStr.find("Name=");

									if (pos != std::string::npos) {
										std::size_t end_pos = RequestURLStr.find('?', pos);

										if (end_pos != std::string::npos)
											RequestURLStr = RequestURLStr.substr(pos + 5, end_pos - pos - 5);
									}

									auto RequestURLCStr = RequestURLStr.c_str();

									if (ImGui::Button(RequestURLCStr))
									{
										std::cout << "RequestURLStr: " << RequestURLStr << '\n';
										std::cout << "wtf! " << i << '\n';
										// std::cout << "Name: " << Name << '\n';
										PlayerTab = i;
									}
								}
							}
						}
					}
				}
			}
		}

		else if (Tab == EVENT_TAB)
		{
			if (ImGui::Button("Load Event (click first, wait a second then Start Event)"))
			{
				Events::LoadEvent();
			}

			if (ImGui::Button("Start Event"))
			{
				Events::StartEvent();
			}

			if ((Fortnite_Version == 7.10 || Fortnite_Version == 11.31 || Fortnite_Version == 15.10) && ImGui::Button("Start New Years Event"))
			{
				Events::StartNewYears();
			}

			if (false && Fortnite_Version == 14.60 && ImGui::Button("Show carrier"))
			{
			}

			if (Fortnite_Version == 8.51)
			{
				std::string unvaultedItemName;

				ImGui::InputText("Unvaulted Item", &unvaultedItemName);

				if (ImGui::Button("Unvault"))
				{
					std::wstring unvaultedItemNameWStr = std::wstring(unvaultedItemName.begin(), unvaultedItemName.end());
					const wchar_t* unvaultedItemNameCWStr = unvaultedItemNameWStr.c_str();
					FString unvaultedItemNameFStr = unvaultedItemNameCWStr;
					FName unvaultedItemNameFName = Helper::Conversion::StringToName(unvaultedItemNameFStr);

					Events::Unvault(unvaultedItemNameFName);
				}
			}
		}

		else if (Tab == DUMP_TAB)
		{
			ImGui::Text("These will all be in your Win64 folder!");

			static std::string ahh = std::format("Fortnite Version {}\n\n", std::to_string(Fortnite_Version));

			if (ImGui::Button("Dump Objects"))
			{
				auto ObjectNum = OldObjects ? OldObjects->Num() : NewObjects->Num();

				std::ofstream obj("ObjectsDump.txt");

				obj << ahh;

				for (int i = 0; i < ObjectNum; i++)
				{
					auto CurrentObject = GetObjectByIndex(i);

					if (!CurrentObject)
						continue;

					obj << CurrentObject->GetFullName() << '\n';
				}
			}

			if (ImGui::Button("Dump Skins (Skins.txt)"))
			{
				std::ofstream SkinsFile("Skins.txt");

				if (SkinsFile.is_open())
				{
					SkinsFile << ahh;

					static auto CIDClass = FindObjectSlow("Class /Script/FortniteGame.AthenaCharacterItemDefinition", false);

					auto AllObjects = Helper::GetAllObjectsOfClass(CIDClass);

					for (int i = 0; i < AllObjects.size(); i++)
					{
						auto CurrentCID = AllObjects.at(i);

						static auto DisplayNameOffset = CurrentCID->GetOffset("DisplayName");
						auto DisplayNameFText = (FText*)(__int64(CurrentCID) + DisplayNameOffset);

						FString DisplayNameFStr = Helper::Conversion::TextToString(*DisplayNameFText);

						if (!DisplayNameFStr.Data.Data)
							continue;

						SkinsFile << std::format("[{}] {}\n", DisplayNameFStr.ToString(), CurrentCID->GetPathName());
					}
				}
			}

			if (ImGui::Button("Dump Playlists (Playlists.txt)"))
			{
				std::ofstream PlaylistsFile("Playlists.txt");

				if (PlaylistsFile.is_open())
				{
					PlaylistsFile << ahh;
					static auto FortPlaylistClass = FindObjectSlow("Class /Script/FortniteGame.FortPlaylist", false);
					// static auto FortPlaylistClass = FindObject("Class /Script/FortniteGame.FortPlaylistAthena");

					auto AllObjects = Helper::GetAllObjectsOfClass(FortPlaylistClass);

					for (int i = 0; i < AllObjects.size(); i++)
					{
						auto Object = AllObjects.at(i);

						// std::string PlaylistName = Object->Member<FName>("PlaylistName")->ToString(); // Short name basically
						static auto UIDisplayNameOffset = Object->GetOffset("UIDisplayName");
						FString PlaylistNameFStr = Helper::Conversion::TextToString(*Get<FText>(Object, UIDisplayNameOffset));

						if (!PlaylistNameFStr.Data.Data)
							continue;

						std::string PlaylistName = PlaylistNameFStr.ToString();

						PlaylistsFile << std::format("[{}] {}\n", PlaylistName, Object->GetPathName());
					}
				}
				else
					std::cout << "Failed to open playlist file!\n";
			}

			if (ImGui::Button("Dump Weapons (Weapons.txt)"))
			{
				std::ofstream WeaponsFile("Weapons.txt");

				if (WeaponsFile.is_open())
				{
					WeaponsFile << ahh;
					static auto FortWeaponItemDefinitionClass = FindObjectSlow("Class /Script/FortniteGame.FortWeaponItemDefinition", false);

					static auto GetWeaponItemDefinition = FindObject<UFunction>("/Script/FortniteGame.FortGadgetItemDefinition.GetWeaponItemDefinition");

					auto auahd = [&WeaponsFile](UObject* Class) {
						auto AllObjects = Helper::GetAllObjectsOfClass(Class);

						for (int i = 0; i < AllObjects.size(); i++)
						{
							auto Object = AllObjects.at(i);

							static auto DisplayNameOffset = Object->GetOffset("DisplayName");
							FString ItemDefinitionFStr = Helper::Conversion::TextToString(*Get<FText>(Object, DisplayNameOffset));

							if (!ItemDefinitionFStr.Data.Data)
								continue;

							std::string ItemDefinitionName = ItemDefinitionFStr.ToString();

							// check if it contains gallery or playset?

							WeaponsFile << std::format("[{}] {}\n", ItemDefinitionName, Object->GetPathName());
						}
					};

					auahd(FortWeaponItemDefinitionClass);

					if (GetWeaponItemDefinition)
					{
						static auto FortGadgetItemDefinitionClass = FindObjectSlow("Class /Script/FortniteGame.FortGadgetItemDefinition", false);
						auahd(FortGadgetItemDefinitionClass);
					}
				}
				else
					std::cout << "Failed to open playlist file!\n";
			}
		}
		else if (Tab == UNBAN_TAB)
		{
			
		}
		else if (Tab == SETTINGS_TAB)
		{
			// ImGui::Checkbox("Use custom lootpool (from Win64/lootpool.txt)", &Defines::bCustomLootpool);
		}
	}

	else if (PlayerTab != 2435892 && bLoaded)
	{
		auto World = Helper::GetWorld();

		{
			static auto NetDriverOffset = World->GetOffset("NetDriver");
			auto NetDriver = *(UObject**)(__int64(World) + NetDriverOffset);

			if (NetDriver)
			{
				static auto ClientConnectionsOffset = NetDriver->GetOffset("ClientConnections");
				auto ClientConnections = (TArray<UObject*>*)(__int64(NetDriver) + ClientConnectionsOffset);

				if (ClientConnections)
				{
					for (int i = 0; i < ClientConnections->Num(); i++)
					{
						auto Connection = ClientConnections->At(i);

						if (!Connection)
							continue;

						static auto Connection_PlayerControllerOffset = Connection->GetOffset("PlayerController");
						auto CurrentController = *(UObject**)(__int64(Connection) + Connection_PlayerControllerOffset);

						if (CurrentController)
						{
							AllControllers.push_back({ CurrentController, Connection });
						}
					}
				}
			}
		}

		if (PlayerTab < AllControllers.size())
		{
			PlayerTabs();

			auto& CurrentPair = AllControllers.at(PlayerTab);
			auto CurrentController = CurrentPair.first;
			auto CurrentPawn = Helper::GetPawnFromController(CurrentController);
			auto CurrentPlayerState = Helper::GetPlayerStateFromController(CurrentController);
	
			if (CurrentPlayerState)
			{
				FString NameFStr;

				auto Connection = CurrentPair.second;
				auto RequestURL = *GetRequestURL(Connection);

				if (RequestURL.Data.Data)
				{
					auto RequestURLStr = RequestURL.ToString();

					std::size_t pos = RequestURLStr.find("Name=");

					if (pos != std::string::npos) {
						std::size_t end_pos = RequestURLStr.find('?', pos);

						if (end_pos != std::string::npos)
							RequestURLStr = RequestURLStr.substr(pos + 5, end_pos - pos - 5);
					}

					auto RequestURLCStr = RequestURLStr.c_str();
					ImGui::Text(("Viewing " + RequestURLStr).c_str());

					if (playerTabTab == MAIN_PLAYERTAB)
					{
						static std::string WID;
						static std::string KickReason = "You have been kicked!";
						static int stud = 0;

						ImGui::InputText("WID To Give", &WID);
						ImGui::InputText("Kick Reason", &KickReason);

						if (CurrentPawn)
						{
							auto CurrentWeapon = Helper::GetCurrentWeapon(CurrentPawn);
							static auto AmmoCountOffset = FindOffsetStruct("Class /Script/FortniteGame.FortWeapon", "AmmoCount");

							auto AmmoCountPtr = (int*)(__int64(CurrentWeapon) + AmmoCountOffset);

							if (ImGui::InputInt("Ammo Count of CurrentWeapon", CurrentWeapon ? AmmoCountPtr : &stud))
							{
								/* if (CurrentWeapon)
								{
									FFortItemEntry::SetLoadedAmmo(Inventory::GetEntryFromWeapon(CurrentController, CurrentWeapon), CurrentController, *AmmoCountPtr);
								} */
							}

							if (ImGui::Button("Spawn Pickup with WID"))
							{
								std::string cpywid = WID;

								if (cpywid.find(".") == std::string::npos)
									cpywid = std::format("{}.{}", cpywid, cpywid);

								if (cpywid.find(" ") != std::string::npos)
									cpywid = cpywid.substr(cpywid.find(" ") + 1);

								auto wid = FindObjectSlow(cpywid);

								if (wid)
									Helper::SummonPickup(CurrentPawn, wid, Helper::GetActorLocationDynamic(CurrentPawn), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset);
								else
									std::cout << "Unable to find WID!\n";
							}
						}

						if (ImGui::Button("Ban"))
						{
							Moderation::Banning::Ban(CurrentController);
						}

						if (ImGui::Button("Give Item"))
						{
							if (!WID.empty())
							{
								std::string cpywid = WID;

								if (cpywid.find(".") == std::string::npos)
									cpywid = std::format("{}.{}", cpywid, cpywid);

								if (cpywid.find(" ") != std::string::npos)
									cpywid = cpywid.substr(cpywid.find(" ") + 1);

								auto wid = FindObjectSlow(cpywid);

								if (wid)
									Inventory::GiveItem(CurrentController, wid, Inventory::WhatQuickBars(wid), 1, true, Helper::GetMaxBullets(wid));
								else
									std::cout << "Unable to find WID!\n";
							}
						}

						if (ImGui::Button("Spawn Llama"))
						{
							if (CurrentPawn)
							{
								static auto LlamaClass = FindObject("/Game/Athena/SupplyDrops/Llama/AthenaSupplyDrop_Llama.AthenaSupplyDrop_Llama_C");

								std::cout << "LlamaClass: " << LlamaClass << '\n';

								if (LlamaClass)
								{
									auto Llama = Helper::Easy::SpawnActor(LlamaClass, Helper::GetActorLocation(CurrentPawn));
								}
							}
						}

						if (ImGui::Button("Kick"))
						{
							std::wstring wstr = std::wstring(KickReason.begin(), KickReason.end());
							FString Reason;
							Reason.Set(wstr.c_str());

							static auto ClientReturnToMainMenu = FindObject<UFunction>("/Script/Engine.PlayerController.ClientReturnToMainMenu");
							CurrentController->ProcessEvent(ClientReturnToMainMenu, &Reason);
						}
					}
					else if (playerTabTab == INVENTORY_PLAYERTAB)
					{
						// for (int i = 6; i < fmax(ItemInstances->Num(), 11); i++)
					}
					else if (playerTabTab == LOADOUT_PLAYERTAB)
					{
						auto CosmeticLoadoutPC = Helper::GetCosmeticLoadoutForPC(CurrentController);

						// static auto CharacterOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortAthenaLoadout", "Character");
						// auto actualCharacterPtr = Get<UObject*>(CosmeticLoadoutPC, CharacterOffset);

						// static std::string CharacterFullName = (*actualCharacterPtr)->GetPathName();
						// ImGui::InputText("Character", &CharacterFullName);

						static auto GliderOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortAthenaLoadout", "Glider");
						auto actualGliderPtr = Get<UObject*>(CosmeticLoadoutPC, GliderOffset);

						static std::string GliderFullName = (*actualGliderPtr)->GetPathName();
						ImGui::InputText("Glider", &GliderFullName);

						if (ImGui::Button("Apply"))
						{
							// auto newCharacter = FindObject(CharacterFullName);
							auto newGlider = FindObject(GliderFullName);

							// if (!newCharacter)
								// std::cout << "[WARNING] Unable to find inputted character!\n";

							if (!newGlider)
								std::cout << "[WARNING] Unable to find inputted glider!\n";

							// *actualCharacterPtr = newCharacter;
							*actualGliderPtr = newGlider;

							if (CurrentPawn)
							{
								auto CosmeticLoadoutPawn = Helper::GetCosmeticLoadoutForPawn(CurrentPawn);

								// *Get<UObject*>(CosmeticLoadoutPawn, CharacterOffset) = *actualCharacterPtr;
								*Get<UObject*>(CosmeticLoadoutPawn, GliderOffset) = *actualGliderPtr;

								/* if (*actualCharacterPtr)
								{
									Helper::ApplyCID(CurrentPawn, *actualCharacterPtr);
								} */
							}
						}
					}
					else if (playerTabTab == FUN_PLAYERTAB)
					{
						static auto LaunchCharacterJump = FindObject<UFunction>("/Script/FortniteGame.FortPawn.LaunchCharacterJump");

						if (CurrentPawn)
						{
							static auto bCanBeDamagedOffset = CurrentPawn->GetOffset("bCanBeDamaged");
							static auto bCanBeDamagedFieldMask = GetFieldMask(CurrentPawn->GetProperty("bCanBeDamaged"));
							// SetBitfield(Get<PlaceholderBitfield>(CurrentPawn, bCanBeDamagedOffset), bCanBeDamagedFieldMask, true);

							bool bCanBeDamaged = ReadBitfield(Get<PlaceholderBitfield>(CurrentPawn, bCanBeDamagedOffset), bCanBeDamagedFieldMask);

							if (ImGui::Checkbox("Can be damaged", &bCanBeDamaged))
							{
								std::cout << "bCanDamaged: " << bCanBeDamaged << '\n';
								SetBitfield(Get<PlaceholderBitfield>(CurrentPawn, bCanBeDamagedOffset), bCanBeDamagedFieldMask, bCanBeDamaged);
							}
						}

						if (LaunchCharacterJump)
						{
							static BothVector velocity;
							InputVector("Velocity", &velocity);

							bool bIgnoreFallDamage = true;

							if (ImGui::Button("Launch"))
							{
								if (Fortnite_Season < 20)
								{
									struct { FVector LaunchVelocity; bool bXYOverride; bool bZOverride; bool bIgnoreFallDamage; bool bPlayFeedbackEvent; } AFortPawn_LaunchCharacterJump_Params{
										velocity.fV, true, true, bIgnoreFallDamage, false
									};

									CurrentPawn->ProcessEvent(LaunchCharacterJump, &AFortPawn_LaunchCharacterJump_Params);
								}
								else
								{
									struct { DVector LaunchVelocity; bool bXYOverride; bool bZOverride; bool bIgnoreFallDamage; bool bPlayFeedbackEvent; } AFortPawn_LaunchCharacterJump_Params{
										velocity.dV, true, true, bIgnoreFallDamage, false
									};

									CurrentPawn->ProcessEvent(LaunchCharacterJump, &AFortPawn_LaunchCharacterJump_Params);
								}
							}

							static std::string ClassOfActorClass = "/Script/Engine.BlueprintGeneratedClass";
							ImGui::InputText("Class of the ActorClass", &ClassOfActorClass);

							static std::string ActorClassToSpawn;
							ImGui::InputText("Actor Class to spawn at player", &ActorClassToSpawn);

							if (ImGui::Button("Spawn BlueprintClass"))
							{
								if (CurrentPawn)
								{
									auto ClassOfActorClassObj = FindObject(ClassOfActorClass);

									if (ClassOfActorClassObj)
									{
										ActorSpawnStruct newSpawn;
										newSpawn.ClassOfClass = ClassOfActorClassObj;
										newSpawn.ClassToSpawn = ActorClassToSpawn;
										newSpawn.SpawnLocation = Helper::GetActorLocationDynamic(CurrentPawn);

										Defines::ActorsToSpawn.push_back(newSpawn);
									}
									else
									{
										std::cout << "Unable to find class of actor class!\n";
									}
								}
							}
						}
					}
				}
			}
		}

		ImGui::NewLine();

		if (ImGui::Button("Back"))
		{
			PlayerTab = -1;
		}
	}
}

void PregameUI()
{
	ImGui::Text(std::format("You will only be able to set these for around {} more second{}!", Defines::SecondsUntilTravel + 6, Defines::SecondsUntilTravel + 6 <= 1 ? "" : "s").c_str());

	ImGui::NewLine();

	if (!Defines::bTraveled)
		ImGui::SliderInt("Seconds until load into game", &Defines::SecondsUntilTravel, 1, 30);

	ImGui::InputText("Map Name", &Defines::MapName);

	if (!Defines::bIsPlayground && !Defines::bIsGoingToPlayMainEvent)
		ImGui::InputText("Playlist", &Defines::Playlist);

	ImGui::Checkbox("Lategame", &Defines::bIsLateGame);

	if (ImGui::Checkbox("Going to play event", &Defines::bIsGoingToPlayMainEvent))
	{
		if (Fortnite_Version != 5.30 && Fortnite_Version != 5.41) // TODO: Figure out why it makes the server crash after a player joins on 5.30, 5.41 and possibly other versions.
		{
			Defines::Playlist = Defines::bIsGoingToPlayMainEvent ? Events::GetEventPlaylistName() :
				Defines::bIsPlayground ? "/Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground"
				: "/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo";
		}
	}

	if (ImGui::Checkbox("Playground", &Defines::bIsPlayground))
	{
		// Defines::bWipeInventoryOnAircraft = Defines::bIsPlayground; // even if its playground it still clears
		Defines::Playlist = Defines::bIsPlayground ? "/Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground"
			: "/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo";

		Defines::bRespawning = Defines::bIsPlayground;
	}
}

DWORD WINAPI GuiThread(LPVOID)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"RebootClass", NULL };
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindowExW(0L, wc.lpszClassName, L"Project Reboot", (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX), 100, 100, Width, Height, NULL, NULL, wc.hInstance, NULL);

	HANDLE hIcon = LoadImageW(wc.hInstance, L"Reboot Resources/images/reboot.ico", IMAGE_ICON, 48, 48, LR_LOADFROMFILE);
	SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

	// SetWindowLongPtrW(hwnd, GWL_STYLE, WS_POPUP); // Disables windows title bar at the cost of dragging and some quality

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.IniFilename = NULL; // Disable imgui.ini generation.
	io.DisplaySize = ImGui::GetMainViewport()->Size;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	// io.Fonts->AddFontFromFileTTF("../vendor/fonts/Aller_Bd.ttf", 17);

	// Setup Dear ImGui style
	InitStyle();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImFontConfig config;
	config.MergeMode = true;
	config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	io.Fonts->AddFontFromFileTTF("Reboot Resources/fonts/fontawesome-webfont.ttf", 13.0f, &config, icon_ranges);

	// Main loop
	bool done = false;
	while (!done)
	{
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				done = true;
				break;
			}
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		auto WindowSize = ImGui::GetMainViewport()->Size;
		// ImGui::SetNextWindowPos(ImVec2(WindowSize.x * 0.5f, WindowSize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f)); // Center
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

		tagRECT rect;

		if (GetWindowRect(hwnd, &rect))
		{
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;
			ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
		}

		if (!ImGui::IsWindowCollapsed())
		{
			ImGui::Begin(("Project Reboot"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

			!Defines::bReadyForStartMatch ? MainUI() : PregameUI();

			ImGui::End();
		}

		// Rendering
		ImGui::EndFrame();
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);

		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}

		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

		// Handle loss of D3D9 device
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			ResetDevice();
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;

	// Create the D3DDevice
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
	//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;

	return true;
}

void CleanupDeviceD3D()
{
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// my implementation of window dragging..
	/* {
		static int dababy = 0;
		if (dababy > 100) // wait until gui is initialized ig?
		{
			if (ImGui::IsMouseDragging(ImGuiMouseButton(0)))
			{
				// if (LOWORD(lParam) > 255 && HIWORD(lParam) > 255)
				{
					POINT p;
					GetCursorPos(&p);

					SetWindowPos(hWnd, nullptr, p.x, p.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				}
			}
		}
		dababy++;
	} */

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}