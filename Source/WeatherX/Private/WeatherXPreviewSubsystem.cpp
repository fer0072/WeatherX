#include "WeatherXPreviewSubsystem.h"
#include "WeatherXVolume.h"
#include "WeatherXSystemSettings.h"
#include "WeatherXLog.h"
#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR

#include "ISequencerModule.h"

UWeatherXPreviewSubsystem::UWeatherXPreviewSubsystem()
{

}

void UWeatherXPreviewSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UWorld* InWorld = GetWorld();
	if (InWorld && InWorld->WorldType == EWorldType::Editor && !IsTemplate())
	{
		// Get the priority of the weather system.
		const UWeatherXSystemSettings* Settings = GetDefault<UWeatherXSystemSettings>();
		WeatherVolumeManager->SetPriority(Settings->WeatherSystemPriority);

		// Create the delegate handle.
		ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>("Sequencer");
		OnSequencerCreatedHandle = SequencerModule.RegisterOnSequencerCreated(FOnSequencerCreated::FDelegate::CreateUObject(this, &UWeatherXPreviewSubsystem::OnSequencerCreated));
	}
}

void UWeatherXPreviewSubsystem::OnSequencerCreated(TSharedRef<ISequencer> Sequencer)
{
	UWorld* InWorld = GetWorld();

	if (InWorld && InWorld->WorldType == EWorldType::Editor && !IsTemplate())
	{
		// Add existed weather volumes to the weather volume manager of the preview subsystem.
		TArray<AActor*> WeatherVolumes;
		UGameplayStatics::GetAllActorsOfClass(InWorld, AWeatherXVolumeBase::StaticClass(), WeatherVolumes);

		for (AActor* Volume : WeatherVolumes)
		{
			if(!Volume)
			{
				continue;
			}
			
			AWeatherXVolumeBase* WeatherVolume = Cast<AWeatherXVolumeBase>(Volume);
			if (!WeatherVolume->IsTemplate())
			{
				WeatherVolumeManager->AddWeatherVolume(WeatherVolume);
				if (WeatherVolume->InitializePlayerForPreview(Sequencer))
				{
					WeatherVolumeManager->AddSequencerOpenedVolume(WeatherVolume);
				}
			}
		}
		WeatherVolumeManager->RemoveInvalidVolumes();

		// Create delegate handles.
		TWeakPtr<ISequencer> WeakSequencer = Sequencer;
		OnSequencerCreatedHandle = Sequencer->OnGlobalTimeChanged().AddUObject(this, &UWeatherXPreviewSubsystem::OnSequencerTimeChanged, WeakSequencer);
		OnSequencerClosedHandle = Sequencer->OnCloseEvent().AddUObject(this, &UWeatherXPreviewSubsystem::OnSequencerClosed);
	}
}

void UWeatherXPreviewSubsystem::OnSequencerTimeChanged(TWeakPtr<ISequencer> Sequencer)
{
	ISequencer* SequencerPtr = Sequencer.Pin().Get();
	if (!SequencerPtr)
	{
		return;
	}

	double CurrentTime = SequencerPtr->GetLocalTime().AsSeconds();

	FVector ViewPos = GetViewPosition();

	WeatherVolumeManager->UpdateForPreview(Sequencer, CurrentTime, ViewPos);
}

void UWeatherXPreviewSubsystem::OnSequencerClosed(TSharedRef<ISequencer> Sequencer)
{
	WeatherVolumeManager->CleanupWeatherVolumes();
}

UWeatherXVolumeManager* UWeatherXPreviewSubsystem::GetWeatherVolumeManager(UWorld* InWorld)
{
	if (!InWorld)
	{
		UE_LOG(LogWeatherX, Error, TEXT("No valid UWorld instance!"));
		
		return nullptr;
	}

	UWeatherXPreviewSubsystem* Subsystem = InWorld->GetSubsystem<UWeatherXPreviewSubsystem>();
	if (Subsystem)
	{
		return Subsystem->WeatherVolumeManager;
	}
	return nullptr;
}

FVector UWeatherXPreviewSubsystem::GetViewPosition()
{
	FVector ViewPos = FVector::ZeroVector;

	auto activeViewport = GEditor->GetActiveViewport();
	FEditorViewportClient* editorViewClient = (activeViewport != nullptr) ? (FEditorViewportClient*)(activeViewport->GetClient()) : nullptr;
	if (editorViewClient)
	{
		ViewPos = editorViewClient->GetViewLocation();
	}

	return ViewPos;
}

#endif // #if WITH_EDITOR