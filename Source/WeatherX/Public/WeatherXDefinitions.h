#pragma once

#include "CoreMinimal.h"
#include "WeatherXDefinitions.generated.h"


/** Types of weather currently supported */
UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EWeatherType : uint8
{
	None  UMETA(Hidden),
	Rainy UMETA(DisplayName = "Rainy"),
	Snowy UMETA(DisplayName = "Snowy"),
};
ENUM_CLASS_FLAGS(EWeatherType);

UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EPlayMode : uint8
{
	None       UMETA(Hidden),
	Queue      UMETA(DisplayName = "Queue"),
	Preemptive UMETA(DisplayName = "Preemptive"),
};
ENUM_CLASS_FLAGS(EPlayMode);

enum class EVolumeState : uint8
{
	None       UMETA(Hidden),
	Playing    UMETA(DisplayName = "Playing"),
	Stopping   UMETA(DisplayName = "Stopping"),
	Idle       UMETA(DisplayName = "Idle"),
};
ENUM_CLASS_FLAGS(EVolumeState);

enum class EWeatherXBlendMode : uint8
{
	None       UMETA(Hidden),
	Increment  UMETA(DisplayName = "Increment"),
	Lerp       UMETA(DisplayName = "Lerp"),
};
ENUM_CLASS_FLAGS(EWeatherXBlendMode);

/** FWeatherXEvent describes when to begin a specific type of weather and how long it lasts. */
USTRUCT(BlueprintType)
struct FWeatherXEvent
{
	GENERATED_BODY()

public:

	FWeatherXEvent() { }

	FWeatherXEvent(const EWeatherType InType, const float InStartTime, const float InDuration) : WeatherType(InType), StartTime(InStartTime), Duration(InDuration) { }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherX Event")
	EWeatherType WeatherType;

	/** The priority of the WeatherX Event, false means it is an event played in order, true means it is an incident event. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherX Event")
	bool IsIncidentEvent = false;

	/** when this type of weather begins */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherX Event", meta = (EditCondition = "!IsIncidentEvent"))
	float StartTime;

	/** how long this type of weather lasts */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherX Event")
	float Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherX Event")
	int32 StartFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherX Event")
	int32 EndFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherX Event")
	int32 LoopStartFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherX Event")
	int32 LoopEndFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherX Event")
	int32 LoopCount;

};

