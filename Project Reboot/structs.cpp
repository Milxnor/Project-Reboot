#include "structs.h"
#include <iostream> // TODO REMOVE
#include <format>
#include "definitions.h"

std::string FName::ToString()
{
	static auto KismetStringLibrary = FindObject("/Script/Engine.Default__KismetStringLibrary");

	static auto Conv_NameToString = FindObject<UFunction>("/Script/Engine.KismetStringLibrary.Conv_NameToString");

	struct { FName InName; FString OutStr; } Conv_NameToString_Params{*this};

	KismetStringLibrary->ProcessEvent(Conv_NameToString, &Conv_NameToString_Params);

	auto Str = Conv_NameToString_Params.OutStr.ToString();

	Conv_NameToString_Params.OutStr.Free();

	return Str;
}

std::string UObject::GetName()
{
	/* if (!StaticFindObjectO)
	{
		FString temp;

		ToStringO(&this->NamePrivate, temp);

		auto Str = temp.ToString();

		temp.Free();

		return Str;
	}
	else */
	{
		static auto GetObjectNameFunction = FindObject<UFunction>("/Script/Engine.KismetSystemLibrary.GetObjectName");
		static auto KismetSystemLibrary = FindObject("/Script/Engine.Default__KismetSystemLibrary");

		struct { UObject* Object; FString ReturnValue; } GetObjectName_Params{ this };

		KismetSystemLibrary->ProcessEvent(GetObjectNameFunction, &GetObjectName_Params);

		auto Ret = GetObjectName_Params.ReturnValue;
		auto RetStr = Ret.ToString();

		Ret.Free();

		return RetStr;
	}
}

std::string UObject::GetPathName()
{	
	/* if (!StaticFindObjectO)
	{
		std::string temp;

		for (auto outer = OuterPrivate; outer; outer = outer->OuterPrivate)
			temp = std::format("{}.{}", outer->GetName(), temp);

		return std::format("{}{}", ClassPrivate->GetName(), temp, this->GetName());
	}
	else */
	{
		static auto GetPathNameFunction = FindObject<UFunction>("/Script/Engine.KismetSystemLibrary.GetPathName");
		static auto KismetSystemLibrary = FindObject("/Script/Engine.Default__KismetSystemLibrary");

		struct { UObject* Object; FString ReturnValue; } GetPathName_Params{ this };

		KismetSystemLibrary->ProcessEvent(GetPathNameFunction, &GetPathName_Params);

		auto Ret = GetPathName_Params.ReturnValue;
		auto RetStr = Ret.ToString();

		Ret.Free();

		return RetStr;
	}
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
	if (!StaticFindObjectO)
		return (ObjectType*)FindObjectSlow(ObjectName); // (ObjectType*)FindObjectSlow(Class->GetName() + " " + ObjectName, false);

	auto& ObjectNameCut = ObjectName; // ObjectName.substr(ObjectName.find(" ") + 1);
	auto ObjectNameWide = std::wstring(ObjectNameCut.begin(), ObjectNameCut.end()).c_str();

	return (ObjectType*)StaticFindObjectO(Class, InOuter, ObjectNameWide, false);
}

int FindOffsetStruct(const std::string& StructName, const std::string& MemberName, bool bExactStruct)
{
	return FindOffsetStruct2(StructName, MemberName);

	auto Struct = bExactStruct ? FindObjectSlow(StructName, false) : FindObject(StructName);

	if (!Struct)
		return 0;

	if (Engine_Version >= 425)
		return Struct->GetOffset(MemberName);

	static auto PropertyClass = FindObject("/Script/CoreUObject.Property");

	auto Prop = FindObject(MemberName, PropertyClass, Struct);

	if (!Prop)
	{
		std::cout << "Failed to find1 " << MemberName << '\n';
		return 0;
	}

	return *(uint32_t*)(__int64(Prop) + Offset_InternalOffset);
}

std::string GetNameOfChild(void* Child)
{
	FName* NamePrivate = nullptr;

	if (Engine_Version >= 425)
		NamePrivate = (FName*)(__int64(Child) + 0x28);
	else
		NamePrivate = &((UField*)Child)->NamePrivate;

	return NamePrivate ? NamePrivate->ToString() : "";
}

void* GetNextOfChild(void* Child)
{
	if (Engine_Version >= 425)
		return *(void**)(__int64(Child) + 0x20);
	else
		return ((UField*)Child)->Next;
}

void* FindPropStruct2(const std::string& StructName, const std::string& MemberName, bool bPrint, bool bContain, bool bWarnIfNotFound)
{
	UObject* CurrentClass = nullptr;

	if (!bContain)
		CurrentClass = FindObjectSlow(StructName, false);
	else
		CurrentClass = FindObject(StructName);

	if (bPrint)
		std::cout << "CurrentClass: " << CurrentClass << '\n';

	if (CurrentClass)
	{
		auto Property = *(void**)(__int64(CurrentClass) + ChildPropertiesOffset);

		if (bPrint)
			std::cout << "Property: " << Property << '\n';

		if (Property)
		{
			auto PropName = GetNameOfChild(Property);

			while (Property)
			{
				if (bPrint)
					std::cout << "PropName: " << PropName << '\n';

				if (PropName == MemberName)
				{
					return Property;
				}
				else
				{
					Property = GetNextOfChild(Property);

					if (Property)
					{
						PropName = GetNameOfChild(Property);
					}
				}
			}
		}
	}

	if (bWarnIfNotFound)
		std::cout << "Unable to find2 " << MemberName << '\n';

	return 0;
}

