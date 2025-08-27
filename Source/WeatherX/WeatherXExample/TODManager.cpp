#include "TODManager.h"
#include "WeatherXSystemSettings.h"
#include "WeatherXSubsystem.h"
#include "WeatherXDataPacket.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/SkyLight.h"
#include "Components/SkyLightComponent.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyAtmosphereComponent.h"

class UDirectionalLightComponent;

ATODManager::ATODManager()
{
	CurrentTimeOfDay = 0.0f;
	TimeUpdateInterval = 0.1f;

	const UWeatherXSystemSettings* Settings = GetDefault<UWeatherXSystemSettings>();
	Priority = Settings->TODPriority;
	Opacity = Settings->TODOpacity;
}

void ATODManager::BeginPlay()
{
	UWorld* InWorld = GetWorld();
	if (InWorld)
	{
		InWorld->GetTimerManager().SetTimer(TimeUpdateHandle, this, &ATODManager::UpdateCurrentTimeOfDay, TimeUpdateInterval, true);

		const UWeatherXSystemSettings* Settings = GetDefault<UWeatherXSystemSettings>();
		Priority = Settings->TODPriority;
		Opacity = Settings->TODOpacity;

		InitializeTODData();
	}
}

#if WITH_EDITOR
void ATODManager::PostLoad()
{
	Super::PostLoad();

	const UWeatherXSystemSettings* Settings = GetDefault<UWeatherXSystemSettings>();
	Priority = Settings->TODPriority;
	Opacity = Settings->TODOpacity;

	InitializeTODData();
}

void ATODManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const UWeatherXSystemSettings* Settings = GetDefault<UWeatherXSystemSettings>();
	Priority = Settings->TODPriority;
	Opacity = Settings->TODOpacity;

	InitializeTODData();
}
#endif	//#if WITH_EDITOR

float ATODManager::GetCurrentTimeOfDay()
{
	return CurrentTimeOfDay;
}

void ATODManager::UpdateCurrentTimeOfDay()
{
	if (!TODActor.IsValid()) return;

	UClass* ActorClass = TODActor->GetClass();
	FProperty* TimeOfDayProperty = ActorClass->FindPropertyByName(*FString("TimeOfDay"));
	if (!TimeOfDayProperty) return;

	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(TimeOfDayProperty))
	{
		CurrentTimeOfDay = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}
}

UWeatherXDataPacket* ATODManager::CollectDataPacket()
{
	UWeatherXDataPacket* DataPacket = NewObject<UWeatherXDataPacket>();

	for (FWeatherXBaseData* WeatherData : WeatherDatasInTOD)
	{
		if (WeatherData->TrackedInstance.IsValid())
		{			
			WeatherData->SetBaseParameters(Opacity, 1.0f, Priority);

			UScriptStruct* InStruct = WeatherData->GetScriptStruct();
			FWeatherXBaseData* CopiedData = (FWeatherXBaseData*)FMemory::Malloc(InStruct->GetStructureSize());
			InStruct->InitializeStruct(CopiedData);
			InStruct->CopyScriptStruct(CopiedData, WeatherData);

			DataPacket->DataList.Add(TSharedPtr<FWeatherXBaseData>(CopiedData));
		}
	}

	return DataPacket;
}

