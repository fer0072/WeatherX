#include "WeatherXDataPacket.h"

/* Merge weather data in the same layer, i.e. data has the same priority */
TSharedPtr<FWeatherXBaseData> UWeatherXDataPacket::MergeDataInSingleLayer(const TArray<TSharedPtr<FWeatherXBaseData>>& DataList)
{
	if (DataList.Num() == 1)
	{
		return DataList[0];
	}

	// Calculate blend ratios based on opacity and weight of weather data.
	TArray<float> RatioList;
	float SumOfWeight = 0.0f;
	for (TSharedPtr<FWeatherXBaseData> WeatherData : DataList)
	{
		RatioList.Add(WeatherData->Opacity * WeatherData->Weight);
		SumOfWeight += WeatherData->Opacity * WeatherData->Weight;
	}
	for (float& Ratio : RatioList)
	{
		Ratio /= SumOfWeight;
	}

	// Initialize the merged data.
	UScriptStruct* InStruct = DataList[0]->GetScriptStruct();

	FWeatherXBaseData* WeightedSumOfDataPtr = (FWeatherXBaseData*)FMemory::Malloc(InStruct->GetStructureSize());

	InStruct->InitializeStruct(WeightedSumOfDataPtr);

	WeightedSumOfDataPtr->TrackedID = DataList[0]->TrackedID;

	TSharedPtr<FWeatherXBaseData> WeightedSumOfData = TSharedPtr<FWeatherXBaseData>(WeightedSumOfDataPtr);

	WeightedSumOfData->MergeInto(DataList, RatioList);

	// Calculate final opacity.
	float FinalOpacity = 0.0f;
	for (int32 Idx = 0; Idx < DataList.Num(); Idx++)
	{
		FinalOpacity += DataList[Idx]->Opacity * RatioList[Idx];
	}
	WeightedSumOfData->Opacity = FinalOpacity;

	return WeightedSumOfData;
}

/* Merge weather data across layers, i.e. data has the different priority */
TSharedPtr<FWeatherXBaseData> UWeatherXDataPacket::MergeDataAcrossLayers(TArray<TSharedPtr<FWeatherXBaseData>>& DataList)
{
	if (DataList.Num() == 1)
	{
		return DataList[0];
	}

	DataList.Sort([](const TSharedPtr<FWeatherXBaseData>& A, const TSharedPtr<FWeatherXBaseData>& B) {
		return A->Priority > B->Priority;
		});

	float FinalOpacity = 0.0f;
	TArray<float> RatioList;
	RatioList.SetNumZeroed(DataList.Num());

	float Remnant = 0.0f;
	for (int32 Idx = 0; Idx < DataList.Num(); Idx++)
	{
		Remnant = Idx == 0 ? 1.0f : Remnant - RatioList[Idx - 1];
		RatioList[Idx] = FMath::Max(Remnant * DataList[Idx]->Opacity, 0.0f);
		FinalOpacity += RatioList[Idx];
	}

	// Normalize the ratio.
	for (float& Ratio : RatioList)
	{
		Ratio /= FinalOpacity;
	}

	UScriptStruct* InStruct = DataList[0]->GetScriptStruct();

	FWeatherXBaseData* WeightedSumOfDataPtr = (FWeatherXBaseData*)FMemory::Malloc(InStruct->GetStructureSize());

	InStruct->InitializeStruct(WeightedSumOfDataPtr);

	WeightedSumOfDataPtr->TrackedID = DataList[0]->TrackedID;

	TSharedPtr<FWeatherXBaseData> WeightedSumOfData = TSharedPtr<FWeatherXBaseData>(WeightedSumOfDataPtr);

	WeightedSumOfData->MergeInto(DataList, RatioList);

	WeightedSumOfData->Opacity = FinalOpacity;

	return WeightedSumOfData;
}


UWeatherXDataPacket* UWeatherXDataPacket::MergeDataPacket(TArray<UWeatherXDataPacket*>& DataPackets)
{
	UWeatherXDataPacket* MergedDataPacket = NewObject<UWeatherXDataPacket>();
	
	for (UWeatherXDataPacket* DataPacket : DataPackets)
	{
		MergedDataPacket->DataList.Append(DataPacket->DataList);
	}

	TArray<TSharedPtr<FWeatherXBaseData>>& DataList = MergedDataPacket->DataList;

	TArray<TSharedPtr<FWeatherXBaseData>> NewMergedData;

	TMap<FString, TArray<int32>> IdToIndicesMap;

	TArray<int32> IndexToBeRemovedArray;

	// Classify the weather data based on the tracked instance.
	for (int32 Index = 0; Index < DataList.Num(); Index++)
	{
		if (IdToIndicesMap.Find(DataList[Index]->TrackedID))
		{
			IdToIndicesMap[DataList[Index]->TrackedID].Add(Index);
		}
		else
		{
			TArray<int32> Indices;
			Indices.Add(Index);
			IdToIndicesMap.Add(DataList[Index]->TrackedID, Indices);
		}
	}

	// Merge weather data that controls the same tracked instance.
	for (auto& Elem : IdToIndicesMap)
	{
		TArray<int32>& IndicesArray = Elem.Value;
		IndicesArray.Sort([&DataList](const int32 a, const int32 b) {
			return DataList[a]->Priority > DataList[b]->Priority;
			});

		TArray<TSharedPtr<FWeatherXBaseData>> MergedDataInEachLayer;

		int32 FrontIdx = 0;
		int32 BackIdx = -1;

		// Merge weather data that has the same priority.
		for (; FrontIdx < IndicesArray.Num(); FrontIdx = BackIdx + 1)
		{
			BackIdx = FrontIdx;
			int32 CurrentPriority = DataList[IndicesArray[FrontIdx]]->Priority;

			TArray<TSharedPtr<FWeatherXBaseData>> PendingMergedData;
			PendingMergedData.Add(DataList[IndicesArray[FrontIdx]]);
			while (BackIdx + 1 < IndicesArray.Num() && DataList[IndicesArray[BackIdx + 1]]->Priority == CurrentPriority)
			{
				BackIdx++;
				PendingMergedData.Add(DataList[IndicesArray[BackIdx]]);
			}

			MergedDataInEachLayer.Add(MergeDataInSingleLayer(PendingMergedData));
		}

		// Merge weather data that has different priority.
		NewMergedData.Add(MergeDataAcrossLayers(MergedDataInEachLayer));

		IndexToBeRemovedArray.Append(IndicesArray);
	}

	IndexToBeRemovedArray.Sort([](const int32 a, const int32 b) {
		return a > b;
		});

	for (int32 Idx : IndexToBeRemovedArray)
	{
		DataList.RemoveAt(Idx, 1, EAllowShrinking::No);
	}

	DataList.Shrink();

	DataList.Append(NewMergedData);

	return MergedDataPacket;
}

void UWeatherXDataPacket::Apply()
{
	for (TSharedPtr<FWeatherXBaseData> DataInst : DataList)
	{
		DataInst->Apply();
	}
}