#pragma once
#include "CoreMinimal.h"
#include "WeatherXVolumeManager.h"
#include "WeatherXBlender.h"
#include "WeatherXSystemSettings.h"
#include "WeatherXSubsystem.generated.h"


UCLASS(Abstract)
class WEATHERX_API UWeatherXBaseSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	
	UWeatherXBaseSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	static UWeatherXVolumeManager* GetWeatherVolumeManager(UWorld* InWorld);
	
	void Update();

	// Shoule be overrided in the project'weather system, according to the TOD system in the project.
	virtual float GetCurrentTimeOfDay() { return 0.0f; }

public:

	//This blender should be constructed in subclass of WeatherSubsystem.
	UPROPERTY()
	UWeatherXBlender* WeatherBlender;

protected:

	// A Manager that manages all the existing volumes in level.
	UPROPERTY()
	UWeatherXVolumeManager* WeatherVolumeManager;

	float UpdateActiveVolumesInterval = 0.1f;

	float CollectDataInterval = 0.05f;

private:

	FTimerHandle UpdateActiveVolumesHandle;

	FTimerHandle CollectDataHandle;

private:

	FVector GetViewPosition();
};