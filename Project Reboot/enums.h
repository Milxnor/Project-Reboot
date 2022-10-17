#pragma once

#include <string>
#include <format>
#include <iostream>
#include <random>
#include <unordered_set>

#define _USE_MATH_DEFINES
#include <math.h>

// TODO Better file name
// This is more like a utility/random stuff file

struct FVector
{
	float X;
	float Y;
	float Z;

	FVector() : X(0), Y(0), Z(0) {}
	FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}
	FVector(int x, int y, int z) : X(x), Y(y), Z(z) {}

	FVector operator+(const FVector& A)
	{
		return FVector{ this->X + A.X, this->Y + A.Y, this->Z + A.Z };
	}

	FVector operator-(const FVector& A)
	{
		return FVector{ this->X - A.X, this->Y - A.Y, this->Z - A.Z };
	}

	FVector operator*(const float A)
	{
		return FVector{ this->X * A, this->Y * A, this->Z * A };
	}

	bool operator==(const FVector& A)
	{
		return X == A.X && Y == A.Y && Z == A.Z;
	}

	void operator+=(const FVector& A)
	{
		*this = *this + A;
	}

	void operator-=(const FVector& A)
	{
		*this = *this - A;
	}

	std::string Describe()
	{
		return std::format("{} {} {}", std::to_string(X), std::to_string(Y), std::to_string(Z));
	}
};

struct FRotator
{
	float Pitch;
	float Yaw;
	float Roll;

	bool operator==(const FRotator& A)
	{
		return Yaw == A.Yaw && Pitch == A.Pitch && Roll == A.Roll;
	}
};

enum ESpawnActorCollisionHandlingMethod
{
	Undefined,
	AlwaysSpawn,
	AdjustIfPossibleButAlwaysSpawn,
	AdjustIfPossibleButDontSpawnIfColliding,
	DontSpawnIfColliding,
};

enum EObjectFlags
{
	RF_NoFlags = 0x00000000,
	RF_Public = 0x00000001,
	RF_Standalone = 0x00000002,
	RF_MarkAsNative = 0x00000004,
	RF_Transactional = 0x00000008,
	RF_ClassDefaultObject = 0x00000010,
	RF_ArchetypeObject = 0x00000020,
	RF_Transient = 0x00000040,
	RF_MarkAsRootSet = 0x00000080,
	RF_TagGarbageTemp = 0x00000100,
	RF_NeedInitialization = 0x00000200,
	RF_NeedLoad = 0x00000400,
	RF_KeepForCooker = 0x00000800,
	RF_NeedPostLoad = 0x00001000,
	RF_NeedPostLoadSubobjects = 0x00002000,
	RF_NewerVersionExists = 0x00004000,
	RF_BeginDestroyed = 0x00008000,
	RF_FinishDestroyed = 0x00010000,
	RF_BeingRegenerated = 0x00020000,
	RF_DefaultSubObject = 0x00040000,
	RF_WasLoaded = 0x00080000,
	RF_TextExportTransient = 0x00100000,
	RF_LoadCompleted = 0x00200000,
	RF_InheritableComponentTemplate = 0x00400000,
	RF_DuplicateTransient = 0x00800000,
	RF_StrongRefOnFrame = 0x01000000,
	RF_NonPIEDuplicateTransient = 0x02000000,
	RF_Dynamic = 0x04000000,
	RF_WillBeLoaded = 0x08000000,
	RF_HasExternalPackage = 0x10000000,
};

struct FText
{
	char UnknownData[0x18];
};

enum class EAthenaGamePhase : uint8_t
{
	None = 0,
	Setup = 1,
	Warmup = 2,
	Aircraft = 3,
	SafeZones = 4,
	EndGame = 5,
	Count = 6,
	EAthenaGamePhase_MAX = 7
};

enum class EFortCustomPartType : uint8_t
{
	Head = 0,
	Body = 1,
	Hat = 2,
	Backpack = 3,
	Charm = 4,
	Face = 5,
	NumTypes = 6,
	EFortCustomPartType_MAX = 7
};

template<class TEnum>
struct TEnumAsByte
{
	uint8_t Value;

	TEnumAsByte(TEnum _value)
		: Value((uint8_t)_value)
	{
	}

	TEnumAsByte() : Value(0) {}

	TEnum Get()
	{
		return (TEnum)Value;
	}
};

struct FFastArraySerializerItem
{
	int                                                ReplicationID;                                            // 0x0000(0x0004) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
	int                                                ReplicationKey;                                           // 0x0004(0x0004) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
	int                                                MostRecentArrayReplicationKey;                            // 0x0008(0x0004) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
};

