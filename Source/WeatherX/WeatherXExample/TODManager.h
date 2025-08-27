#pragma once

#include "CoreMinimal.h"
#include "WeatherXDataInterface.h"
#include "WeatherXBaseData.h"
#include "WeatherXExtendedData.h"
#include "WeatherXDefinitions.h"
#include "TODManager.generated.h"


/** A simple TimeOfDay System. 
 *  To adapt other TOD system into WeatherSystem, one should implement the ITODInterface and IWeatherDataInterface in the corresponding TOD system 
 */
UCLASS(hideCategories = (Rendering, Replication, Collision, Input, Actor, LOD, Cooking))
class WEATHERX_API ATODManager : public AActor, public IWeatherXDataSourceInterface
{
	GENERATED_BODY()

public:

	FWeatherXDirectionalLightData DirLightData;

	FWeatherXSkyAtmosphereData SkyAtmosphereData;

	FWeatherXSkyLightData SkyLightData;

	FWeatherXExponentialHeightFogData ExponentialHeightFogData;

	FWeatherXMaterialData MaterialData;

	FWeatherXNiagaraData NiagaraData;

	TArray<FWeatherXBaseData*> WeatherDatasInTOD;

	int32 Priority = 1;

	float Opacity = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TOD Actor")
	TWeakObjectPtr<AActor> TODActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherX Material Data")
	TSoftObjectPtr<UMaterialParameterCollection> MPCPath;

public:

	ATODManager();

	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostLoad() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif	//#if WITH_EDITOR

	float GetCurrentTimeOfDay();

	void UpdateCurrentTimeOfDay();

	virtual UWeatherXDataPacket* CollectDataPacket() override;

	void InitializeTODData();

	UFUNCTION(BlueprintCallable, Category = "TOD Events")
	void UpdateTODData();

private:
	
	float CurrentTimeOfDay;

	float TimeUpdateInterval;

	FTimerHandle TimeUpdateHandle;
};
