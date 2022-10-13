#include "helper.h"
#include <format>

UObject* Helper::Easy::SpawnActor(UObject* Class, FVector Location, FRotator Rotation)
{
	FActorSpawnParameters SpawnParameters{};
	return SpawnActorO(Helper::GetWorld(), Class, &Location, &Rotation, SpawnParameters);
}

UObject* Helper::GetWorld()
{
	auto Engine = GetEngine();

	static auto GameViewportOffset = Engine->GetOffset("GameViewport");
	auto GameViewport = *Get<UObject*>(Engine, GameViewportOffset);

	static auto PropertyClass = FindObject("Class /Script/CoreUObject.Property");
	(uint32_t*)(__int64(Engine) + FindObject("World", PropertyClass, FindObject("Class /Script/Engine.GameViewportClient")));

	static auto WorldOffset = 0x78; // GameViewport->GetOffset("World"); // IDK WHY NO WORK
	std::cout << "WorldOffset: " << WorldOffset << '\n';

	auto World = *Get<UObject*>(GameViewport, WorldOffset);

	return World;
}

UObject* Helper::GetTransientPackage()
{
	static auto TransientPackage = FindObject("Package /Engine/Transient");
	return TransientPackage;
}

UObject* Helper::GetEngine()
{
	static auto Engine = FindObjectSlow("FortEngine_");

	return Engine;
}

UObject* Helper::GetLocalPlayerController()
{
	auto Engine = GetEngine();

	static auto GameInstanceOffset = Engine->GetOffset("GameInstance");
	auto GameInstance = *Get<UObject*>(Engine, GameInstanceOffset);

	if (!GameInstance)
		return nullptr;

	static auto LocalPlayersOffset = GameInstance->GetOffset("LocalPlayers");
	auto LocalPlayers = Get<TArray<UObject*>>(GameInstance, LocalPlayersOffset);

	if (!LocalPlayers)
		return nullptr;

	auto LocalPlayer = LocalPlayers->At(0);
	
	if (!LocalPlayer)
		return nullptr;

	static auto PlayerControllerOffset = LocalPlayer->GetOffset("PlayerController");

	return *Get<UObject*>(LocalPlayer, PlayerControllerOffset);
}

FName Helper::Conversion::StringToName(FString String)
{
	static auto fn = FindObject<UFunction>(("Function /Script/Engine.KismetStringLibrary.Conv_StringToName"));
	static auto KSL = FindObject(("KismetStringLibrary /Script/Engine.Default__KismetStringLibrary"));

	struct {
		FString InString;
		FName ReturnValue;
	} params{ String };

	KSL->ProcessEvent(fn, &params);

	return params.ReturnValue;
}