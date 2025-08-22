#include "WeatherXExtendedData.h"
#include "WeatherXBaseData.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraActor.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
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

// Override implementation of CalcTrackedID.
void FWeatherXVolumetricCloudData::CalcTrackedID()
{
	Super::CalcTrackedID();

	if (MPCPath)
	{
		FString MPCStr = MPCPath->GetFName().GetPlainNameString();
		TrackedID.Append(MPCStr);
	}
}

void FWeatherXMaterialData::CalcTrackedID()
{
	Super::CalcTrackedID();

	if (MPCPath)
	{
		FString MPCStr = MPCPath->GetFName().GetPlainNameString();
		TrackedID.Append(MPCStr);
	}
}

// Override implementation of MergeInto.
void FWeatherXMaterialData::MergeInto(const TArray<TSharedPtr<FWeatherXBaseData>>& DataList, TArray<float> RatioList)
{
	Super::MergeInto(DataList, RatioList);

	FWeatherXMaterialData* TempData = static_cast<FWeatherXMaterialData*>(DataList[0].Get());

	MPCPath = TempData->MPCPath;
}

void FWeatherXVolumetricCloudData::MergeInto(const TArray<TSharedPtr<FWeatherXBaseData>>& DataList, TArray<float> RatioList)
{
	Super::MergeInto(DataList, RatioList);

	FWeatherXVolumetricCloudData* TempData = static_cast<FWeatherXVolumetricCloudData*>(DataList[0].Get());

	MPCPath = TempData->MPCPath;
}

// Override implementation of Apply.
void FWeatherXLightData::Apply()
{
	ADirectionalLight* LightActor = Cast<ADirectionalLight>(TrackedInstance);
	if (LightActor)
	{
		UDirectionalLightComponent* LighComponent = LightActor->GetComponent();
		LighComponent->SetIntensity(DirLightIntensity);
		LighComponent->SetLightColor(DirLightColor);
		LighComponent->SetLightSourceAngle(DirLightSourceAngle);
		LighComponent->SetTemperature(DirLightTemoerature);
	}
}

void FWeatherXParticleData::Apply()
{
	ANiagaraActor* InNiagaraActor = Cast<ANiagaraActor>(TrackedInstance);
	if (InNiagaraActor)
	{
		InNiagaraActor->GetNiagaraComponent()->SetFloatParameter(FName("Spawn Rate"), SpawnRate);
	}
}

void FWeatherXMaterialData::Apply()
{
	if (MPCPath)
	{
		AActor* Inst = TrackedInstance.Get();
		
		UKismetMaterialLibrary::SetScalarParameterValue(Inst, MPCPath.Get(), TEXT("RainIntensity"), RainIntensity);
		UKismetMaterialLibrary::SetScalarParameterValue(Inst, MPCPath.Get(), TEXT("SnowIntensity"), FMath::Pow(SnowIntensity, 5.0f));//这里立方是发现线性使变化太过剧烈

		//debug
		//UE_LOG(LogWeatherX, Log, TEXT("---------------------------------------------------"));
		//UE_LOG(LogWeatherX, Log, TEXT("RainIntensity : %f, SnowIntensity : %f"), RainIntensity, SnowIntensity);
	}
}

void FWeatherXSkyAtmosphereData::Apply()
{
	ASkyAtmosphere* SkyAtmosphereActor = Cast<ASkyAtmosphere>(TrackedInstance);
	if (SkyAtmosphereActor)
	{
		USkyAtmosphereComponent* SkyAtmosphereComponent = SkyAtmosphereActor->GetComponent();
		SkyAtmosphereComponent->SetRayleighScattering(RayleighScattering);
		SkyAtmosphereComponent->SetRayleighScatteringScale(RayleighScatteringScale);
	}
}

void FWeatherXVolumetricCloudData::Apply()
{
	if (MPCPath)
	{
		AActor* Inst = TrackedInstance.Get();
		UKismetMaterialLibrary::SetScalarParameterValue(Inst, MPCPath.Get(), TEXT("CloudScale"), CloudScale);
		UKismetMaterialLibrary::SetVectorParameterValue(Inst, MPCPath.Get(), TEXT("CloudColor"), CloudColor);
		//debug
		//UE_LOG(LogWeatherX, Log, TEXT("-----------------------------------------"));
		//UE_LOG(LogWeatherX, Log, TEXT("CloudScale: %f"), CloudScale);
		//UE_LOG(LogWeatherX, Log, TEXT("Cloud Color : %f, %f, %f, %f"), CloudColor.R, CloudColor.G, CloudColor.B, CloudColor.A);
	}
}

void FWeatherXExponentialHeightFogData::Apply()
{
	AExponentialHeightFog* FogActor = Cast<AExponentialHeightFog>(TrackedInstance);
	if (FogActor)
	{
		UExponentialHeightFogComponent* FogComponent = FogActor->GetComponent();
		FogComponent->SetFogDensity(FogDensity);
		FogComponent->SetFogInscatteringColor(FogInscatteringColor);
		FogComponent->SetDirectionalInscatteringColor(DirectionalInscatteringColor);

		//debug
		//UE_LOG(LogWeatherX, Log, TEXT("----------------------------"));
		//UE_LOG(LogWeatherX, Log, TEXT("FogDensity : %f"), FogDensity);
		//UE_LOG(LogWeatherX, Log, TEXT("FogInscatteringColor : %f, %f, %f, %f"), FogInscatteringColor.R, FogInscatteringColor.G, FogInscatteringColor.B, FogInscatteringColor.A);
		//UE_LOG(LogWeatherX, Log, TEXT("DirectionalInscatteringColor : %f, %f, %f, %f"), DirectionalInscatteringColor.R, DirectionalInscatteringColor.G, DirectionalInscatteringColor.B, DirectionalInscatteringColor.A);
	}
}