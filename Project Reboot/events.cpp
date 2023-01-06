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
	else if (Fortnite_Version == 12.41)
		Loader = FindObject("/CycloneJerky/Levels/JerkyLoaderLevel.JerkyLoaderLevel.PersistentLevel.BP_Jerky_Loader_2");
	else if (Fortnite_Version == 12.61)
		Loader = FindObject("/Fritter/Level/FritterLoaderLevel.FritterLoaderLevel.PersistentLevel.BP_Fritter_Loader_0");
	else if (Fortnite_Version == 14.60)
		Loader = FindObject("/Junior/Levels/JuniorLoaderLevel.JuniorLoaderLevel.PersistentLevel.BP_Junior_Loader_2");

	std::cout << "Loader: " << Loader << '\n';

	return Loader;
}

UObject* GetEventScripting()
{
	UObject* Scripting = nullptr;
	
	if (Fortnite_Version == 7.30)
		Scripting = FindObject("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations:PersistentLevel.BP_FestivusManager");
	else if (Fortnite_Version == 8.51)
		Scripting = FindObject("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_SnowScripting_2");
	else if (Fortnite_Version == 9.40 || Fortnite_Version == 9.41)
		Scripting = FindObject("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_CattusDoggus_Scripting_2");
	else if (Fortnite_Version == 10.40)
		Scripting = FindObject("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_NightNight_Scripting_2");
	else if (Fortnite_Version == 11.30)
		Scripting = FindObject("/Game/Athena/Prototype/Blueprints/Galileo/Galileo_scripting.Galileo_scripting.PersistentLevel.BP_Galileo_Script2_2");
	else if (Fortnite_Version == 12.41)
		Scripting = FindObject("/CycloneJerky/Levels/JerkySequenceMap.JerkySequenceMap.PersistentLevel.BP_Jerky_Scripting_2");
	else if (Fortnite_Version == 12.61)
		Scripting = FindObject("/Fritter/Level/FritterSequenceLevel.FritterSequenceLevel.PersistentLevel.BP_Fritter_Script_2");
	else if (Fortnite_Version == 14.60)
		Scripting = FindObject("/Junior/Levels/Junior_Map.Junior_Map.PersistentLevel.BP_Junior_Scripting_Child_2");
	else if (Fortnite_Season == 16)
		Scripting = FindObject("/Yogurt/Levels/YogurtLoaderLevel.YogurtLoaderLevel:PersistentLevel.BP_Yogurt_Scripting_2");
	else if (Fortnite_Version == 17.30)
		Scripting = FindObject("/Buffet/Levels/Buffet_P.Buffet_P.PersistentLevel.BP_Event_Master_Scripting_2");
	else if (Fortnite_Version == 17.50)
		Scripting = FindObject("/Kiwi/Levels/Kiwi_P.Kiwi_P:PersistentLevel.BP_Kiwi_Master_Scripting_2");
	else if (Fortnite_Version == 18.40)
		Scripting = FindObject("/Guava/Levels/Guava_Persistent.Guava_Persistent.PersistentLevel.BP_Guava_SpecialEventScript_2");
	else if (Fortnite_Version == 20.40)
		Scripting = FindObject("/Armadillo/Levels/Armadillo_Persistent.Armadillo_Persistent:PersistentLevel.BP_Armadillo_SpecialEventScript_C_1");

	std::cout << "Scripting: " << Scripting << '\n';

	return Scripting;
}

