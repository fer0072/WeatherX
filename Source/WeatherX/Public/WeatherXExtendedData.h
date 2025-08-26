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

	UPROPERTY()
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

	UPROPERTY()
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

	UPROPERTY()
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
	FLinearColor EmissiveColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Weather Exponential Height Fog Data")
	float ExtinctionScale = 0.5f;

	UPROPERTY()
	TWeakObjectPtr<UExponentialHeightFogComponent> ExponentialHeightFogComponent;

};

USTRUCT(BlueprintType)
struct WEATHERX_API FWeatherXMaterialData : public FWeatherXBaseData
{
	GENERATED_BODY()

public:

	FWeatherXMaterialData() = default;

	virtual ~FWeatherXMaterialData() = default;

	virtual UScriptStruct* GetScriptStruct() const override { return FWeatherXMaterialData::StaticStruct(); };

	virtual void MergeInto(const TArray<TSharedPtr<FWeatherXBaseData>>& DataList, TArray<float> RatioList) override;

	virtual void Apply() override;

	virtual void CalcTrackedID() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherX Material Data")
	TSoftObjectPtr<UMaterialParameterCollection> MPCPath;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "WeatherX Material Data")
	float IsSnowy = 0.0f;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "WeatherX Material Data")
	float SnowAmount = 0.0f;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "WeatherX Material Data")
	float IsRainy = 0.0f;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "WeatherX Material Data")
	float EnableRaindropsPostprocessEffect = 0.0f;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "WeatherX Material Data")
	float PuddleSize = 0.0f;

};

