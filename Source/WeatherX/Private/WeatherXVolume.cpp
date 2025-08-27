#include "WeatherXVolume.h"
#include "Components/BrushComponent.h"
#include "WeatherXSubsystem.h"
#include "WeatherXVolumeManager.h"
#include "WeatherXDataPacket.h"
#include "WeatherXLog.h"

#if WITH_EDITOR
#include "WeatherXPreviewSubsystem.h"
#include "ISequencerModule.h"
#endif


AWeatherXVolumeBase::AWeatherXVolumeBase()
{

}

void AWeatherXVolumeBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (HasAnyFlags(RF_Transient))
	{
		return;
	}

	if (!LevelSequencer)
	{
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//Spawn an ALevelSequenceActor and attach it to this weather volume.
		LevelSequencer = World->SpawnActor<ALevelSequenceActor>(GetActorLocation(), GetActorRotation(), SpawnParams);
		LevelSequencer->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void AWeatherXVolumeBase::InitializeWeatherData()
{
	if (!IsTemplate())
	{
		WeatherDatasInVolume.Empty();

		UClass* ObjClass = GetClass();

		bool IsTrackedInstanceValid = false;
		for (TFieldIterator<FProperty> Itt(ObjClass); Itt; ++Itt)
		{
			FStructProperty* StructProperty = CastField<FStructProperty>(*Itt);
			if (StructProperty && StructProperty->Struct->IsChildOf(FWeatherXBaseData::StaticStruct()))
			{
				for (int32 ArrayIdx = 0; ArrayIdx < StructProperty->ArrayDim; ++ArrayIdx)
				{
					WeatherDatasInVolume.Add(StructProperty->ContainerPtrToValuePtr<FWeatherXBaseData>(this, ArrayIdx));
					IsTrackedInstanceValid |= WeatherDatasInVolume[ArrayIdx]->TrackedInstance.IsValid();
				}
			}
		}

		if (!IsTemplate() && !IsTrackedInstanceValid)
		{
			UE_LOG(LogWeatherX, Error, TEXT("The tracked instances of the weather data are not set correctly! Please check the settings of the weather data!"));
		}

		for (FWeatherXBaseData* WeatherData : WeatherDatasInVolume)
		{
			WeatherData->CalcTrackedID();
		}
	}
}

void AWeatherXVolumeBase::BeginPlay()
{
	Super::BeginPlay();

	ActiveRadius = CalcVolumeHalfDiagonalLength() + TransitionWidth;

	UWeatherXVolumeManager* Manager = UWeatherXBaseSubsystem::GetWeatherVolumeManager(GetWorld());

	if(Manager)
	{
		Manager->AddWeatherVolume(this);
	}

	InitializeWeatherData();
}

void AWeatherXVolumeBase::SetBaseParameters(float InOpacity)
{
	for (FWeatherXBaseData* WeatherData : WeatherDatasInVolume)
	{
		if (WeatherData->TrackedInstance.IsValid())
		{
			WeatherData->SetBaseParameters(InOpacity, Weight, Priority);
		}
	}
}

void AWeatherXVolumeBase::InitializePlayer(EWeatherType InitWeatherType)
{
	if (!WeatherType.Contains(InitWeatherType))
	{
		UE_LOG(LogWeatherX, Error, TEXT("The weather events uses weather type that isn't included in this volume! Please assign correct weather type!"));

		return;
	}

	if (WeatherType[InitWeatherType].IsValid())
	{
		ULevelSequence* LevelSequenceAsset = GetSequence(InitWeatherType);
		LevelSequenceAsset = LevelSequenceAsset ? LevelSequenceAsset : LoadSequence(InitWeatherType);

		ULevelSequencePlayer* LevelSequencePlayer = LevelSequencer->GetSequencePlayer();

		if (!LevelSequenceAsset)
		{
			UE_LOG(LogWeatherX, Error, TEXT("This weather type does not have valid level sequencer asset!"));
			return;
		}

		if (LevelSequenceAsset != LevelSequencer->LevelSequenceAsset)
		{
			LevelSequencer->LevelSequenceAsset = WeatherType[InitWeatherType].LoadSynchronous();
			LevelSequencePlayer->Initialize(LevelSequenceAsset, GetLevel(), CameraSettings);
		}	

		for (FWeatherXEvent WeatherEvent : WeatherEventList)
		{
			if (WeatherEvent.WeatherType == InitWeatherType)
			{
				LevelSequencer->LevelSequenceAsset = WeatherType[InitWeatherType].LoadSynchronous();
				TRange<FFrameNumber> Range = LevelSequenceAsset->MovieScene->GetPlaybackRange();
				LevelSequencePlayer->SetFrameRange(Range.GetLowerBoundValue().Value, Range.GetUpperBoundValue().Value);
				return;
			}
		}
		UE_LOG(LogWeatherX, Log, TEXT("A weather type has a corresponding level sequencer asset but isn't used in any weather event!"));
		return;
	}
	UE_LOG(LogWeatherX, Log, TEXT("This weather type does not exist on this weather volume!"));
}

bool AWeatherXVolumeBase::IsActive(const FVector& ViewPos)
{
#if WITH_EDITOR
	ActiveRadius = CalcVolumeHalfDiagonalLength() + TransitionWidth;
#endif

	float DistToVolume = CalcDistToVolumeBoundary(ViewPos);
	bool IsActivated = (FVector::DistSquared(ViewPos, GetActorLocation()) <= (ActiveRadius * ActiveRadius)) && (DistToVolume < TransitionWidth && DistToVolume >= 0.0f);
	
	if (IsActivated)
	{
		// Set opacity based on the distance to the volume.
		if (FMath::IsNearlyEqual(DistToVolume, 0.0f))
		{
			SetBaseParameters(1.0f);
		}
		else
		{
			float Opacity = 1.0f - DistToVolume / TransitionWidth;
			SetBaseParameters(Opacity);
		}
	}
	
	return IsActivated;
}

float AWeatherXVolumeBase::CalcDistToVolumeBoundary(const FVector& ViewPos)
{
	float OutSquaredDist;

	if (GetBrushComponent())
	{
		FVector ClosestPoint;

		if (GetBrushComponent()->GetSquaredDistanceToCollision(ViewPos, OutSquaredDist, ClosestPoint) == false)
		{
			ensureMsgf(false, TEXT("Attempted to get squared distance to collision, but failed!"));
			return -1.0f;
		}

		return FMath::Sqrt(OutSquaredDist);
	}
	else
	{
		ensureMsgf(false, TEXT("Brush Component do not contains a collision!"));
		return -1.0f;
	}
}

void AWeatherXVolumeBase::UpdateEvents(const float CurrentTime)
{
	for (FWeatherXEvent& WeatherEvent : WeatherEventList)
	{
		if(!ShouldUpdateWeatherEvents())
		{
			break;
		}

		// Skip the incident events, since it can only be played by user interfaces.
		if (WeatherEvent.IsIncidentEvent)
		{
			continue;
		}
		
		if ((WeatherEvent.LoopCount < 0 && CurrentTime >= WeatherEvent.StartTime) || (CurrentTime >= WeatherEvent.StartTime && CurrentTime <= (WeatherEvent.StartTime + WeatherEvent.Duration)))
		{
			if(CurrentWeatherEvent != &WeatherEvent && NextWeatherEvent != &WeatherEvent)
			{
				Play(WeatherEvent, EPlayMode::Queue);
			}
		}
	}
}

void AWeatherXVolumeBase::UpdateSequencer(const float CurrentTime)
{
	if(VolumeState == EVolumeState::Playing)
	{
		if (!CurrentWeatherEvent)
		{
			return;
		}

		if (TriggeredTime < 0 || StoppedTime >= 0)
		{
			TriggeredTime = CurrentTime;
			ResetStoppedTime();
		}

		EventStartTime = EventStartTime < 0.0f ? CurrentTime : EventStartTime;
		
		PlaySequencer(CurrentWeatherEvent, CurrentTime);

		if (IsWeatherEventFinished(CurrentTime))
		{
			VolumeState = EVolumeState::Stopping;
		}		
	}
	else if (VolumeState == EVolumeState::Stopping)
	{
		if (!CurrentWeatherEvent)
		{
			return;
		}

		if (StoppedTime < 0)
		{
			StoppedTime = CurrentTime;
		}

		if (StopSequencer(CurrentWeatherEvent, CurrentTime)) // Current Event is stopped.
		{
			if (NextWeatherEvent)
			{
				Play(*NextWeatherEvent, EPlayMode::Preemptive);
				NextWeatherEvent = nullptr;
			}
			else
			{
				ResetVolume();
			}
		}
		
	}
	else // VolumeState == EVolumeState::Idle.
	{
		return;
	}
}

void AWeatherXVolumeBase::Update(const float CurrentTime)
{
	UpdateEvents(CurrentTime);

	UpdateSequencer(CurrentTime);
}

void AWeatherXVolumeBase::ResetTimes()
{
	ResetEventStartTime();
	ResetStoppedTime();
	ResetTriggeredTime();
}

UWeatherXDataPacket* AWeatherXVolumeBase::CollectDataPacket()
{
	UWeatherXDataPacket* DataPacket = NewObject<UWeatherXDataPacket>();

	for (FWeatherXBaseData* WeatherData : WeatherDatasInVolume)
	{
		if (WeatherData->TrackedInstance.IsValid() && static_cast<int32>(CurrentWeatherType))
		{
			UScriptStruct* InStruct = WeatherData->GetScriptStruct();
			FWeatherXBaseData* CopiedData = (FWeatherXBaseData*)FMemory::Malloc(InStruct->GetStructureSize());
			InStruct->InitializeStruct(CopiedData);
			InStruct->CopyScriptStruct(CopiedData, WeatherData);

			DataPacket->DataList.Add(TSharedPtr<FWeatherXBaseData>(CopiedData));
		}
	}

	return DataPacket;
}

//~ Begin user interfaces.
void AWeatherXVolumeBase::Play(FWeatherXEvent& InWeatherEvent, EPlayMode InPlayMode)
{
	if (&InWeatherEvent == nullptr || InPlayMode == EPlayMode::None)
	{
		return;
	}

	if (InPlayMode == EPlayMode::Preemptive)
	{
		CurrentWeatherEvent = &InWeatherEvent;
		ResetTimes();
		VolumeState = EVolumeState::Playing;

		CurrentWeatherType = CurrentWeatherEvent->WeatherType;
		InitializePlayer(CurrentWeatherType);
	}
	else if (InPlayMode == EPlayMode::Queue)
	{
		if (CurrentWeatherEvent)
		{
			if (InWeatherEvent.IsIncidentEvent)
				// If the weather event is an incident event, stop current event and play it.
			{
				NextWeatherEvent = &InWeatherEvent;
				VolumeState = EVolumeState::Stopping;
			}
			else if (!NextWeatherEvent || (NextWeatherEvent && !NextWeatherEvent->IsIncidentEvent))
			{
				NextWeatherEvent = &InWeatherEvent;
				if (VolumeState == EVolumeState::Idle)
				{
					VolumeState = EVolumeState::Playing;
				}
			}
		}
		else
		{
			CurrentWeatherEvent = &InWeatherEvent;
			VolumeState = EVolumeState::Playing;

			CurrentWeatherType = CurrentWeatherEvent->WeatherType;
			InitializePlayer(CurrentWeatherType);
		}
	}
}

void AWeatherXVolumeBase::PlayPreemptively(UPARAM(ref) FWeatherXEvent& InWeatherEvent)
{
	if (!InWeatherEvent.IsIncidentEvent)
	{
		UE_LOG(LogWeatherX, Log, TEXT("WeatherVolumeBase::PlayPreemptively: This weather event cannot be played preemptively since it's not marked as incident event!"));
		return;
	}

	IsVolumeStopped = false;
	Play(InWeatherEvent, EPlayMode::Preemptive);
}

void AWeatherXVolumeBase::PlayInQueue(UPARAM(ref) FWeatherXEvent& InWeatherEvent)
{
	if (!InWeatherEvent.IsIncidentEvent)
	{
		UE_LOG(LogWeatherX, Log, TEXT("AWeatherXVolumeBase::PlayInQueue: This weather event cannot jump in the queue since it's not marked as incident event!"));
		return;
	}

	IsVolumeStopped = false;
	Play(InWeatherEvent, EPlayMode::Queue);
}

FWeatherXEvent& AWeatherXVolumeBase::GetWeatherEvent(int32 InIndex)
{
	check(InIndex < WeatherEventList.Num());

	return WeatherEventList[InIndex];
}

void AWeatherXVolumeBase::FadeOutWeatherEvent()
{
	VolumeState = EVolumeState::Stopping;
}

void AWeatherXVolumeBase::StopWeatherEvent()
{
	ResetVolume();
	ResetTimes();
}

void AWeatherXVolumeBase::StopVolume(float CurrentTime)
{
	if (VolumeState == EVolumeState::Playing)
	{
		VolumeState = EVolumeState::Stopping;
	}
	NextWeatherEvent = nullptr;
	CleanSequence();
	UpdateSequencer(CurrentTime);
	IsVolumeStopped = true;
}
//~ End user interfaces.

/**
* Preview related functions that are only used in the editor mode.
*/
#if WITH_EDITOR

//~ Begin UObject Interface
void AWeatherXVolumeBase::PostLoad()
{
	// Deal with existed volumes loaded from the disk within the editor preview mode.
	Super::PostLoad();

	InitializeWeatherData();
}

void AWeatherXVolumeBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// Deal with the new added volumes within the editor preview mode.
	Super::PostEditChangeProperty(PropertyChangedEvent);

	InitializeWeatherData();
}

