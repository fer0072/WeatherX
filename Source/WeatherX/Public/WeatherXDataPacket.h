#pragma once

#include "CoreMinimal.h"
#include "WeatherXBaseData.h"
#include "WeatherXExtendedData.h"
#include "WeatherXDefinitions.h"
#include "WeatherXDataPacket.generated.h"

UCLASS()
class WEATHERX_API UWeatherXDataPacket : public UObject
{
	GENERATED_BODY()

public:

	TArray<TSharedPtr<FWeatherXBaseData>> DataList;


public:

	static TSharedPtr<FWeatherXBaseData> MergeDataInSingleLayer(const TArray<TSharedPtr<FWeatherXBaseData>>& DataList);

	static TSharedPtr<FWeatherXBaseData> MergeDataAcrossLayers(TArray<TSharedPtr<FWeatherXBaseData>>& DataList);

	static UWeatherXDataPacket* MergeDataPacket(TArray<UWeatherXDataPacket*>& DataPackets);

	void Apply();

};
