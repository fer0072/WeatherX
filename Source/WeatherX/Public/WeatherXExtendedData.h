#pragma once

#include "CoreMinimal.h"
#include "WeatherXDefinitions.h"
#include "Engine/VolumeTexture.h"
#include "Materials/MaterialParameterCollection.h"
#include "WeatherXBaseData.h"
#include "WeatherXExtendedData.generated.h"

class AExponentialHeightFog;
class UDirectionalLightComponent;


USTRUCT(BlueprintType)
struct WEATHERX_API FWeatherXLightData : public FWeatherXBaseData
{
	GENERATED_BODY()

public:

	FWeatherXLightData() = default;

	virtual ~FWeatherXLightData() = default;

	virtual UScriptStruct* GetScriptStruct() const override { return FWeatherXLightData::StaticStruct(); };

	virtual void Apply() override;

public:

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "WeatherX Light Data")
	float DirLightIntensity = 0.0f;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "WeatherX Light Data")
	FLinearColor DirLightColor = FLinearColor();

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "WeatherX Light Data")
	float DirLightSourceAngle = 0.0f;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "WeatherX Light Data")
	float DirLightTemoerature = 0.0f;

private:
	TWeakObjectPtr<UDirectionalLightComponent> DirLightComponent;
};

USTRUCT(BlueprintType)
struct WEATHERX_API FWeatherXParticleData : public FWeatherXBaseData
{
	GENERATED_BODY()

public:

	FWeatherXParticleData() = default;

	virtual ~FWeatherXParticleData() = default;

	virtual UScriptStruct* GetScriptStruct() const override { return FWeatherXParticleData::StaticStruct(); };

	virtual void Apply() override;

public:

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "WeatherX Particle Data")
    float SpawnRate = 0.0f;
};

USTRUCT(BlueprintType)
struct WEATHERX_API FWeatherXMaterialData : public FWeatherXBaseData
{
	GENERATED_BODY()

public:

	FWeatherXMaterialData() = default;

	~FWeatherXMaterialData() = default;

	virtual UScriptStruct* GetScriptStruct() const override { return FWeatherXMaterialData::StaticStruct(); };

	virtual void MergeInto(const TArray<TSharedPtr<FWeatherXBaseData>>& DataList, TArray<float> RatioList) override;

	virtual void Apply() override;

	virtual void CalcTrackedID() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather Material Data")
	TSoftObjectPtr<UMaterialParameterCollection> MPCPath;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Weather Material Data")
    float RainIntensity = 0.0f;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Weather Material Data")
    float SnowIntensity = 0.0f;

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
	FLinearColor RayleighScattering = FLinearColor(0.0f, 0.170605f, 1.0f, 1.0f);

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Weather Sky Atmosphere Data")
    float RayleighScatteringScale = 0.0331f;
};


USTRUCT(BlueprintType)
struct WEATHERX_API FWeatherXVolumetricCloudData : public FWeatherXBaseData
{
	GENERATED_BODY()

public:

	FWeatherXVolumetricCloudData() = default;

	virtual ~FWeatherXVolumetricCloudData() = default;

	virtual UScriptStruct* GetScriptStruct() const override { return FWeatherXVolumetricCloudData::StaticStruct(); };

	virtual void MergeInto(const TArray<TSharedPtr<FWeatherXBaseData>>& DataList, TArray<float> RatioList);

	virtual void Apply() override;

	virtual void CalcTrackedID() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather Volumetric Cloud Data")
	TSoftObjectPtr<UMaterialParameterCollection> MPCPath;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Weather Volumetric Cloud Data")
    float CloudScale = 0.0f;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Weather Volumetric Cloud Data")
	FLinearColor CloudColor = FLinearColor();
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
    float FogDensity = 0.0f;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Weather Exponential Height Fog Data")
	FLinearColor FogInscatteringColor = FLinearColor();

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Weather Exponential Height Fog Data")
	FLinearColor DirectionalInscatteringColor = FLinearColor();
};
