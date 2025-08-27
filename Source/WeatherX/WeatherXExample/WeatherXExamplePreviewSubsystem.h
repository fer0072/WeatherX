#pragma once

#include "CoreMinimal.h"

#if WITH_EDITOR

#include "WeatherXPreviewSubsystem.h"
#include "WeatherXExamplePreviewSubsystem.generated.h"

class ATODManager;

UCLASS()
class WEATHERX_API UWeatherXExamplePreviewSubsystem : public UWeatherXPreviewSubsystem
{
	GENERATED_BODY()

public:

	UWeatherXExamplePreviewSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void OnSequencerCreated(TSharedRef<ISequencer> Sequencer) override;

	virtual void OnSequencerTimeChanged(TWeakPtr<ISequencer> Sequencer) override;

	virtual void OnSequencerClosed(TSharedRef<ISequencer> Sequencer) override;

	virtual void RegisterTOD() override;


public:

	TWeakObjectPtr<ATODManager> TimeOfDayActor;

};


#endif //#if WITH_EDITOR