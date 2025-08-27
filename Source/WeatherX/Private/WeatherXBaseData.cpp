#include "Math/UnrealMathUtility.h"
#include "WeatherXBaseData.h"
#include "Kismet/KismetSystemLibrary.h"


namespace WeatherDataBlendProcess
{
	template<class T>
	T IncrementValue(const T& BaseData, const T& IncrementalData, float Ratio)
	{
		return BaseData + IncrementalData * Ratio;
	}

	template<class T>
	void IncrementInternal(const FStructProperty* StructProperty, void* Result, const void* BaseData, const void* IncrementalData, float Ratio)
	{
		for (int32 ArrayIndex = 0; ArrayIndex < StructProperty->ArrayDim; ++ArrayIndex)
		{
			const T* BaseValue = StructProperty->ContainerPtrToValuePtr<T>(BaseData, ArrayIndex);
			const T* IncrementalValue = StructProperty->ContainerPtrToValuePtr<T>(IncrementalData, ArrayIndex);

			T* ResultPtr = StructProperty->ContainerPtrToValuePtr<T>(Result, ArrayIndex);

			T ResultValue = IncrementValue(*BaseValue, *IncrementalValue, Ratio);

			StructProperty->CopySingleValue(ResultPtr, &ResultValue);
		}
	}

	void Increment(const UScriptStruct* InStruct, void* Result, const void* BaseData, const void* IncrementalData, float Ratio)
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
							const void* BasePropertyData = NumericProperty->ContainerPtrToValuePtr<const void>(BaseData, ArrayIndex);
							double BaseValue = NumericProperty->GetFloatingPointPropertyValue(BasePropertyData);

							const void* IncrementalPropertyData = NumericProperty->ContainerPtrToValuePtr<const void>(IncrementalData, ArrayIndex);
							double IncrementalValue = NumericProperty->GetFloatingPointPropertyValue(IncrementalPropertyData);

