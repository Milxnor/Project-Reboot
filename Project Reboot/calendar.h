// Filename is a bit misleading, this file is for specific things that happen on versions. For example missing pois, waterlevel, etc.

#pragma once

#include "structs.h"

static inline void ShowFoundation(UObject* Foundation, bool bShow = true)
{
	if (!Foundation)
		return;

	static auto DynamicFoundationTypeOffset = Foundation->GetOffset("DynamicFoundationType");
	auto DynamicFoundationType = (uint8_t*)(__int64(Foundation) + DynamicFoundationTypeOffset);

	if (DynamicFoundationType && *DynamicFoundationType)
		*DynamicFoundationType = bShow ? 0 : 3; // StartEnabled_Stationary : StartDisabled

	static auto bServerStreamedInLevelOffset = Foundation->GetOffset("bServerStreamedInLevel");

	((BITaa*)(__int64(Foundation) + bServerStreamedInLevelOffset))->bServerStreamedInLevel = bShow;

	static auto OnRep_ServerStreamedInLevel = FindObject<UFunction>("/Script/FortniteGame.BuildingFoundation.OnRep_ServerStreamedInLevel");
	Foundation->ProcessEvent(OnRep_ServerStreamedInLevel);

	if (Fortnite_Version >= 10.0) {
		static auto DynamicFoundationRepDataOffset = Foundation->GetOffset("DynamicFoundationRepData");
		auto DynamicFoundationRepData = (__int64*)(__int64(Foundation) + DynamicFoundationRepDataOffset);

		static int EnabledState_Offset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.DynamicBuildingFoundationRepData", "EnabledState");
		*reinterpret_cast<uint8_t*>(__int64(DynamicFoundationRepData) + EnabledState_Offset) = bShow;

		static auto FoundationEnabledStateOffset = Foundation->GetOffset("FoundationEnabledState");
		*(uint8_t*)(__int64(Foundation) + FoundationEnabledStateOffset) = bShow; // Enabled : Disabled

		static auto OnRep_DynamicFoundationRepData = FindObject<UFunction>("/Script/FortniteGame.BuildingFoundation.OnRep_DynamicFoundationRepData");
		Foundation->ProcessEvent(OnRep_DynamicFoundationRepData);
	}
}


namespace Calendar
{
	void FixLocations();
	void SetWaterLevel(int WaterLevel = 0);
}