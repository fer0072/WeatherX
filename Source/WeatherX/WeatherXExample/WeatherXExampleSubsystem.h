#pragma once

#include "CoreMinimal.h"
#include "WeatherXSubsystem.h"
#include "TODManager.h"
#include "WeatherXExampleVolumeManager.h"

#include "WeatherXExampleSubsystem.generated.h"


UCLASS()
class WEATHERX_API UWeatherXExampleSubsystem : public UWeatherXBaseSubsystem
{
	GENERATED_BODY()

public:
	
	UWeatherXExampleSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void OnWeatherXExampleWorldBeginPlay();
	
	UFUNCTION(BlueprintCallable, Category = "WeatherX")
	virtual float GetCurrentTimeOfDay() override;
	
	void RegisterTOD();

	UFUNCTION(BlueprintCallable, Category = "WeatherX")
	UWeatherXExampleVolumeManager* GetWeatherXExampleVolumeManager();

public:

	TWeakObjectPtr<ATODManager> TimeOfDayActor;

	FTimerHandle CollectDataHandle;

	//float CollectDataInterval = 0.05f;
};