void AWeatherXVolumeBase::PostEditImport()
{
	// Allow the object to perform any cleanup for properties which shouldn't be duplicated or are unsupported by the script serialization.
	Super::PostEditImport();

	TArray<AActor*> AttachedSequencer;
	GetAttachedActors(AttachedSequencer);
	if (AttachedSequencer.Num() != 0)
	{
		// Don't copy the LevelSequencer from the parent actor.
		LevelSequencer = Cast<ALevelSequenceActor>(AttachedSequencer[0]);
	}
}

void AWeatherXVolumeBase::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	// Deal with the copied volumes within the editor preview mode.
	Super::PostDuplicate(DuplicateMode);

	InitializeWeatherData();
}
//~ End UObject Interface.

bool AWeatherXVolumeBase::InitializePlayerForPreview(TWeakPtr<ISequencer> Sequencer)
{
	ULevelSequence* LevelSequenceAsset = Cast<ULevelSequence>(Sequencer.Pin()->GetFocusedMovieSceneSequence());

	if (!LevelSequenceAsset)
	{
		return false;
	}

	for (auto Weather : WeatherType)
	{
		// If the weather type on the volume uses the opened sequencer.
		if (Weather.Value.LoadSynchronous() == LevelSequenceAsset)
		{
			ULevelSequencePlayer* LevelSequencePlayer = LevelSequencer->GetSequencePlayer();
			if (LevelSequenceAsset != LevelSequencer->GetSequence())
			{
				LevelSequencePlayer->Initialize(LevelSequenceAsset, GetLevel(), CameraSettings);
			}
			LevelSequencer->LevelSequenceAsset = Weather.Value.LoadSynchronous();

			TRange<FFrameNumber> Range = LevelSequenceAsset->MovieScene->GetPlaybackRange();
			LevelSequencePlayer->SetFrameRange(Range.GetLowerBoundValue().Value, Range.GetUpperBoundValue().Value);

			CurrentWeatherType = Weather.Key;
			return true;
		}
	}

	return false;
}

