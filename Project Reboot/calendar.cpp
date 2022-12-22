#include "calendar.h"

void Calendar::FixLocations()
{
	if (Fortnite_Season == 6)
	{
		if (Fortnite_Version != 6.10)
		{
			auto Lake = FindObject(("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Lake1"));
			auto Lake2 = FindObject("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Lake2");

			Fortnite_Version <= 6.21 ? ShowFoundation(Lake) : ShowFoundation(Lake2);
			// ^ This shows the lake after or before the event i dont know if this is needed.
		}
		else
		{
			auto Lake = FindObject(("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_StreamingTest12"));
			ShowFoundation(Lake);
		}

		auto FloatingIsland = Fortnite_Version == 6.10 ? FindObject(("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_StreamingTest13")) :
			FindObject(("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_FloatingIsland"));

		ShowFoundation(FloatingIsland);
	}

	if (Fortnite_Season >= 7 && Fortnite_Season <= 10)
	{
		if (Fortnite_Season == 7)
		{
			if (Fortnite_Version == 7.30)
			{
				auto PleasantParkIdk = FindObject(("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.PleasentParkFestivus"));
				ShowFoundation(PleasantParkIdk);

				auto PleasantParkGround = FindObject("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.PleasentParkDefault");
				ShowFoundation(PleasantParkGround);
			}

			auto PolarPeak = FindObject(("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_POI_25x36"));
			ShowFoundation(PolarPeak);

			auto tiltedtower = FindObject("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.ShopsNew");
			ShowFoundation(tiltedtower); // 7.40 specific?
		}

		else if (Fortnite_Season == 8)
		{
			auto Volcano = FindObject(("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_POI_50x53_Volcano"));
			ShowFoundation(Volcano);
		}

		else if (Fortnite_Season == 10)
		{
			if (Fortnite_Version >= 10.20)
			{
				auto Island = FindObject("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_StreamingTest16");
				ShowFoundation(Island);
			}
		}

		auto TheBlock = FindObject("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.SLAB_2"); // SLAB_3 is blank
		ShowFoundation(TheBlock);
	}

	if (Fortnite_Version == 17.50f) {
		auto FarmAfter = FindObject(("/Game/Athena/Apollo/Maps/Apollo_Mother.Apollo_Mother.PersistentLevel.farmbase_2"));
		ShowFoundation(FarmAfter);

		auto FarmPhase = FindObject(("/Game/Athena/Apollo/Maps/Apollo_Mother.Apollo_Mother.PersistentLevel.Farm_Phase_03")); // Farm Phases (Farm_Phase_01, Farm_Phase_02 and Farm_Phase_03)
		ShowFoundation(FarmPhase);
	}

	if (Fortnite_Version == 17.40f) {
		auto AbductedCoral = FindObject(("/Game/Athena/Apollo/Maps/Apollo_Mother.Apollo_Mother.PersistentLevel.CoralPhase_02")); // Coral Castle Phases (CoralPhase_01, CoralPhase_02 and CoralPhase_03)
		ShowFoundation(AbductedCoral);

		auto CoralFoundation_01 = FindObject(("/Game/Athena/Apollo/Maps/Apollo_Mother.Apollo_Mother.PersistentLevel.LF_Athena_16x16_Foundation_0"));
		ShowFoundation(CoralFoundation_01);

		auto CoralFoundation_05 = FindObject(("/Game/Athena/Apollo/Maps/Apollo_Mother.Apollo_Mother.PersistentLevel.LF_Athena_16x16_Foundation6"));
		ShowFoundation(CoralFoundation_05);

		auto CoralFoundation_07 = FindObject(("/Game/Athena/Apollo/Maps/Apollo_Mother.Apollo_Mother.PersistentLevel.LF_Athena_16x16_Foundation3"));
		ShowFoundation(CoralFoundation_07);

		auto CoralFoundation_10 = FindObject(("/Game/Athena/Apollo/Maps/Apollo_Mother.Apollo_Mother.PersistentLevel.LF_Athena_16x16_Foundation2_1"));
		ShowFoundation(CoralFoundation_10);

		auto CoralFoundation_13 = FindObject(("/Game/Athena/Apollo/Maps/Apollo_Mother.Apollo_Mother.PersistentLevel.LF_Athena_16x16_Foundation4"));
		ShowFoundation(CoralFoundation_13);

		auto CoralFoundation_17 = FindObject(("/Game/Athena/Apollo/Maps/Apollo_Mother.Apollo_Mother.PersistentLevel.LF_Athena_16x16_Foundation5"));
		ShowFoundation(CoralFoundation_17);
	}

	if (Fortnite_Version == 17.30f) {
		auto AbductedSlurpy = FindObject(("LF_Athena_POI_50x50_C /Game/Athena/Apollo/Maps/Apollo_Mother.Apollo_Mother.PersistentLevel.Slurpy_Phase03")); // Slurpy Swamp Phases (Slurpy_Phase01, Slurpy_Phase02 and Slurpy_Phase03)
		ShowFoundation(AbductedSlurpy);
	}

	if (Fortnite_Season == 13)
	{
		auto SpawnIsland = FindObject("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.Lobby_Foundation");
		ShowFoundation(SpawnIsland);

		// SpawnIsland->RepData->Soemthing = FoundationSetup->LobbyLocation;
	}

	if (Fortnite_Version == 12.41)
	{
		auto JS03 = FindObject(("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.LF_Athena_POI_19x19_2"));
		ShowFoundation(JS03);

		auto JH00 = FindObject(("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.BP_Jerky_Head6_18"));
		ShowFoundation(JH00);

		auto JH01 = FindObject(("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.BP_Jerky_Head5_14"));
		ShowFoundation(JH01);

		auto JH02 = FindObject(("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.BP_Jerky_Head3_8"));
		ShowFoundation(JH02);

		auto JH03 = FindObject(("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.BP_Jerky_Head_2"));
		ShowFoundation(JH03);

		auto JH04 = FindObject(("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.BP_Jerky_Head4_11"));
		ShowFoundation(JH04);
	}
}

void Calendar::SetWaterLevel(int WaterLevel)
{
	// There was 7 water levels

	UObject* WL = FindObject("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.Apollo_WaterSetup_2"); // Is this 13.40 specific?
	
	if (WL)
	{
		static auto SetWaterLevel = FindObject<UFunction>("/Game/Athena/Apollo/Environments/Blueprints/Apollo_WaterSetup.Apollo_WaterSetup_C.SetWaterLevel");
		static auto OnRep_CurrentWaterLevel = FindObject<UFunction>("/Game/Athena/Apollo/Environments/Blueprints/Apollo_WaterSetup.Apollo_WaterSetup_C.OnRep_CurrentWaterLevel");
		
		WL->ProcessEvent(SetWaterLevel, &WaterLevel);
		WL->ProcessEvent(OnRep_CurrentWaterLevel);

		/* UObject* FoundationSetup = FindObject("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations:PersistentLevel.Apollo_WaterSetup_Foundations_2");

		if (FoundationSetup)
		{
			static auto SetWaterFoundations = FindObject<UFunction>("/Game/Athena/Apollo/Environments/Blueprints/Apollo_WaterSetup_Foundations.Apollo_WaterSetup_Foundations_C.SetWaterFoundations");
		
			FoundationSetup->ProcessEvent(SetWaterFoundations, &WaterLevel);
		} */
	}
}