#include "events.h"
#include "helper.h"

bool Events::HasEvent()
{
	std::vector<double> v = { 14.60, 6.21 };

	return std::find(v.begin(), v.end(), Fortnite_Version) != v.end();
}

UObject* GetEventLoader()
{
	UObject* Loader = nullptr;

	if (Fortnite_Version == 14.60)
	{
		Loader = FindObjectSlow("/Junior/Levels/JuniorLoaderLevel.JuniorLoaderLevel.PersistentLevel.BP_Junior_Loader_");
		static auto LoadJuniorLevel = FindObject<UFunction>("/Junior/Blueprints/BP_Junior_Loader.BP_Junior_Loader_C.LoadJuniorLevel");

		if (!Loader)
		{
			std::cout << "special!\n";
			Loader = FindObject("/Junior/Levels/JuniorLoaderLevel.JuniorLoaderLevel.PersistentLevel.BP_Junior_Loader_2");
		}
	}

	std::cout << "Loader: " << Loader << '\n';

	return Loader;
}

UObject*& GetEventScripting()
{
	UObject* Scripting = nullptr;

	if (Fortnite_Version == 14.60)
	{
		Scripting = FindObject("/Junior/Levels/Junior_Map.Junior_Map.PersistentLevel.BP_Junior_Scripting_Child_2");
	}

	std::cout << "Scripting: " << Scripting << '\n';

	return Scripting;
}

void Events::LoadEvent()
{
	if (Fortnite_Version == 14.60)
	{
		auto Loader = GetEventLoader();
		auto LoadJuniorLevel = FindObject<UFunction>("/Junior/Blueprints/BP_Junior_Loader.BP_Junior_Loader_C.LoadJuniorLevel");

		std::cout << "LoadJuniorLevel: " << LoadJuniorLevel << '\n';

		__int64 CO = true;

		if (Loader)
			Loader->ProcessEvent(LoadJuniorLevel, &CO);
		else
			std::cout << "Unable to find loader!\n";
	}
}

void Events::StartEvent()
{
	if (Fortnite_Version == 6.21)
	{
		UObject* BF = FindObject("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_Butterfly_4");
		UFunction* Func = FindObject<UFunction>("/Game/Athena/Prototype/Blueprints/Island/BP_Butterfly.BP_Butterfly_C.ButterflySequence");
		BF->ProcessEvent(Func);
	}

	else if (Fortnite_Version == 14.60)
	{
		auto Scripting = GetEventScripting();

		static auto startevent = FindObject<UFunction>("/Junior/Blueprints/BP_Junior_Scripting.BP_Junior_Scripting_C.startevent");

		auto Loader = GetEventLoader();

		static auto BB = FindObject<UFunction>("/Junior/Blueprints/BP_Junior_Loader.BP_Junior_Loader_C.OnReady_A7045E554BE892DB5B8EE0B1B9B949AE");

		std::cout << "startevent: " << startevent << '\n';

		if (Loader)
		{
			struct { UObject* GameState; UObject* Playlist; FGameplayTagContainer PlaylistContextTags; } bbparms{Helper::GetGameState(), *Helper::GetPlaylist(), 
				FGameplayTagContainer()};

			Loader->ProcessEvent(BB, &bbparms);

			static auto CC = FindObject<UFunction>("/Junior/Blueprints/BP_Event_Master_Scripting.BP_Event_Master_Scripting_C.OnReady_872E6C4042121944B78EC9AC2797B053");

			// Loader->ProcessEvent(CC, &bbparms);
		}

		std::cout << "RHGEh!\n";

		float SecondsSinceEventBegan = 0;

		if (Scripting)
			Scripting->ProcessEvent(startevent, &SecondsSinceEventBegan);

		/* auto SKIDDER = FindObject("/Junior/Levels/Junior_Map.Junior_Map:PersistentLevel.Junior_Master_Rep_2.AnimationPlayer");
		static auto adfkaefi23 = FindObject<UFunction>("/Script/MovieScene.MovieSceneSequencePlayer.Play");

		std::cout << "SKIDDER: " << SKIDDER << '\n';

		if (SKIDDER)
			SKIDDER->ProcessEvent(adfkaefi23); */

		/* auto Loader = GetEventLoader();
		static auto startevent = FindObject<UFunction>("/Junior/Blueprints/BP_Junior_Loader.BP_Junior_Loader_C.startevent");

		std::cout << "startevent: " << startevent << '\n';

		if (Loader)
			Loader->ProcessEvent(startevent);
		else
			std::cout << "Unable to find Loader!\n"; */
	}
}