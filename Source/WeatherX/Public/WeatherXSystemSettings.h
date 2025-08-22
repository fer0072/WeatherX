#pragma once

#include "Engine/DeveloperSettings.h"
#include "WeatherXSystemSettings.generated.h"

UCLASS(config = WeatherSystem)
class WEATHERX_API UWeatherXSystemSettings : public UObject
{
	GENERATED_BODY()

public:
	
	UWeatherXSystemSettings() = default;

	~UWeatherXSystemSettings() = default;

	UPROPERTY(config, EditAnywhere, Category = WeatherX)
	float UpdateActiveVolumesInterval = 0.1f;

	UPROPERTY(config, EditAnywhere, Category = WeatherX)
	float CollectDataInterval = 0.05f;

	UPROPERTY(config, EditAnywhere, Category = WeatherX)
	int32 TODPriority = 1;

	UPROPERTY(config, EditAnywhere, Category = WeatherX)
	float TODOpacity = 0.5f;

	UPROPERTY(config, EditAnywhere, Category = WeatherX)
	int32 WeatherSystemPriority = 2;
};