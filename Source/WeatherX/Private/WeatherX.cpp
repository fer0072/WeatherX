// Copyright Epic Games, Inc. All Rights Reserved.

#include "WeatherX.h"
#include "ISettingsModule.h"
#include "WeatherXSystemSettings.h"
#include "WeatherXLog.h"

#define LOCTEXT_NAMESPACE "FWeatherXModule"

void FWeatherXModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").RegisterSettings(
		"Project",
		"Plugins",
		TEXT("WeatherX"),
		FText::FromString("WeatherX"),
		FText::FromString("Settings for WeatherX"),
		GetMutableDefault<UWeatherXSystemSettings>()
	);
}

void FWeatherXModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").UnregisterSettings("Project", "Plugins", "WeatherX");
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWeatherXModule, WeatherX)
DEFINE_LOG_CATEGORY(LogWeatherX);