							void* ResultPropertyData = NumericProperty->ContainerPtrToValuePtr<void>(Result, ArrayIndex);
							double ResultValue = BaseValue + IncrementalValue * Ratio;
							NumericProperty->SetFloatingPointPropertyValue(ResultPropertyData, ResultValue);
						}
					}
					else if (NumericProperty->IsInteger())
					{
						for (int32 ArrayIndex = 0; ArrayIndex < NumericProperty->ArrayDim; ++ArrayIndex)
						{
							const void* BasePropertyData = NumericProperty->ContainerPtrToValuePtr<const void>(BaseData, ArrayIndex);
							int64 BaseValue = NumericProperty->GetFloatingPointPropertyValue(BasePropertyData);

							const void* IncrementalPropertyData = NumericProperty->ContainerPtrToValuePtr<const void>(IncrementalData, ArrayIndex);
							int64 IncrementalValue = NumericProperty->GetFloatingPointPropertyValue(IncrementalPropertyData);

							void* ResultPropertyData = NumericProperty->ContainerPtrToValuePtr<void>(Result, ArrayIndex);
							int64 ResultValue = BaseValue + IncrementalValue * Ratio;
							NumericProperty->SetFloatingPointPropertyValue(ResultPropertyData, ResultValue);
						}
					}
				}
				else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
				{
					if (StructProperty->Struct->GetFName() == NAME_Vector)
					{
						IncrementInternal<FVector>(StructProperty, Result, BaseData, IncrementalData, Ratio);
					}
					else if (StructProperty->Struct->GetFName() == NAME_Vector4)
					{
						IncrementInternal<FVector4>(StructProperty, Result, BaseData, IncrementalData, Ratio);
					}
					else if (StructProperty->Struct->GetFName() == NAME_Rotator)
					{
						IncrementInternal<FRotator>(StructProperty, Result, BaseData, IncrementalData, Ratio);
					}
					else if (StructProperty->Struct->GetFName() == NAME_Quat)
					{
						IncrementInternal<FQuat>(StructProperty, Result, BaseData, IncrementalData, Ratio);
					}
					else if (StructProperty->Struct->GetFName() == NAME_LinearColor)
					{
						IncrementInternal<FLinearColor>(StructProperty, Result, BaseData, IncrementalData, Ratio);
					}
				}
			}
		}
	}

	template<class T>
	T LerpValue(const T& BaseData, const T& InterpolativeData, float Alpha)
	{
		return FMath::Lerp(BaseData, InterpolativeData, Alpha);
	}

	template<class T>
	void LerpInternal(const FStructProperty* StructProperty, void* Result, const void* BaseData, const void* InterpolativeData, float Alpha)
	{
		for (int32 ArrayIndex = 0; ArrayIndex < StructProperty->ArrayDim; ++ArrayIndex)
		{
			const T* BaseValue = StructProperty->ContainerPtrToValuePtr<T>(BaseData, ArrayIndex);
			const T* InterpolativeValue = StructProperty->ContainerPtrToValuePtr<T>(InterpolativeData, ArrayIndex);

			T* ResultPtr = StructProperty->ContainerPtrToValuePtr<T>(Result, ArrayIndex);

			T ResultValue = LerpValue(*BaseValue, *InterpolativeValue, Alpha);

			StructProperty->CopySingleValue(ResultPtr, &ResultValue);
		}
	}

	void Lerp(const UScriptStruct* InStruct, void* Result, const void* BaseData, const void* InterpolativeData, float Alpha)
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
							const void* BasePropertyData = NumericProperty->ContainerPtrToValuePtr<const void>(BaseData, ArrayIndex);
							double BaseValue = NumericProperty->GetFloatingPointPropertyValue(BasePropertyData);

							const void* InterpolativePropertyData = NumericProperty->ContainerPtrToValuePtr<const void>(InterpolativeData, ArrayIndex);
							double InterpolativeValue = NumericProperty->GetFloatingPointPropertyValue(InterpolativePropertyData);

							void* ResultPropertyData = NumericProperty->ContainerPtrToValuePtr<void>(Result, ArrayIndex);
							double ResultValue = FMath::Lerp(BaseValue, InterpolativeValue, Alpha);
							NumericProperty->SetFloatingPointPropertyValue(ResultPropertyData, ResultValue);							
						}
					}
					else if (NumericProperty->IsInteger())
					{
						for (int32 ArrayIndex = 0; ArrayIndex < NumericProperty->ArrayDim; ++ArrayIndex)
						{
							const void* BasePropertyData = NumericProperty->ContainerPtrToValuePtr<const void>(BaseData, ArrayIndex);
							int64 BaseValue = NumericProperty->GetFloatingPointPropertyValue(BasePropertyData);

							const void* InterpolativePropertyData = NumericProperty->ContainerPtrToValuePtr<const void>(InterpolativeData, ArrayIndex);
							int64 InterpolativeValue = NumericProperty->GetFloatingPointPropertyValue(InterpolativePropertyData);

							void* ResultPropertyData = NumericProperty->ContainerPtrToValuePtr<void>(Result, ArrayIndex);
							int64 ResultValue = FMath::Lerp(BaseValue, InterpolativeValue, Alpha);
							NumericProperty->SetFloatingPointPropertyValue(ResultPropertyData, ResultValue);
						}
					}
				}
				else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
				{
					if (StructProperty->Struct->GetFName() == NAME_Vector)
					{
						LerpInternal<FVector>(StructProperty, Result, BaseData, InterpolativeData, Alpha);
					}
					else if (StructProperty->Struct->GetFName() == NAME_Vector4)
					{
						LerpInternal<FVector4>(StructProperty, Result, BaseData, InterpolativeData, Alpha);
					}
					else if (StructProperty->Struct->GetFName() == NAME_Rotator)
					{
						LerpInternal<FRotator>(StructProperty, Result, BaseData, InterpolativeData, Alpha);
					}
					else if (StructProperty->Struct->GetFName() == NAME_Quat)
					{
						LerpInternal<FQuat>(StructProperty, Result, BaseData, InterpolativeData, Alpha);
					}
					else if (StructProperty->Struct->GetFName() == NAME_LinearColor)
					{
						LerpInternal<FLinearColor>(StructProperty, Result, BaseData, InterpolativeData, Alpha);
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

	if (DataList[0]->BlendMode == EWeatherXBlendMode::Lerp)
	{
		WeatherDataBlendProcess::Lerp(InStruct, reinterpret_cast<void*>(this), reinterpret_cast<const void*>(DataList[0].Get()), reinterpret_cast<const void*>(DataList[1].Get()), RatioList[1] / (RatioList[0] + RatioList[1]));
		float UsedAlpha = RatioList[0] + RatioList[1];

		for (int32 Idx = 2; Idx < DataList.Num(); Idx++)
		{
			WeatherDataBlendProcess::Lerp(InStruct, reinterpret_cast<void*>(this), reinterpret_cast<const void*>(this), reinterpret_cast<const void*>(DataList[Idx].Get()), RatioList[Idx] / (RatioList[Idx] + UsedAlpha));
			UsedAlpha += RatioList[Idx];
		}
	}
	else if (DataList[0]->BlendMode == EWeatherXBlendMode::Increment)
	{
		for (int32 Idx = 0; Idx < DataList.Num(); Idx++)
		{
			WeatherDataBlendProcess::Increment(InStruct, reinterpret_cast<void*>(this), reinterpret_cast<void*>(this), reinterpret_cast<const void*>(DataList[Idx].Get()), RatioList[Idx]);
		}
	}
	
}

void FWeatherXBaseData::Apply()
{
	//Base Data has nothing to apply
}