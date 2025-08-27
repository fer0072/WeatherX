#include "WeatherXExampleSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "WeatherXSystemSettings.h"
#include "WeatherXExampleBlender.h"


UWeatherXExampleSubsystem::UWeatherXExampleSubsystem()
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		WeatherBlender = CreateDefaultSubobject<UWeatherXExampleBlender>(TEXT("Weather Blender"));
	}

	//参考UVCamInputSubsystem
	if (!HasAnyFlags(RF_DefaultSubObject))
	{
		WeatherVolumeManager = CreateDefaultSubobject<UWeatherXExampleVolumeManager>(TEXT("Weather Volume Manager"));
	}
}

void UWeatherXExampleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	if(!HasAnyFlags(RF_ClassDefaultObject))
	{
		UWorld* InWorld = GetWorld();
		if (InWorld)
		{
			if(UWeatherXExampleBlender* WeatherXExampleBlender = Cast<UWeatherXExampleBlender>(WeatherBlender))
			{
				InWorld->OnWorldBeginPlay.AddUObject(this, &UWeatherXExampleSubsystem::OnWeatherXExampleWorldBeginPlay);
			}		
		}
	}
}

void UWeatherXExampleSubsystem::OnWeatherXExampleWorldBeginPlay()
{
	UWorld* InWorld = GetWorld();

	if ((InWorld->WorldType == EWorldType::PIE || InWorld->WorldType == EWorldType::Game) && !IsTemplate())
	{
		if(UWeatherXExampleBlender* WeatherXExampleBlender = Cast<UWeatherXExampleBlender>(WeatherBlender))
		{
			WeatherXExampleBlender->CollectDataSources();
			InWorld->GetTimerManager().SetTimer(CollectDataHandle, WeatherXExampleBlender, &UWeatherXExampleBlender::MergeData, CollectDataInterval, true, 0.2f);
			RegisterTOD();
		}
	}
}

void UWeatherXExampleSubsystem::RegisterTOD()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATODManager::StaticClass(), OutActors);

	//check num == 1
	if (OutActors.Num() > 1)
	{
		UE_LOG(LogWeatherX, Error, TEXT("Too many TOD manager actors!"));
		return;
	}
	if (OutActors.Num() == 0)
	{
		UE_LOG(LogWeatherX, Log, TEXT("No TOD manager actor exists!"));
		return;
	}

	ATODManager* TODActor = Cast<ATODManager>(OutActors[0]);
	if (TODActor)
	{
		TimeOfDayActor = TODActor;
	}
}


float UWeatherXExampleSubsystem::GetCurrentTimeOfDay()
{
	if (TimeOfDayActor.IsValid())
	{
		return TimeOfDayActor->GetCurrentTimeOfDay();
	}

	return 0.0f;
}

UWeatherXExampleVolumeManager* UWeatherXExampleSubsystem::GetWeatherXExampleVolumeManager()
{
	UWeatherXExampleVolumeManager* Manager = Cast<UWeatherXExampleVolumeManager>(GetWeatherVolumeManager(GetWorld()));
	if(Manager)
	{
		return Manager;
	}

	return nullptr;
}