void Events::LoadEvent()
{
	__int64 Condition = true;

	if (Fortnite_Version == 5.30)
	{
		UObject* AEC = FindObject(("/Game/Athena/Maps/Streaming/Athena_GameplayActors.Athena_GameplayActors.PersistentLevel.BP_Athena_Event_Components_54"));

		if (AEC)
		{
			auto ProgressionFunc = FindObject<UFunction>("/Game/Athena/Events/BP_Athena_Event_Components.BP_Athena_Event_Components_C.OnRep_CrackProgression");
			auto CrackOpacityOffset = AEC->GetOffset("CrackOpacity");
			*Get<float>(AEC, CrackOpacityOffset) = 0.0f; // Hide the initial crack
			AEC->ProcessEvent(ProgressionFunc);

			auto CorruptionFunc = FindObject<UFunction>("/Game/Athena/Events/BP_Athena_Event_Components.BP_Athena_Event_Components_C.OnRep_Corruption");
			auto CorruptionOffset = AEC->GetOffset("Corruption");
			*Get<float>(AEC, CorruptionOffset) = 1.0f; // Show the smaller purple crack
			AEC->ProcessEvent(CorruptionFunc);
		}
	}

	else if (Fortnite_Version == 6.21)
	{
		UObject* BF = FindObject(("BP_Butterfly_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_Butterfly_4"));

		if (BF)
		{
			auto loadfun = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Island/BP_Butterfly.BP_Butterfly_C.LoadButterflySublevel");
			BF->ProcessEvent(loadfun);

			bHasBeenLoaded = true;
		}

	}

	else if (Fortnite_Version == 8.51) // needed idk
	{
		auto Scripting = GetEventScripting();

		if (Scripting)
		{
			auto LoadSnowLevel = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/White/BP_SnowScripting.BP_SnowScripting_C.LoadSnowLevel");
			Scripting->ProcessEvent(LoadSnowLevel);

			bHasBeenLoaded = true;
		}
	}

	else if (Fortnite_Version == 9.40 || Fortnite_Version == 9.41)
	{
		auto Scripting = GetEventScripting();

		if (Scripting)
		{
			auto LoadCattusLevel = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Cattus/BP_CattusDoggus_Scripting.BP_CattusDoggus_Scripting_C.LoadCattusLevel");
			Scripting->ProcessEvent(LoadCattusLevel, &Condition);

			bHasBeenLoaded = true;
		}
	}

	else if (Fortnite_Version == 10.40)
	{
		auto Scripting = GetEventScripting();

		if (Scripting)
		{
			auto LoadNightNightLevel = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/NightNight/BP_NightNight_Scripting.BP_NightNight_Scripting_C.LoadNightNightLevel");
			Scripting->ProcessEvent(LoadNightNightLevel, &Condition);

			bHasBeenLoaded = true;
		}
	}

	else if (Fortnite_Version == 11.30)
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

	else if (Fortnite_Version == 12.61)
	{
		auto Loader = GetEventLoader();

		if (Loader)
		{
			auto LoadFritterLevel = FindObject<UFunction>("/Fritter/BP_Fritter_Loader.BP_Fritter_Loader_C.LoadFritterLevel");
			Loader->ProcessEvent(LoadFritterLevel, &Condition);

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

	else if (Fortnite_Version == 17.30)
	{
		auto Scripting = GetEventScripting();

		if (Scripting)
		{
			auto initializelevelloader = FindObject<UFunction>("/Buffet/Gameplay/Blueprints/BP_Buffet_Master_Scripting.BP_Buffet_Master_Scripting_C.InitializeLevelLoader");
			Scripting->ProcessEvent(initializelevelloader);

			auto buffetcllaoder = FindObject("/Buffet/Gameplay/Blueprints/BP_Buffet_Level_Loader.BP_Buffet_Level_Loader_C");

			auto newloader = Helper::Easy::SpawnActor(buffetcllaoder, FVector());

			int PhasEindex = 0;

			struct { bool CO; int PhaseIndex; } loadeparm{ true, 0 };

			auto augfu8 = FindObject<UFunction>("/Buffet/Gameplay/Blueprints/BP_Buffet_Level_Loader.BP_Buffet_Level_Loader_C.LoadBuffetLevel");
			newloader->ProcessEvent(augfu8, &loadeparm);

			auto Buffet_LevelLoaderBPOffset = Scripting->GetOffset("Buffet_LevelLoaderBP");
			*Get<UObject*>(Scripting, Buffet_LevelLoaderBPOffset) = newloader;

			bHasBeenLoaded = true;
		}
	}
}

void Events::StartEvent()
{
	Events::LoadEvent();

	static bool bFirst = bHasBeenLoaded;

	if (bFirst)
	{
		bFirst = false;

		Sleep(500);
	}

	float SecondsSinceEventBegan = 0;

	struct { UObject* GameState; UObject* Playlist; FGameplayTagContainer PlaylistContextTags; } bbparms{ Helper::GetGameState(), *Helper::GetPlaylist(),
		FGameplayTagContainer() };

	if (Fortnite_Version == 5.30) // Cube spawning event
	{
		auto AEC = FindObject("/Game/Athena/Maps/Streaming/Athena_GameplayActors.Athena_GameplayActors.PersistentLevel.BP_Athena_Event_Components_54");
		auto FinalAECFunc = FindObject<UFunction>("/Game/Athena/Events/BP_Athena_Event_Components.BP_Athena_Event_Components_C.Final");
		AEC->ProcessEvent(FinalAECFunc);

		static auto bCubeSpawned = false;

		if (!bCubeSpawned)
		{
			auto Cube = FindObject("/Game/Athena/Maps/Test/Level_CUBE.Level_CUBE.PersistentLevel.CUBE_2");
			auto FinalCubeFunc = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Cube/CUBE.CUBE_C.Final");
			Cube->ProcessEvent(FinalCubeFunc);

			bCubeSpawned = true;
		}
	}

	else if (Fortnite_Version == 5.41) // Impact lake with cube
	{
		auto cube = FindObject("/Game/Athena/Maps/Test/Level_CUBE.Level_CUBE.PersistentLevel.CUBE_2");

		auto cubedest = FindObject("/Temp/Game/Athena/Maps/POI/Athena_POI_Lake_001_53fd26f4.Athena_POI_Lake_001.PersistentLevel.Cube_Dest_Scripting_2");

		std::cout << "cubedest: " << cubedest << '\n';

		// static auto setuplake = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Cube/CUBE.CUBE_C.SetupLake");
		// cube->ProcessEvent(setuplake); // this kinda does the same thing as impact lake

		auto impactlake = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Cube/CUBE.CUBE_C.ImpactLake");
		cube->ProcessEvent(impactlake);
	}

	else if (Fortnite_Version == 7.30)
	{
		auto FestivusManager = GetEventScripting();

		if (FestivusManager)
		{
			auto BB = FindObject<UFunction>("/Game/Athena/Environments/Festivus/Blueprints/BP_FestivusManager.BP_FestivusManager_C.OnReady_EE7676604ADFD92D7B2972AC0ABD4BB8");
			FestivusManager->ProcessEvent(BB, &bbparms);

			auto PlayConcert = FindObject<UFunction>("/Game/Athena/Environments/Festivus/Blueprints/BP_FestivusManager.BP_FestivusManager_C.PlayConcert");
			FestivusManager->ProcessEvent(PlayConcert);
		}
	}

	// S16+ they changed events kinda

	else if (Fortnite_Season == 16)
	{
		auto Scripting = GetEventScripting();

		if (Scripting)
		{
			auto cc = FindObject<UFunction>("/Yogurt/Blueprints/BP_Yogurt_Scripting.BP_Yogurt_Scripting_C.PlayBPTrailer");
			Scripting->ProcessEvent(cc);

			auto bb = FindObject<UFunction>("/Yogurt/Blueprints/BP_Yogurt_Scripting.BP_Yogurt_Scripting_C.OnReady_F6E6B09F4E6AA115465FAABCD4B97504");
			Scripting->ProcessEvent(bb, &bbparms);

			if (true)
			{
				// SecondsSinceEventBegan = 1; // LocalEventPhase

				auto startvent = FindObject<UFunction>("/Yogurt/Blueprints/BP_Yogurt_Scripting.BP_Yogurt_Scripting_C.startevent");
				Scripting->ProcessEvent(startvent, &SecondsSinceEventBegan); // very weird afeiugfq24ug2487 3q5hiu8e jheroig rewi ug9 uh35u bg35iy hu35 vgiuy53we	gh bewr5ti9ou	HG 
			}
			else
			{
				auto playermaste = FindObject<UFunction>("/Yogurt/Blueprints/BP_Yogurt_Scripting.BP_Yogurt_Scripting_C.PlayMasterAtFrame");
				Scripting->ProcessEvent(playermaste, &SecondsSinceEventBegan); // idk
			}
		}
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

			// Scripting->ProcessEvent(LoadNextBuffetLevel, &ccparms);
			Scripting->ProcessEvent(BB, &bbparms);
			static auto skidder = FindObject<UFunction>("/Buffet/Gameplay/Blueprints/BP_Buffet_PhaseScripting_Phase0.BP_Buffet_PhaseScripting_Phase0_C.OnReady_9397C2DA49D638685F20BF8BBAC112AD");

			static auto afi2 = FindObject<UFunction>("/Buffet/Gameplay/Blueprints/BP_Buffet_PhaseScripting_Paint.BP_Buffet_PhaseScripting_Paint_C.OnReady_4E0ADA484A9A29A99CA6DD97BE645F09");

			static auto beasfwq9 = FindObject<UFunction>("/Buffet/Gameplay/Blueprints/Buffet_SpecialEventScript.Buffet_SpecialEventScript_C.OnReady_D357CD7841974BE4734824A7031B6C50");

			// Scripting->ProcessEvent(GetSequenceAndPlay, &SecondsSinceEventBegan); // idk does a lot of stuff for client

			static auto BP_OnScriptReady = FindObject<UFunction>("/Buffet/Gameplay/Blueprints/Buffet_SpecialEventScript.Buffet_SpecialEventScript_C.BP_OnScriptReady");
			static auto StartEventAtIndex = FindObject<UFunction>("/Script/SpecialEventGameplayRuntime.SpecialEventScript.StartEventAtIndex");

			static auto wufg289 = FindObject<UFunction>("/Buffet/Gameplay/Blueprints/Buffet_SpecialEventScript.Buffet_SpecialEventScript_C.OnReady_9CE4B3D844C02EE607CEE79E5E8FB819");

			auto GameState = Helper::GetGameState();
			auto FortTimeOfDayManagerOffset = GameState->GetOffset("FortTimeOfDayManager");

			auto FortTimeOfDayManager = *Get<UObject*>(GameState, FortTimeOfDayManagerOffset);

			auto SpecialEventScripting = FindObject("/Buffet/Levels/Buffet_P.Buffet_P.PersistentLevel.Buffet_SpecialEventScript_2");
			SpecialEventScripting->ProcessEvent(beasfwq9);
			SpecialEventScripting->ProcessEvent(wufg289, &FortTimeOfDayManager);

			std::cout << "SpecialEventScripting: " << SpecialEventScripting << '\n';

			static auto Buffet_LevelLoaderBPOffset = Scripting->GetOffset("Buffet_LevelLoaderBP");
			std::cout << "Buffet_LevelLoaderBP: " << *Get<UObject*>(Scripting, Buffet_LevelLoaderBPOffset) << '\n';

			SpecialEventScripting->ProcessEvent(BP_OnScriptReady);

			static auto MeshRootStartEvent = FindObject<UFunction>("/Script/SpecialEventGameplayRuntime.SpecialEventScriptMeshActor.MeshRootStartEvent");
			auto MeshScriptActor = FindObject("/Game/Athena/Apollo/Maps/Apollo_Terrain.Apollo_Terrain.PersistentLevel.MeshScriptActorBP_2");

			/* static auto RootStartTimeOffset = MeshScriptActor->GetOffset("RootStartTime");
			Get<FDateTime>(MeshScriptActor, RootStartTimeOffset)->Ticks = 99; // FDateTime.Now()

			static auto OnRep_RootStartTime = FindObject<UFunction>("/Script/SpecialEventGameplayRuntime.SpecialEventScriptMeshActor.OnRep_RootStartTime");
			MeshScriptActor->ProcessEvent(OnRep_RootStartTime); */

			// SpecialEventScripting->ProcessEvent(StartEventAtIndex, &SecondsSinceEventBegan);
			
			std::cout << "Buffet_LevelLoaderBPAfter: " << *Get<UObject*>(Scripting, Buffet_LevelLoaderBPOffset) << '\n';

			// FindObject("/Buffet/Levels/Buffet_Part_0.Buffet_Part_0.PersistentLevel.BP_Buffet_PhaseScripting_Phase0_2")->ProcessEvent(skidder, &bbparms); // start evebnt somehow

			Scripting->ProcessEvent(startevent, &SecondsSinceEventBegan);

			if (false)
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
			auto BB = FindObject<UFunction>("/Kiwi/Gameplay/BP_Kiwi_Master_Scripting.BP_Kiwi_Master_Scripting_C.OnReady_F1A32853487CB7603278E6847A5F2625");
			Scripting->ProcessEvent(BB, &bbparms);

			auto eventscript = FindObject("/Kiwi/Levels/Kiwi_P.Kiwi_P:PersistentLevel.Kiwi_EventScript_2");
			auto CC = FindObject<UFunction>("/Kiwi/Gameplay/Kiwi_EventScript.Kiwi_EventScript_C.OnReady_F51BF8E143832CE6C552938B26BEFA93");
			auto DD = FindObject<UFunction>("/Kiwi/Gameplay/Kiwi_EventScript.Kiwi_EventScript_C.LoadKiwiAssets");
			auto StartEventAtIndex = FindObject<UFunction>("/Script/SpecialEventGameplayRuntime.SpecialEventScript.StartEventAtIndex");
			auto BP_OnScriptReady = FindObject<UFunction>("/Kiwi/Gameplay/Kiwi_EventScript.Kiwi_EventScript_C.BP_OnScriptReady");

			// eventscript->ProcessEvent(CC, &bbparms);
			eventscript->ProcessEvent(DD, &bbparms);
			eventscript->ProcessEvent(BP_OnScriptReady, &bbparms);
			eventscript->ProcessEvent(StartEventAtIndex, &SecondsSinceEventBegan);

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
			// SpecialScripting->ProcessEvent(BB, &bbparms); // no parms

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

	else if (Fortnite_Version == 20.40)
	{
		auto SpecialScripting = GetEventScripting();

		if (SpecialScripting)
		{
			auto bb = FindObject<UFunction>("/ArmadilloPlaylist/Blueprints/BP_Armadillo_PreEventManager.BP_Armadillo_PreEventManager_C.OnReady_D381C71746BBAD0F4EA129BFBE7272DA");
			auto preeventmanager = FindObject("/ArmadilloPlaylist/Levels/Armadillo_Preshow_Presistent.Armadillo_Preshow_Presistent:PersistentLevel.BP_Armadillo_PreEventManager_C_1");
			preeventmanager->ProcessEvent(bb, &bbparms);

			// auto StartEventAtIndex = FindObject<UFunction>("/Script/SpecialEventGameplayRuntime.SpecialEventScript.StartEventAtIndex");
			// SpecialScripting->ProcessEvent(StartEventAtIndex, &SecondsSinceEventBegan);

			auto startevent = FindObject<UFunction>("/ArmadilloPlaylist/Blueprints/BP_Armadillo_PreEventManager.BP_Armadillo_PreEventManager_C.startevent");
			preeventmanager->ProcessEvent(startevent, &SecondsSinceEventBegan);
		}
	}

	if (bHasBeenLoaded) // These versions require manual loading stuff
	{
		if (Fortnite_Version == 6.21)
		{
			UObject* BF = FindObject("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_Butterfly_4");
			UFunction* Func = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Island/BP_Butterfly.BP_Butterfly_C.ButterflySequence");
			BF->ProcessEvent(Func);
		}

		else if (Fortnite_Version == 8.51)
		{
			auto Scripting = GetEventScripting();

			if (Scripting)
			{
				static auto bb = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/White/BP_SnowScripting.BP_SnowScripting_C.OnReady_3281D978468AC0749E26BA9A32FA5FDC");
				Scripting->ProcessEvent(bb, &bbparms);

				static auto cc = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/White/BP_SnowScripting.BP_SnowScripting_C.OnReady_C97C252C4EFA0CBBAD2A7D8BFA4F01D6");
				Scripting->ProcessEvent(cc, &bbparms);

				static auto dd = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/White/BP_SnowScripting.BP_SnowScripting_C.OnReady_F1878E954C5364B45E9D7987F26E0369");
				Scripting->ProcessEvent(dd, &bbparms);

				static auto ee = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/White/BP_SnowScripting.BP_SnowScripting_C.OnReady_4E9C34F84F10C94BEACF96AB6972B0E2");
				Scripting->ProcessEvent(ee, &bbparms);

				static auto FinalSequence = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/White/BP_SnowScripting.BP_SnowScripting_C.FinalSequence");
				Scripting->ProcessEvent(FinalSequence);
			}
		}

		else if (Fortnite_Version == 9.40 || Fortnite_Version == 9.41)
		{
			auto Scripting = GetEventScripting();

			if (Scripting)
			{
				auto bb = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Cattus/BP_CattusDoggus_Scripting.BP_CattusDoggus_Scripting_C.OnReady_C11CA7624A74FBAEC54753A3C2BD4506");
				Scripting->ProcessEvent(bb, &bbparms);

				auto startevent = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Cattus/BP_CattusDoggus_Scripting.BP_CattusDoggus_Scripting_C.startevent");
				Scripting->ProcessEvent(startevent);
			}
		}

		else if (Fortnite_Version == 10.40)
		{
			auto Scripting = GetEventScripting();

			if (Scripting)
			{
				auto bb = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/NightNight/BP_NightNight_Scripting.BP_NightNight_Scripting_C.OnReady_D0847F7B4E80F01E77156AA4E7131AF6");
				Scripting->ProcessEvent(bb, &bbparms);

				auto startevent = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/NightNight/BP_NightNight_Scripting.BP_NightNight_Scripting_C.startevent");
				Scripting->ProcessEvent(startevent);
			}
		}

		else if (Fortnite_Version == 11.30)
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
					auto scripting_startevent = FindObject<UFunction>("/CycloneJerky/Gameplay/BP_Jerky_Scripting.BP_Jerky_Scripting_C.startevent");
					auto loader_startevent = FindObject<UFunction>("/CycloneJerky/Gameplay/BP_Jerky_Loader.BP_Jerky_Loader_C.startevent");
					auto BB = FindObject<UFunction>("/CycloneJerky/Gameplay/BP_Jerky_Scripting.BP_Jerky_Scripting_C.OnReady_093B6E664C060611B28F79B5E7052A39");
					auto CC = FindObject<UFunction>("/CycloneJerky/Gameplay/BP_Jerky_Loader.BP_Jerky_Loader_C.OnReady_7FE9744D479411040654F5886C078D08");
					auto loader_callstarteventonscripting = FindObject<UFunction>("/CycloneJerky/Gameplay/BP_Jerky_Loader.BP_Jerky_Loader_C.CallStartEventOnScripting");
					auto DebugStartSequence = FindObject<UFunction>("/CycloneJerky/Gameplay/BP_Jerky_Parent.BP_Jerky_Parent_C.DebugStartSequence");
					auto DD = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Sky/BP_SkyJerk.BP_SkyJerk_C.OnReady_BF7D9B86447A86EF0458DDAF0DA3EE1A");

					std::cout << "DebugStartSequence: " << DebugStartSequence << '\n';

					Loader->ProcessEvent(CC, &bbparms);
					Scripting->ProcessEvent(BB, &bbparms);
					Loader->ProcessEvent(loader_startevent, &SecondsSinceEventBegan);
					Scripting->ProcessEvent(scripting_startevent, &SecondsSinceEventBegan);
					Scripting->ProcessEvent(DebugStartSequence, &SecondsSinceEventBegan);
					Loader->ProcessEvent(loader_callstarteventonscripting, &SecondsSinceEventBegan);

					std::cout << "aa!\n";
				}
			}
		}

		else if (Fortnite_Version == 12.61)
		{
			// if (false)
			{
				auto Scripting = GetEventScripting();

				if (Scripting)
				{
					auto bb = FindObject<UFunction>("/Fritter/BP_Fritter_Script.BP_Fritter_Script_C.OnReady_ACE66C28499BF8A59B3D88A981DDEF41");
					Scripting->ProcessEvent(bb, &bbparms);

					auto startevent = FindObject<UFunction>("/Fritter/BP_Fritter_Script.BP_Fritter_Script_C.startevent");
					// Scripting->ProcessEvent(startevent, &SecondsSinceEventBegan);
				}
			}
			// else
			{
				auto Loader = GetEventLoader();

				if (Loader)
				{
					auto bb = FindObject<UFunction>("/Fritter/BP_Fritter_Loader.BP_Fritter_Loader_C.OnReady_1216203B4B63E3DFA03042A62380A674");
					Loader->ProcessEvent(bb, &bbparms);

					auto startevent = FindObject<UFunction>("/Fritter/BP_Fritter_Loader.BP_Fritter_Loader_C.startevent");
					auto callstarteventonscripting = FindObject<UFunction>("/Fritter/BP_Fritter_Loader.BP_Fritter_Loader_C.CallStartEventOnScripting");
					// Loader->ProcessEvent(startevent, &SecondsSinceEventBegan);
					Loader->ProcessEvent(callstarteventonscripting, &SecondsSinceEventBegan);
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
			: (Engine_Version < 500 ? FindObject("/NewYears/Levels/Apollo_NYE_Celebration.Apollo_NYE_Celebration.PersistentLevel.BP_NewYearTimer_2") : // 15.10
				nullptr); // C3

		std::cout << "newyeartimer: " << newyeartimer << '\n';

		if (newyeartimer)
		{
			auto startnye = Fortnite_Version < 15.10 ? FindObject<UFunction>("/Game/Athena/Events/NewYear/BP_NewYearTimer.BP_NewYearTimer_C.startNYE") :
				FindObject<UFunction>("/NewYears/Blueprints/BP_NewYearTimer.BP_NewYearTimer_C.startNYE");

			// /Game/Athena/Events/NewYear/BP_NewYearTimer.BP_NewYearTimer_C:TimeOfDaySetup

			std::cout << "startnye: " << startnye << '\n';

			if (startnye)
			{
				newyeartimer->ProcessEvent(startnye);
			}
		}
	}
}

std::string Events::GetEventPlaylistName()
{
	if (Fortnite_Version == 20.40)
		return "/ArmadilloPlaylist/Playlist/Playlist_Armadillo.Playlist_Armadillo";
	else if (Fortnite_Version == 18.40)
		return "/GuavaPlaylist/Playlist/Playlist_Guava.Playlist_Guava";
	else if (Fortnite_Version == 17.50)
		return "/KiwiPlaylist/Playlists/Playlist_Kiwi.Playlist_Kiwi";
	else if (Fortnite_Version == 17.30)
		return "/BuffetPlaylist/Playlist/Playlist_Buffet.Playlist_Buffet";
	else if (Fortnite_Season == 16)
		return "/Yogurt/Playlist/Playlist_Yogurt.Playlist_Yogurt";
	else if (Fortnite_Version == 14.60)
		return "/Game/Athena/Playlists/Music/Playlist_Junior_32.Playlist_Junior_32";
	else if (Fortnite_Version == 12.61)
		return "/Game/Athena/Playlists/Fritter/Playlist_Fritter_High.Playlist_Fritter_High";
	else if (Engine_Version >= 422 && Fortnite_Version <= 12.41)
		return "/Game/Athena/Playlists/Music/Playlist_Music_High.Playlist_Music_High";

	return "/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo";
}

BothVector Events::GetSpawnLocation(bool* outSuccess)
{
	if (outSuccess)
		*outSuccess = true; 

	if (Fortnite_Version == 20.40)
		return BothVector(DVector(-104890.109, 120585.164, 117040.789));

	if (outSuccess)
		*outSuccess = false;

	return BothVector();
}

void Events::Unvault(FName ItemToUnvault)
{
	if (Fortnite_Version != 8.51)
	{
		std::cout << "Attempted to unvault item not on 8.51!\n";
		return;
	}

	if (!Defines::bIsGoingToPlayMainEvent)
	{
		std::cout << "Attempted to unvault item not on main event!\n";
		return;
	}

	auto Scripting = GetEventScripting();

	if (Scripting)
	{
		static auto UnvaultedItemNameOffset = Scripting->GetOffset("UnvaultedItemName");
		*Get<FName>(Scripting, UnvaultedItemNameOffset) = ItemToUnvault;

		static auto onrep = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/White/BP_SnowScripting.BP_SnowScripting_C.OnRep_UnvaultedItemName");
		Scripting->ProcessEvent(onrep);

		static auto PillarsConcluded = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/White/BP_SnowScripting.BP_SnowScripting_C.PillarsConcluded");
		Scripting->ProcessEvent(PillarsConcluded, &ItemToUnvault);

		// PillarsCompletedUTCTime <= PRETTY Sure this is how we're supposed to do it
	}
}