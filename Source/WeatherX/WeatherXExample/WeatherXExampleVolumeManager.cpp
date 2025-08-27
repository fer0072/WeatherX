#include "WeatherXExampleVolumeManager.h"

void UWeatherXExampleVolumeManager::SelectActiveVolumes(const FVector& ViewPos)
{
	ActiveVolumes.Empty();

	for (TWeakObjectPtr<AWeatherXVolumeBase> Volume : WeatherVolumes)
	{
		check(Volume.IsValid());

		if (Volume->IsActive(ViewPos))
		{
			Volume->CleanSequence();
			ActiveVolumes.Add(Volume);
		}
		else if(!Cast<AWeatherXExampleVolumeIndexed>(Volume) && !Volume->IsActive(ViewPos) && Volume->GetVolumeState() != EVolumeState::Stopping)
		{
			Volume->ResetTriggeredTime();
		}
	}
}

void UWeatherXExampleVolumeManager::Update(const float CurrentTime, const FVector ViewPos)
{
	SelectActiveVolumes(ViewPos);

	TWeakObjectPtr<AWeatherXVolumeBase> NextActiveVolume = nullptr;

	for (TWeakObjectPtr<AWeatherXVolumeBase> Volume : ActiveVolumes)
	{
		if (!Volume.IsValid())
		{
			continue;
		}

		if (Cast<AWeatherXExampleVolumeIndexed>(Volume))
		// Deal with the indexed volumes.
		{
			if (NextActiveVolume.IsValid())
			{
				ActiveVolumes.Remove(Volume);
			}
			else
			{
				NextActiveVolume = Volume;
			}
		}
		else
		// Update normal volumes as usual.
		{
			Volume->Update(CurrentTime);
		}
	}

	//~=============================================================================
	// Deal with the update logic of indexed volumes.

	if (!CurrentActiveVolume.IsValid() && !NextActiveVolume.IsValid())
	{
		return;
	}
	
	CurrentActiveVolume = !CurrentActiveVolume.IsValid() ? NextActiveVolume : CurrentActiveVolume;	

	// If the current active volume doesn't change, update it as usual.
	if(CurrentActiveVolume == NextActiveVolume)
	{
		CurrentActiveVolume->CleanSequence();
		CurrentActiveVolume->Update(CurrentTime);
	}
	else
	// If the active volume is changed, stop current active volume.
	{
		ActiveVolumes.Remove(NextActiveVolume);

		CurrentActiveVolume->StopVolume(CurrentTime);

		ActiveVolumes.Add(CurrentActiveVolume);

		// If current active volume is idle, set it to nullptr.
		if (CurrentActiveVolume->GetVolumeState() == EVolumeState::Idle)
		{
			CurrentActiveVolume = nullptr;
		}
	}
	
	return;
}