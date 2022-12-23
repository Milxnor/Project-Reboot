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

struct DVector // lmao
{
	double X;
	double Y;
	double Z;

	DVector operator*(const double A)
	{
		return DVector{ this->X * A, this->Y * A, this->Z * A };
	}

	DVector operator+(const DVector& A)
	{
		return DVector{ this->X + A.X, this->Y + A.Y, this->Z + A.Z };
	}
};

struct FColor
{
	uint8_t B;
	uint8_t G;
	uint8_t R;
	uint8_t A;
};

template< class T >
constexpr FORCEINLINE T Clamp(const T X, const T Min, const T Max)
{
	return (X < Min) ? Min : (X < Max) ? X : Max;
}

struct FLinearColor
{
public:
	float                                        R;                                                 // 0x0(0x4)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                        G;                                                 // 0x4(0x4)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                        B;                                                 // 0x8(0x4)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                        A;                                                 // 0xC(0x4)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)

	FColor ToFColor(bool sRGB) // https://github.com/FabianFG/CUE4Parse/blob/cc744cfb54e219d7f5d9440d30c4cd55b56afb3c/CUE4Parse/UE4/Objects/Core/Math/FLinearColor.cs#L20
	{
		float floatR = Clamp(R, 0.0f, 1.0f);
		float floatG = Clamp(G, 0.0f, 1.0f);
		float floatB = Clamp(B, 0.0f, 1.0f);
		float floatA = Clamp(A, 0.0f, 1.0f);

		if (sRGB)
		{
			floatR = floatR <= 0.0031308f ? floatR * 12.92f : pow(floatR, 1.0f / 2.4f) * 1.055f - 0.055f;
			floatG = floatG <= 0.0031308f ? floatG * 12.92f : pow(floatG, 1.0f / 2.4f) * 1.055f - 0.055f;
			floatB = floatB <= 0.0031308f ? floatB * 12.92f : pow(floatB, 1.0f / 2.4f) * 1.055f - 0.055f;
		}

		int intA = std::floor(floatA * 255.999f);
		int intR = std::floor(floatR * 255.999f);
		int intG = std::floor(floatG * 255.999f);
		int intB = std::floor(floatB * 255.999f);

		return FColor((uint8_t)intR, (uint8_t)intG, (uint8_t)intB, (uint8_t)intA);
	}

	std::string Describe()
	{
		return std::format("{} {} {} {}", std::to_string(R), std::to_string(G), std::to_string(B), std::to_string(A));
	}
};

enum class EFortItemTier : uint8_t
{
	No_Tier = 0,
	I = 1,
	II = 2,
	III = 3,
	IV = 4,
	V = 5,
	VI = 6,
	VII = 7,
	VIII = 8,
	IX = 9,
	X = 10,
	NumItemTierValues = 11,
	EFortItemTier_MAX = 12,
};

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

	DVector ToDouble()
	{
		DVector newVec = DVector(X, Y, Z); // ??
		return newVec;
	}
};

enum class EInteractionBeingAttempted : uint8_t
{
	FirstInteraction = 0,
	SecondInteraction = 1,
	AllInteraction = 2,
	EInteractionBeingAttempted_MAX = 3
};

static float UE_Fmod(float X, float Y)
{
	const float AbsY = fabs(Y);
	if (AbsY <= 1.e-8f)
	{
		// FmodReportError(X, Y);
		return 0.0;
	}

	// Convert to double for better precision, since intermediate rounding can lose enough precision to skew the result.
	const double DX = double(X);
	const double DY = double(Y);

	const double Div = (DX / DY);
	const double IntPortion = DY * trunc(Div);
	const double Result = DX - IntPortion;
	// Convert back to float. This is safe because the result will by definition not exceed the X input.
	return float(Result);
}

enum class EFortWeaponUpgradeCosts : uint8_t
{
	NotSet = 0,
	WoodUncommon = 1,
	WoodRare = 2,
	WoodVeryRare = 3,
	WoodSuperRare = 4,
	MetalUncommon = 5,
	MetalRare = 6,
	MetalVeryRare = 7,
	MetalSuperRare = 8,
	BrickUncommon = 9,
	BrickRare = 10,
	BrickVeryRare = 11,
	BrickSuperRare = 12,
	HorizontalWoodCommon = 13,
	HorizontalWoodUncommon = 14,
	HorizontalWoodRare = 15,
	HorizontalWoodVeryRare = 16,
	HorizontalWoodSuperRare = 17,
	HorizontalMetalCommon = 18,
	HorizontalMetalUncommon = 19,
	HorizontalMetalRare = 20,
	HorizontalMetalVeryRare = 21,
	HorizontalMetalSuperRare = 22,
	HorizontalBrickCommon = 23,
	HorizontalBrickUncommon = 24,
	HorizontalBrickRare = 25,
	HorizontalBrickVeryRare = 26,
	HorizontalBrickSuperRare = 27,
	EFortWeaponUpgradeCosts_MAX = 28,
};

