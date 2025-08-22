#include "WeatherXVolumeManager.h"
#include "WeatherXBaseData.h"
#include "WeatherXExtendedData.h"
#include "WeatherXDataPacket.h"
#include "MovieSceneSequencePlayer.h"
#include "WeatherXLog.h"

#if WITH_EDITOR
void UWeatherXVolumeManager::CleanupWeatherVolumes()
{
	for (TWeakObjectPtr<AWeatherXVolumeBase> Volume : WeatherVolumes)
	{
		if (!Volume.IsValid())
		{
			continue;
		}
		Volume->ResetVolume();
	}	

	WeatherVolumes.Empty();
	ActiveVolumes.Empty();
	SequencerOpenedVolumes.Empty();
}

void UWeatherXVolumeManager::UpdateForPreview(TWeakPtr<ISequencer> Sequencer, const float CurrentTime, const FVector ViewPos)
{
	SelectActiveVolumes(ViewPos);

	for (TWeakObjectPtr<AWeatherXVolumeBase> Volume : ActiveVolumes)
	{
		if (!Volume.IsValid())
		{
			continue;
		}

		bool IsSequencerOpened = SequencerOpenedVolumes.Contains(Volume);
		if (IsSequencerOpened)
		{
			Volume->UpdateForPreview(Sequencer, CurrentTime);
		}
	}
}
#endif//WITH_EDITOR

void UWeatherXVolumeManager::RemoveInvalidVolumes()
{
	for (TWeakObjectPtr<AWeatherXVolumeBase> Volume : WeatherVolumes)
	{
		if (Volume.IsStale() || !Volume.IsValid())
		{
			WeatherVolumes.Remove(Volume);
		}
	}
}

int32 UWeatherXVolumeManager::AddWeatherVolume(TWeakObjectPtr<AWeatherXVolumeBase> InVolume)
{
	if (!InVolume.IsValid())
	{
		UE_LOG(LogWeatherX, Warning, TEXT("Weather volume weak pointer is not valid!"))
		return -1;
	}

	int32 InVolumePriority = InVolume->Priority;
	int32 InsertIndex = 0;
	for (; InsertIndex < WeatherVolumes.Num(); InsertIndex++)
	{
		if (InVolumePriority > WeatherVolumes[InsertIndex]->Priority)
		{
			break;
		}
		if (InVolume == WeatherVolumes[InsertIndex])
		{
			return InsertIndex;
		}
	}
	WeatherVolumes.Insert(InVolume, InsertIndex);
	RemoveInvalidVolumes();
	return InsertIndex;
}

void UWeatherXVolumeManager::AddSequencerOpenedVolume(TWeakObjectPtr<AWeatherXVolumeBase> InVolume)
{
	if (InVolume.IsValid() && !SequencerOpenedVolumes.Contains(InVolume))
	{
		SequencerOpenedVolumes.Add(InVolume);
	}
}

void UWeatherXVolumeManager::SelectActiveVolumes(const FVector& ViewPos)
{
	ActiveVolumes.Empty();

	for (TWeakObjectPtr<AWeatherXVolumeBase> Volume : WeatherVolumes)
	{
		if (!Volume.IsValid())
		{
			continue;
		}

		if (!Volume->IsActive(ViewPos) && Volume->GetVolumeState() != EVolumeState::Stopping)
		{
			Volume->ResetTriggeredTime();
		}
		else
		{
			Volume->CleanSequence();
			ActiveVolumes.Add(Volume);
		}		
	}
}

void UWeatherXVolumeManager::Update(const float CurrentTime, const FVector ViewPos)
{
	SelectActiveVolumes(ViewPos);

	for (TWeakObjectPtr<AWeatherXVolumeBase> Volume : ActiveVolumes)
	{
		if (!Volume.IsValid())
		{
			continue;
		}
		
		Volume->Update(CurrentTime);
	}
}

UWeatherXDataPacket* UWeatherXVolumeManager::CollectDataPacket()
{
	TArray<UWeatherXDataPacket*> CollectedDataPackets;

	for (TWeakObjectPtr<AWeatherXVolumeBase> Volume : ActiveVolumes)
	{
		if (!Volume.IsValid())
		{
			continue;
		}
		CollectedDataPackets.Add(Volume->CollectDataPacket());
	}
	
	UWeatherXDataPacket* MergedDataPacket = UWeatherXDataPacket::MergeDataPacket(CollectedDataPackets);

	// Set the priority of all the collected data in this weather system as the priority of the weather system.
	int32 WeatherSystemPriority = Priority;
	for (TSharedPtr<FWeatherXBaseData> WeatherData : MergedDataPacket->DataList)
	{
		WeatherData->Priority = WeatherSystemPriority;
	}

	return MergedDataPacket;
}