#pragma once

#include "CoreMinimal.h"
#include "WeatherXVolumeManager.h"
#include "WeatherXExampleVolume.h"
#include "WeatherXExampleVolumeManager.generated.h"


UCLASS()
class WEATHERX_API UWeatherXExampleVolumeManager : public UWeatherXVolumeManager
{
	GENERATED_BODY()

public:
	
	virtual void SelectActiveVolumes(const FVector& ViewPos) override;

	virtual void Update(const float CurrentTime, const FVector ViewPos) override;
	
	UFUNCTION(BlueprintCallable, Category = "Weather System")
	void SetCurrentRegionIndex(int32 InIndex){ CurrentRegionIndex = InIndex; }

	int32 GetCurrentRegionIndex() { return CurrentRegionIndex;	}

	UFUNCTION(BlueprintCallable, Category = "Weather System")
	AWeatherXVolumeBase* GetCurrentActiveVolume(){ return CurrentActiveVolume.Get(); }

	bool IsSequencerOpened(AWeatherXVolumeBase* InVolume) { return SequencerOpenedVolumes.Contains(InVolume); }

private:

	int32 CurrentRegionIndex = -1;

	TWeakObjectPtr<AWeatherXVolumeBase> CurrentActiveVolume = nullptr;
};