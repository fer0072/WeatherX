#pragma once

#include "CoreMinimal.h"
#include "WeatherXDefinitions.h"
#include "Engine/VolumeTexture.h"
#include "Materials/MaterialParameterCollection.h"
#include "WeatherXBaseData.generated.h"


USTRUCT(BlueprintType)
struct WEATHERX_API FWeatherXBaseData
{
	GENERATED_BODY()

public:

	FWeatherXBaseData() = default;

	virtual ~FWeatherXBaseData() = default;

	virtual UScriptStruct* GetScriptStruct() const { return FWeatherXBaseData::StaticStruct(); };

	virtual void MergeInto(const TArray<TSharedPtr<FWeatherXBaseData>>& DataList, TArray<float> RatioList);

	void SetBaseParameters(float InOpacity, float InWeight, float InPriority);

	virtual void CalcTrackedID();

	virtual void Apply();
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tracked Actor")
	TWeakObjectPtr<AActor> TrackedInstance;

	int32 Priority = 1;

	float Weight = 1.0f;

	float Opacity = 1.0f;

	FString TrackedID;

	EWeatherXBlendMode BlendMode = EWeatherXBlendMode::Lerp;

	bool ShouldDataBeMerged = true;

	/*
	 * attributes in derived class should be tagged with EditAnywhere for Sequencer to track, e.g:
	 * UPROPERTY(interp, EditAnywhere)
	 * float FogMaxOpacity
	 */
};
