#include "WeatherXExamplePreviewSubsystem.h"
#include "WeatherXExampleBlender.h"
#include "TODManager.h"
#include "WeatherXExampleVolumeManager.h"
#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR

#include "ISequencerModule.h"

UWeatherXExamplePreviewSubsystem::UWeatherXExamplePreviewSubsystem()
{
	if (!HasAnyFlags(RF_DefaultSubObject))
	{
		WeatherBlender = CreateDefaultSubobject<UWeatherXExampleBlender>(TEXT("Weather Blender"));
	}

	if (!HasAnyFlags(RF_DefaultSubObject))
	{
		WeatherVolumeManager = CreateDefaultSubobject<UWeatherXExampleVolumeManager>(TEXT("Weather Volume Manager"));
	}
}

void UWeatherXExamplePreviewSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UWeatherXExamplePreviewSubsystem::OnSequencerCreated(TSharedRef<ISequencer> Sequencer)
{
	Super::OnSequencerCreated(Sequencer);

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		UWorld* InWorld = GetWorld();
		if (InWorld && InWorld->WorldType == EWorldType::Editor && !IsTemplate())
		{
			if (UWeatherXExampleBlender* WeatherXExampleBlender = Cast<UWeatherXExampleBlender>(WeatherBlender))
			{
				WeatherXExampleBlender->CollectDataSources();
				RegisterTOD();
			}
		}
	}
}

void UWeatherXExamplePreviewSubsystem::OnSequencerTimeChanged(TWeakPtr<ISequencer> Sequencer)
{
	Super::OnSequencerTimeChanged(Sequencer);

	WeatherBlender->MergeData();

	return;
}

void UWeatherXExamplePreviewSubsystem::OnSequencerClosed(TSharedRef<ISequencer> Sequencer)
{
	Super::OnSequencerClosed(Sequencer);

	WeatherBlender->MergeData();

	return;
}


void UWeatherXExamplePreviewSubsystem::RegisterTOD()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATODManager::StaticClass(), OutActors);

	//check num == 1
	if (OutActors.Num() > 1)
	{
		UE_LOG(LogWeatherX, Log, TEXT("Too many ATODManager Actors"));
		return;
	}
	if (OutActors.Num() == 0)
	{
		UE_LOG(LogWeatherX, Log, TEXT("No ATODManager Actor exists"));
		return;
	}

	ATODManager* TODActor = Cast<ATODManager>(OutActors[0]);
	if (TODActor)
	{
		TimeOfDayActor = TODActor;
	}
}

#endif // #if WITH_EDITOR