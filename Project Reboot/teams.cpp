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

	static int NextTeamIndex = 3;

	auto GameState = Helper::GetGameState();
	auto PlayerState = Helper::GetPlayerStateFromController(Controller);

	static auto TeamsOffset = FindOffsetStruct("Class /Script/FortniteGame.FortGameState", "Teams", true);
	auto AllTeams = (TArray<AFortTeamInfo*>*)(__int64(GameState) + TeamsOffset);

	std::cout << "AllTeams: " << AllTeams->Num() << '\n';

	if (AllTeams->Num() <= NextTeamIndex)
	{
		std::cout << "Not enough teams for next player!\n";
		return false;
	}
	
	static auto SquadIdOffset = FindOffsetStruct("Class /Script/FortniteGame.FortPlayerStateAthena", "SquadId", true);

	auto TeamIndexPtr = Helper::GetTeamIndex(PlayerState);
	auto SquadIdPtr = Get<int>(PlayerState, SquadIdOffset);

	auto NextSquadId = NextTeamIndex - 0;

	*TeamIndexPtr = NextTeamIndex;

	// static auto OnRep_TeamIndex = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateAthena.OnRep_TeamIndex");
	// PlayerState->ProcessEvent(OnRep_TeamIndex);

	*SquadIdPtr = NextSquadId;

	static auto OnRep_SquadId = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateAthena.OnRep_SquadId");
	PlayerState->ProcessEvent(OnRep_SquadId);

	if (Engine_Version >= 422)
	{
		static auto GameMemberInfoArrayOffset = GameState->GetOffset("GameMemberInfoArray");
		auto GameMemberInfoArray = (void*)(__int64(GameState) + GameMemberInfoArrayOffset);

		static auto MembersOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.GameMemberInfoArray", "Members");
		auto Members = (TArray<FGameMemberInfo>*)(__int64(GameMemberInfoArray) + MembersOffset);

		static auto UniqueIdOffset = PlayerState->GetOffset("UniqueId");

		FGameMemberInfo MemberInfo = FGameMemberInfo{ -1, -1, -1 };
		MemberInfo.TeamIndex = *TeamIndexPtr;
		MemberInfo.SquadId = *SquadIdPtr;
		MemberInfo.MemberUniqueId = *(FUniqueNetIdRepl*)(__int64(PlayerState) + UniqueIdOffset);

		if (Members)
		{
			Members->Add(MemberInfo);
			FastTArray::MarkArrayDirty(GameMemberInfoArray);
		}
	}

	// NextTeamIndex++;
	
	return true;
}