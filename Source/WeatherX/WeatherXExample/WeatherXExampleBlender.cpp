#include "WeatherXExampleBlender.h"
#include "Kismet/GameplayStatics.h"
#include "TODManager.h"
#include "WeatherXExampleSubsystem.h"
#include "WeatherXDataPacket.h"

#if WITH_EDITOR
#include "WeatherXExamplePreviewSubsystem.h"
#endif

void UWeatherXExampleBlender::CollectDataSources()
{
	// Get the TODManager actor.
	TArray<AActor*> TODManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATODManager::StaticClass(), TODManagers);
	if (TODManagers.Num() == 0)
	{
		UE_LOG(LogWeatherX, Log, TEXT("No valid TOD manager!"));
		return;
	}
	else if (TODManagers.Num() > 1)
	{
		UE_LOG(LogWeatherX, Error, TEXT("Too many TOD manager actors in current level!"));
		return;
	}
	else
	{
		ATODManager* TODActor = Cast<ATODManager>(TODManagers[0]);
		if (TODActor)
		{
			TODActor->InitializeTODData();
			TODManager = TODActor;
		}
		else
		{
			UE_LOG(LogWeatherX, Log, TEXT("No valid TOD manager!"));
			return;
		}
	}

	// Get the Weather Subsystem actor.
	UWorld* InWorld = GetWorld();
	if (UWeatherXExampleSubsystem* Subsystem = InWorld->GetSubsystem<UWeatherXExampleSubsystem>())
	{
		WeatherXExampleSystem = Subsystem;
	}
	else
	{
		UE_LOG(LogWeatherX, Error, TEXT("No valid WeatherXExampleSubsystem!"))
		return;
	}
}

void UWeatherXExampleBlender::MergeData()
{
	TArray<UWeatherXDataPacket*> PendingMergedDataPacket;

	UWorld* InWorld = GetWorld();

	// Collect the data packet from the weather system.
	UWeatherXVolumeManager* WeatherVolumeManager;
	if (InWorld->WorldType == EWorldType::Editor)
	{
		WeatherVolumeManager = UWeatherXPreviewSubsystem::GetWeatherVolumeManager(InWorld);
	}
	else
	{
		WeatherVolumeManager = UWeatherXBaseSubsystem::GetWeatherVolumeManager(InWorld);
	}

	if (WeatherVolumeManager)
	{
		UWeatherXDataPacket* WeatherDataPacket = WeatherVolumeManager->CollectDataPacket();
		if(WeatherDataPacket->DataList.Num() != 0) PendingMergedDataPacket.Add(WeatherDataPacket);
	}	
	
	// Collect the data packet from the TOD manager.
	if (TODManager.IsValid())
	{
		if (InWorld->WorldType == EWorldType::Editor)
		{
			TODManager->UpdateTODData();
		}
		UWeatherXDataPacket* TODDataPacket = TODManager->CollectDataPacket();
		if(TODDataPacket->DataList.Num() != 0) PendingMergedDataPacket.Add(TODDataPacket);
	}

	UWeatherXDataPacket* MergedDataPacket = UWeatherXDataPacket::MergeDataPacket(PendingMergedDataPacket);
	
	MergedDataPacket->Apply();
}