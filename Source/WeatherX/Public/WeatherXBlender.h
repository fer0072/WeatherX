#pragma once

#include "CoreMinimal.h"
#include "WeatherXDataInterface.h"
#include "WeatherXBlender.generated.h"

class UWeatherXDataPacket;

/**
 *  Weather Blender
 */
UCLASS(Abstract)
class WEATHERX_API UWeatherXBlender : public UObject, public IWeatherXDataSourceInterface
{
	GENERATED_BODY()
public:

	UWeatherXDataPacket* CollectDataPacket();

	virtual void CollectDataSources(){ }

	virtual void MergeData(){ }
};