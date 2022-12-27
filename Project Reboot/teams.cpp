#include "team.h"
#include "helper.h"

struct FGameMemberInfo : public FFastArraySerializerItem
{
	unsigned char                                      SquadId;                                                  // 0x000C(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	unsigned char                                      TeamIndex;                                                // 0x000D(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	short                                      funny; // seems like this is this max teams (100) + squad id + teamindex
	FUniqueNetIdRepl                            MemberUniqueId;                                           // 0x0010(0x0028) (HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

bool Teams::AssignTeam(UObject* Controller)
{
	using AFortTeamInfo = UObject;
	using AFortTeamPrivateInfo = UObject;
	using AController = UObject;

	auto GameState = Helper::GetGameState();
	auto PlayerState = Helper::GetPlayerStateFromController(Controller);

	static int MaxPlayersPerTeam = 0;

	if (MaxPlayersPerTeam == 0)
	{
		auto PlaylistPtr = Helper::GetPlaylist();

		if (!IsBadReadPtr(PlaylistPtr) && !IsBadReadPtr(*PlaylistPtr))
		{
			auto Playlist = *PlaylistPtr;

			static auto MaxSquadSizeOffset = Playlist->GetOffset("MaxSquadSize");

			MaxPlayersPerTeam = Defines::bIsPlayground ? 1 : *Get<int>(Playlist, MaxSquadSizeOffset);
		}
		else
		{
			MaxPlayersPerTeam = 1; // for now
		}
	}

	std::cout << "MaxPlayersPerTeam: " << MaxPlayersPerTeam << '\n';

	static auto TeamsOffset = FindOffsetStruct("Class /Script/FortniteGame.FortGameState", "Teams", true);
	auto AllTeams = (TArray<AFortTeamInfo*>*)(__int64(GameState) + TeamsOffset);

	std::cout << "AllTeams: " << AllTeams->Num() << '\n';

	if (AllTeams->Num() - 4 <= NextTeamIndex)
	{
		std::cout << "Not enough teams for next player!\n";
		return false;
	}
	
	static auto SquadIdOffset = FindOffsetStruct("Class /Script/FortniteGame.FortPlayerStateAthena", "SquadId", true);

	auto TeamIndexPtr = Helper::GetTeamIndex(PlayerState);
	auto OldTeamIndex = *TeamIndexPtr;
	auto SquadIdPtr = Get<int>(PlayerState, SquadIdOffset);

	auto NextSquadId = NextTeamIndex; // Engine_Version < 424 ? NextTeamIndex - 0 : NextTeamIndex + 1;

	*TeamIndexPtr = NextTeamIndex;

	static auto OnRep_TeamIndex = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateAthena.OnRep_TeamIndex");

	if (OnRep_TeamIndex)
		PlayerState->ProcessEvent(OnRep_TeamIndex, &OldTeamIndex);

	*SquadIdPtr = NextSquadId;

	static auto OnRep_SquadId = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateAthena.OnRep_SquadId");
	PlayerState->ProcessEvent(OnRep_SquadId);

	using AController = UObject;

	static auto PlayerTeamOffset = PlayerState->GetOffset("PlayerTeam");
	auto PlayerTeam = Get<UObject*>(PlayerState, PlayerTeamOffset);
	auto NextPlayerTeam = AllTeams->At(*TeamIndexPtr - StartingTeamIndex);

	std::cout << "PlayerTeam: " << *PlayerTeam << '\n';

	*PlayerTeam = NextPlayerTeam;

	static auto TeamMembersOffset = (*PlayerTeam)->GetOffset("TeamMembers");
	auto TeamMembers = Get<TArray<AController*>>(*PlayerTeam, TeamMembersOffset);

	// SquadMembers

	if (CurrentNumPlayersOnTeam == 0)
		TeamMembers->Free();

	TeamMembers->Add(Controller);

	static auto PlayerTeamPrivateOffset = PlayerState->GetOffset("PlayerTeamPrivate");
	auto PlayerTeamPrivate = Get<UObject*>(PlayerState, PlayerTeamPrivateOffset);

	std::cout << "PlayerTeamPrivate: " << *PlayerTeamPrivate << '\n';

	static auto PrivateInfoOffset = (*PlayerTeam)->GetOffset("PrivateInfo");
	*PlayerTeamPrivate = *Get<UObject*>(*PlayerTeam, PrivateInfoOffset);

	if (Fortnite_Version >= 7.40)
	{
		static auto GameMemberInfoArrayOffset = GameState->GetOffset("GameMemberInfoArray");
		auto GameMemberInfoArray = (void*)(__int64(GameState) + GameMemberInfoArrayOffset);

		static auto MembersOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.GameMemberInfoArray", "Members");
		auto Members = (TArray<FGameMemberInfo>*)(__int64(GameMemberInfoArray) + MembersOffset);

		static auto UniqueIdOffset = PlayerState->GetOffset("UniqueId");

		FGameMemberInfo MemberInfo;
		MemberInfo.TeamIndex = *TeamIndexPtr;
		MemberInfo.SquadId = *SquadIdPtr;
		// MemberInfo.funny = AllTeams->Num() + *SquadIdPtr + *TeamIndexPtr;
		MemberInfo.MemberUniqueId = *Get<FUniqueNetIdRepl>(PlayerState, UniqueIdOffset);

		// std::cout << "Members Size: " << Members->size() << '\n';

		Members->Add(MemberInfo);
		FastTArray::MarkArrayDirty(GameMemberInfoArray);
	}

	CurrentNumPlayersOnTeam++;

	if (CurrentNumPlayersOnTeam == MaxPlayersPerTeam)
	{
		NextTeamIndex++;
		CurrentNumPlayersOnTeam = 0;
	}

	return true;
}