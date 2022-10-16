#include "structs.h"
#include <iostream> // TODO REMOVE
#include <format>

std::string FName::ToString()
{
	static auto KismetStringLibrary = FindObject("KismetStringLibrary /Script/Engine.Default__KismetStringLibrary");

	static auto Conv_NameToString = FindObject<UFunction>("Function /Script/Engine.KismetStringLibrary.Conv_NameToString");

	struct { FName InName; FString OutStr; } Conv_NameToString_Params{*this};

	KismetStringLibrary->ProcessEvent(Conv_NameToString, &Conv_NameToString_Params);

	auto Str = Conv_NameToString_Params.OutStr.ToString();

	Conv_NameToString_Params.OutStr.Free();

	return Str;
}

std::string UObject::GetName()
{
	static auto GetObjectNameFunction = FindObject<UFunction>("Function /Script/Engine.KismetSystemLibrary.GetObjectName");
	static auto KismetSystemLibrary = FindObject("KismetSystemLibrary /Script/Engine.Default__KismetSystemLibrary");

	struct { UObject* Object; FString ReturnValue; } GetObjectName_Params{ this };

	KismetSystemLibrary->ProcessEvent(GetObjectNameFunction, &GetObjectName_Params);

	auto Ret = GetObjectName_Params.ReturnValue;
	auto RetStr = Ret.ToString();

	Ret.Free();

	return GetObjectName_Params.ReturnValue.ToString();
}

std::string UObject::GetPathName()
{
	static auto GetPathNameFunction = FindObject<UFunction>("Function /Script/Engine.KismetSystemLibrary.GetPathName");
	static auto KismetSystemLibrary = FindObject("KismetSystemLibrary /Script/Engine.Default__KismetSystemLibrary");

	struct { UObject* Object; FString ReturnValue; } GetPathName_Params{this};

	KismetSystemLibrary->ProcessEvent(GetPathNameFunction, &GetPathName_Params);

	auto Ret = GetPathName_Params.ReturnValue;
	auto RetStr = Ret.ToString();

	Ret.Free();

	return RetStr;
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

int FindOffsetStruct(const std::string& StructName, const std::string& MemberName, bool bExactStruct)
{
	static auto PropertyClass = FindObject("Class /Script/CoreUObject.Property");

	auto Prop = FindObject(MemberName, PropertyClass, bExactStruct ? FindObjectSlow(StructName, false) : FindObject(StructName));

	if (!Prop)
	{
		std::cout << "Failed to find1 " << MemberName << '\n';
		return 0;
	}

	return *(uint32_t*)(__int64(Prop) + Offset_InternalOffset);
}

int FindOffsetStruct2(const std::string& StructName, const std::string& MemberName)
{
	auto CurrentClass = FindObjectSlow(StructName, false);

	if (CurrentClass)
	{
		auto Property = *(UField**)(__int64(CurrentClass) + ChildPropertiesOffset);

		if (Property)
		{
			auto PropName = Property->GetName();

			while (Property)
			{
				// std::cout << "PropName 2: " << PropName << '\n';

				if (PropName == MemberName)
				{
					return *(int*)(__int64(Property) + Offset_InternalOffset);
				}
				else
				{
					Property = Property->Next;

					if (Property)
					{
						PropName = Property->GetName();
					}
				}
			}
		}
	}

	std::cout << "Unable to find2 " << MemberName << '\n';

	return 0;
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

int UObject::GetOffset(const std::string& MemberName, bool bIsSuperStruct)
{
	static auto PropertyClass = FindObject("Class /Script/CoreUObject.Property");

	UObject* Property = nullptr;

	if (bIsSuperStruct)
	{
		Property = FindObject(MemberName, PropertyClass, this);
	}
	else
	{
		UObject* super = ClassPrivate;

		while (super && !Property)
		{
			Property = FindObject(MemberName, PropertyClass, super);

			super = *(UObject**)(__int64(super) + SuperStructOffset);
		}
	}

	if (!Property) // Didn't find property
	{
		std::cout << "Failed to find " << MemberName << '\n';
		return 0;
	}

	auto offsetPtr = (uint32_t*)(__int64(Property) + Offset_InternalOffset);
	return offsetPtr ? *offsetPtr : 0;
}

int UObject::GetOffsetSlow(const std::string& MemberName)
{
	for (auto CurrentClass = ClassPrivate; CurrentClass; CurrentClass = *(UObject**)(__int64(CurrentClass) + SuperStructOffset))
	{
		auto Property = *(UField**)(__int64(CurrentClass) + ChildPropertiesOffset);

		if (Property)
		{
			if (Property->GetName() == MemberName) // IDK WHY I NEED THIS
			{
				return *(int*)(__int64(Property) + Offset_InternalOffset);
			}

			while (Property)
			{
				auto PropName = Property->GetName();

				// std::cout << "PropName: " << PropName << '\n';

				if (PropName == MemberName)
				{
					return *(int*)(__int64(Property) + Offset_InternalOffset);
				}

				Property = Property->Next;
			}
		}
	}

	std::cout << "Failed to find " << MemberName << '\n';

	return 0;
}

bool UObject::IsA(UObject* otherClass)
{
	UObject* super = ClassPrivate;

	while (super)
	{
		if (otherClass == super)
			return true;

		super = *(UObject**)(__int64(super) + SuperStructOffset);
	}

	return false;
}