void AWeatherXVolumeBase::ResetVolume()
{
	CleanSequence();

	CurrentWeatherType = EWeatherType::None;

	CurrentWeatherEvent = nullptr;

	NextWeatherEvent = nullptr;

	VolumeState = EVolumeState::Idle;

	// Reset the level sequencer to the attached empty sequencer.
	TArray<AActor*> AttachedSequencer;
	GetAttachedActors(AttachedSequencer);
	if (AttachedSequencer.Num() != 0)
	{
		LevelSequencer = Cast<ALevelSequenceActor>(AttachedSequencer[0]);
	}
}

void AWeatherXVolumeBase::UpdateForPreview(TWeakPtr<ISequencer> Sequencer, const float CurrentTime)
{
	ULevelSequencePlayer* LevelSequencePlayer = LevelSequencer->GetSequencePlayer();
	UMovieSceneSequence* MovieSequence = LevelSequencePlayer->GetSequence();
	if (!MovieSequence)
	{
		return;
	}

	UMovieScene* MovieScene = MovieSequence->GetMovieScene();
	int32 FramePerSecond = MovieScene->GetDisplayRate().Numerator;

	LevelSequencePlayer->SetPlaybackPosition(FMovieSceneSequencePlaybackParams(CurrentTime, EUpdatePositionMethod::Jump));
	LevelSequencePlayer->Play();
}
#endif//WITH_EDITOR

