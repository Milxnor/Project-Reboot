#include "events.h"
#include "helper.h"

bool Events::HasEvent() // Only add support versions here thanks.
{
	std::vector<double> v = { 18.40, 17.50, 17.30, 14.60, 12.41, 6.21 };

	return std::find(v.begin(), v.end(), Fortnite_Version) != v.end();
}

bool Events::IsEventSupported()
{
	return HasEvent();
}

UObject* GetEventLoader()
{
	UObject* Loader = nullptr;

	if (Fortnite_Version == 11.30)
		Loader = FindObject("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.BP_GalileoLoader_2");

	if (Fortnite_Version == 12.41)
		Loader = FindObject("/CycloneJerky/Levels/JerkyLoaderLevel.JerkyLoaderLevel.PersistentLevel.BP_Jerky_Loader_2");

	if (Fortnite_Version == 14.60)
		Loader = FindObject("/Junior/Levels/JuniorLoaderLevel.JuniorLoaderLevel.PersistentLevel.BP_Junior_Loader_2");

	std::cout << "Loader: " << Loader << '\n';

	return Loader;
}

UObject* GetEventScripting()
{
	UObject* Scripting = nullptr;
	
	if (Fortnite_Version == 11.30)
		Scripting = FindObject("/Game/Athena/Prototype/Blueprints/Galileo/Galileo_scripting.Galileo_scripting.PersistentLevel.BP_Galileo_Script2_2");
	else if (Fortnite_Version == 12.41)
		Scripting = FindObject("/CycloneJerky/Levels/JerkySequenceMap.JerkySequenceMap.PersistentLevel.BP_Jerky_Scripting_2");
	else if (Fortnite_Version == 14.60)
		Scripting = FindObject("/Junior/Levels/Junior_Map.Junior_Map.PersistentLevel.BP_Junior_Scripting_Child_2");
	else if (Fortnite_Version == 17.30)
		Scripting = FindObject("/Buffet/Levels/Buffet_P.Buffet_P.PersistentLevel.BP_Event_Master_Scripting_2");
	else if (Fortnite_Version == 17.50)
		Scripting = FindObject("/Kiwi/Levels/Kiwi_P.Kiwi_P:PersistentLevel.BP_Kiwi_Master_Scripting_2");
	else if (Fortnite_Version == 18.40)
		Scripting = FindObject("/Guava/Levels/Guava_Persistent.Guava_Persistent.PersistentLevel.BP_Guava_SpecialEventScript_2");

	std::cout << "Scripting: " << Scripting << '\n';

	return Scripting;
}

void Events::LoadEvent()
{
	__int64 Condition = true;

	if (Fortnite_Version == 11.30)
	{
		auto Loader = GetEventLoader();

		if (Loader)
		{
			auto LoadMap = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Galileo/BP_GalileoLoader.BP_GalileoLoader_C.LoadMap");
			Loader->ProcessEvent(LoadMap);

			bHasBeenLoaded = true;
		}
	}

	else if (Fortnite_Version == 12.41)
	{
		auto Loader = GetEventLoader();

		if (Loader)
		{
			auto LoadJerkyLevel = FindObject<UFunction>("/CycloneJerky/Gameplay/BP_Jerky_Loader.BP_Jerky_Loader_C.LoadJerkyLevel");
			Loader->ProcessEvent(LoadJerkyLevel, &Condition);

			bHasBeenLoaded = true;
		}
	}

	else if (Fortnite_Version == 14.60)
	{
		auto Loader = GetEventLoader();

		if (Loader)
		{
			auto LoadJuniorLevel = FindObject<UFunction>("/Junior/Blueprints/BP_Junior_Loader.BP_Junior_Loader_C.LoadJuniorLevel");
			Loader->ProcessEvent(LoadJuniorLevel, &Condition);

			bHasBeenLoaded = true;
		}
	}
}

