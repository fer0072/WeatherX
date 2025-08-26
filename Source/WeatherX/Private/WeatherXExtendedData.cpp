#include "WeatherXExtendedData.h"
#include "WeatherXBaseData.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraActor.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/SkyLight.h"
#include "Components/SkyLightComponent.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/Material.h"

// Override implementation of Apply.
void FWeatherXDirectionalLightData::Apply()
{
	if (!TrackedInstance.IsValid())
	{
		UE_LOG(LogWeatherX, Error, TEXT("No Tracked Instance in the WeatherX Directional Light Data! Please Assign Tracked Instance!"));
		
		return;
	}

	if (!DirLightComponent.IsValid())
	{
		ADirectionalLight* DirLightActor = Cast<ADirectionalLight>(TrackedInstance);
		if (DirLightActor)
		{
			DirLightComponent = DirLightActor->GetComponent();
		}
		else
		{
			for (auto Component : TrackedInstance->GetComponents())
			{
				DirLightComponent = Cast<UDirectionalLightComponent>(Component);
				if (DirLightComponent.IsValid() && DirLightComponent->IsVisible()) break;
			}
		}
	}

	if (DirLightComponent.IsValid())
	{
		DirLightComponent->SetIntensity(DirLightIntensity);
		DirLightComponent->SetLightColor(DirLightColor);
		DirLightComponent->SetLightSourceAngle(DirLightSourceAngle);
		DirLightComponent->SetTemperature(DirLightTemoerature);
	}
}

void FWeatherXSkyLightData::Apply()
{
	if (!TrackedInstance.IsValid())
	{
		UE_LOG(LogWeatherX, Error, TEXT("No Tracked Instance in the WeatherX Sky Light Data! Please Assign Tracked Instance!"));

		return;
	}

	if (!SkyLightComponent.IsValid())
	{
		ASkyLight* SkyLightActor = Cast<ASkyLight>(TrackedInstance);
		if (SkyLightActor)
		{
			SkyLightComponent = SkyLightActor->GetLightComponent();
		}
		else
		{
			for (auto Component : TrackedInstance->GetComponents())
			{
				SkyLightComponent = Cast<USkyLightComponent>(Component);
				if (SkyLightComponent.IsValid() && SkyLightComponent->IsVisible()) break;
			}
		}
	}

	if (SkyLightComponent.IsValid())
	{
		SkyLightComponent->SetIntensity(SkyLightIntensity);
	}
}

void FWeatherXSkyAtmosphereData::Apply()
{
	if (!TrackedInstance.IsValid())
	{
		UE_LOG(LogWeatherX, Error, TEXT("No Tracked Instance in the WeatherX Sky Atmosphere Data! Please Assign Tracked Instance!"));

		return;
	}

	if (!SkyAtmosphereComponent.IsValid())
	{
		ASkyAtmosphere* SkyAtmosphereActor = Cast<ASkyAtmosphere>(TrackedInstance);
		if (SkyAtmosphereActor)
		{
			SkyAtmosphereComponent = SkyAtmosphereActor->GetComponent();
		}
		else
		{
			for (auto Component : TrackedInstance->GetComponents())
			{
				SkyAtmosphereComponent = Cast<USkyAtmosphereComponent>(Component);
				if (SkyAtmosphereComponent.IsValid() && SkyAtmosphereComponent->IsVisible()) break;
			}
		}
	}

	if (SkyAtmosphereComponent.IsValid())
	{
		SkyAtmosphereComponent->SetMultiScatteringFactor(MultiScattering);
		SkyAtmosphereComponent->SetRayleighScattering(RayleighScattering);
		SkyAtmosphereComponent->SetMieScatteringScale(MieScatteringScale);
		SkyAtmosphereComponent->SetMieAbsorptionScale(MieAbsorptionScale);
		SkyAtmosphereComponent->SetMieAnisotropy(MieAnisotropy);
		SkyAtmosphereComponent->SetAerialPespectiveViewDistanceScale(AerialPerspectiveViewDistanceScale);
	}
}

void FWeatherXExponentialHeightFogData::Apply()
{
	if (!TrackedInstance.IsValid())
	{
		UE_LOG(LogWeatherX, Error, TEXT("No Tracked Instance in the WeatherX Exponential Height Fog Data! Please Assign Tracked Instance!"));

		return;
	}

	if (!ExponentialHeightFogComponent.IsValid())
	{
		AExponentialHeightFog* ExponentialHeightFogActor = Cast<AExponentialHeightFog>(TrackedInstance);
		if (ExponentialHeightFogActor)
		{
			ExponentialHeightFogComponent = ExponentialHeightFogActor->GetComponent();
		}
		else
		{
			for (auto Component : TrackedInstance->GetComponents())
			{
				ExponentialHeightFogComponent = Cast<UExponentialHeightFogComponent>(Component);
				if (ExponentialHeightFogComponent.IsValid() && ExponentialHeightFogComponent->IsVisible()) break;
			}
		}
	}

	if (ExponentialHeightFogComponent.IsValid())
	{
		ExponentialHeightFogComponent->SetVolumetricFogEmissive(EmissiveColor);
		ExponentialHeightFogComponent->SetVolumetricFogExtinctionScale(ExtinctionScale);
	}
}

void FWeatherXMaterialData::MergeInto(const TArray<TSharedPtr<FWeatherXBaseData>>& DataList, TArray<float> RatioList)
{
	Super::MergeInto(DataList, RatioList);

	FWeatherXMaterialData* TempData = static_cast<FWeatherXMaterialData*>(DataList[0].Get());

	MPCPath = TempData->MPCPath;
}

void FWeatherXMaterialData::Apply()
{
	if (!MPCPath)
	{
		UE_LOG(LogWeatherX, Error, TEXT("No Tracked Instance in the WeatherX Material Data! Please Assign Tracked Material Parameter Collection!"));

		return;
	}

	if (MPCPath)
	{
		UKismetMaterialLibrary::SetScalarParameterValue(TrackedInstance.Get(), MPCPath.Get(), TEXT("IsSnowy"), IsSnowy);
		UKismetMaterialLibrary::SetScalarParameterValue(TrackedInstance.Get(), MPCPath.Get(), TEXT("SnowAmount"), SnowAmount);
		UKismetMaterialLibrary::SetScalarParameterValue(TrackedInstance.Get(), MPCPath.Get(), TEXT("IsRainy"), IsRainy);
		UKismetMaterialLibrary::SetScalarParameterValue(TrackedInstance.Get(), MPCPath.Get(), TEXT("EnableRaindropsPostprocessEffect"), EnableRaindropsPostprocessEffect);
		UKismetMaterialLibrary::SetScalarParameterValue(TrackedInstance.Get(), MPCPath.Get(), TEXT("PuddleSize"), PuddleSize);
	}
}

void FWeatherXMaterialData::CalcTrackedID()
{
	Super::CalcTrackedID();

	BlendMode = EWeatherXBlendMode::Increment;

	if (MPCPath)
	{
		FString MPCStr = MPCPath->GetFName().GetPlainNameString();
		TrackedID.Append(MPCStr);
	}
}
