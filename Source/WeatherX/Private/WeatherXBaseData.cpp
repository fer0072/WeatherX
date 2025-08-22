#include "WeatherXBaseData.h"
#include "Kismet/KismetSystemLibrary.h"


namespace WeatherDataIncrementationProcess
{
	void Increment(const UScriptStruct* InStruct, void* FinalData, const void* IncrementalData, float Ratio);

	template<class T>
	T IncrementValue(T& BaseData, const T& IncrementalData, float Ratio)
	{
		return BaseData + IncrementalData * Ratio;
	}

	template<>
	FTransform IncrementValue(FTransform& BaseData, const FTransform& IncrementalData, float Ratio)
	{
		if (Ratio < KINDA_SMALL_NUMBER)
		{
			return BaseData;
		}
		else
		{
			BaseData.AccumulateWithShortestRotation(IncrementalData, ScalarRegister(Ratio));
		}
		return BaseData;
	}

	template<class T>
	void IncrementInternal(const FStructProperty* StructProperty, void* FinalData, const void* IncrementalData, float Ratio)
	{
		for (int32 ArrayIndex = 0; ArrayIndex < StructProperty->ArrayDim; ++ArrayIndex)
		{
			const T* IncrementalValue = StructProperty->ContainerPtrToValuePtr<T>(IncrementalData, ArrayIndex);

			T* ValueResultPtr = StructProperty->ContainerPtrToValuePtr<T>(FinalData, ArrayIndex);

			T ValueResult = IncrementValue(*ValueResultPtr, *IncrementalValue, Ratio);

			StructProperty->CopySingleValue(ValueResultPtr, &ValueResult);
		}
	}


	void Increment(const UScriptStruct* InStruct, void* FinalData, const void* IncrementalData, float Ratio)
	{
		for (TFieldIterator<FProperty> Itt(InStruct); Itt; ++Itt)
		{
			FProperty* Property = *Itt;
			if (Property->HasAllPropertyFlags(CPF_Interp))
			{
				if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property))
				{
					if (NumericProperty->IsFloatingPoint())
					{
						for (int32 ArrayIndex = 0; ArrayIndex < NumericProperty->ArrayDim; ++ArrayIndex)
						{
							const void* IncrementalPropertyData = NumericProperty->ContainerPtrToValuePtr<const void>(IncrementalData, ArrayIndex);
							double IncrementalValue = NumericProperty->GetFloatingPointPropertyValue(IncrementalPropertyData);

							void* PropertyDataResult = NumericProperty->ContainerPtrToValuePtr<void>(FinalData, ArrayIndex);
							double ValueResult = NumericProperty->GetFloatingPointPropertyValue(PropertyDataResult);

							ValueResult += IncrementalValue * Ratio;
							NumericProperty->SetFloatingPointPropertyValue(PropertyDataResult, ValueResult);
						}
					}
					else if (NumericProperty->IsInteger())
					{
						for (int32 ArrayIndex = 0; ArrayIndex < NumericProperty->ArrayDim; ++ArrayIndex)
						{
							const void* IncrementalPropertyData = NumericProperty->ContainerPtrToValuePtr<const void>(IncrementalData, ArrayIndex);
							int64 IncrementalValue = NumericProperty->GetSignedIntPropertyValue(IncrementalPropertyData);

							void* PropertyDataResult = NumericProperty->ContainerPtrToValuePtr<void>(FinalData, ArrayIndex);
							int64 ValueResult = NumericProperty->GetSignedIntPropertyValue(PropertyDataResult);

							ValueResult += IncrementalValue * Ratio;
							NumericProperty->SetIntPropertyValue(PropertyDataResult, ValueResult);
						}
					}
				}
				else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
				{
					if (StructProperty->Struct->GetFName() == NAME_Vector)
					{
						IncrementInternal<FVector>(StructProperty, FinalData, IncrementalData, Ratio);
					}
					else if (StructProperty->Struct->GetFName() == NAME_Vector4)
					{
						IncrementInternal<FVector4>(StructProperty, FinalData, IncrementalData, Ratio);
					}
					else if (StructProperty->Struct->GetFName() == NAME_Rotator)
					{
						IncrementInternal<FRotator>(StructProperty, FinalData, IncrementalData, Ratio);
					}
					else if (StructProperty->Struct->GetFName() == NAME_Quat)
					{
						IncrementInternal<FQuat>(StructProperty, FinalData, IncrementalData, Ratio);
					}
					else if (StructProperty->Struct->GetFName() == NAME_Transform)
					{
						IncrementInternal<FTransform>(StructProperty, FinalData, IncrementalData, Ratio);
					}
					else if (StructProperty->Struct->GetFName() == NAME_LinearColor)
					{
						IncrementInternal<FLinearColor>(StructProperty, FinalData, IncrementalData, Ratio);
					}
				}
			}
		}
	}
}

void FWeatherXBaseData::SetBaseParameters(float InOpacity, float InWeight, float InPriority)
{
	Opacity = InOpacity;
	Weight = InWeight;
	Priority = InPriority;
}

void FWeatherXBaseData::CalcTrackedID()
{
	TrackedID.Reset();
	
	UScriptStruct* InStruct = GetScriptStruct();
	
	FString TypeStr = InStruct->GetFName().GetPlainNameString();

	FString TrackedInstStr = UKismetSystemLibrary::GetObjectName(TrackedInstance.Get());

	TrackedID.Append(TypeStr);
	TrackedID.Append(TrackedInstStr);
}


void FWeatherXBaseData::MergeInto(const TArray<TSharedPtr<FWeatherXBaseData>>& DataList, TArray<float> RatioList)
{
	UScriptStruct* InStruct = DataList[0]->GetScriptStruct();

	TrackedInstance = DataList[0]->TrackedInstance;
	
	Priority = DataList[0]->Priority;

	for (int32 Idx = 0; Idx < DataList.Num(); Idx++)
	{
		WeatherDataIncrementationProcess::Increment(InStruct, reinterpret_cast<void*>(this), reinterpret_cast<const void*>(DataList[Idx].Get()), RatioList[Idx]);
	}

}

void FWeatherXBaseData::Apply()
{
	//Base Data has nothing to apply
}