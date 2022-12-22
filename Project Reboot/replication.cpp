#include "replication.h"
#include "helper.h"

int PrepConnections(UObject* NetDriver)
{
    int ReadyConnections = 0;

    static auto ClientConnectionsOffset = NetDriver->GetOffset("ClientConnections");
    auto ClientConnections = (TArray<UObject*>*)(__int64(NetDriver) + ClientConnectionsOffset);

    for (int ConnIdx = 0; ConnIdx < ClientConnections->Num(); ConnIdx++)
    {
        UObject* Connection = ClientConnections->At(ConnIdx);

        if (!Connection)
            continue;

        static auto Connection_ViewTargetOffset = Connection->GetOffset("ViewTarget");
        auto ViewTarget = (UObject**)(__int64(Connection) + Connection_ViewTargetOffset);

        static auto OwningActorOffset = Connection->GetOffset("OwningActor");
        UObject* OwningActor = *(UObject**)(__int64(Connection) + OwningActorOffset);

        if (OwningActor)
        {
            ReadyConnections++;
            UObject* DesiredViewTarget = OwningActor;

            static auto Connection_PCOffset = Connection->GetOffset("PlayerController");
            auto PlayerController = *(UObject**)(__int64(Connection) + Connection_PCOffset);

            if (PlayerController)
            {
                static auto GetViewTarget = FindObject<UFunction>("/Script/Engine.Controller.GetViewTarget");
                UObject* ViewTarget = nullptr;
                PlayerController->ProcessEvent(GetViewTarget, &ViewTarget);

                if (ViewTarget)
                    DesiredViewTarget = ViewTarget;
            }

            *ViewTarget = DesiredViewTarget;

            /* for(int ChildIdx = 0; ChildIdx < Connection->Children.Num(); ++ChildIdx)
            {
                UNetConnection* ChildConnection = Connection->Children[ChildIdx];
                if (ChildConnection && ChildConnection->PlayerController && ChildConnection->ViewTarget)
                {
                    ChildConnection->ViewTarget = DesiredViewTarget;
                }
            } */
        }
        else
        {
            *ViewTarget = nullptr;

            /* for (int ChildIdx = 0; ChildIdx < Connection->Children.Num(); ++ChildIdx)
            {
                UNetConnection* ChildConnection = Connection->Children[ChildIdx];
                if (ChildConnection && ChildConnection->PlayerController && ChildConnection->ViewTarget)
                {
                    ChildConnection->ViewTarget = nullptr;
                }
            } */
        }
    }

    return ReadyConnections;
}

void BuildConsiderList(UObject* NetDriver, std::vector<FNetworkObjectInfo*>& OutConsiderList, UObject* World)
{
    bool bSupportsNetworkObjectList = false;

    static auto ActorsClass = FindObjectSlow("Class /Script/Engine.Actor", false);

    auto Actors = Helper::GetAllActorsOfClass(ActorsClass);

    for (int i = 0; i < Actors.Num(); i++)
    {
        auto Actor = Actors.At(i);

        if (!Actor)
            continue;

        static auto bActorIsBeingDestroyedOffset = Actor->GetOffset("bActorIsBeingDestroyed");

        if ((((PlaceholderBitfield*)(__int64(Actor) + bActorIsBeingDestroyedOffset))->Third))
        {
            // std::cout << "bActorIsBeingDestroyed!\n";
            continue;
        }

        static auto RemoteRoleOffset = Actor->GetOffset("RemoteRole");

        if ((*(uint8_t*)(__int64(Actor) + RemoteRoleOffset)) == 0)
        {
            // std::cout << "skidder!\n";
            continue;
        }

        static auto NetDormancyOffset = Actor->GetOffset("NetDormancy");
        static auto bNetStartupOffset = Actor->GetOffset("bNetStartup");

        bool bNetStartupIsThird = Engine_Version < 420;

        if ((*(uint8_t*)(__int64(Actor) + NetDormancyOffset) == 4) && (bNetStartupIsThird ? ((PlaceholderBitfield*)(__int64(Actor) + bNetStartupOffset))->Third :
                ((PlaceholderBitfield*)(__int64(Actor) + bNetStartupOffset))->Second))
        {
            // std::cout << "daick1!\n";
            continue;
        }

        if (Actor->NamePrivate.ComparisonIndex != 0)
        {
            Defines::CallPreReplication(Actor, NetDriver);

            auto Info = new FNetworkObjectInfo();
            Info->Actor = Actor;

            OutConsiderList.push_back(Info);
        }
    }

    Actors.Free();
}

UObject* FindChannel(UObject* Actor, UObject* Connection)
{
    static auto OpenChannelsOffset = Connection->GetOffset("OpenChannels");
    auto OpenChannels = (TArray<UObject*>*)(__int64(Connection) + OpenChannelsOffset);

    for (int i = 0; i < OpenChannels->Num(); i++)
    {
        auto Channel = OpenChannels->At(i);

        if (Channel)
        {
            static UObject* ActorChannelClass = FindObject("/Script/Engine.ActorChannel");

            if (Channel->ClassPrivate == ActorChannelClass)
            {
                static auto ActorOffset = Channel->GetOffsetSlow("Actor");

                if (*(UObject**)(__int64(Channel) + ActorOffset) == Actor)
                    return Channel;
            }
        }
    }

    return nullptr;
}