enum class EFortQuickBars : uint8_t // This isn't always correct due to them adding Creative Quickbars but for our usage it's fine.
{
	Primary = 0,
	Secondary = 1,
	Max_None = 2,
	EFortQuickBars_MAX = 3
};

struct FGuid
{
	unsigned int A;
	unsigned int B;
	unsigned int C;
	unsigned int D;

	bool operator==(const FGuid& other)
	{
		return A == other.A && B == other.B && C == other.C && D == other.D;
	}

	void Describe()
	{
		std::cout << std::format("{} {} {} {}\n", A, B, C, D);
	}
};

struct PadHex18 { char Pad[0x18]; };
struct PadHex10 { char Pad[0x10]; };
struct PadHexC8 { char Pad[0xC8]; };
struct PadHexE0 { char Pad[0xE0]; };

struct FGameplayAbilitySpecHandle
{
	int Handle;

	void GenerateNewHandle() // weird
	{
		// Must be in C++ to avoid duplicate statics across execution units
		/* static int32_t GHandle = 1;
		Handle = GHandle++; */

		static std::unordered_set<int> Handles;

		auto newHandle = rand();

		while (Handles.find(newHandle) != Handles.end())
		{
			newHandle = rand();
		}

		Handle = newHandle;
		Handles.emplace(Handle);
	}
};

enum class EFortResourceType : uint8_t
{
	Wood = 0,
	Stone = 1,
	Metal = 2,
	Permanite = 3,
	None = 4,
	EFortResourceType_MAX = 5
};

struct PlaceholderBitfield
{
	uint8_t First : 1;
	uint8_t Second : 1;
	uint8_t Third : 1;
	uint8_t Fourth : 1;
	uint8_t Fifth : 1;
	uint8_t Sixth : 1;
	uint8_t Seventh : 1;
	uint8_t Eighth : 1;
};

// i dont think it can ever actually be max
static double GetRandomDouble(float Min, float Max)
{
	std::random_device rd; 
	std::mt19937 gen(rd());

	std::uniform_real_distribution<> distr(Min, Max);

	return distr(gen);
}

static float GetRandomFloat(float Min, float Max) // bruh
{
	return GetRandomDouble(Min, Max);
}

static bool IsBadReadPtr(void* p)
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	if (::VirtualQuery(p, &mbi, sizeof(mbi)))
	{
		DWORD mask = (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
		bool b = !(mbi.Protect & mask);
		// check the page is not a guard page
		if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) b = true;

		return b;
	}
	return true;
}

enum class EFortPickupSourceTypeFlag : uint8_t
{
	Other = 0,
	Player = 1,
	Destruction = 2,
	Container = 3,
	AI = 4,
	Tossed = 5,
	FloorLoot = 6,
	EFortPickupSourceTypeFlag_MAX = 7
};

enum class EFortPickupSpawnSource : uint8_t
{
	Unset = 0,
	PlayerElimination = 1,
	Chest = 2,
	SupplyDrop = 3,
	AmmoBox = 4,
	EFortPickupSpawnSource_MAX = 5
};

struct FQuat
{
	float W;
	float X;
	float Y;
	float Z;
};

struct FTransform // https://github.com/EpicGames/UnrealEngine/blob/c3caf7b6bf12ae4c8e09b606f10a09776b4d1f38/Engine/Source/Runtime/Core/Public/Math/TransformNonVectorized.h#L28
{
	FQuat Rotation;
	FVector Translation;
	char pad_1C[0x4]; // Padding never changes
	FVector Scale3D = FVector{ 1, 1, 1 };
	char pad_2C[0x4];

	/* bool ContainsNaN() const
	{
		return (Translation.ContainsNaN() || Rotation.ContainsNaN() || Scale3D.ContainsNaN());
	} */
};

static FORCEINLINE void SinCos(float* ScalarSin, float* ScalarCos, float  Value)
{
	// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
	float quotient = (0.31830988618f * 0.5f) * Value;
	if (Value >= 0.0f)
	{
		quotient = (float)((int)(quotient + 0.5f));
	}
	else
	{
		quotient = (float)((int)(quotient - 0.5f));
	}
	float y = Value - (2.0f * M_PI) * quotient;

	// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
	float sign;
	if (y > 1.57079632679f)
	{
		y = M_PI - y;
		sign = -1.0f;
	}
	else if (y < -1.57079632679f)
	{
		y = -M_PI - y;
		sign = -1.0f;
	}
	else
	{
		sign = +1.0f;
	}

	float y2 = y * y;

	// 11-degree minimax approximation
	*ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

	// 10-degree minimax approximation
	float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
	*ScalarCos = sign * p;
}

template <class  T>
static auto DegreesToRadians(T const& DegVal) -> decltype(DegVal* (M_PI / 180.f))
{
	return DegVal * (M_PI / 180.f);
}
