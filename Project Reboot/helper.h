#pragma once

#include "definitions.h"
#include "enums.h"

namespace Helper
{
	namespace Easy
	{
		UObject* SpawnActor(UObject* Class, FVector Location, FRotator Rotation = FRotator());
	}

	UObject* GetWorld();
	UObject* GetTransientPackage();
	UObject* GetEngine();
	UObject* GetLocalPlayerController();

	namespace Conversion
	{
		FName StringToName(FString String);
	}
}