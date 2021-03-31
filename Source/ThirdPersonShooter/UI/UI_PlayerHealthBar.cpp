// Fill out your copyright notice in the Description page of Project Settings.


#include "./UI_PlayerHealthBar.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UUI_PlayerHealthBar::SetHealthRatio(const int CurrentHealth, const int MaxHealth) const
{
	const FString currentHealthString = FString::FromInt(CurrentHealth);
	const FString maxHealthString = FString::FromInt(MaxHealth);
	CurrentHealthText->SetText(FText::FromString(currentHealthString));
	MaxHealthText->SetText(FText::FromString(maxHealthString));

	const float ratio = static_cast<float>(CurrentHealth) / static_cast<float>(MaxHealth);
	HealthProgress->SetPercent(ratio);

	if (ratio <= MinHealthPercent)
	{
		const float newRatio = (1 / MinHealthPercent) * ratio;
		const FLinearColor mappedColor = FLinearColor::LerpUsingHSV(MinHealthColor, MaxHealthColor, newRatio);
		HealthProgress->SetFillColorAndOpacity(mappedColor);
	}
	else
	{
		HealthProgress->SetFillColorAndOpacity(MaxHealthColor);
	}
}
