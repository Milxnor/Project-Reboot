#include "calendar.h"

void ShowFoundation(UObject* Foundation, bool bShow = true)
{
	if (!Foundation)
		return;

	static auto DynamicFoundationTypeOffset = Foundation->GetOffset("DynamicFoundationType");
	auto DynamicFoundationType = (uint8_t*)(__int64(Foundation) + DynamicFoundationTypeOffset);

	if (DynamicFoundationType && *DynamicFoundationType)
		*DynamicFoundationType = bShow ? 0 : 3;

	static auto bServerStreamedInLevelOffset = Foundation->GetOffset("bServerStreamedInLevel");

	((PlaceholderBitfield*)(__int64(Foundation) + bServerStreamedInLevelOffset))->Second = bShow;

	static auto OnRep_ServerStreamedInLevel = FindObject<UFunction>("/Script/FortniteGame.BuildingFoundation.OnRep_ServerStreamedInLevel");

	if (OnRep_ServerStreamedInLevel)
		Foundation->ProcessEvent(OnRep_ServerStreamedInLevel);

	if (Fortnite_Version >= 10.0) {
		static auto DynamicFoundationRepDataOffset = Foundation->GetOffset("DynamicFoundationRepData");
		auto DynamicFoundationRepData = (__int64*)(__int64(Foundation) + DynamicFoundationRepDataOffset);

		static int EnabledState_Offset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.DynamicBuildingFoundationRepData", "EnabledState");
		*reinterpret_cast<uint8_t*>(__int64(DynamicFoundationRepData) + EnabledState_Offset) = 1;

		static auto FoundationEnabledStateOffset = Foundation->GetOffset("FoundationEnabledState");
		*(uint8_t*)(__int64(Foundation) + FoundationEnabledStateOffset) = 1;

		static auto OnRep_DynamicFoundationRepData = FindObject<UFunction>("/Script/FortniteGame.BuildingFoundation.OnRep_DynamicFoundationRepData");

		if (OnRep_DynamicFoundationRepData)
			Foundation->ProcessEvent(OnRep_DynamicFoundationRepData);
	}
}

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

		auto FloatingIsland = Fortnite_Version == 6.10f ? FindObject(("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_StreamingTest13")) :
			FindObject(("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_FloatingIsland"));

		ShowFoundation(FloatingIsland);
	}

	if (Fortnite_Season >= 7 && Fortnite_Season <= 10)
	{
		if (Fortnite_Season == 7)
		{
			if (Fortnite_Version == 7.30)
			{

			}

			auto PolarPeak = FindObject(("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_POI_25x36"));
			ShowFoundation(PolarPeak);

			auto tiltedtower = FindObject("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.ShopsNew");
			ShowFoundation(tiltedtower);
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

		else if (Fortnite_Season == 12)
		{
			if (Fortnite_Version == 12.41)
			{
				auto JS03 = FindObject(("LF_Athena_POI_19x19_C /Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.LF_Athena_POI_19x19_2"));
				ShowFoundation(JS03);

				auto JH00 = FindObject(("LF_1x1_FoundationNoSnap_C /Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.BP_Jerky_Head6_18"));
				ShowFoundation(JH00);

				auto JH01 = FindObject(("LF_1x1_FoundationNoSnap_C /Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.BP_Jerky_Head5_14"));
				ShowFoundation(JH01);

				auto JH02 = FindObject(("LF_1x1_FoundationNoSnap_C /Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.BP_Jerky_Head3_8"));
				ShowFoundation(JH02);

				auto JH03 = FindObject(("LF_1x1_FoundationNoSnap_C /Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.BP_Jerky_Head_2"));
				ShowFoundation(JH03);

				auto JH04 = FindObject(("LF_1x1_FoundationNoSnap_C /Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.BP_Jerky_Head4_11"));
				ShowFoundation(JH04);
			}
		}

		auto TheBlock = FindObject("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.SLAB_2"); // SLAB_3 is blank
		ShowFoundation(TheBlock);
	}
}

void Calendar::SetWaterLevel(int WaterLevel)
{
	

	// There was 7 water levels

	UObject* WL = FindObject("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.Apollo_WaterSetup_2"); // Is this 13.40 specific?
	// UObject* Func = WL->Function("SetWaterLevel");
	// UObject* Func2 = WL->Function("OnRep_CurrentWaterLevel");
	int NewWaterLevel = WaterLevel;
	// WL->ProcessEvent(Func, &NewWaterLevel);
	// WL->ProcessEvent(Func2);
}