void Events::StartEvent()
{
	Events::LoadEvent();

	float SecondsSinceEventBegan = 0;

	struct { UObject* GameState; UObject* Playlist; FGameplayTagContainer PlaylistContextTags; } bbparms{ Helper::GetGameState(), *Helper::GetPlaylist(),
		FGameplayTagContainer() };

	if (Fortnite_Version == 6.21)
	{
		UObject* BF = FindObject("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_Butterfly_4");
		UFunction* Func = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Island/BP_Butterfly.BP_Butterfly_C.ButterflySequence");
		BF->ProcessEvent(Func);
	}

	else if (Fortnite_Version == 17.30)
	{
		auto Scripting = GetEventScripting();

		if (Scripting)
		{ 
			static auto GetSequenceAndPlay = FindObject<UFunction>("/Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.GetSequenceAndPlay");
			static auto startevent = FindObject<UFunction>("/Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.startevent");
			static auto debugstartevent = FindObject<UFunction>("/Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.debugstartevent");
			static auto BB = FindObject<UFunction>("/Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.OnReady_C6091CF24046D602CBB778A594DB5BA8");
			static auto LoadNextBuffetLevel = FindObject<UFunction>("/Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.LoadNextBuffetLevel");

			int phaseIndex = 0;

			struct {
				int PhaseIndex;
				FString CallFunc_Conv_IntToString_ReturnValue;
				FString CallFunc_Concat_StrStr_ReturnValue;
			} ccparms{ phaseIndex };

			Scripting->ProcessEvent(LoadNextBuffetLevel, &ccparms);
			Scripting->ProcessEvent(BB, &bbparms);
			static auto skidder = FindObject<UFunction>("/Buffet/Gameplay/Blueprints/BP_Buffet_PhaseScripting_Phase0.BP_Buffet_PhaseScripting_Phase0_C.OnReady_9397C2DA49D638685F20BF8BBAC112AD");
			FindObject("/Buffet/Levels/Buffet_Part_0.Buffet_Part_0:PersistentLevel.BP_Buffet_PhaseScripting_Phase0_2")->ProcessEvent(skidder, &bbparms);

			Scripting->ProcessEvent(GetSequenceAndPlay, &SecondsSinceEventBegan); // idk does a lot of stuff for client

			// if (false)
			{
				static auto UpdateMasterSequence = FindObject<UFunction>("/Buffet/Gameplay/Blueprints/Buffet_SpecialEventScript.Buffet_SpecialEventScript_C.UpdateMasterSequence");
				float Time = 330.f;

				auto spcialstgidtnj = FindObject("/Buffet/Levels/Buffet_P.Buffet_P.PersistentLevel.Buffet_SpecialEventScript_2");
				spcialstgidtnj->ProcessEvent(UpdateMasterSequence, &Time); // makes portal appear
			}

			if (false)
			{
				static auto StartEventAtPhase = FindObject<UFunction>("/Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.StartEventAtPhase");
				Scripting->ProcessEvent(StartEventAtPhase, &phaseIndex); // does nothing
			}
		}
	}
	
	else if (Fortnite_Version == 17.50)
	{
		auto Scripting = GetEventScripting();

		if (Scripting)
		{
			static auto BB = FindObject<UFunction>("/Kiwi/Gameplay/BP_Kiwi_Master_Scripting.BP_Kiwi_Master_Scripting_C.OnReady_F1A32853487CB7603278E6847A5F2625");
			Scripting->ProcessEvent(BB, &bbparms);

			auto eventscript = FindObject("/Kiwi/Levels/Kiwi_P.Kiwi_P:PersistentLevel.Kiwi_EventScript_2");
			static auto CC = FindObject<UFunction>("/Kiwi/Gameplay/Kiwi_EventScript.Kiwi_EventScript_C.OnReady_F51BF8E143832CE6C552938B26BEFA93");

			eventscript->ProcessEvent(CC, &bbparms);

			static auto StartEvent = FindObject<UFunction>("/Kiwi/Gameplay/BP_Kiwi_Master_Scripting.BP_Kiwi_Master_Scripting_C.startevent");
			Scripting->ProcessEvent(StartEvent, &SecondsSinceEventBegan);
		}
	}

	else if (Fortnite_Version == 18.40)
	{
		auto SpecialScripting = GetEventScripting();

		if (SpecialScripting)
		{
			static auto BB = FindObject<UFunction>("/Guava/Gameplay/BP_Guava_SpecialEventScript.BP_Guava_SpecialEventScript_C.OnReady_F66ABDFF42CD295C079773B6FB935F15");
			SpecialScripting->ProcessEvent(BB, &bbparms); // no parms

			static auto CC = FindObject<UFunction>("/GuavaPlaylist/Gameplay/BP_Guava_PreEventManager.BP_Guava_PreEventManager_C.OnReady_51E4D8B1444FA2741D53508865F6746F");
			FindObject("/GuavaPlaylist/Levels/Guava_Preshow_Persistent.Guava_Preshow_Persistent.PersistentLevel.BP_Guava_PreEventManager_2")->ProcessEvent(CC, &bbparms);

			int InStartingIndex = 0;
			static auto StartEventAtIndex = FindObject<UFunction>("/Script/SpecialEventGameplayRuntime.SpecialEventScript.StartEventAtIndex");
			// SpecialScripting->ProcessEvent(StartEventAtIndex, &InStartingIndex); // UNTESTED

			float Time = 355.f;

			static auto UpdateMasterSequence = FindObject<UFunction>("/Guava/Gameplay/BP_Guava_SpecialEventScript.BP_Guava_SpecialEventScript_C.UpdateMasterSequence");
			SpecialScripting->ProcessEvent(UpdateMasterSequence, &Time); // skunked
		}
	}

	if (bHasBeenLoaded) // These versions require manual loading stuff
	{
		if (Fortnite_Version == 11.30)
		{
			auto Scripting = GetEventScripting();

			if (Scripting)
			{
				auto bb = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Galileo/BP_Galileo_Script.BP_Galileo_Script_C.OnReady_234EA81140DFA3305CBE47A6A4D96714");
				Scripting->ProcessEvent(bb, &bbparms);

				auto cc = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Galileo/BP_Galileo_Script.BP_Galileo_Script_C.OnReady_32DFBAAF47885A343BD19686EA225F63");
				Scripting->ProcessEvent(cc, &bbparms);

				auto dd = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Galileo/BP_Galileo_Script.BP_Galileo_Script_C.OnReady_478335584926B225595C1DA6DC16A72D");
				Scripting->ProcessEvent(dd, &bbparms);

				static auto ee = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Galileo/BP_GalileoLoader.BP_GalileoLoader_C.OnReady_C15AE8DE438DF649EB5B4A8237FF92C6");
				FindObject("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.BP_GalileoLoader_2")->ProcessEvent(ee, &bbparms);

				auto startevent = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Galileo/BP_Galileo_Script.BP_Galileo_Script_C.startevent");
				Scripting->ProcessEvent(startevent);
			}
		}

		else if (Fortnite_Version == 12.41)
		{
			auto Loader = GetEventLoader();

			if (Loader)
			{
				auto Scripting = GetEventScripting();

				if (Scripting)
				{
					static auto startevent = FindObject<UFunction>("/CycloneJerky/Gameplay/BP_Jerky_Scripting.BP_Jerky_Scripting_C.startevent");
					static auto BB = FindObject<UFunction>("/CycloneJerky/Gameplay/BP_Jerky_Scripting.BP_Jerky_Scripting_C.OnReady_093B6E664C060611B28F79B5E7052A39");

					Scripting->ProcessEvent(BB, &bbparms);
					Scripting->ProcessEvent(startevent, &SecondsSinceEventBegan);
				}
			}
		}

		else if (Fortnite_Version == 14.60)
		{
			auto Scripting = GetEventScripting();
			auto Loader = GetEventLoader();
			
			if (Loader)
			{
				static auto BB = FindObject<UFunction>("/Junior/Blueprints/BP_Junior_Loader.BP_Junior_Loader_C.OnReady_A7045E554BE892DB5B8EE0B1B9B949AE");
			}

			if (Scripting)
			{
				static auto CC = FindObject<UFunction>("/Junior/Blueprints/BP_Event_Master_Scripting.BP_Event_Master_Scripting_C.OnReady_872E6C4042121944B78EC9AC2797B053");
				static auto startevent = FindObject<UFunction>("/Junior/Blueprints/BP_Junior_Scripting.BP_Junior_Scripting_C.startevent");

				Scripting->ProcessEvent(CC, &bbparms);
				Scripting->ProcessEvent(startevent, &SecondsSinceEventBegan);
			}
		}
	}
}

void Events::StartNewYears()
{
	if (Engine_Version < 500) // 7.1, 11.31, 15.1
	{
		auto newyeartimer = Fortnite_Version < 15.10 ? FindObject("/Game/Athena/Maps/Streaming/Athena_NYE_Celebration.Athena_NYE_Celebration.PersistentLevel.BP_NewYearTimer_2") // C1
			: (Engine_Version < 500 ? nullptr : // 15.10
				nullptr); // C3

		std::cout << "newyeartimer: " << newyeartimer << '\n';

		if (newyeartimer)
		{
			static auto startnye = FindObject<UFunction>("/Game/Athena/Events/NewYear/BP_NewYearTimer.BP_NewYearTimer_C.startNYE");
			// /Game/Athena/Events/NewYear/BP_NewYearTimer.BP_NewYearTimer_C:TimeOfDaySetup

			newyeartimer->ProcessEvent(startnye);
		}
	}
}