enum class EChannelCloseReason : uint8_t
{
	Destroyed,
	Dormancy,
	LevelUnloaded,
	Relevancy,
	TearOff,
	/* reserved */
	MAX = 15		// this value is used for serialization, modifying it may require a network version change
};

template <class  T>
static auto DegreesToRadians(T const& DegVal) -> decltype(DegVal* (M_PI / 180.f))
{
	return DegVal * (M_PI / 180.f);
}

enum ENetRole
{
	ROLE_None,
	ROLE_SimulatedProxy,
	ROLE_AutonomousProxy,
	ROLE_Authority,
	ROLE_MAX,
};

#define CHECK_PATTERN(addr) if (!addr) \
{ \
    MessageBoxA(0, (std::string("Unable to find ") + #addr + " aborting..").c_str(), "Project Reboot", MB_ICONERROR);\
    FreeLibraryAndExitThread(GetModuleHandleW(0), 0); \
} \

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

struct FRotator
{
	float Pitch;
	float Yaw;
	float Roll;

	bool operator==(const FRotator& A)
	{
		return Yaw == A.Yaw && Pitch == A.Pitch && Roll == A.Roll;
	}

	void Describe() { std::cout << std::format("{} {} {}\n", Pitch, Yaw, Roll); }

	inline struct FQuat Quaternion() const;

	static __forceinline float ClampAxis(float Angle)
	{
		// returns Angle in the range (-360,360)
		Angle = UE_Fmod(Angle, 360.f);

		if (Angle < 0.f)
		{
			// shift to [0,360) range
			Angle += 360.f;
		}

		return Angle;
	}

	static __forceinline float NormalizeAxis(float Angle)
	{
		// returns Angle in the range [0,360)
		Angle = ClampAxis(Angle);

		if (Angle > 180.f)
		{
			// shift to (-180,180]
			Angle -= 360.f;
		}

		return Angle;
	}

	FVector Vector() const
	{
		float CP, SP, CY, SY;
		SinCos(&SP, &CP, DegreesToRadians(Pitch));
		SinCos(&SY, &CY, DegreesToRadians(Yaw));
		FVector V = FVector(CP * CY, CP * SY, SP);

		return V;
	}
};

struct DRotator // lmao
{
	double Pitch;
	double Yaw;
	double Roll;
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

enum class EFortCustomBodyType : uint8_t
{
	Small = 0,
	Medium = 1,
	MediumAndSmall = 2,
	Large = 3,
	LargeAndSmall = 4,
	LargeAndMedium = 5,
	All = 6,
	Deprecated = 7,
	EFortCustomBodyType_MAX = 8
};

// Enum FortniteGame.EFortCustomGender
enum class EFortCustomGender : uint8_t
{
	Invalid = 0,
	Male = 1,
	Female = 2,
	Both = 3,
	EFortCustomGender_MAX = 4
};

enum class EReachLocationValidationMode : uint8_t
{
	None = 0,
	Storm = 1,
	Leash = 2,
	SoftLeash = 3,
	EReachLocationValidationMode_MAX = 4
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

enum class EFortSharedAnimationState : uint8_t
{
	Anim_Walk = 0,
	Anim_Run = 1,
	Anim_Turn = 2,
	Anim_Attack = 3,
	Anim_Death = 4,
	Anim_Knockback = 5,
	Anim_FullBodyHit = 6,
	Anim_Pushed = 7,
	Anim_Dance = 8,
	Anim_Idle = 9,
	Anim_RangedAttack = 10,
	Anim_MAX = 11
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

struct FAthenaMatchTeamStats
{
	int                                                Place;                                                    // 0x0000(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int                                                TotalPlayers;                                             // 0x0004(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

enum EChannelType
{
	CHTYPE_None = 0,  // Invalid type.
	CHTYPE_Control = 1,  // Connection control.
	CHTYPE_Actor = 2,  // Actor-update channel.

	// @todo: Remove and reassign number to CHTYPE_Voice (breaks net compatibility)
	CHTYPE_File = 3,  // Binary file transfer.

	CHTYPE_Voice = 4,  // VoIP data channel
	CHTYPE_MAX = 8,  // Maximum.
};

enum class EChannelCreateFlags : uint32_t
{
	None = (1 << 0),
	OpenedLocally = (1 << 1)
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

	FFastArraySerializerItem()
	{
		ReplicationID = -1;
		ReplicationKey = -1;
		MostRecentArrayReplicationKey = -1;
	}
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

enum class ESetChannelActorFlags : uint32_t
{
	None = 0,
	SkipReplicatorCreation = (1 << 0),
	SkipMarkActive = (1 << 1),
};

struct PadHex18 { char Pad[0x18]; };
struct PadHex10 { char Pad[0x10]; };
struct PadHex78 { char Pad[0x78]; };
struct PadHexC0 { char Pad[0xC0]; };
struct PadHexC8 { char Pad[0xC8]; };
struct PadHexE0 { char Pad[0xE0]; };
struct PadHexE8 { char Pad[0xE8]; };

struct FSimpleCurveKey
{
public:
	float                                        Time;                                              // 0x0(0x4)(Edit, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                        Value;                                             // 0x4(0x4)(Edit, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

struct FRichCurveKey
{
	// TEnumAsByte<ERichCurveInterpMode>                  InterpMode;                                               // 0x0000(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	// TEnumAsByte<ERichCurveTangentMode>                 TangentMode;                                              // 0x0001(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	// TEnumAsByte<ERichCurveTangentWeightMode>           TangentWeightMode;                                        // 0x0002(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	unsigned char                                      InterpMode;                                       // 0x0003(0x0001) MISSED OFFSET
	unsigned char                                      TangentMode;                                       // 0x0003(0x0001) MISSED OFFSET
	unsigned char                                      TangentWeightMode;                                       // 0x0003(0x0001) MISSED OFFSET
	unsigned char                                      UnknownData00[0x1];                                       // 0x0003(0x0001) MISSED OFFSET
	float                                              Time;                                                     // 0x0004(0x0004) (Edit, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                              Value;                                                    // 0x0008(0x0004) (Edit, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                              ArriveTangent;                                            // 0x000C(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                              ArriveTangentWeight;                                      // 0x0010(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                              LeaveTangent;                                             // 0x0014(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                              LeaveTangentWeight;                                       // 0x0018(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

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

struct FTimerHandle
{
	uint64_t Handle;
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

struct FMarkerID
{
	int                                                PlayerId;                                                 // 0x0000(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int                                                InstanceID;                                               // 0x0004(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
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

struct BITaa
{
	uint8_t                                        bConditionalFoundation : 1;                        // Mask : 0x1 0xC68(0x1)(Edit, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
	uint8_t                                        bServerStreamedInLevel : 1;
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

static int GetRandomInt(int Min, int Max)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int<> distr(Min, Max);

	return distr(gen);
}

struct FDateTime
{
	__int64 Ticks;
};

enum class ECurveTableMode : unsigned char
{
	Empty,
	SimpleCurves,
	RichCurves
};

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

struct DQuat
{
	double W;
	double X;
	double Y;
	double Z;
};

struct FQuat
{
	float W;
	float X;
	float Y;
	float Z;

	void Describe() { std::cout << std::format("{} {} {} {}\n", W, X, Y, Z); }

	DQuat ToDouble()
	{
		DQuat Quat = DQuat(W, X, Y, Z); // ??
		return Quat;
	}

	inline FRotator Rotator() const;
};

struct FTransform
{
	FQuat Rotation;
	FVector Translation;
	char pad_1C[0x4]; // Padding never changes
	FVector Scale3D = FVector{ 1, 1, 1 };
	char pad_2C[0x4];
};

struct DTransform
{
	DQuat Rotation;
	DVector Translation;
	char pad_1C[0x4]; // Padding never changes
	DVector Scale3D = DVector{ 1, 1, 1 };
	char pad_2C[0x4];
};

static bool RandomBoolWithWeight(float Weight, float Min = 0.f, float Max = 1.f)
{
	return Weight <= 0.0f ? false : Weight >= GetRandomFloat(Min, Max);
}

FQuat FRotator::Quaternion() const
{
	const float DEG_TO_RAD = M_PI / (180.f);
	const float RADS_DIVIDED_BY_2 = DEG_TO_RAD / 2.f;
	float SP, SY, SR;
	float CP, CY, CR;

	const float PitchNoWinding = fmod(Pitch, 360.0f);
	const float YawNoWinding = fmod(Yaw, 360.0f);
	const float RollNoWinding = fmod(Roll, 360.0f);

	SinCos(&SP, &CP, PitchNoWinding * RADS_DIVIDED_BY_2);
	SinCos(&SY, &CY, YawNoWinding * RADS_DIVIDED_BY_2);
	SinCos(&SR, &CR, RollNoWinding * RADS_DIVIDED_BY_2);

	FQuat RotationQuat{};
	RotationQuat.X = CR * SP * SY - SR * CP * CY;
	RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
	RotationQuat.Z = CR * CP * SY - SR * SP * CY;
	RotationQuat.W = CR * CP * CY + SR * SP * SY;

	return RotationQuat;
}

static float UE_Atan2(float Y, float X)
{
	//return atan2f(Y,X);
	// atan2f occasionally returns NaN with perfectly valid input (possibly due to a compiler or library bug).
	// We are replacing it with a minimax approximation with a max relative error of 7.15255737e-007 compared to the C library function.
	// On PC this has been measured to be 2x faster than the std C version.

	const float absX = fabsf(X);
	const float absY = fabsf(Y);
	const bool yAbsBigger = (absY > absX);
	float t0 = yAbsBigger ? absY : absX; // Max(absY, absX)
	float t1 = yAbsBigger ? absX : absY; // Min(absX, absY)

	if (t0 == 0.f)
		return 0.f;

	float t3 = t1 / t0;
	float t4 = t3 * t3;

	static const float c[7] = {
		+7.2128853633444123e-03f,
		-3.5059680836411644e-02f,
		+8.1675882859940430e-02f,
		-1.3374657325451267e-01f,
		+1.9856563505717162e-01f,
		-3.3324998579202170e-01f,
		+1.0f
	};

	t0 = c[0];
	t0 = t0 * t4 + c[1];
	t0 = t0 * t4 + c[2];
	t0 = t0 * t4 + c[3];
	t0 = t0 * t4 + c[4];
	t0 = t0 * t4 + c[5];
	t0 = t0 * t4 + c[6];
	t3 = t0 * t3;

	t3 = yAbsBigger ? (0.5f * M_PI) - t3 : t3;
	t3 = (X < 0.0f) ? M_PI - t3 : t3;
	t3 = (Y < 0.0f) ? -t3 : t3;

	return t3;
}

#define FASTASIN_HALF_PI (1.5707963050f)
/**
* Computes the ASin of a scalar value.
*
* @param Value  input angle
* @return ASin of Value
*/
static FORCEINLINE float FastAsin(float Value)
{
	// Clamp input to [-1,1].
	bool nonnegative = (Value >= 0.0f);
	float x = fabsf(Value);
	float omx = 1.0f - x;
	if (omx < 0.0f)
	{
		omx = 0.0f;
	}
	float root = sqrtf(omx);
	// 7-degree minimax approximation
	float result = ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x - 0.0501743046f) * x + 0.0889789874f) * x - 0.2145988016f) * x + FASTASIN_HALF_PI;
	result *= root;  // acos(|x|)
	// acos(x) = pi - acos(-x) when x < 0, asin(x) = pi/2 - acos(x)
	return (nonnegative ? FASTASIN_HALF_PI - result : result - FASTASIN_HALF_PI);
}
#undef FASTASIN_HALF_PI

FRotator FQuat::Rotator() const
{
	// DiagnosticCheckNaN();
	const float SingularityTest = Z * X - W * Y;
	const float YawY = 2.f * (W * Z + X * Y);
	const float YawX = (1.f - 2.f * ((Y * Y) + (Z * Z)));

	// reference 
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/

	// this value was found from experience, the above websites recommend different values
	// but that isn't the case for us, so I went through different testing, and finally found the case 
	// where both of world lives happily.

	const float SINGULARITY_THRESHOLD = 0.4999995f;
	const float RAD_TO_DEG = (180.f) / M_PI;
	FRotator RotatorFromQuat = FRotator();

	if (SingularityTest < -SINGULARITY_THRESHOLD)
	{
		RotatorFromQuat.Pitch = -90.f;
		RotatorFromQuat.Yaw = UE_Atan2(YawY, YawX) * RAD_TO_DEG;
		RotatorFromQuat.Roll = FRotator::NormalizeAxis(-RotatorFromQuat.Yaw - (2.f * UE_Atan2(X, W) * RAD_TO_DEG));
	}
	else if (SingularityTest > SINGULARITY_THRESHOLD)
	{
		RotatorFromQuat.Pitch = 90.f;
		RotatorFromQuat.Yaw = UE_Atan2(YawY, YawX) * RAD_TO_DEG;
		RotatorFromQuat.Roll = FRotator::NormalizeAxis(RotatorFromQuat.Yaw - (2.f * UE_Atan2(X, W) * RAD_TO_DEG));
	}
	else
	{
		RotatorFromQuat.Pitch = FastAsin(2.f * (SingularityTest)) * RAD_TO_DEG;
		RotatorFromQuat.Yaw = UE_Atan2(YawY, YawX) * RAD_TO_DEG;
		RotatorFromQuat.Roll = UE_Atan2(-2.f * (W * X + Y * Z), (1.f - 2.f * ((X * X) + (Y * Y)))) * RAD_TO_DEG;
	}

	return RotatorFromQuat;
}