UObject* LoadObject(UObject* Class, const std::string& Name) // dont trust ret
{
	UObject* Object = FindObject(Name);

	if (!Object)
	{
		Defines::ObjectsToLoad.push_back(std::make_pair(Class, Name));

		int attempts = 0;

		while (attempts < 1000 && !Object)
		{
			Object = FindObject(Name);
			attempts++;
			// Sleep(5);
		}
	}

	return Object;
}

int FindOffsetStruct2(const std::string& StructName, const std::string& MemberName, bool bPrint, bool bContain, bool bWarnIfNotFound)
{
	UObject* CurrentClass = nullptr;

	if (!bContain)
		CurrentClass = FindObjectSlow(StructName, false);
	else
		CurrentClass = FindObject(StructName);

	if (bPrint)
		std::cout << "CurrentClass: " << CurrentClass << '\n';

	if (CurrentClass)
	{
		auto Property = *(void**)(__int64(CurrentClass) + ChildPropertiesOffset);

		if (bPrint)
			std::cout << "Property: " << Property << '\n';

		if (Property)
		{
			auto PropName = GetNameOfChild(Property);

			while (Property)
			{
				if (bPrint)
					std::cout << "PropName: " << PropName << '\n';

				if (PropName == MemberName)
				{
					return *(int*)(__int64(Property) + Offset_InternalOffset);
				}
				else
				{
					Property = GetNextOfChild(Property);

					if (Property)
					{
						PropName = GetNameOfChild(Property);
					}
				}
			}
		}
	}

	if (bWarnIfNotFound)
		std::cout << "Unable to find2 " << MemberName << '\n';

	return 0;
}

int GetEnumValue(UObject* Enum, const std::string& EnumMemberName)
{
	if (!Enum)
		return -1;

	auto Names = (TArray<TPair<FName, __int64>>*)(__int64(Enum) + sizeof(UField) + sizeof(FString));

	if (Names)
	{
		for (int i = 0; i < Names->Num(); i++)
		{
			auto Pair = Names->At(i);
			auto& Name = Pair.Key();
			auto Value = Pair.Value();

			if (Name.ComparisonIndex && Name.ToString().contains(EnumMemberName))
				return Value;
		}
	}

	return -1;
}

UObject* GetDefaultObject(UObject* Class)
{
	auto name = Class->GetFullName();

	// skunked class to default
	auto ending = name.substr(name.find_last_of(".") + 1);
	auto path = name.substr(0, name.find_last_of(".") + 1);

	path = path.substr(path.find_first_of(" ") + 1);

	auto DefaultAbilityName = std::format("{0}Default__{1}", path, ending);

	return FindObject(DefaultAbilityName);
}

void* UObject::GetProperty(const std::string& MemberName, bool bIsSuperStruct, bool bPrint, bool bWarnIfNotFound)
{
	if (Engine_Version >= 425) // fprop dont think work with this
		return GetPropertySlow(MemberName, bPrint, bWarnIfNotFound);

	static auto PropertyClass = FindObject("/Script/CoreUObject.Property");

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
		if (bWarnIfNotFound)
			std::cout << "Failed to find3 " << MemberName << '\n';

		return 0;
	}

	return Property;
}

void* UObject::GetPropertySlow(const std::string& MemberName, bool bPrint, bool bWarnIfNotFound)
{
	for (auto CurrentClass = ClassPrivate; CurrentClass; CurrentClass = *(UObject**)(__int64(CurrentClass) + SuperStructOffset))
	{
		auto Property = *(void**)(__int64(CurrentClass) + ChildPropertiesOffset);

		if (Property)
		{
			auto PropName = GetNameOfChild(Property);

			if (bPrint)
				std::cout << "PropName: " << PropName << '\n';

			if (PropName == MemberName) // somehow it didnt work without this?!?!?!?!?!?!?!?!!?!!?!?!?!?
			{
				return Property;
			}

			while (Property)
			{
				if (bPrint)
					std::cout << "PropName: " << PropName << '\n';

				if (PropName == MemberName)
				{
					return Property;
				}

				Property = GetNextOfChild(Property);
				PropName = Property ? GetNameOfChild(Property) : "";
			}
		}
	}

	if (bWarnIfNotFound)
		std::cout << "Failed to find0 " << MemberName << '\n';

	return nullptr;
}

int UObject::GetOffset(const std::string& MemberName, bool bIsSuperStruct, bool bPrint, bool bWarnIfNotFound)
{
	auto Property = GetProperty(MemberName, bIsSuperStruct, bPrint, bWarnIfNotFound);

	if (Property)
		return *(int*)(__int64(Property) + Offset_InternalOffset);

	return 0;
}

int UObject::GetOffsetSlow(const std::string& MemberName, bool bPrint, bool bWarnIfNotFound)
{
	auto Property = GetPropertySlow(MemberName, bPrint, bWarnIfNotFound);

	if (Property)
		return *(int*)(__int64(Property) + Offset_InternalOffset);

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