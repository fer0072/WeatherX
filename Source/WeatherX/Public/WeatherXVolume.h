#pragma once
#include "CoreMinimal.h"
#include "WeatherXDataInterface.h"
#include "GameFramework/Volume.h"
#include "WeatherXDefinitions.h"
#include "WeatherXBaseData.h"
#include "WeatherXExtendedData.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlayer.h"
#include "WeatherXVolume.generated.h"

class UWeatherXDataPacket;
class ULevelSequencePlayer;
class ULevelSequence;
class ALevelSequenceActor;

/**
 * Cuboid style weather volume. Used to control weather effect.
 */
UCLASS(Abstract)
class WEATHERX_API AWeatherXVolumeBase : public AVolume, public IWeatherXDataSourceInterface
{
	GENERATED_BODY()

public:
	
	// Map weather type to level sequence asset that records the behaviour of this weather type.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherX Volume Settings", meta = (DisplayName = "Weather Type"))
	TMap<EWeatherType, TSoftObjectPtr<ULevelSequence>> WeatherType;

	// A Weather Event describe how to play the level sequence of the corresponding weather type, including start time, duration, loop times and so on.
	UPROPERTY(EditAnywhere, Category = "WeatherX Volume Settings", meta = (DisplayName = "Weather Events"))
	TArray<FWeatherXEvent> WeatherEventList;

	// The width of transition zone.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherX Volume Settings")
	float TransitionWidth = 100.0f;

	// The weight of this weather volume, used to blend weather effects of overlapping weather volume.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherX Volume Settings")
	float Weight = 1.0f;

	// Priority of this volume, decides whose weather effects is predominant when there are multiple overlapping weather volumes. 
	// The order is undefined if two or more overlapping volumes have the same priority.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeatherX Volume Settings")
	int32 Priority = 2;

private:

	FLevelSequenceCameraSettings CameraSettings;

	// The Level Sequence actor bound to this weather volume, used to playback the sequence assets defined in the TMap WeatherType.
	// Once the weather volume finishes its construction, it spawns a level sequence actor and binds it to the weather volume.
	// VisibleDefaultsOnly prevent users messing up the binding.
	UPROPERTY(VisibleDefaultsOnly, Category = "WeatherX Volume Settings")
	ALevelSequenceActor* LevelSequencer;

	FTimerHandle WeatherLoopHandle;

	int32 RemainedLoopCount;

	FWeatherXEvent EventToPlayback;

	TArray<FWeatherXBaseData*> WeatherDatasInVolume;

	// When distance between view position and the center of volume is larger than the ActiveRadius, this volume would be marked as inactive and would not be updated.
	// ActiveRadius is the sum of the diagonal length of volume and the TransitionWidth;
	float ActiveRadius;

	EWeatherType CurrentWeatherType = EWeatherType::None;

	FWeatherXEvent* CurrentWeatherEvent = nullptr;

	FWeatherXEvent* NextWeatherEvent = nullptr;

	EVolumeState VolumeState = EVolumeState::Idle;

	// Decides if the volume is stopped completely.
	bool IsVolumeStopped = false;

	float EventStartTime = -1.0f;

	float TriggeredTime = -1.0f;

	float StoppedTime = -1.0f;

public:

	AWeatherXVolumeBase();

	virtual void OnConstruction(const FTransform& Transform) override;

	void InitializeWeatherData();

	virtual void BeginPlay() override;

	void SetBaseParameters(float InOpacity);

	// Used to initialize the level sequence player of the level sequence actor */
	void InitializePlayer(EWeatherType InitWeatherType);

	virtual bool IsActive(const FVector& ViewPos);

	virtual float CalcDistToVolumeBoundary(const FVector& ViewPos);

	void CleanSequence();

	void UpdateEvents(const float CurrentTime);

	void UpdateSequencer(const float CurrentTime);

	void Update(const float CurrentTime);

	void ResetTriggeredTime() { TriggeredTime = -1.0f; }

	void ResetStoppedTime() { StoppedTime = -1.0f; }

	void ResetEventStartTime() { EventStartTime = -1.0f; }

	void ResetTimes();

	virtual UWeatherXDataPacket* CollectDataPacket() override;

	EVolumeState GetVolumeState() { return VolumeState; }

	//~ Begin user interfaces.
	void Play(FWeatherXEvent& InWeatherEvent, EPlayMode InPlayMode);

	UFUNCTION(BlueprintCallable, Category = "WeatherX")
	void PlayPreemptively(UPARAM(ref) FWeatherXEvent& InWeatherEvent);

	UFUNCTION(BlueprintCallable, Category = "WeatherX")
	void PlayInQueue(UPARAM(ref) FWeatherXEvent& InWeatherEvent);

	UFUNCTION(BlueprintCallable, Category = "WeatherX")
	void FadeOutWeatherEvent();

	UFUNCTION(BlueprintCallable, Category = "WeatherX")
	void StopWeatherEvent();

	UFUNCTION(BlueprintCallable, Category = "WeatherX")
	void StopVolume(float CurrentTime);

	UFUNCTION(BlueprintCallable, Category = "WeatherX")
	FWeatherXEvent& GetWeatherEvent(int32 InIndex);
	//~ End user interfaces.
	
#if WITH_EDITOR
	//~ Begin UObject Interface.
	virtual void PostLoad() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void PostEditImport() override;

	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
	//~ End UObject Interface.

	//~ Begin editor preview related functions.
	bool InitializePlayerForPreview(TWeakPtr<ISequencer> Sequencer);

	void ResetVolume();

	void UpdateForPreview(TWeakPtr<ISequencer> Sequencer, const float CurrentTime);
	//~ End editor preview related functions.
#endif //WITH_EDITOR

private:
	// Try to get ptr of the level sequence if it has been loaded.
	ULevelSequence* GetSequence(EWeatherType InitWeatherType) const;

	// Load the level sequence asset of the wather type.
	ULevelSequence* LoadSequence(EWeatherType InitWeatherType) const;

	void PlaySequencer(const FWeatherXEvent* WeatherEvent, const float CurrentTime);

	bool StopSequencer(const FWeatherXEvent* WeatherEvent, const float CurrentTime);

	void RollbackSequencerPlayPosition();

	float CalcVolumeHalfDiagonalLength();

	bool ShouldUpdateWeatherEvents();

	bool IsWeatherEventFinished(const float CurrentTime);
};