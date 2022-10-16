#pragma once

#include <Windows.h>
#include <string>
#include <locale>
#include <vector>

#include "enums.h"

template <typename T>
T* Get(void* addr, uint64_t off) { return (T*)(__int64(addr) + off); }

inline double Fortnite_Version;
inline int Engine_Version; // NOTE: 427 == 4.26.1
inline int Fortnite_Season;
inline int Offset_InternalOffset = 0x0;
inline int SuperStructOffset = 0x0;
inline int ChildPropertiesOffset = 0x0;
inline int PropertiesSizeOffset = 0x0;
inline int ServerReplicateActorsOffset = 0x0;

namespace FMemory
{
	inline void (*Free)(void* Original);
	inline void* (*Realloc)(void* Original, SIZE_T Count, uint32_t Alignment /* = DEFAULT_ALIGNMENT */);
}

template <class ElementType>
struct TArray
{
	ElementType* Data = nullptr;
	int32_t ArrayNum = 0;
	int32_t ArrayMax = 0;

	inline ElementType At(int i, int Size = sizeof(ElementType)) const { return *(ElementType*)(__int64(Data) + (static_cast<long long>(Size) * i)); }
	inline ElementType at(int i, int Size = sizeof(ElementType)) const { return *(ElementType*)(__int64(Data) + (static_cast<long long>(Size) * i)); }
	inline ElementType* AtPtr(int i, int Size = sizeof(ElementType)) const { return (ElementType*)(__int64(Data) + (static_cast<long long>(Size) * i)); }

	inline int Num() const { return ArrayNum; }
	inline int size() const { return ArrayNum; }

	void Reserve(int Number, int Size = sizeof(ElementType))
	{
		Data = (ElementType*)FMemory::Realloc(Data, (ArrayMax = ArrayNum + Number) * Size, 0);
	}

	int Add(const ElementType& New, int Size = sizeof(ElementType))
	{
		Reserve(1, Size);

		if (Data)
		{
			memcpy_s((ElementType*)(__int64(Data) + (ArrayNum * Size)), Size, (void*)&New, Size);
			++ArrayNum;
			return ArrayNum; // - 1;
		}

		return -1;
	};

	std::vector<ElementType> ToVector()
	{
		std::vector<ElementType> vector;

		for (int i = 0; i < ArrayNum; i++)
			vector.push_back(this->At(i));

		return vector;
	}

	inline bool RemoveAt(const int Index) // , int Size = sizeof(ElementType)) // NOT MINE
	{
		if (Index < ArrayNum)
		{
			if (Index != ArrayNum - 1)
			{
				// memcpy_s((ElementType*)(__int64(Data) + (Index * Size)), Size, (ElementType*)(__int64(Data) + ((ArrayNum - 1) * Size)), Size);
				Data[Index] = Data[ArrayNum - 1];
			}

			--ArrayNum;

			return true;
		}

		return false;
	};

