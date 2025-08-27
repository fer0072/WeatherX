#include "WeatherXSubsystem.h"
#include "WeatherXLog.h"
#include "Kismet/GameplayStatics.h"

UWeatherXBaseSubsystem::UWeatherXBaseSubsystem()
{
	
}


void UWeatherXBaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UWorld* InWorld = GetWorld();
	if (InWorld)
	{	
		const UWeatherXSystemSettings* Settings = GetDefault<UWeatherXSystemSettings>();
		UpdateActiveVolumesInterval = Settings->UpdateActiveVolumesInterval;
		CollectDataInterval = Settings->CollectDataInterval;
		WeatherVolumeManager->SetPriority(Settings->WeatherSystemPriority);
	}
}

void UWeatherXBaseSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{	
	Super::OnWorldBeginPlay(InWorld);

	if ((InWorld.WorldType == EWorldType::PIE || InWorld.WorldType == EWorldType::Game) && !IsTemplate())
	{
		InWorld.GetTimerManager().SetTimer(UpdateActiveVolumesHandle, this, &UWeatherXBaseSubsystem::Update, UpdateActiveVolumesInterval, true, 0.1f);
	}
	
}

UWeatherXVolumeManager* UWeatherXBaseSubsystem::GetWeatherVolumeManager(UWorld* InWorld)
{
	if (!InWorld)
	{
		UE_LOG(LogWeatherX, Warning, TEXT("No valid UWorld instance!"));

		return nullptr;
	}

	if (UWeatherXBaseSubsystem* Subsystem = InWorld->GetSubsystem<UWeatherXBaseSubsystem>())
	{
		return Subsystem->WeatherVolumeManager;
	}

	return nullptr;
}

void UWeatherXBaseSubsystem::Update()
{
	float CurrentTime = GetCurrentTimeOfDay();

	FVector ViewPos = GetViewPosition();
	
	WeatherVolumeManager->Update(CurrentTime, ViewPos);
}


FVector UWeatherXBaseSubsystem::GetViewPosition()
{
	FVector ViewPos = FVector::ZeroVector;
	auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APawn* Pawn = nullptr;
	if (PlayerController)
	{
		Pawn = PlayerController->GetPawn();
		if (Pawn)
		{
			ViewPos = Pawn->GetActorLocation();
			return ViewPos;
		}
		else
		{
			UE_LOG(LogWeatherX, Error, TEXT("No valid player pawn!"));
		}
	}
	else
	{
		UE_LOG(LogWeatherX, Log, TEXT("No valid player controller!"));
	}
	return ViewPos;
}