/**
* Private functions.
*/
ULevelSequence* AWeatherXVolumeBase::GetSequence(EWeatherType InitWeatherType) const
{
	if (WeatherType.Find(InitWeatherType))
	{
		return WeatherType[InitWeatherType].LoadSynchronous();
	}
	else
	{
		return nullptr;
	}
}

ULevelSequence* AWeatherXVolumeBase::LoadSequence(EWeatherType InitWeatherType) const
{
	return WeatherType[InitWeatherType].LoadSynchronous();
}

void AWeatherXVolumeBase::CleanSequence()
{
	if (LevelSequencer)
	{
		LevelSequencer->GetSequencePlayer()->Stop();
	}

	UWorld* InWorld = GetWorld();
	if (InWorld)
	{
		InWorld->GetTimerManager().ClearTimer(WeatherLoopHandle);
	}
}

void AWeatherXVolumeBase::PlaySequencer(const FWeatherXEvent* WeatherEvent, const float CurrentTime)
{
	ULevelSequencePlayer* LevelSequencePlayer = LevelSequencer->GetSequencePlayer();
	if (!LevelSequencer || !WeatherEvent || LevelSequencePlayer->IsPlaying())
	{
		return;
	}

	UMovieSceneSequence* MovieSequence = LevelSequencePlayer->GetSequence();
	if (!MovieSequence)
	{
		return;
	}

	UMovieScene* MovieScene = MovieSequence->GetMovieScene();
	int32 FramePerSecond = MovieScene->GetDisplayRate().Numerator;

	const float StartTransitionTime = float(WeatherEvent->LoopStartFrame - WeatherEvent->StartFrame) / FramePerSecond;
	const float EndTransitionTime = float(WeatherEvent->EndFrame - WeatherEvent->LoopEndFrame) / FramePerSecond;
	const float FullPerformanceTimePerLoop = float(WeatherEvent->LoopEndFrame - WeatherEvent->LoopStartFrame) / FramePerSecond;

	float RelativeCurrentTime = CurrentTime - TriggeredTime;

	float PlayStartTime = 0.0f;

	if (WeatherEvent->LoopCount < 0) // The weather event is in an endless loop.
	{
		float FirstDelay = FullPerformanceTimePerLoop - FMath::Fmod(RelativeCurrentTime - StartTransitionTime, FullPerformanceTimePerLoop);
		PlayStartTime = FullPerformanceTimePerLoop + StartTransitionTime - FirstDelay;

		RemainedLoopCount = -1;

		int32 FrameDuration = LevelSequencePlayer->GetFrameDuration();
		LevelSequencePlayer->SetPlaybackPosition(FMovieSceneSequencePlaybackParams(PlayStartTime, EUpdatePositionMethod::Jump));
		LevelSequencePlayer->Play();
		//Set Timer
		UWorld* InWorld = GetWorld();
		if (InWorld)
		{
			InWorld->GetTimerManager().SetTimer(WeatherLoopHandle, this, &AWeatherXVolumeBase::RollbackSequencerPlayPosition, FullPerformanceTimePerLoop, true, FirstDelay);
			EventToPlayback = *WeatherEvent;
		}

		return;
	}

	int32 FinalLoopCount = FMath::Max(FMath::Min(WeatherEvent->LoopCount, FMath::FloorToInt((WeatherEvent->Duration - StartTransitionTime - EndTransitionTime) / FullPerformanceTimePerLoop)), 0);

	if (RelativeCurrentTime <= StartTransitionTime || WeatherEvent->LoopEndFrame == WeatherEvent->LoopStartFrame || WeatherEvent->LoopCount == 0 || FinalLoopCount == 0)
		// The weather event is at the start transition stage or has no loop stage.
	{
		PlayStartTime = RelativeCurrentTime;
	}
	else
	{
		if (RelativeCurrentTime < (StartTransitionTime + FinalLoopCount * FullPerformanceTimePerLoop))
			// The weather event is at the loop stage.
		{
			int PassedLoopCount = FMath::Max(FMath::FloorToInt((StoppedTime - TriggeredTime - StartTransitionTime) / FullPerformanceTimePerLoop), 0);
			PlayStartTime = RelativeCurrentTime - PassedLoopCount * FullPerformanceTimePerLoop;
		}
		else if (RelativeCurrentTime >= (StartTransitionTime + FinalLoopCount * FullPerformanceTimePerLoop))
			// The weather event is at the end transition stage.
		{
			PlayStartTime = RelativeCurrentTime - (FinalLoopCount - 1) * FullPerformanceTimePerLoop;
		}
	}

	if (RelativeCurrentTime >= FinalLoopCount * FullPerformanceTimePerLoop + StartTransitionTime + EndTransitionTime)
		// The duration of the weather event is run out, stop at the end position.
	{
		PlayStartTime = StartTransitionTime + FullPerformanceTimePerLoop + EndTransitionTime;
	}

	LevelSequencePlayer->SetPlaybackPosition(FMovieSceneSequencePlaybackParams(PlayStartTime, EUpdatePositionMethod::Jump));
	LevelSequencePlayer->Play();

	return;
}

