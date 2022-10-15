#pragma once

#include <vector>

#include "definitions.h"
#include "enums.h"

namespace Helper
{
	namespace Easy
	{
		UObject* SpawnActor(UObject* Class, FVector Location, FRotator Rotation = FRotator(), UObject* Owner = nullptr);
		UObject* SpawnObject(UObject* Class, UObject* Outer);
	}

	static int GetSizeOfClass(UObject* Class) { return Class ? *(int*)(__int64(Class) + PropertiesSizeOffset) : 0; }
	UObject* GetWorld();
	UObject* GetTransientPackage();
	UObject* GetEngine();
	UObject* GetGameMode();
	UObject* GetGameState();
	UObject* GetLocalPlayerController();
	UObject* GetPlayerStateFromController(UObject* Controller);
	UObject* GetPawnFromController(UObject* Controller);
	UObject* SpawnPawn(UObject* Controller, FVector Location, bool bAssignCharacterParts = false);
	void ChoosePart(UObject* Pawn, TEnumAsByte<EFortCustomPartType> Part, UObject* ChosenCharacterPart);
	void SetOwner(UObject* Actor, UObject* Owner);
	UObject* GetAbilitySystemComponent(UObject* Pawn);
	void InitializeBuildingActor(UObject* Controller, UObject* BuildingActor, bool bUsePlayerBuildAnimations = false, UObject* ReplacedBuilding = nullptr);
	UObject** GetPlaylist();
	std::vector<UObject*> GetAllActorsOfClass(UObject* Class); // Vector!?!? We use a vector so we can ensure that the TArray gets free'd and theres no memory leak.

	namespace Conversion
	{
		FName StringToName(FString& String); // We don't want the fstring to never get free'd
	}
}