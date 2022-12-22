#pragma once

#include <Windows.h>
#include <string>
#include <locale>
#include <vector>

#include "enums.h"
#include "mem.h"

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

	inline ElementType& At(int i, int Size = sizeof(ElementType)) const { return *(ElementType*)(__int64(Data) + (static_cast<long long>(Size) * i)); }
	inline ElementType& at(int i, int Size = sizeof(ElementType)) const { return *(ElementType*)(__int64(Data) + (static_cast<long long>(Size) * i)); }
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

	int Add(ElementType* New, int Size = sizeof(ElementType))
	{
		Reserve(1, Size);

		if (Data)
		{
			memcpy_s((ElementType*)(__int64(Data) + (ArrayNum * Size)), Size, (void*)New, Size);
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

	bool RemoveAt(const int Index, int Size = sizeof(ElementType)) // NOT MINE
	{
		if (Index < ArrayNum)
		{
			if (Index != ArrayNum - 1)
			{
				// memcpy_s((ElementType*)(__int64(Data) + (Index * Size)), Size, (ElementType*)(__int64(Data) + ((ArrayNum - 1) * Size)), Size);
				// Data[Index] = Data[ArrayNum - 1];
				memcpy_s((void*)(ElementType*)(__int64(Data) + (Index * Size)), Size, (void*)(ElementType*)(__int64(Data) + ((ArrayNum - 1) * Size)), Size);
			}

			--ArrayNum;

			return true;
		}

		return false;

		/* if (Index < ArrayNum && Index != ArrayNum - 1)
		{
			memcpy_s((void*)((uint8_t*)(Data)+ Index * Size), Size, (void*)((uint8_t*)(Data)+ ((ArrayNum--) * Size)), Size);
			return true;
		} 

		return false; */
	};

	void Free()
	{
		if (Data && FMemory::Free)
			FMemory::Free(Data);

		Data = nullptr;

		ArrayNum = 0;
		ArrayMax = 0;
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

	std::string ToString();
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

	void* GetProperty(const std::string& MemberName, bool bIsSuperStruct = false, bool bPrint = false, bool bWarnIfNotFound = true);
	void* GetPropertySlow(const std::string& MemberName, bool bPrint = false, bool bWarnIfNotFound = true);

	int GetOffset(const std::string& MemberName, bool bIsSuperStruct = false, bool bPrint = false, bool bWarnIfNotFound = true);
	int GetOffsetSlow(const std::string& MemberName, bool bPrint = false, bool bWarnIfNotFound = true);

	bool IsA(UObject* otherClass);
};

struct UField : UObject { UField* Next; };

struct FFunctionStorage
{
	void* HeapAllocation;
#if TFUNCTION_USES_INLINE_STORAGE
	// Inline storage for an owned object
	TAlignedBytes<TFUNCTION_INLINE_SIZE, TFUNCTION_INLINE_ALIGNMENT> InlineAllocation;
#endif
};

template <bool bUnique>
struct TFunctionStorage : FFunctionStorage
{

};

template <typename StorageType, typename Ret, typename... ParamTypes>
struct TFunctionRefBase
{
	// Ret(*Callable)(void*, ParamTypes&...) = nullptr;
	void* Callable;
	StorageType Storage;
};

template<typename FuncType>
class TFunction : public TFunctionRefBase<TFunctionStorage<false>, FuncType>
{

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

struct FActorSpawnParametersNew
{
	FName Name;
	UObject* Template; // AActor*
	UObject* Owner; // AActor*
	UObject* Instigator; // APawn*
	UObject* OverrideLevel; // ULevel*
	UObject* OverrideParentComponent;
	ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingOverride;
	// ESpawnActorScaleMethod TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;
	uint8_t TransformScaleMethod;
	uint16_t	bRemoteOwned : 1;
	uint16_t	bNoFail : 1;
	uint16_t	bDeferConstruction : 1;
	uint16_t	bAllowDuringConstructionScript : 1;

	enum class ESpawnActorNameMode : uint8_t
	{
		Required_Fatal,
		Required_ErrorAndReturnNull,
		Required_ReturnNull,
		Requested
	};

	ESpawnActorNameMode NameMode;
	EObjectFlags ObjectFlags;

	TFunction<void(UObject*)> CustomPreSpawnInitalization; // my favorite
};

inline UObject* (*StaticFindObjectO)(UObject* Class, UObject* InOuter, const TCHAR* Name, bool ExactClass);
inline UObject* (*StaticLoadObjectO)(UObject* Class, UObject* InOuter, const TCHAR* Name, const TCHAR* Filename, uint32_t LoadFlags, UObject* Sandbox, bool bAllowObjectReconciliation, void* InSerializeContext);
inline void (*ProcessEventO)(UObject* object, UObject* func, void* Parameters);
inline UObject* (*SpawnActorO)(UObject* World, UObject* Class, void* Position, void* Rotation, void* SpawnParameters);
inline UObject* (*SpawnActorTransform)(UObject* World, UObject* Class, void* UserTransformPtr, void* SpawnParameters);
inline void (*ToStringO)(struct FName*, class FString&); // DONT UYSE

template <typename T = UObject>
static T* StaticLoadObject(UObject* Class, UObject* Outer, const std::string& name, int LoadFlags = 0)
{
	if (!StaticLoadObjectO)
		return nullptr;

	auto Object = FindObject<T>(name, Class);

	if (!Object)
	{
		auto Name = std::wstring(name.begin(), name.end()).c_str();
		Object = (T*)StaticLoadObjectO(Class, Outer, Name, nullptr, LoadFlags, nullptr, false, nullptr);
	}

	return Object;
}

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
			continue;

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
int FindOffsetStruct2(const std::string& StructName, const std::string& MemberName, bool bPrint = false, bool bContain = false, bool bWarnIfNotFound = true);

void* FindPropStruct2(const std::string& StructName, const std::string& MemberName, bool bPrint = false, bool bContain = false, bool bWarnIfNotFound = true);
UObject* LoadObject(UObject* Class, const std::string& Name);

int GetEnumValue(UObject* Enum, const std::string& EnumMemberName);

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

struct FWeakObjectPtr
{
public:
	inline bool SerialNumbersMatch(FUObjectItem* ObjectItem) const
	{
		return ObjectItem->SerialNumber == ObjectSerialNumber;
	}

	int32_t ObjectIndex;
	int32_t ObjectSerialNumber;
};

template<typename TObjectID>
class TPersistentObjectPtr
{
public:
	FWeakObjectPtr WeakPtr;
	int32_t TagAtLastTest;
	TObjectID ObjectID;
};

struct FSoftObjectPath
{
	FName AssetPathName;
	FString SubPathString;
};

class FSoftObjectPtr : public TPersistentObjectPtr<FSoftObjectPath>
{

};

struct TSoftObjectPtr : public FSoftObjectPtr
{
	UObject* Get(UObject* Class, bool bFindObject = false)
	{
		if (!ObjectID.AssetPathName.ComparisonIndex)
			return nullptr;

		auto objectName = ObjectID.AssetPathName.ToString();
		// std::cout << "objectName: " << objectName << '\n';

		return bFindObject ? FindObject(objectName, Class) : StaticLoadObject(Class, nullptr, objectName);
	}
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

	struct FFastArraySerializerSE // 264
	{
		// TMap<int32_t, int32_t> ItemMap;
		char ItemMap[0x50];

		int32_t IDCounter;
		int32_t ArrayReplicationKey;

		char GuidReferencesMap[0x50];
		char GuidReferencesMap_StructDelta[0x50];

		int32_t CachedNumItems;
		int32_t CachedNumItemsToConsiderForWriting;
		uint8_t DeltaFlags; // 256
		// structural padding here i guess 4
		int idkwhatthisis;

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

		void MarkAllItemsDirty() // This is my function, not ue.
		{

		}

		void MarkArrayDirty()
		{
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
		if (Fortnite_Version < 8.30)
			((FFastArraySerializerOL*)Array)->MarkArrayDirty();
		else
			((FFastArraySerializerSE*)Array)->MarkArrayDirty();
	}

	static void MarkItemDirty(void* Array, FFastArraySerializerItem* Item)
	{
		if (Fortnite_Version < 8.30)
			((FFastArraySerializerOL*)Array)->MarkItemDirty(Item);
		else
			((FFastArraySerializerSE*)Array)->MarkItemDirty(Item);
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

struct FGameplayTag
{
	static const int npos = -1;
	FName                                       TagName;                                                  // 0x0000(0x0008) (Edit, ZeroConstructor, EditConst, IsPlainOldData)

	bool operator==(const FGameplayTag& OtherTag) { return TagName.ComparisonIndex == OtherTag.TagName.ComparisonIndex;  }
};

struct FGameplayTagContainer
{
	TArray<FGameplayTag>                        GameplayTags;                                             // 0x0000(0x0010) (BlueprintVisible, ZeroConstructor)
	TArray<FGameplayTag>                        ParentTags;                                               // 0x0010(0x0010) (ZeroConstructor, Transient)

	std::string ToStringSimple(bool bQuoted) const
	{
		std::string RetString;
		for (int i = 0; i < GameplayTags.Num(); ++i)
		{
			if (bQuoted)
			{
				RetString += ("\"");
			}
			RetString += GameplayTags.At(i).TagName.ToString();
			if (bQuoted)
			{
				RetString += ("\"");
			}

			if (i < GameplayTags.Num() - 1)
			{
				RetString += (", ");
			}
		}
		return RetString;
	}

	int HasTag(const std::string& Str)
	{
		for (int i = 0; i < GameplayTags.Num(); i++)
		{
			if (GameplayTags.At(i).TagName.ToString() == Str)
				return i;
		}
		
		return FGameplayTag::npos;
	}

	bool HasTag(FGameplayTag& Tag)
	{
		return HasTag(Tag.TagName.ToString());
	}

	void Reset()
	{
		GameplayTags.Free();
		ParentTags.Free();
	}
};

struct FCurveTableRowHandle
{
	UObject* CurveTable;
	FName RowName;

	bool Eval(float InVal, float* OutVal)
	{
#ifdef balls
		// EvaluateCurveTableRow

		struct
		{
			FCurveTableRowHandle                  CurveTableRowHandle;                               // 0x0(0x10)(Parm, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			float                                        InXY;                                              // 0x10(0x4)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			float                                        OutXY;                                             // 0x14(0x4)(Parm, OutParm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			FString                                ContextString;                                     // 0x18(0x10)(Parm, ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			bool                                         ReturnValue;                                       // 0x28(0x1)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		} UFortKismetLibrary_EvaluateCurveTableRow_Params{*this, InVal};

		if (OutVal)
			*OutVal = UFortKismetLibrary_EvaluateCurveTableRow_Params.OutXY;

		static auto fn = FindObject<UFunction>("/Script/FortniteGame.FortKismetLibrary.EvaluateCurveTableRow");
		static auto FKLClass = FindObject("/Script/FortniteGame.Default__FortKismetLibrary");

		FKLClass->ProcessEvent(fn, &UFortKismetLibrary_EvaluateCurveTableRow_Params);

		return UFortKismetLibrary_EvaluateCurveTableRow_Params.ReturnValue;
#else
		struct
		{
		public:
			UObject* CurveTable;                                        // 0x0(0x8)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			FName                                  RowName;                                           // 0x8(0x8)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			float                                        InXY;                                              // 0x10(0x4)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			uint8_t         OutResult;                                         // 0x14(0x1)(Parm, OutParm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			float                                        OutXY;                                             // 0x18(0x4)(Parm, OutParm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			FString                                ContextString;                                     // 0x20(0x10)(Parm, ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		} UDataTableFunctionLibrary_EvaluateCurveTableRow_Params{CurveTable, RowName, InVal};

		static auto DTFL = FindObject("/Script/Engine.Default__DataTableFunctionLibrary");
		static auto fn = FindObject<UFunction>("/Script/Engine.DataTableFunctionLibrary.EvaluateCurveTableRow");

		DTFL->ProcessEvent(fn, &UDataTableFunctionLibrary_EvaluateCurveTableRow_Params);

		if (OutVal)
			*OutVal = UDataTableFunctionLibrary_EvaluateCurveTableRow_Params.OutXY;

		return true;
#endif

		bool (__fastcall* evalO)(FCurveTableRowHandle* CurveTableRowHandle, __int64 Unused, float* YValue, FString ContextString) = nullptr;

		evalO = decltype(evalO)(Memory::FindPattern("4C 8B DC 53 48 83 EC 70 49 8B D8 0F 29 74 24 ? 45 33 C0 48 8D 05 ? ? ? ? 44 38 05 ? ? ? ? 4C"));

		return evalO(this, 0, OutVal, FString());
	}
};


struct FScalableFloat
{
public:
	float                                        Value;                                             // 0x0(0x4)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int idk;                    // Fixing Size After Last Property  [ Dumper-7 ]
	FCurveTableRowHandle Curve; // struct FCurveTableRowHandle                  Curve;                                             // 0x8(0x10)(Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	void* idk2;                              // Fixing Size Of Struct [ Dumper-7 ]
};

struct FUniqueNetIdRepl
{
	unsigned char UnknownData00[0x1];
	unsigned char UnknownData01[0x17];
	TArray<unsigned char> ReplicationBytes;
};

struct FGameplayAttributeData
{
public:
	uint8_t                                        Pad_241C[0x8];                                     // Fixing Size After Last Property  [ Dumper-7 ]
	float                                        BaseValue;                                         // 0x8(0x4)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	float                                        CurrentValue;                                      // 0xC(0x4)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
};

struct FFortGameplayAttributeData : public FGameplayAttributeData
{
public:
	float                                        Minimum;                                           // 0x10(0x4)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	float                                        Maximum;                                           // 0x14(0x4)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	bool                                         bIsCurrentClamped;                                 // 0x18(0x1)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	bool                                         bIsBaseClamped;                                    // 0x19(0x1)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	bool                                         bShouldClampBase;                                  // 0x1A(0x1)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	uint8_t                                        Pad_3AF2[0x1];                                     // Fixing Size After Last Property  [ Dumper-7 ]
	float                                        UnclampedBaseValue;                                // 0x1C(0x4)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	float                                        UnclampedCurrentValue;                             // 0x20(0x4)(BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	uint8_t                                        Pad_3AF3[0x4];                                     // Fixing Size Of Struct [ Dumper-7 ]
};

template<class T = UObject, class TWeakObjectPtrBase = FWeakObjectPtr>
struct TWeakObjectPtr : public FWeakObjectPtr
{
public:
	inline T* Get() {
		return (T*)GetObjectByIndex(ObjectIndex);
	}

	TWeakObjectPtr(int32_t ObjectIndex)
	{
		this->ObjectIndex = ObjectIndex;
		this->ObjectSerialNumber = GetSerialNumber(GetByIndex<UObject>(ObjectIndex));
	}

	TWeakObjectPtr(UObject* Obj)
	{
		this->ObjectIndex = Obj->InternalIndex;
		this->ObjectSerialNumber = GetSerialNumber(GetByIndex<UObject>(Obj->InternalIndex));
	}

	TWeakObjectPtr()
	{
	}
};

struct BothVector
{
	FVector fV;
	DVector dV;

	BothVector(float X, float Y, float Z)
	{
		fV = FVector(X, Y, Z);
	}

	BothVector(FVector vec)
	{
		fV = vec;
	}

	BothVector(DVector vec)
	{
		dV = vec;
	}

	BothVector(double X, double Y, double Z)
	{
		dV = DVector(X, Y, Z);
	}

	BothVector() {}

	BothVector operator+(const BothVector& otherVec)
	{
		return Fortnite_Season < 20 ? BothVector(fV.X + otherVec.fV.X, fV.Y + otherVec.fV.Y, fV.Z + otherVec.fV.Z) :
			BothVector(dV.X + otherVec.dV.X, dV.Y + otherVec.dV.Y, dV.Z + otherVec.dV.Z);
	}
};

struct BothRotator
{
	FRotator fR = FRotator();
	DRotator dR = DRotator();

	BothRotator(float X, float Y, float Z)
	{
		fR = FRotator(X, Y, Z);
	}

	BothRotator(FRotator rot)
	{
		fR = rot;
	}

	BothRotator(DRotator rot)
	{
		dR = rot;
	}

	BothRotator(double X, double Y, double Z)
	{
		dR = DRotator(X, Y, Z);
	}

	BothRotator() {}
};

struct ActorSpawnStruct
{
	UObject* ClassOfClass;
	std::string ClassToSpawn;
	BothVector SpawnLocation;
	BothRotator SpawnRotation;
	// std::function<void(UObject*)> OnSpawned;
};

struct FGameplayEffectContextHandle
{
	char UKD_00[0x30];
};

struct FActiveGameplayEffectHandle
{
	int                                                Handle;                                                   // 0x0000(0x0004) (ZeroConstructor, IsPlainOldData)
	bool                                               bPassedFiltersAndWasExecuted;                             // 0x0004(0x0001) (ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData00[0x3];                                       // 0x0005(0x0003) MISSED OFFSET
};


inline uint8_t GetFieldMask(void* Property)
{
	if (!Property)
		return -1;

	// 3 = sizeof(FieldSize) + sizeof(ByteOffset) + sizeof(ByteMask)

	if (Engine_Version <= 420)
		return *(uint8_t*)(__int64(Property) + (112 + 3));
	else if (Engine_Version >= 421 && Engine_Version <= 424)
		return *(uint8_t*)(__int64(Property) + (112 + 3));
	else if (Engine_Version >= 425)
		return *(uint8_t*)(__int64(Property) + (120 + 3));

	return -1;
}

inline bool ReadBitfield(void* Addr, uint8_t FieldMask)
{
	auto Bitfield = (PlaceholderBitfield*)Addr;

	// niceeeee

	if (FieldMask == 0x1)
		return Bitfield->First;
	else if (FieldMask == 0x2)
		return Bitfield->Second;
	else if (FieldMask == 0x4)
		return Bitfield->Third;
	else if (FieldMask == 0x8)
		return Bitfield->Fourth;
	else if (FieldMask == 0x10)
		return Bitfield->Fifth;
	else if (FieldMask == 0x20)
		return Bitfield->Sixth;
	else if (FieldMask == 0x40)
		return Bitfield->Seventh;
	else if (FieldMask == 0x80)
		return Bitfield->Eighth;
	else if (FieldMask == 0xFF)
		return *(bool*)Bitfield;

	return false;
}

inline void SetBitfield(void* Addr, uint8_t FieldMask, bool NewVal)
{
	auto Bitfield = (PlaceholderBitfield*)Addr;

	// niceeeee

	if (FieldMask == 0x1)
		Bitfield->First = NewVal;
	else if (FieldMask == 0x2)
		Bitfield->Second = NewVal;
	else if (FieldMask == 0x4)
		Bitfield->Third = NewVal;
	else if (FieldMask == 0x8)
		Bitfield->Fourth = NewVal;
	else if (FieldMask == 0x10)
		Bitfield->Fifth = NewVal;
	else if (FieldMask == 0x20)
		Bitfield->Sixth = NewVal;
	else if (FieldMask == 0x40)
		Bitfield->Seventh = NewVal;
	else if (FieldMask == 0x80)
		Bitfield->Eighth = NewVal;
	else if (FieldMask == 0xFF)
		*(bool*)Bitfield = NewVal;
}

struct FFortItemEntryStateValue
{
public:
	int* GetIntValue()
	{
		static auto IntValueOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortItemEntryStateValue", "IntValue");
		return (int*)(__int64(this) + IntValueOffset);
	}

	uint8_t* GetStateType()
	{
		static auto StateTypeOffset = FindOffsetStruct2("ScriptStruct /Script/FortniteGame.FortItemEntryStateValue", "StateType");
		return (uint8_t*)(__int64(this) + StateTypeOffset);
	}
};