int* GetReplicationFrame(UObject* NetDriver)
{
    // return (int*)(__int64(NetDriver) + 0x428);
    if (Fortnite_Season == 1)
        return (int*)(__int64(NetDriver) + 0x288);
    if (Fortnite_Season == 2)
        return (int*)(__int64(NetDriver) + 0x2C8);
    else if (Fortnite_Season == 20 || Fortnite_Season == 21) // tested on 20.40 and 21.00
        return (int*)(__int64(NetDriver) + 0x3D8);
    else if (Fortnite_Season == 22)
        return (int*)(__int64(NetDriver) + 0x428);
    
    return nullptr;
}

int ServerReplicateActors(UObject* NetDriver)
{
    (*GetReplicationFrame(NetDriver))++;

    const int32_t NumClientsToTick = PrepConnections(NetDriver);

    // std::cout << "NumClientsToTick: " << NumClientsToTick << '\n';

    if (NumClientsToTick == 0)
    {
        // No connections are ready this frame
        return 0;
    }

    std::vector<FNetworkObjectInfo*> ConsiderList;
    BuildConsiderList(NetDriver, ConsiderList);

    std::vector<UObject*> ConnectionsToClose;

    static auto ClientConnectionsOffset = NetDriver->GetOffset("ClientConnections");
    // auto& ClientConnections = *NetDriver->CachedMember<TArray<UObject*>>(("ClientConnections"));

    auto ClientConnections = (TArray<UObject*>*)(__int64(NetDriver) + ClientConnectionsOffset);

    // std::cout << "Consider list size: " << ConsiderList.size() << '\n';

    for (int i = 0; i < ClientConnections->Num(); i++)
    {
        auto Connection = ClientConnections->At(i);

        if (!Connection)
            continue;

        if (i >= NumClientsToTick)
            continue;

        static auto Connection_ViewTargetOffset = Connection->GetOffset("ViewTarget");

        auto ViewTarget = *(UObject**)(__int64(Connection) + Connection_ViewTargetOffset);

        if (!ViewTarget)
            continue;

        static auto Connection_PCOffset = Connection->GetOffset("PlayerController");
        auto PC = *(UObject**)(__int64(Connection) + Connection_PCOffset);

        if (PC)
        {
            if (Defines::SendClientAdjustment)
                Defines::SendClientAdjustment(PC);
        }

        for (int i = 0; i < ConsiderList.size(); i++)
        {
            auto ActorInfo = ConsiderList.at(i);

            // std::cout << "Consider list size: " << ConsiderList.size() << '\n';

            auto Actor = ActorInfo->Actor;

            if (!Actor)
                continue;

            // std::cout << "Considering: " << Actor->GetFullName() << '\n';

            auto Channel = FindChannel(Actor, Connection);

            /*

            static auto bAlwaysRelevantOffset = Actor->GetOffset("bAlwaysRelevant");
            static auto bNetUseOwnerRelevancyOffset = Actor->GetOffset("bNetUseOwnerRelevancy");
            static auto bOnlyRelevantToOwnerOffset = Actor->GetOffset("bOnlyRelevantToOwner");

            if (Defines::ActorChannelClose)
            {
                if (!((PlaceholderBitfield*)(__int64(Actor) + bAlwaysRelevantOffset))->Fourth
                    && !((PlaceholderBitfield*)(__int64(Actor) + bNetUseOwnerRelevancyOffset))->Fifth
                    && !((PlaceholderBitfield*)(__int64(Actor) + bOnlyRelevantToOwnerOffset))->Third)
                {
                    if (Connection && ViewTarget)
                    {
                        auto LocBoth = Helper::GetActorLocationDynamic(ViewTarget);

                        void* Loc = Fortnite_Season >= 20 ? (void*)&LocBoth.dV : &LocBoth.fV;

                        static int Index = Fortnite_Season == 20 ? 0x99 : 0;
                        Defines::IsNetRelevantFor = decltype(Defines::IsNetRelevantFor)(Actor->VFTable[Index]);

                        if (!Defines::IsNetRelevantFor(Actor, ViewTarget, ViewTarget, Loc))
                        {
                            if (Channel)
                                Defines::ActorChannelClose(Channel, EChannelCloseReason::Relevancy);

                            continue;
                        }
                    }
                }
            }
            else */
            {
                static auto PlayerControllerClass = FindObject("/Script/Engine.PlayerController");

                if (Actor->IsA(PlayerControllerClass) && Actor != PC)
                    continue;
            }

            if (!Channel)
            {
                if (Engine_Version >= 422)
                {
                    FString ActorStr = L"Actor";
                    auto ActorName = Helper::Conversion::StringToName(ActorStr);

                    int ChannelIndex = -1; // 4294967295
                    Channel = Defines::CreateChannelByName(Connection, &ActorName, EChannelCreateFlags::OpenedLocally, ChannelIndex);
                }
                else
                {
                    Channel = Defines::CreateChannel(Connection, EChannelType::CHTYPE_Actor, true, -1);
                }

                if (Channel) {
                    Defines::SetChannelActor(Channel, Actor, ESetChannelActorFlags::None);
                    // *Channel->Member<UObject*>("Connection") = Connection;
                }
            }

            if (Channel)
                Defines::ReplicateActor(Channel);
        }
    }

    ConsiderList.clear();

    return 0;
}