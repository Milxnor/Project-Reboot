#pragma once

#include <Windows.h>
#include <string>
#include <locale>

#include "enums.h"

template <typename T>
T* Get(void* addr, uint64_t off) { return (T*)(__int64(addr) + off); }

inline double Fortnite_Version;
inline int Engine_Version; // NOTE: 427 == 4.26.1
inline int Fortnite_Season;
inline int Offset_InternalOffset = 0x0;
inline int SuperStructOffset = 0x0;

template <class ElementType>
struct TArray
{
	ElementType* Data = nullptr;
	int32_t ArrayNum = 0;
	int32_t ArrayMax = 0;

	inline ElementType At(int i, int Size = sizeof(ElementType)) const
	{
		return *(ElementType*)(__int64(Data) + (static_cast<long long>(Size) * i));
	}
};

class FString
{
public:
	TArray<TCHAR> Data;

	std::string ToString() const
	{
		auto length = std::wcslen(Data.Data);
		std::string str(length, '\0');
		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data.Data, Data.Data + length, '?', &str[0]);

		return str;
	}

	void Set(const wchar_t* NewStr) // by fischsalat
	{
		if (!NewStr || std::wcslen(NewStr) == 0) return;

		Data.ArrayMax = Data.ArrayNum = *NewStr ? (int)std::wcslen(NewStr) + 1 : 0;

		if (Data.ArrayNum)
			Data.Data = const_cast<wchar_t*>(NewStr);
	}

	FString() {}

	FString(const wchar_t* str)
	{
		Set(str);
	}
};

struct FName
{
	uint32_t ComparisonIndex;
	uint32_t Number;
};

struct UObject
{
	void** VFTable;
	int32_t ObjectFlags;
	int32_t InternalIndex;
	UObject* ClassPrivate;
	FName NamePrivate;
	UObject* OuterPrivate;

	std::string GetName();
	std::string GetPathName();
	std::string GetFullName();

	void ProcessEvent(struct UFunction* Function, void* Parameters);

	int GetOffset(const std::string& MemberName);
};

struct FActorSpawnParameters
{
	FName Name;
	UObject* Template; // AActor*
	UObject* Owner; // AActor*
	UObject* Instigator; // APawn*
	UObject* OverrideLevel; // ULevel*
	ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingOverride;
	uint16_t	bRemoteOwned : 1;
	uint16_t	bNoFail : 1;
	uint16_t	bDeferConstruction : 1;
	uint16_t	bAllowDuringConstructionScript : 1;
	EObjectFlags ObjectFlags;
};

inline UObject* (*StaticFindObjectO)(UObject* Class, UObject* InOuter, const TCHAR* Name, bool ExactClass);
inline UObject* (*StaticLoadObjectO)(UObject* Class, UObject* InOuter, const TCHAR* Name, const TCHAR* Filename, uint32_t LoadFlags, UObject* Sandbox, bool bAllowObjectReconciliation, void* InSerializeContext);
inline void (*ProcessEventO)(UObject* object, UObject* func, void* Parameters);
inline UObject* (*SpawnActorO)(UObject* World, UObject* Class, FVector* Position, FRotator* Rotation, const FActorSpawnParameters& SpawnParameters);

struct FUObjectItem // https://github.com/EpicGames/UnrealEngine/blob/4.27/Engine/Source/Runtime/CoreUObject/Public/UObject/UObjectArray.h#L26
{
	UObject* Object;
	int32_t Flags;
	int32_t ClusterRootIndex;
	int32_t SerialNumber;
	// int pad_01;
};

struct FFixedUObjectArray
{
	FUObjectItem* Objects;
	int32_t MaxElements;
	int32_t NumElements;

	const int32_t Num() const { return NumElements; }

	const int32_t Capacity() const { return MaxElements; }

	bool IsValidIndex(int32_t Index) const { return Index < Num() && Index >= 0; }

	UObject* GetObjectById(int32_t Index) const
	{
		return Objects[Index].Object;
	}