	void Free()
	{
		// if (Data)
			VirtualFree(this, 0, MEM_RELEASE);

		// ArrayNum = 0;
		// ArrayMax = 0;
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

	void Free()
	{
		Data.Free();
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

	void ProcessEvent(struct UFunction* Function, void* Parameters = nullptr);

	int GetOffset(const std::string& MemberName, bool bIsSuperStruct = false);
	int GetOffsetSlow(const std::string& MemberName);

	bool IsA(UObject* otherClass);
};

struct UField : UObject { UField* Next; };

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

struct UFunction : UObject {};

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

int FindOffsetStruct(const std::string& StructName, const std::string& MemberName, bool bExactStruct = false);




// OTHER

template<typename ElementType>
union TSparseArrayElementOrFreeListLink
{
	/** If the element is allocated, its value is stored here. */
	ElementType ElementData;

	struct
	{
		/** If the element isn't allocated, this is a link to the previous element in the array's free list. */
		int32_t PrevFreeIndex;

		/** If the element isn't allocated, this is a link to the next element in the array's free list. */
		int32_t NextFreeIndex;
	};
};

template <typename ElementType>
union TSparseArrayElementOrListLink
{
	TSparseArrayElementOrListLink(ElementType& InElement)
		: ElementData(InElement)
	{
	}
	TSparseArrayElementOrListLink(ElementType&& InElement)
		: ElementData(InElement)
	{
	}

	TSparseArrayElementOrListLink(int32_t InPrevFree, int32_t InNextFree)
		: PrevFreeIndex(InPrevFree)
		, NextFreeIndex(InNextFree)
	{
	}

	TSparseArrayElementOrListLink<ElementType> operator=(const TSparseArrayElementOrListLink<ElementType>& Other)
	{
		return TSparseArrayElementOrListLink(Other.NextFreeIndex, Other.PrevFreeIndex);
	}

	/** If the element is allocated, its value is stored here. */
	ElementType ElementData;

	struct
	{
		/** If the element isn't allocated, this is a link to the previous element in the array's free list. */
		int32_t PrevFreeIndex;

		/** If the element isn't allocated, this is a link to the next element in the array's free list. */
		int32_t NextFreeIndex;
	};
};

template <int32_t NumElements>
struct TInlineAllocator
{
	template <int32_t Size, int32_t Alignment>
	struct alignas(Alignment) TAlligendBytes
	{
		uint8_t Pad[Size];
	};

	template <typename ElementType>
	struct TTypeCompatibleBytes : public TAlligendBytes<sizeof(ElementType), alignof(ElementType)>
	{
	};

	template <typename ElementType>
	class ForElementType
	{
		friend class TBitArray;

	private:
		TTypeCompatibleBytes<ElementType> InlineData[NumElements];

		ElementType* SecondaryData;
	};
};;

class TBitArray
{
private:
	TInlineAllocator<4>::ForElementType<uint32_t> Data;
	int32_t NumBits;
	int32_t MaxBits;
};

template<typename InElementType>//, typename Allocator /*= FDefaultSparseArrayAllocator */>
class TSparseArray
{
public:
	typedef TSparseArrayElementOrListLink<InElementType> FSparseArrayElement;

	TArray<FSparseArrayElement> Data;
	TBitArray AllocationFlags;
	int32_t FirstFreeIndex;
	int32_t NumFreeIndices;
};

class FSetElementId { int32_t Index; };

template<typename InElementType>//, bool bTypeLayout>
class TSetElementBase
{
public:
	typedef InElementType ElementType;

	/** The element's value. */
	ElementType Value;

	/** The id of the next element in the same hash bucket. */
	mutable FSetElementId HashNextId;

	/** The hash bucket that the element is currently linked to. */
	mutable int32_t HashIndex;
};

template <typename InElementType>
class TSetElement : public TSetElementBase<InElementType>//, THasTypeLayout<InElementType>::Value>
{

};

template<
	typename InElementType//, typename KeyFuncs /*= DefaultKeyFuncs<ElementType>*/ //, typename Allocator /*= FDefaultSetAllocator*/
>
class TSet
{
public:
	typedef TSetElement<InElementType> SetElementType;

	typedef TSparseArray<SetElementType/*, typename Allocator::SparseArrayAllocator*/>     ElementArrayType;
	// typedef typename Allocator::HashAllocator::template ForElementType<FSetElementId> HashType;
	typedef int32_t HashType;

	ElementArrayType Elements;

	mutable HashType Hash;
	mutable int32_t	 HashSize;
};

template <typename KeyType, typename ValueType>
class TPair // this is a very simplified version fo tpair when in reality its a ttuple and a ttuple has a base and stuff but this works
{
public:
	KeyType First;
	ValueType Second;

	TPair(KeyType Key, ValueType Value)
		: First(Key)
		, Second(Value)
	{
	}

	inline KeyType& Key()
	{
		return First;
	}
	inline const KeyType& Key() const
	{
		return First;
	}
	inline ValueType& Value()
	{
		return Second;
	}
	inline const ValueType& Value() const
	{
		return Second;
	}
};

template<typename KeyType, typename ValueType> // , typename SetAllocator, typename KeyFuncs>
struct TMap
{
	typedef TPair<KeyType, ValueType> ElementType;

	typedef TSet<ElementType/*, KeyFuncs, SetAllocator */> ElementSetType;

	/** A set of the key-value pairs in the map. */
	ElementSetType Pairs;
};

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

namespace FastTArray
{
	inline UObject* FastArraySerializerStruct = nullptr;

	struct FFastArraySerializerOL
	{
		char ItemMap[0x50];
		int32_t IDCounter;
		int32_t ArrayReplicationKey;

		char GuidReferencesMap[0x50];

		int32_t CachedNumItems;
		int32_t CachedNumItemsToConsiderForWriting;

		void MarkItemDirty(FFastArraySerializerItem* Item)
		{
			if (Item->ReplicationID == -1)
			{
				Item->ReplicationID = ++IDCounter;
				if (IDCounter == -1)
					IDCounter++;
			}

			Item->ReplicationKey++;
			MarkArrayDirty();
		}

		void MarkArrayDirty()
		{
			// ItemMap.Reset();		// This allows to clients to add predictive elements to arrays without affecting replication.
			IncrementArrayReplicationKey();

			// Invalidate the cached item counts so that they're recomputed during the next write
			CachedNumItems = -1;
			CachedNumItemsToConsiderForWriting = -1;
		}

		void IncrementArrayReplicationKey()
		{
			ArrayReplicationKey++;

			if (ArrayReplicationKey == -1)
				ArrayReplicationKey++;
		}
	};

	static void MarkArrayDirty(void* Array)
	{
		((FFastArraySerializerOL*)Array)->MarkArrayDirty();
	}

	static void MarkItemDirty(void* Array, FFastArraySerializerItem* Item)
	{
		((FFastArraySerializerOL*)Array)->MarkItemDirty(Item);
	}

	/* inline void IncrementArrayReplicationKey(void* Array)
	{
		static auto ArrayReplicationKeyOffset = FastArraySerializerStruct->GetOffset("ArrayReplicationKey", true);

		auto ArrayReplicationKey = Get<int>(Array, ArrayReplicationKeyOffset);

		(*ArrayReplicationKey)++;

		if (*ArrayReplicationKey == -1)
			(*ArrayReplicationKey)++;
	}

	inline void MarkArrayDirty(void* Array)
	{
		// ItemMap.Reset();		// This allows to clients to add predictive elements to arrays without affecting replication.
		IncrementArrayReplicationKey(Array);

		// Invalidate the cached item counts so that they're recomputed during the next write
		static auto CachedNumItemsOffset = FastArraySerializerStruct->GetOffset("CachedNumItems", true);
		*Get<int>(Array, CachedNumItemsOffset) = -1;

		static auto CachedNumItemsToConsiderForWritingOffset = FastArraySerializerStruct->GetOffset("CachedNumItemsToConsiderForWriting", true);
		*Get<int>(Array, CachedNumItemsToConsiderForWritingOffset) = -1;
	}

	inline void MarkItemDirty(void* Array, FFastArraySerializerItem* Item)
	{
		static auto IDCounterOffset = FastArraySerializerStruct->GetOffset("IDCounter", true);

		if (Item->ReplicationID == -1)
		{
			auto IDCounter = Get<int>(Array, IDCounterOffset);
			Item->ReplicationID = ++(*IDCounter);

			if (*IDCounter == -1)
				(*IDCounter)++;
		}

		Item->ReplicationKey++;
		MarkArrayDirty(Array);
	} */
}