void ATODManager::UpdateTODData()
{
	UClass* ActorClass = TODActor->GetClass();

	// Directional light data.
	FProperty* DirLightIntensityProperty = ActorClass->FindPropertyByName(*FString("DirLightIntensity"));
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(DirLightIntensityProperty))
	{
		DirLightData.DirLightIntensity = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}

	FProperty* DirLightColorProperty = ActorClass->FindPropertyByName(*FString("DirLightColor"));
	if (FStructProperty* StructProperty = CastField<FStructProperty>(DirLightColorProperty))
	{
		DirLightData.DirLightColor = *StructProperty->ContainerPtrToValuePtr<FLinearColor>(TODActor.Get());
	}

	FProperty* DirLightSourceAngleProperty = ActorClass->FindPropertyByName(*FString("DirLightSourceAngle"));
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(DirLightSourceAngleProperty))
	{
		DirLightData.DirLightSourceAngle = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}

	FProperty* DirLightTemperatureProperty = ActorClass->FindPropertyByName(*FString("DirLightTemperature"));
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(DirLightTemperatureProperty))
	{
		DirLightData.DirLightTemperature = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}

	// Sky light data.
	FProperty* SkyLightIntensityProperty = ActorClass->FindPropertyByName(*FString("SkyLightIntensity"));
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(SkyLightIntensityProperty))
	{
		SkyLightData.SkyLightIntensity = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}

	// Sky atmosphere data.
	FProperty* MultiScatteringProperty = ActorClass->FindPropertyByName(*FString("MultiScattering"));
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(MultiScatteringProperty))
	{
		SkyAtmosphereData.MultiScattering = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}

	FProperty* RayleighScatteringProperty = ActorClass->FindPropertyByName(*FString("RayleighScattering"));
	if (FStructProperty* StructProperty = CastField<FStructProperty>(RayleighScatteringProperty))
	{
		SkyAtmosphereData.RayleighScattering = *StructProperty->ContainerPtrToValuePtr<FLinearColor>(TODActor.Get());
	}

	FProperty* MieScatteringScaleProperty = ActorClass->FindPropertyByName(*FString("MieScatteringScale"));
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(MieScatteringScaleProperty))
	{
		SkyAtmosphereData.MieScatteringScale = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}

	FProperty* MieAbsorptionScaleProperty = ActorClass->FindPropertyByName(*FString("MieAbsorptionScale"));
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(MieAbsorptionScaleProperty))
	{
		SkyAtmosphereData.MieAbsorptionScale = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}

	FProperty* MieAnisotropyProperty = ActorClass->FindPropertyByName(*FString("MieAnisotropy"));
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(MieAnisotropyProperty))
	{
		SkyAtmosphereData.MieAnisotropy = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}

	FProperty* AerialPerspectiveViewDistanceScaleProperty = ActorClass->FindPropertyByName(*FString("AerialPerspectiveViewDistanceScale"));
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(AerialPerspectiveViewDistanceScaleProperty))
	{
		SkyAtmosphereData.AerialPerspectiveViewDistanceScale = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}

	// Exponential height fog data.
	FProperty* EmissiveColorProperty = ActorClass->FindPropertyByName(*FString("EmissiveColor"));
	if (FStructProperty* StructProperty = CastField<FStructProperty>(EmissiveColorProperty))
	{
		ExponentialHeightFogData.EmissiveColor = *StructProperty->ContainerPtrToValuePtr<FLinearColor>(TODActor.Get());
	}

	FProperty* ExtinctionScaleProperty = ActorClass->FindPropertyByName(*FString("ExtinctionScale"));
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(ExtinctionScaleProperty))
	{
		ExponentialHeightFogData.ExtinctionScale = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}

	// Material data.
	FProperty* IsSnowyProperty = ActorClass->FindPropertyByName(*FString("IsSnowy"));
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(IsSnowyProperty))
	{
		MaterialData.IsSnowy = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}

	FProperty* SnowAmountProperty = ActorClass->FindPropertyByName(*FString("SnowAmount"));
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(SnowAmountProperty))
	{
		MaterialData.SnowAmount = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}

	FProperty* IsRainyProperty = ActorClass->FindPropertyByName(*FString("IsRainy"));
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(IsRainyProperty))
	{
		MaterialData.IsRainy = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}

	FProperty* EnableRaindropsPostprocessEffectProperty = ActorClass->FindPropertyByName(*FString("EnableRaindropsPostprocessEffect"));
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(EnableRaindropsPostprocessEffectProperty))
	{
		MaterialData.EnableRaindropsPostprocessEffect = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}

	FProperty* PuddleSizeProperty = ActorClass->FindPropertyByName(*FString("PuddleSize"));
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(PuddleSizeProperty))
	{
		MaterialData.PuddleSize = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}

	// Niagara Data.
	FProperty* SpawnRateProperty = ActorClass->FindPropertyByName(*FString("SpawnRate"));
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(SpawnRateProperty))
	{
		NiagaraData.SpawnRate = DoubleProperty->GetPropertyValue_InContainer(TODActor.Get());
	}
}

void ATODManager::InitializeTODData()
{
	WeatherDatasInTOD.Empty();

	if (!TODActor.IsValid())
	{
		UE_LOG(LogWeatherX, Error, TEXT("The tracked dynamic sky is not valid in the TOD manager! Please assign valid actor!"));
		return;
	}

	if (!MPCPath)
	{
		UE_LOG(LogWeatherX, Error, TEXT("The tracked Material Parameter Collection is not valid in the TOD manager! Please assign valid material parameter collection!"));
		return;
	}

	UpdateTODData();

	DirLightData.TrackedInstance = TODActor;
	SkyLightData.TrackedInstance = TODActor;
	SkyAtmosphereData.TrackedInstance = TODActor;
	ExponentialHeightFogData.TrackedInstance = TODActor;
	MaterialData.TrackedInstance = TODActor;
	NiagaraData.TrackedInstance = TODActor;

	MaterialData.MPCPath = MPCPath;
	
	WeatherDatasInTOD.Add(&DirLightData);
	WeatherDatasInTOD.Add(&SkyLightData);
	WeatherDatasInTOD.Add(&SkyAtmosphereData);
	WeatherDatasInTOD.Add(&ExponentialHeightFogData);
	WeatherDatasInTOD.Add(&MaterialData); 
	WeatherDatasInTOD.Add(&NiagaraData);
	

	for (auto WeatherData : WeatherDatasInTOD)
	{
		WeatherData->CalcTrackedID();
	}
}