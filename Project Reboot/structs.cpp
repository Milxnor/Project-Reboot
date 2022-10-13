#include "structs.h"
#include <iostream> // TODO REMOVE
#include <format>

std::string UObject::GetName()
{
	static auto GetObjectNameFunction = FindObject<UFunction>("Function /Script/Engine.KismetSystemLibrary.GetObjectName");
	static auto KismetSystemLibrary = FindObject("KismetSystemLibrary /Script/Engine.Default__KismetSystemLibrary");

	struct { UObject* Object; FString ReturnValue; } GetObjectName_Params{ this };

	KismetSystemLibrary->ProcessEvent(GetObjectNameFunction, &GetObjectName_Params);

	return GetObjectName_Params.ReturnValue.ToString();
}

std::string UObject::GetPathName()
{
	static auto GetPathNameFunction = FindObject<UFunction>("Function /Script/Engine.KismetSystemLibrary.GetPathName");
	static auto KismetSystemLibrary = FindObject("KismetSystemLibrary /Script/Engine.Default__KismetSystemLibrary");

	struct { UObject* Object; FString ReturnValue; } GetPathName_Params{this};

	KismetSystemLibrary->ProcessEvent(GetPathNameFunction, &GetPathName_Params);

	return GetPathName_Params.ReturnValue.ToString();
}

std::string UObject::GetFullName()
{
	return ClassPrivate ? ClassPrivate->GetName() + " " + GetPathName() : "NoClassPrivate";
}

void UObject::ProcessEvent(struct UFunction* Function, void* Parameters)
{
	return ProcessEventO(this, reinterpret_cast<UObject*>(Function), Parameters);
}

template <typename ObjectType>
ObjectType* FindObject(const std::string& ObjectName, UObject* Class, UObject* InOuter)
{
	auto ObjectNameCut = ObjectName.substr(ObjectName.find(" ") + 1);
	auto ObjectNameWide = std::wstring(ObjectNameCut.begin(), ObjectNameCut.end()).c_str();

	return (ObjectType*)StaticFindObjectO(Class, InOuter, ObjectNameWide, false);
}

UObject* GetDefaultObject(UObject* Class)
{
	auto name = Class->GetFullName();

	// skunked class to default
	auto ending = name.substr(name.find_last_of(".") + 1);
	auto path = name.substr(0, name.find_last_of(".") + 1);

	path = path.substr(path.find_first_of(" ") + 1);

	auto DefaultAbilityName = std::format("{1} {0}Default__{1}", path, ending);

	return FindObject(DefaultAbilityName);
}

int UObject::GetOffset(const std::string& MemberName)
{
	static auto PropertyClass = FindObject("Class /Script/CoreUObject.Property");

	UObject* Property = nullptr;

	UObject* super = ClassPrivate;

	while (super && !Property)
	{
		Property = FindObject(MemberName, PropertyClass, super);

		super = *(UObject**)(__int64(super) + SuperStructOffset);
	}

	if (!Property) // Didn't find property
		return 0;

	auto offsetPtr = (uint32_t*)(__int64(Property) + Offset_InternalOffset);
	return offsetPtr ? *offsetPtr : 0;
}