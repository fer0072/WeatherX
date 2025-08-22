#pragma once

#include "CoreMinimal.h"
#include "WeatherXVolume.h"
#include "WeatherXDataInterface.h"
#include "WeatherXVolumeManager.generated.h"


UCLASS()
class WEATHERX_API UWeatherXVolumeManager : public UObject, public IWeatherXDataSourceInterface
{
	GENERATED_BODY()

public:

	void RemoveInvalidVolumes();

	// Insert WeatherVolume according to its priority, larger priority has higher rank. 
	// Returns index of the inserted volumes in the list.
	int32 AddWeatherVolume(TWeakObjectPtr<AWeatherXVolumeBase> InVolume);

	void AddSequencerOpenedVolume(TWeakObjectPtr<AWeatherXVolumeBase> InVolume);

	virtual void Update(const float CurrentTime, const FVector ViewPos);

	UWeatherXDataPacket* CollectDataPacket() override;

	void SetPriority(int32 InPriority) { Priority = InPriority; }

#if WITH_EDITOR
	void CleanupWeatherVolumes();

	void UpdateForPreview(TWeakPtr<ISequencer> Sequencer, const float CurrentTime, const FVector ViewPos);
#endif//WITH_EDITOR

protected:
	
	virtual void SelectActiveVolumes(const FVector& ViewPos);

protected:

	// List of all the weather volumes in current level.
	TArray<TWeakObjectPtr<AWeatherXVolumeBase>> WeatherVolumes;

	// List of active weather volumes in current level.
	TArray<TWeakObjectPtr<AWeatherXVolumeBase>> ActiveVolumes;

#if WITH_EDITOR
	// List of weather volumes whose level sequencer has been opened.
	TArray<TWeakObjectPtr<AWeatherXVolumeBase>> SequencerOpenedVolumes;
#endif//WITH_EDITOR

private:

	int32 Priority = 2;
};