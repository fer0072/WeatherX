#pragma once

#include "CoreMinimal.h"
#include "WeatherXBlender.h"
#include "WeatherXVolume.h"
#include "WeatherXVolumeManager.h"
#include "WeatherXPreviewSubsystem.generated.h"


UCLASS(Abstract)

class WEATHERX_API UWeatherXPreviewSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

#if WITH_EDITOR
	UWeatherXPreviewSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void OnSequencerCreated(TSharedRef<ISequencer> Sequencer);

	virtual void OnSequencerTimeChanged(TWeakPtr<ISequencer> Sequencer);

	virtual void OnSequencerClosed(TSharedRef<ISequencer> Sequencer);

	static UWeatherXVolumeManager* GetWeatherVolumeManager(UWorld* InWorld);

	FVector GetViewPosition();

	virtual void RegisterTOD() {};
#endif // #if WITH_EDITOR

public:

#if WITH_EDITORONLY_DATA
	//this blender should be constructed in subclass of WeatherSubsystem
	UPROPERTY()
	UWeatherXBlender* WeatherBlender;

protected:

	/** A Manager that manages all the existing volumes in level */
	UPROPERTY()
	UWeatherXVolumeManager* WeatherVolumeManager;

private:

	FDelegateHandle OnSequencerCreatedHandle;

	FDelegateHandle OnSequencerTimerChangedHandle;

	FDelegateHandle OnSequencerClosedHandle;
#endif // #if WITH_EDITORONLY_DATA
};