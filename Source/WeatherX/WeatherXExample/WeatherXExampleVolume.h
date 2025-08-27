#pragma once

#include "CoreMinimal.h"
#include "WeatherXBaseData.h"
#include "WeatherXExtendedData.h"
#include "WeatherXVolume.h"


#include "WeatherXExampleVolume.generated.h"

UCLASS()
class WEATHERX_API AWeatherXExampleVolume : public AWeatherXVolumeBase
{
	GENERATED_BODY()

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather Datas", meta = (DisplayName = "WeatherX Light Data"))
	FWeatherXDirectionalLightData LightData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather Datas", meta = (DisplayName = "WeatherX Sky Atmosphere Data"))
	FWeatherXSkyAtmosphereData SkyAtmosphereData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather Datas", meta = (DisplayName = "WeatherX Sky Light Data"))
	FWeatherXSkyLightData SkyLightData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather Datas", meta = (DisplayName = "WeatherX Exponential Height Fog Data"))
	FWeatherXExponentialHeightFogData ExponentialHeightFogData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather Datas", meta = (DisplayName = "WeatherX Material Data"))
	FWeatherXMaterialData MaterialData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather Datas", meta = (DisplayName = "WeatherX Niagara Data"))
	FWeatherXNiagaraData NiagaraData;
};

UCLASS()
class WEATHERX_API AWeatherXExampleVolumeIndexed : public AWeatherXExampleVolume
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather Volume Settings", meta = (DisplayName = "Region Index"))
	int32 RegionIndex = -1;

	virtual bool IsActive(const FVector& ViewPos) override;

};
