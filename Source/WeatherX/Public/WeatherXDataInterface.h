#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "WeatherXDataInterface.generated.h"

class UWeatherXDataPacket;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UWeatherXDataSourceInterface : public UInterface
{
	GENERATED_BODY()
};

class WEATHERX_API IWeatherXDataSourceInterface
{
	GENERATED_BODY()

public:

	virtual UWeatherXDataPacket* CollectDataPacket() = 0;
};