#pragma once

#include "CoreMinimal.h"
#include "WeatherXBlender.h"

#include "WeatherXExampleBlender.generated.h"

class UWeatherXExampleSubsystem;
class ATODManager;


UCLASS(BlueprintType)
class WEATHERX_API UWeatherXExampleBlender : public UWeatherXBlender
{
	GENERATED_BODY()

public:

	virtual void CollectDataSources() override;

	virtual	void MergeData() override;
	
public:

	TWeakObjectPtr<ATODManager> TODManager;

	TWeakObjectPtr<UWeatherXExampleSubsystem> WeatherXExampleSystem;
};
