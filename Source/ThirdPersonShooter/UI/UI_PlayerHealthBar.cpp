// Fill out your copyright notice in the Description page of Project Settings.


#include "./UI_PlayerHealthBar.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UUI_PlayerHealthBar::SetHealthRatio(const int CurrentHealth, const int MaxHealth) const
{
	const float ratio = static_cast<float>(CurrentHealth) / static_cast<float>(MaxHealth);
	HealthProgress->Percent = ratio;

	const FString currentHealthString = FString::FromInt(CurrentHealth);
	const FString maxHealthString = FString::FromInt(MaxHealth);

	CurrentHealthText->SetText(FText::FromString(currentHealthString));
	MaxHealthText->SetText(FText::FromString(maxHealthString));

	FLinearColor mappedColor;
	if (ratio <= 0.5f)
	{
		mappedColor = FLinearColor::LerpUsingHSV(MinHealthColor, HalfHealthColor, ratio * 2.0f);
	}
	else
	{
		mappedColor = FLinearColor::LerpUsingHSV(HalfHealthColor, FullHealthColor, (ratio - 0.5f) * 2.0f);
	}

	CurrentHealthText->SetColorAndOpacity(FSlateColor(mappedColor));
	MaxHealthText->SetColorAndOpacity(FSlateColor(mappedColor));
}