	FUObjectItem* GetItemById(int32_t Index) const
	{
		return &Objects[Index];
	}
};

struct FChunkedFixedUObjectArray // https://github.com/EpicGames/UnrealEngine/blob/7acbae1c8d1736bb5a0da4f6ed21ccb237bc8851/Engine/Source/Runtime/CoreUObject/Public/UObject/UObjectArray.h#L321
{
	enum
	{
		NumElementsPerChunk = 64 * 1024,
	};

	FUObjectItem** Objects;
	FUObjectItem* PreAllocatedObjects;
	int32_t MaxElements;
	int32_t NumElements;
	int32_t MaxChunks;
	int32_t NumChunks;

	inline const int32_t Num() const { return NumElements; }

	const int32_t Capacity() const { return MaxElements; }

	UObject* GetObjectById(int32_t Index) const
	{
		if (Index > NumElements || Index < 0) return nullptr;

		const int32_t ChunkIndex = Index / NumElementsPerChunk;
		const int32_t WithinChunkIndex = Index % NumElementsPerChunk;

		if (ChunkIndex > NumChunks) return nullptr;
		FUObjectItem* Chunk = Objects[ChunkIndex];
		if (!Chunk) return nullptr;

		auto obj = (Chunk + WithinChunkIndex)->Object;

		return obj;
	}

	FUObjectItem* GetItemById(int32_t Index) const
	{
		if (Index > NumElements || Index < 0) return nullptr;

		const int32_t ChunkIndex = Index / NumElementsPerChunk;
		const int32_t WithinChunkIndex = Index % NumElementsPerChunk;

		if (ChunkIndex > NumChunks) return nullptr;
		FUObjectItem* Chunk = Objects[ChunkIndex];
		if (!Chunk) return nullptr;

		auto obj = (Chunk + WithinChunkIndex);

		return obj;
	}
};

inline FFixedUObjectArray* OldObjects;
inline FChunkedFixedUObjectArray* NewObjects;

static UObject* GetObjectByIndex(int Index) { return OldObjects ? OldObjects->GetObjectById(Index) : NewObjects->GetObjectById(Index); }

struct UFunction {};

#define ANY_PACKAGE ((UObject*)-1)

template <typename ObjectType = UObject> // idk why i cant put this func in the cpp
ObjectType* FindObjectSlow(const std::string& ObjectName, bool bContains = true) // Loops through GObjects and gets each name and compares
{
	auto ObjectNum = OldObjects ? OldObjects->Num() : NewObjects->Num();

	for (int i = 0; i < ObjectNum; i++)
	{
		auto CurrentObject = GetObjectByIndex(i);

		if (!CurrentObject)
			return nullptr;

		auto objectName = CurrentObject->GetFullName();

		if (bContains ? objectName.contains(ObjectName) : objectName == ObjectName)
			return CurrentObject;
	}

	return nullptr;
}

UObject* GetDefaultObject(UObject* Class);

template <typename ObjectType = UObject>
ObjectType* FindObject(const std::string& ObjectName, UObject* Class = nullptr, UObject* InOuter = nullptr); // Calls StaticFindObject






// OTHER

struct FURL
{
	FString                                     Protocol;                                                 // 0x0000(0x0010) (ZeroConstructor)
	FString                                     Host;                                                     // 0x0010(0x0010) (ZeroConstructor)
	int                                         Port;                                                     // 0x0020(0x0004) (ZeroConstructor, IsPlainOldData)
	int                                         Valid;                                                    // 0x0024(0x0004) (ZeroConstructor, IsPlainOldData)
	FString                                     Map;                                                      // 0x0028(0x0010) (ZeroConstructor)
	FString                                     RedirectUrl;                                              // 0x0038(0x0010) (ZeroConstructor)
	TArray<FString>                             Op;                                                       // 0x0048(0x0010) (ZeroConstructor)
	FString                                     Portal;                                                   // 0x0058(0x0010) (ZeroConstructor)
};