bool AWeatherXVolumeBase::StopSequencer(const FWeatherXEvent* WeatherEvent, const float CurrentTime)
{
	if (TriggeredTime < 0.0f || StoppedTime < 0.0f)
	{
		return false;
	}

	ULevelSequencePlayer* LevelSequencePlayer = LevelSequencer->GetSequencePlayer();
	if (LevelSequencePlayer->IsPlaying())
	{
		return false;
	}

	UMovieSceneSequence* MovieSequence = LevelSequencePlayer->GetSequence();
	if (!MovieSequence)
	{
		return true;
	}
	UMovieScene* MovieScene = MovieSequence->GetMovieScene();
	int32 FramePerSecond = MovieScene->GetDisplayRate().Numerator;

	float StartTransitionTime = float(WeatherEvent->LoopStartFrame - WeatherEvent->StartFrame) / FramePerSecond;
	float EndTransitionTime = float(WeatherEvent->EndFrame - WeatherEvent->LoopEndFrame) / FramePerSecond;
	float FullPerformanceTimePerLoop = float(WeatherEvent->LoopEndFrame - WeatherEvent->LoopStartFrame) / FramePerSecond;

	float RelativeCurrentTime = CurrentTime - TriggeredTime;

	float PlayStartTime = 0.0f;

	int32 FinalLoopCount = FMath::Max(FMath::Min(WeatherEvent->LoopCount, FMath::FloorToInt((WeatherEvent->Duration - StartTransitionTime - EndTransitionTime) / FullPerformanceTimePerLoop)), 1);

	if (RelativeCurrentTime <= StartTransitionTime)
		// Sequencer is at the start transition stage, continue playing.
	{
		PlayStartTime = RelativeCurrentTime;
		LevelSequencePlayer->SetPlaybackPosition(FMovieSceneSequencePlaybackParams(PlayStartTime, EUpdatePositionMethod::Jump));
		LevelSequencePlayer->Play();

		StoppedTime = TriggeredTime + StartTransitionTime;
	}
	else
	{
		if (WeatherEvent->LoopCount < 0 || (WeatherEvent->LoopCount >= 0 && RelativeCurrentTime < (StartTransitionTime + FinalLoopCount * FullPerformanceTimePerLoop)))
			// Sequencer is at the loop stage, finish this loop and then jump to the end transition stage.
		{
			int PassedLoopCount = FMath::Max(FMath::FloorToInt((StoppedTime - TriggeredTime - StartTransitionTime) / FullPerformanceTimePerLoop), 0);
			PlayStartTime = RelativeCurrentTime - PassedLoopCount * FullPerformanceTimePerLoop;
			LevelSequencePlayer->SetPlaybackPosition(FMovieSceneSequencePlaybackParams(PlayStartTime, EUpdatePositionMethod::Jump));
			LevelSequencePlayer->Play();
		}
		else if (WeatherEvent->LoopCount >= 0 && RelativeCurrentTime >= (StartTransitionTime + FinalLoopCount * FullPerformanceTimePerLoop))
			// Sequencer is at the end transition stage, continue playing.
		{
			PlayStartTime = RelativeCurrentTime - (FinalLoopCount - 1) * FullPerformanceTimePerLoop;
			LevelSequencePlayer->SetPlaybackPosition(FMovieSceneSequencePlaybackParams(PlayStartTime, EUpdatePositionMethod::Jump));
			LevelSequencePlayer->Play();
		}
	}

	const UWeatherXSystemSettings* Settings = GetDefault<UWeatherXSystemSettings>();
	float UpdateActiveVolumesInterval = Settings->UpdateActiveVolumesInterval;
	if (PlayStartTime + UpdateActiveVolumesInterval >= StartTransitionTime + FullPerformanceTimePerLoop + EndTransitionTime)
		// Sequencer is ended, reset the triggered time and the stopped time.
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AWeatherXVolumeBase::RollbackSequencerPlayPosition()
{
	RemainedLoopCount--;

	if (RemainedLoopCount == 0)
	{
		UWorld* InWorld = GetWorld();
		if (InWorld)
		{
			InWorld->GetTimerManager().ClearTimer(WeatherLoopHandle);
		}

		return;
	}
	if (RemainedLoopCount < 0)
	{
		RemainedLoopCount = -1;
	}

	ULevelSequencePlayer* LevelSequencePlayer = LevelSequencer->GetSequencePlayer();
	LevelSequencePlayer->SetPlaybackPosition(FMovieSceneSequencePlaybackParams(FFrameTime(EventToPlayback.LoopStartFrame), EUpdatePositionMethod::Jump));
	LevelSequencePlayer->Play();
}

float AWeatherXVolumeBase::CalcVolumeHalfDiagonalLength()
{
	FVector VolumeScale = GetActorScale3D();
	return 100.0f * FMath::Sqrt(VolumeScale.X * VolumeScale.X + VolumeScale.Y * VolumeScale.Y + VolumeScale.Z * VolumeScale.Z);
}

bool AWeatherXVolumeBase::ShouldUpdateWeatherEvents()
{
	// Don't need to update the events if there are two waiting events in queue, or the volume is stopped by the user.
	if ((CurrentWeatherEvent && NextWeatherEvent) || IsVolumeStopped)
	{
		return false;
	}
	
	return true;
}

bool AWeatherXVolumeBase::IsWeatherEventFinished(const float CurrentTime)
{
	const UWeatherXSystemSettings* Settings = GetDefault<UWeatherXSystemSettings>();
	float UpdateActiveVolumesInterval = Settings->UpdateActiveVolumesInterval;
	if ((CurrentWeatherEvent->IsIncidentEvent && CurrentTime - EventStartTime + UpdateActiveVolumesInterval >= CurrentWeatherEvent->Duration) || 
		(!CurrentWeatherEvent->IsIncidentEvent && CurrentWeatherEvent->LoopCount >= 0 && CurrentTime - CurrentWeatherEvent->StartTime + UpdateActiveVolumesInterval >= CurrentWeatherEvent->Duration))
	{
		return true;
	}

	return false;
}
