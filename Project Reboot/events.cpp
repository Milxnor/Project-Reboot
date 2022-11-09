#include "events.h"
#include "helper.h"

bool Events::HasEvent() // Only add support versions here thanks.
{
	std::vector<double> v = { 14.60, 12.41, 6.21 };

	return std::find(v.begin(), v.end(), Fortnite_Version) != v.end();
}

UObject* GetEventLoader()
{
	UObject* Loader = nullptr;

	if (Fortnite_Version == 12.41)
		Loader = FindObject("/CycloneJerky/Levels/JerkyLoaderLevel.JerkyLoaderLevel.PersistentLevel.BP_Jerky_Loader_2");

	if (Fortnite_Version == 14.60)
	{
		Loader = FindObjectSlow("/Junior/Levels/JuniorLoaderLevel.JuniorLoaderLevel.PersistentLevel.BP_Junior_Loader_");

		if (!Loader)
		{
			std::cout << "special!\n";
			Loader = FindObject("/Junior/Levels/JuniorLoaderLevel.JuniorLoaderLevel.PersistentLevel.BP_Junior_Loader_2");
		}
	}

	std::cout << "Loader: " << Loader << '\n';

	return Loader;
}

UObject* GetEventScripting()
{
	UObject* Scripting = nullptr;
	
	if (Fortnite_Version == 12.41)
		Scripting = FindObject("/CycloneJerky/Levels/JerkySequenceMap.JerkySequenceMap.PersistentLevel.BP_Jerky_Scripting_2");
	else if (Fortnite_Version == 14.60)
		Scripting = FindObject("/Junior/Levels/Junior_Map.Junior_Map.PersistentLevel.BP_Junior_Scripting_Child_2");

	std::cout << "Scripting: " << Scripting << '\n';

	return Scripting;
}

void Events::LoadEvent()
{
	__int64 Condition = true;

	if (Fortnite_Version == 12.41)
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

	if (bHasBeenLoaded) // These versions require loading stuff
	{
		if (Fortnite_Version == 12.41)
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

			static auto startevent = FindObject<UFunction>("/Junior/Blueprints/BP_Junior_Scripting.BP_Junior_Scripting_C.startevent");

			auto Loader = GetEventLoader();

			static auto BB = FindObject<UFunction>("/Junior/Blueprints/BP_Junior_Loader.BP_Junior_Loader_C.OnReady_A7045E554BE892DB5B8EE0B1B9B949AE");

			if (Scripting)
			{
				static auto CC = FindObject<UFunction>("/Junior/Blueprints/BP_Event_Master_Scripting.BP_Event_Master_Scripting_C.OnReady_872E6C4042121944B78EC9AC2797B053");

				Scripting->ProcessEvent(CC, &bbparms);
				Scripting->ProcessEvent(startevent, &SecondsSinceEventBegan);
			}
		}
	}
}