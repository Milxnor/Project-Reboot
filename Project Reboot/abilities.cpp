#include "abilities.h"
#include "helper.h"

void* Abilities::GenerateNewSpec(UObject* DefaultObject)
{
	static auto SizeOfGameplayAbilitySpec = Helper::GetSizeOfClass(GameplayAbilitySpecClass);

	auto GameplayAbilitySpec = malloc(SizeOfGameplayAbilitySpec);

	// we shouldd check if its invalid but that can only happen if we are out of memory iirc

	RtlSecureZeroMemory(GameplayAbilitySpec, SizeOfGameplayAbilitySpec);

	FGameplayAbilitySpecHandle Handle{};
	Handle.GenerateNewHandle();

	((FFastArraySerializerItem*)GameplayAbilitySpec)->MostRecentArrayReplicationKey = -1;
	((FFastArraySerializerItem*)GameplayAbilitySpec)->ReplicationID = -1;
	((FFastArraySerializerItem*)GameplayAbilitySpec)->ReplicationKey = -1;

	static auto HandleOffset = GameplayAbilitySpecClass->GetOffset("Handle", true);
	static auto AbilityOffset = GameplayAbilitySpecClass->GetOffset("Ability", true);
	static auto LevelOffset = GameplayAbilitySpecClass->GetOffset("Level", true);
	static auto InputIDOffset = GameplayAbilitySpecClass->GetOffset("InputID", true);

	*(FGameplayAbilitySpecHandle*)(__int64(GameplayAbilitySpec) + HandleOffset) = Handle;
	*(UObject**)(__int64(GameplayAbilitySpec) + AbilityOffset) = DefaultObject;
	*(int*)(__int64(GameplayAbilitySpec) + LevelOffset) = 1;
	*(int*)(__int64(GameplayAbilitySpec) + InputIDOffset) = -1;

	return GameplayAbilitySpec;
}