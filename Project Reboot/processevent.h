#include <unordered_map>
#include <functional>
#include <iostream>

#include "structs.h"

static std::unordered_map<UFunction*, std::function<bool(UObject*, UFunction*, void*)>> FunctionsToHook; // INSPIRED by kem0x/raider3.5 hooking

bool ServerAcknowledgePossession(UObject* Object, UFunction* Function, void* Parameters);
bool HandleStartingNewPlayer(UObject* Object, UFunction* Function, void* Parameters);
bool ServerReadyToStartMatch(UObject* Controller, UFunction* Function, void* Parameters);
bool ReadyToStartMatch(UObject* GameMode, UFunction* Function, void* Parameters);
bool ClientOnPawnDied(UObject* DeadController, UFunction* fn, void* Parameters);
bool ServerAttemptAircraftJump(UObject* Controller, UFunction*, void* Parameters);
bool commitExecuteWeapon(UObject* Ability, UFunction*, void* Parameters);
bool OnGamePhaseChanged(UObject* MatchAnaylitics, UFunction*, void* Parameters);
bool ServerUpdateVehicleInputStateUnreliable(UObject* Pawn, UFunction* Function, void* Parameters);
bool ServerUpdatePhysicsParams(UObject* Vehicle, UFunction* Function, void* Parameters);
bool ServerGiveCreativeItem(UObject* Controller, UFunction* Function, void* Parameters);
bool ServerLoadingScreenDropped(UObject* Controller, UFunction* Function, void* Parameters);
bool OnGatherOrInteract(UObject* CBGAParent, UFunction* Function, void* Parameters);
bool ServerClientIsReadyToRespawn(UObject* Controller, UFunction*, void* Parameters);
bool ServerPlayEmoteItem(UObject* Controller, UFunction*, void* Parameters);
bool SpawnDefaultPawnFor(UObject* GameMode, UFunction*, void* Parameters);
bool ServerSendZiplineState(UObject* Pawn, UFunction*, void* Parameters);
bool HandleOwnerAsBuildingActorDestroyed(UObject* ObjectComponent, UFunction* func, void* Parameters);
bool OnDeathServer(UObject* BuildingActor, UFunction* func, void* Parameters);
bool OnAircraftExitedDropZone(UObject* GameMode, UFunction*, void* Parameters);
bool ServerCheat(UObject* PlayerController, UFunction* Function, void* Parameters);
bool PlayerCanRestart(UObject* GameMode, UFunction*, void* Parameters);
bool ServerChoosePart(UObject* Pawn, UFunction*, void* Parameters);
static bool UFuncRetTrue(UObject*, UFunction*, void* Parameters) { return true; }
bool ServerUpdateStateSync(UObject* Vehicle, UFunction*, void* Parameters);
bool onendabilitydance(UObject* Ability, UFunction*, void* Parameters);
bool spawntoynotify(UObject* Ability, UFunction*, void* Parameters);
// static bool ClientWasKicked(UObject* Controller, UFunction*, void* Parameters) { return true; }

void AddHook(const std::string& str, std::function<bool(UObject*, UFunction*, void*)> func);
void ProcessEventDetour(UObject* Object, UFunction* Function, void* Parameters);

namespace preoffsets
{
	inline int KillScore = 0;
	inline int bMarkedAlive = 0;
	inline int TeamScorePlacement = 0;
	inline int TeamScore = 0;
	inline int Place = 0;
	inline int AlivePlayers = 0;
	inline int GamePhase = 0;
	inline int LastFallDistance = 0;
	inline int DeathCause = 0;
	inline int FinisherOrDowner = 0;
	inline int bDBNO = 0;
	inline int Distance = 0;
	inline int FinisherOrDownerTags = 0;
	inline int DeathInfo = 0;
	inline int Tags = 0;
	inline int KillerPawn = 0;
	inline int KillerPlayerState = 0;
	inline int TeamsLeft = 0;
	inline int bInitialized = 0;
	inline int WinningPlayerState = 0;
	inline int DamageCauser = 0;
}