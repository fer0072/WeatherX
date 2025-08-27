#include "WeatherXExampleVolume.h"
#include "WeatherXExampleVolumeManager.h"
#include "WeatherXSubsystem.h"
#include "WeatherXPreviewSubsystem.h"

bool AWeatherXExampleVolumeIndexed::IsActive(const FVector& ViewPos)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	UWeatherXExampleVolumeManager* Manager = nullptr;
	bool IsActivated = false;

	if (World->WorldType == EWorldType::Editor)
	// Check if the volume is active under the preview mode.
	{
		Manager = Cast<UWeatherXExampleVolumeManager>(UWeatherXPreviewSubsystem::GetWeatherVolumeManager(GetWorld()));

		if (!Manager)
		{
			return false;
		}
		
		AWeatherXVolumeBase* Volume = Cast<AWeatherXVolumeBase>(this);
		IsActivated = Manager->IsSequencerOpened(Volume);
	}
	else
	// Check if the volume is active under the game / PIE mode.
	{
		Manager = Cast<UWeatherXExampleVolumeManager>(UWeatherXBaseSubsystem::GetWeatherVolumeManager(GetWorld()));

		if (!Manager)
		{
			return false;
		}

		IsActivated = Manager->GetCurrentRegionIndex() == RegionIndex;
	}
	
	if (IsActivated)
	{
		SetBaseParameters(1.0f);
	}

	return IsActivated;
}