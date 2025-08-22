#pragma once

#include "CoreMinimal.h"
#include "WeatherXDefinitions.h"
#include "Engine/VolumeTexture.h"
#include "Materials/MaterialParameterCollection.h"
#include "WeatherXBaseData.h"
#include "WeatherXExtendedData.generated.h"

class UDirectionalLightComponent;
class USkyLightComponent;
class USkyAtmosphereComponent;
class UExponentialHeightFogComponent;


USTRUCT(BlueprintType)
struct WEATHERX_API FWeatherXDirectionalLightData : public FWeatherXBaseData
{
	GENERATED_BODY()

public:

	FWeatherXDirectionalLightData() = default;

	virtual ~FWeatherXDirectionalLightData() = default;

	virtual UScriptStruct* GetScriptStruct() const override { return FWeatherXDirectionalLightData::StaticStruct(); };

	virtual void Apply() override;

public:

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "WeatherX Light Data")
	float DirLightIntensity = 7.0f;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "WeatherX Light Data")
	FLinearColor DirLightColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "WeatherX Light Data")
	float DirLightSourceAngle = 0.5357f;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "WeatherX Light Data")
	float DirLightTemoerature = 7000.0f;

private:
	TWeakObjectPtr<UDirectionalLightComponent> DirLightComponent;
};

USTRUCT(BlueprintType)
struct WEATHERX_API FWeatherXSkyLightData : public FWeatherXBaseData
{
	GENERATED_BODY()

public:

	FWeatherXSkyLightData() = default;

	virtual ~FWeatherXSkyLightData() = default;

	virtual UScriptStruct* GetScriptStruct() const override { return FWeatherXSkyLightData::StaticStruct(); };

	virtual void Apply() override;

public:

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "WeatherX Light Data")
	float SkyLightIntensity = 1.0f;

private:
	TWeakObjectPtr<USkyLightComponent> SkyLightComponent;
};

USTRUCT(BlueprintType)
struct WEATHERX_API FWeatherXSkyAtmosphereData : public FWeatherXBaseData
{
	GENERATED_BODY()

public:

	FWeatherXSkyAtmosphereData() = default;

	virtual ~FWeatherXSkyAtmosphereData() = default;

	virtual UScriptStruct* GetScriptStruct() const override { return FWeatherXSkyAtmosphereData::StaticStruct(); };

	virtual void Apply() override;

public:

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Weather Sky Atmosphere Data")
	float MultiScattering = 1.0f;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Weather Sky Atmosphere Data")
	FLinearColor RayleighScattering = FLinearColor(0.145833f, 0.386545f, 1.0f, 1.0f);

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Weather Sky Atmosphere Data")
	float MieScatteringScale = 0.003996f;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Weather Sky Atmosphere Data")
	float MieAbsorptionScale = 0.000444f;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Weather Sky Atmosphere Data")
	float MieAnisotropy = 0.8f;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Weather Sky Atmosphere Data")
	float AerialPerspectiveViewDistanceScale = 1.0f;

private:
	TWeakObjectPtr<USkyAtmosphereComponent> SkyAtmosphereComponent;
};

USTRUCT(BlueprintType)
struct WEATHERX_API FWeatherXExponentialHeightFogData : public FWeatherXBaseData
{
	GENERATED_BODY()

public:

	FWeatherXExponentialHeightFogData() = default;

	virtual ~FWeatherXExponentialHeightFogData() = default;

	virtual UScriptStruct* GetScriptStruct() const override { return FWeatherXExponentialHeightFogData::StaticStruct(); };

	virtual void Apply() override;

public:

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Weather Exponential Height Fog Data")
	FLinearColor EmissiveColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Weather Exponential Height Fog Data")
	float ExtinctionScale = 0.5f;

private:
	TWeakObjectPtr<UExponentialHeightFogComponent> ExponentialHeightFogComponent;
};
