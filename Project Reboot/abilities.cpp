#include "abilities.h"
#include "helper.h"
#include <functional>
#include "patterns.h"

void Abilities::ClientActivateAbilityFailed(UObject* ASC, FGameplayAbilitySpecHandle AbilityToActivate, int16_t PredictionKey)
{
    struct { FGameplayAbilitySpecHandle AbilityToActivate; int16_t PredictionKey; } UAbilitySystemComponent_ClientActivateAbilityFailed_Params{ AbilityToActivate, PredictionKey };
    static auto fn = FindObject<UFunction>("/Script/GameplayAbilities.AbilitySystemComponent.ClientActivateAbilityFailed");

    ASC->ProcessEvent(fn, &UAbilitySystemComponent_ClientActivateAbilityFailed_Params);
}

void* Abilities::GenerateNewSpec(UObject* DefaultObject, UObject* SourceObject)
{
	static auto SizeOfGameplayAbilitySpec = Helper::GetSizeOfClass(GameplayAbilitySpecClass);

	auto GameplayAbilitySpec = Alloc(SizeOfGameplayAbilitySpec);

    if (!GameplayAbilitySpec)
        return nullptr;

	RtlSecureZeroMemory(GameplayAbilitySpec, SizeOfGameplayAbilitySpec);

	FGameplayAbilitySpecHandle Handle{};
	Handle.GenerateNewHandle();

	((FFastArraySerializerItem*)GameplayAbilitySpec)->MostRecentArrayReplicationKey = -1;
	((FFastArraySerializerItem*)GameplayAbilitySpec)->ReplicationID = -1;
	((FFastArraySerializerItem*)GameplayAbilitySpec)->ReplicationKey = -1;

    static auto HandleOffset = FindOffsetStruct2("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Handle");
    static auto AbilityOffset = FindOffsetStruct2("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Ability");
    static auto LevelOffset = FindOffsetStruct2("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Level");
    static auto SourceObjectOffset = FindOffsetStruct2("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "SourceObject");
    static auto InputIDOffset = FindOffsetStruct2("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "InputID");

    std::cout << "AbilityOffset: " << AbilityOffset << '\n';

	*(FGameplayAbilitySpecHandle*)(__int64(GameplayAbilitySpec) + HandleOffset) = Handle;
    *(UObject**)(__int64(GameplayAbilitySpec) + SourceObjectOffset) = SourceObject;
	*(UObject**)(__int64(GameplayAbilitySpec) + AbilityOffset) = DefaultObject;
	*(int*)(__int64(GameplayAbilitySpec) + LevelOffset) = 1;
	*(int*)(__int64(GameplayAbilitySpec) + InputIDOffset) = -1;

	return GameplayAbilitySpec;
}

__int64* GetActivatableAbilities(UObject* ASC)
{
    static auto ActivatableAbilitiesOffset = ASC->GetOffset("ActivatableAbilities");

    return (__int64*)(__int64(ASC) + ActivatableAbilitiesOffset);
}

UObject** GetAbilityFromSpec(void* Spec)
{
    static auto AbilityOffset = FindOffsetStruct2("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Ability");

    return (UObject**)(__int64(Spec) + AbilityOffset);
}

int16_t* GetCurrent(void* Key)
{
    static auto CurrentOffset = FindOffsetStruct2("ScriptStruct /Script/GameplayAbilities.PredictionKey", "Current");
    return (int16_t*)(__int64(Key) + CurrentOffset);
}

void LoopSpecs(UObject* ASC, std::function<void(__int64*)> func)
{
    auto ActivatableAbilities = GetActivatableAbilities(ASC);

    static auto ItemsOffset = FindOffsetStruct2("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpecContainer", "Items");
    auto Items = (TArray<__int64>*)(__int64(ActivatableAbilities) + ItemsOffset);

    static auto SpecStruct = Abilities::GameplayAbilitySpecClass;
    static auto SpecSize = Helper::GetSizeOfClass(SpecStruct);

    if (ActivatableAbilities && Items)
    {
        for (int i = 0; i < Items->Num(); i++)
        {
            auto CurrentSpec = (__int64*)(__int64(Items->Data) + (static_cast<long long>(SpecSize) * i));
            func(CurrentSpec);
        }
    }
}

__int64* FindAbilitySpecFromHandle(UObject* ASC, FGameplayAbilitySpecHandle Handle)
{
    __int64* SpecToReturn = nullptr;

    auto compareHandles = [&Handle, &SpecToReturn](__int64* Spec) {
        static auto HandleOffset = FindOffsetStruct2("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Handle");

        auto CurrentHandle = (FGameplayAbilitySpecHandle*)(__int64(Spec) + HandleOffset);

        if ((*CurrentHandle).Handle == Handle.Handle)
        {
            SpecToReturn = Spec;
            return;
        }
    };

    LoopSpecs(ASC, compareHandles);

    return SpecToReturn;
}

void InternalServerTryActivateAbility(UObject* ASC, FGameplayAbilitySpecHandle Handle, bool InputPressed, void* PredictionKey, __int64* TriggerEventData)
{
    void* Spec = FindAbilitySpecFromHandle(ASC, Handle);
    auto CurrentPredictionKey = GetCurrent(PredictionKey);

    if (!Spec)
    {
        // Can potentially happen in race conditions where client tries to activate ability that is removed server side before it is received.
        std::cout << ("InternalServerTryActivateAbility. Rejecting ClientActivation of ability with invalid SpecHandle!\n");
        Abilities::ClientActivateAbilityFailed(ASC, Handle, *CurrentPredictionKey);
        return;
    }

    const UObject* AbilityToActivate = *GetAbilityFromSpec(Spec);

    if (!AbilityToActivate) //!ensure(AbilityToActivate))
    {
        std::cout << ("InternalServerTryActiveAbility. Rejecting ClientActivation of unconfigured spec ability!\n");
        Abilities::ClientActivateAbilityFailed(ASC, Handle, *CurrentPredictionKey);
        return;
    }

    static auto InputPressedOffset = FindOffsetStruct2("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "InputPressed");

    auto inad = (char*)(__int64(Spec) + InputPressedOffset);

    if (((bool(1) << 1) & *(bool*)(inad)) != 1)
    {
        *inad = (*inad & ~1) | (true ? 1 : 0);
    }

    bool res = false;

    if (Engine_Version == 426 && Fortnite_Season < 17)
        res = Defines::InternalTryActivateAbilityFTS(ASC, Handle, *(PadHex10*)PredictionKey, nullptr, nullptr, TriggerEventData);
    else
        res = Defines::InternalTryActivateAbility(ASC, Handle, *(PadHex18*)PredictionKey, nullptr, nullptr, TriggerEventData);

    if (!res)
    {
        std::cout << std::format("InternalServerTryActivateAbility. Rejecting ClientActivation of {}. InternalTryActivateAbility failed\n", (*GetAbilityFromSpec(Spec))->GetName());
        Abilities::ClientActivateAbilityFailed(ASC, Handle, *GetCurrent(PredictionKey));

        if (((bool(1) << 1) & *(bool*)(inad)) != 1)
        {
            *inad = (*inad & ~1) | (false ? 1 : 0);
        }

        FastTArray::MarkItemDirty(GetActivatableAbilities(ASC), (FFastArraySerializerItem*)Spec); // TODO: Start using the proper func again
    }
}

std::vector<UObject*> Abilities::DoesASCHaveAbility(UObject* ASC, UObject* Ability)
{
    std::vector<UObject*> AbilitiesToReturn;

    if (!ASC || !Ability)
        return AbilitiesToReturn;

    auto compareAbilities = [&AbilitiesToReturn, &Ability](__int64* Spec) {
        auto CurrentAbility = GetAbilityFromSpec(Spec);

        if ((*CurrentAbility)->ClassPrivate == Ability->ClassPrivate)
        {
            AbilitiesToReturn.push_back(*CurrentAbility);
            // AbilityToReturn = *CurrentAbility;
            // return;
        }
    };

    LoopSpecs(ASC, compareAbilities);

    return AbilitiesToReturn;
}

void* Abilities::GrantGameplayAbility(UObject* TargetPawn, UObject* GameplayAbilityClass, UObject* SourceObject)
{
    auto AbilitySystemComponent = Helper::GetAbilitySystemComponent(TargetPawn);

    if (!AbilitySystemComponent)
        return nullptr;

    UObject* DefaultObject = nullptr;

    if (GameplayAbilityClass->GetName().contains("Default__"))
        DefaultObject = GameplayAbilityClass; //->CreateDefaultObject(); // Easy::SpawnObject(GameplayAbilityClass, GameplayAbilityClass->OuterPrivate);
    else
    {
        // im dumb
        static std::unordered_map<std::string, UObject*> defaultAbilities; // normal class name, default ability.

        auto name = GameplayAbilityClass->GetFullName();

        auto defaultafqaf = defaultAbilities.find(name);

        if (defaultafqaf != defaultAbilities.end())
        {
            DefaultObject = defaultafqaf->second;
        }
        else
        {
            // skunked class to default
            auto ending = name.substr(name.find_last_of(".") + 1);
            auto path = name.substr(0, name.find_last_of(".") + 1);

            path = path.substr(path.find_first_of(" ") + 1);

            auto DefaultAbilityName = std::format("{0}Default__{1}", path, ending);

            std::cout << "DefaultAbilityName: " << DefaultAbilityName << '\n';

            DefaultObject = FindObject(DefaultAbilityName);
            defaultAbilities.emplace(name, DefaultObject);
        }
    }

    if (!DefaultObject)
    {
        std::cout << "Failed to create defaultobject for GameplayAbilityClass: " << GameplayAbilityClass->GetFullName() << '\n';
        return nullptr;
    }

    static auto HandleOffset = FindOffsetStruct2("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Handle");

    void* NewSpec = GenerateNewSpec(DefaultObject, SourceObject);

    if (!NewSpec)
        return nullptr;

    auto Handle = (FGameplayAbilitySpecHandle*)(__int64(NewSpec) + HandleOffset);

    if (!NewSpec || DoesASCHaveAbility(AbilitySystemComponent, *GetAbilityFromSpec(NewSpec)).size())
        return nullptr;

    // https://github.com/EpicGames/UnrealEngine/blob/4.22/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L232

    std::cout << "giving ability: " << DefaultObject->GetFullName() << '\n';

    if (Fortnite_Season == 13)
        Defines::GiveAbilityS13(AbilitySystemComponent, Handle, *(PadHexC0*)NewSpec);
    else if (Fortnite_Season >= 14 && Fortnite_Season < 17)
        Defines::GiveAbilityS14ABOVE(AbilitySystemComponent, Handle, *(PadHexE0*)NewSpec);
    else if (Fortnite_Season >= 17)
        Defines::GiveAbilityS17ABOVE(AbilitySystemComponent, Handle, *(PadHexE8*)NewSpec);
    else if (Engine_Version < 426 && Engine_Version >= 420)
        Defines::GiveAbility(AbilitySystemComponent, Handle, *(PadHexC8*)NewSpec);
    else
        Defines::GiveAbilityOld(AbilitySystemComponent, Handle, *(PadHex78*)NewSpec);

    return NewSpec;
}

void Abilities::GiveAbilityAndActivateOnce(UObject* ASC, UObject* Class, UObject* SourceObject, __int64* EventData) // https://github.com/EpicGames/UnrealEngine/blob/5c73d9fb0afbacff0d0c9f9c5d8a0b5cc1e0119c/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L251
{
    if (!GiveAbilityAndActivateOnceAddress)
        return;

    unsigned int* (*GiveAbilityAndActivateOnce)(UObject* ASC, int* outHandle, PadHexE8 Spec) = decltype(GiveAbilityAndActivateOnce)(GiveAbilityAndActivateOnceAddress);
    unsigned int* (*GiveAbilityAndActivateOnceNew)(UObject* ASC, int* outHandle, PadHexE8 Spec, __int64* GameplayEventData) = decltype(GiveAbilityAndActivateOnceNew)(GiveAbilityAndActivateOnceAddress);

    if (!Class)
        return;

    if (!Class->GetName().contains("Default__"))
       Class = GetDefaultObject(Class);

    if (!Class)
        return;

    int outHandle = 0;

    auto NewSpec = GenerateNewSpec(Class, SourceObject);

    if (Engine_Version < 500)
    {
        GiveAbilityAndActivateOnce(ASC, &outHandle, *(PadHexE8*)NewSpec);
    }
    else
    {
        GiveAbilityAndActivateOnceNew(ASC, &outHandle, *(PadHexE8*)NewSpec, EventData);
    }

    return;

    /*

    // we should set bActivateOnce if it exists before granting it

    auto Spec = Abilities::GrantGameplayAbility(Pawn, Class, SourceObject);

    if (!Spec)
        return nullptr;

    auto ASC = Helper::GetAbilitySystemComponent(Pawn);

    bool res = false;

    static auto HandleOffset = FindOffsetStruct2("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Handle");
    static auto RemoveAfterActivationOffset = FindOffsetStruct2("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "RemoveAfterActivation");
    static auto RemoveAfterActivationFieldMask = 0x2;

    auto Handle = (FGameplayAbilitySpecHandle*)(__int64(Spec) + HandleOffset);

    SetBitfield(Spec, RemoveAfterActivationFieldMask, true);
    std::cout << "RemoveAfterActivation: " << ReadBitfield(Spec, RemoveAfterActivationFieldMask) << '\n';

    if (Engine_Version == 426 && Fortnite_Season < 17)
        res = Defines::InternalTryActivateAbilityFTS(ASC, *Handle, PadHex10(), nullptr, nullptr, EventData);
    else
        res = Defines::InternalTryActivateAbility(ASC, *Handle, PadHex18(), nullptr, nullptr, EventData);

    if (!res)
    {
        // ClearAbility
        return nullptr;
    }

    return Handle;

    */
}

bool Abilities::ServerTryActivateAbility(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters)
{
    if (!Parameters)
        return false;

    struct UAbilitySystemComponent_ServerTryActivateAbility_Params { FGameplayAbilitySpecHandle AbilityToActivate; bool InputPressed; __int64 PredictionKey; };

    auto Params = (UAbilitySystemComponent_ServerTryActivateAbility_Params*)Parameters;

    static auto AbilityToActivateOffset = FindOffsetStruct2("/Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbility", "AbilityToActivate", true, true);
    static auto PredictionKeyOffset = FindOffsetStruct2("/Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbility", "PredictionKey", true, true);
    static auto InputPressedOffset = FindOffsetStruct2("/Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbility", "InputPressed", true, true);

    InternalServerTryActivateAbility(AbilitySystemComponent, *(FGameplayAbilitySpecHandle*)(__int64(Parameters) + AbilityToActivateOffset),
        *(bool*)(__int64(Params) + InputPressedOffset), (void*)(__int64(Parameters) + PredictionKeyOffset), nullptr);

    return false;
}

bool Abilities::ServerTryActivateAbilityWithEventData(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters)
{
    if (!Parameters)
        return false;

    static auto PredictionKeyOffset = FindOffsetStruct2("/Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbilityWithEventData", "PredictionKey", true, true);
    static auto TriggerEventDataOffset = FindOffsetStruct2("/Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbilityWithEventData", "TriggerEventData", true, true);

    struct UAbilitySystemComponent_ServerTryActivateAbilityWithEventData_Params {
        FGameplayAbilitySpecHandle                  AbilityToActivate;                                        // (Parm)
        bool                                               InputPressed;                                             // (Parm, ZeroConstructor, IsPlainOldData)
        PadHex10                              PredictionKey;                                            // (Parm)
        __int64                          TriggerEventData;                                         // (Parm)
    };

    auto Params = (UAbilitySystemComponent_ServerTryActivateAbilityWithEventData_Params*)Parameters;

    InternalServerTryActivateAbility(AbilitySystemComponent, Params->AbilityToActivate, Params->InputPressed,
        (__int64*)(__int64(Parameters) + PredictionKeyOffset),
        (__int64*)(__int64(Parameters) + TriggerEventDataOffset));

    return false;
}

bool Abilities::ServerAbilityRPCBatch(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters)
{
    if (!Parameters)
        return false;

    static auto AbilitySpecHandleOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.ServerAbilityRPCBatch", "AbilitySpecHandle"); // Function->GetParam<FGameplayAbilitySpecHandle>("AbilitySpecHandle", Parameters);
    static auto InputPressedOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.ServerAbilityRPCBatch", "InputPressed");
    static auto PredictionKeyOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.ServerAbilityRPCBatch", "PredictionKey");

    auto BatchInfo = (__int64*)(Parameters);

    if (!BatchInfo)
        return false;

    auto AbilitySpecHandle = (FGameplayAbilitySpecHandle*)(__int64(BatchInfo) + AbilitySpecHandleOffset);
    auto InputPressed = (bool*)(__int64(BatchInfo) + InputPressedOffset);
    auto PredictionKey = (__int64*)(__int64(BatchInfo) + PredictionKeyOffset);

    auto AbilitySpec = FindAbilitySpecFromHandle(AbilitySystemComponent, *AbilitySpecHandle);

    if (!AbilitySpec)
        return false;

    InternalServerTryActivateAbility(AbilitySystemComponent, *AbilitySpecHandle, *InputPressed, PredictionKey, nullptr);

    return false;
}

void GiveFortAbilitySet(UObject* Pawn, UObject* FortAbilitySet)
{
    if (!FortAbilitySet)
        return;

    static auto GameplayAbilitiesOffset = FortAbilitySet->GetOffset("GameplayAbilities");
    auto Abilities = Get<TArray<UObject*>>(FortAbilitySet, GameplayAbilitiesOffset);

    for (int i = 0; i < Abilities->Num(); i++)
    {
        auto Ability = Abilities->At(i);

        if (!Ability)
            continue;

        Abilities::GrantGameplayAbility(Pawn, Ability);
    }

    static auto GrantedGameplayEffectsOffset = FortAbilitySet->GetOffset("GrantedGameplayEffects");

    if (GrantedGameplayEffectsOffset != 0)
    {
        struct FGameplayEffectApplicationInfoHard
        {
            UObject* GameplayEffectClass;                                           // 0x0000(0x0008) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            float                                              Level;                                                    // 0x0008(0x0004) (Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
            unsigned char                                      UnknownData00[0x4];                                       // 0x000C(0x0004) MISSED OFFSET
        };

        auto GrantedGameplayEffects = Get<TArray<FGameplayEffectApplicationInfoHard>>(FortAbilitySet, GrantedGameplayEffectsOffset);

        for (int i = 0; i < GrantedGameplayEffects->Num(); i++)
        {
            auto& GameplayEffectInfo = GrantedGameplayEffects->At(i);

            auto GameplayEffectToGrant = GameplayEffectInfo.GameplayEffectClass;
            Helper::ApplyGameplayEffect(Pawn, GameplayEffectToGrant, GameplayEffectInfo.Level);
        }
    }
}