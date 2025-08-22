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
#include "Components/DirectionalLightComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/VolumetricCloudComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/Material.h"

// Override implementation of Apply.
void FWeatherXDirectionalLightData::Apply()
{
	if (!TrackedInstance.IsValid())
	{
		UE_LOG(LogWeatherX, Error, TEXT("No Tracked Instance in the WeatherX Directional Light Data! Please Assign Tracked Instance!"));
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