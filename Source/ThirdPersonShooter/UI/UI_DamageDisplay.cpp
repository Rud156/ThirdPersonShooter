// Fill out your copyright notice in the Description page of Project Settings.


#include "./UI_DamageDisplay.h"

#include "Components/TextBlock.h"

void UUI_DamageDisplay::SetDamageDone(const int DamageAmount) const
{
	const FString damageString = FString::FromInt(DamageAmount);
	DamageDoneText->SetText(FText::FromString(damageString));
}

void UUI_DamageDisplay::SetBulletsHit(const int BulletsHit) const
{
	const FString bulletsHitString = "( " + FString::FromInt(BulletsHit) + " )";
	BulletsHitText->SetText(FText::FromString(bulletsHitString));
}
