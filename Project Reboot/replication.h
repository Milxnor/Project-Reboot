#include "structs.h"

struct FNetworkObjectInfo
{
    UObject* Actor; // AActor*

    TWeakObjectPtr<UObject> WeakActor; // AActor

    double NextUpdateTime;

    double LastNetReplicateTime;

    float OptimalNetUpdateDelta;

    float LastNetUpdateTime;

    uint32_t bPendingNetUpdate : 1;

    uint32_t bForceRelevantNextUpdate : 1;

    TSet<TWeakObjectPtr<UObject>> DormantConnections; // UNetConnection

    TSet<TWeakObjectPtr<UObject>> RecentlyDormantConnections; // UNetConnection
};

int PrepConnections(UObject* NetDriver);

void BuildConsiderList(UObject* NetDriver, std::vector<FNetworkObjectInfo*>& OutConsiderList, UObject* World = nullptr);

int ServerReplicateActors(UObject* NetDriver);