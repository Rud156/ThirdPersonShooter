// Fill out your copyright notice in the Description page of Project Settings.


#include "./UI_Interaction.h"

#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void UUI_Interaction::NativeConstruct()
{
	Super::NativeConstruct();
	SetInteractionProgress(0);
}

void UUI_Interaction::SetInteractionProgress(const float Amount) const
{
	auto ringMaterial = InteractionRing->GetDynamicMaterial();
	if (Amount <= 0 || Amount >= 1)
	{
		ringMaterial->SetScalarParameterValue(AlphaScalarName, 0);
		return;
	}
	
	ringMaterial->SetScalarParameterValue(AlphaScalarName, Amount);
}
