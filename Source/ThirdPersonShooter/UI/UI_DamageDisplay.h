// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_DamageDisplay.generated.h"

class UTextBlock;

UCLASS()
class THIRDPERSONSHOOTER_API UUI_DamageDisplay : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(Category="Display", BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess="true"))
	UTextBlock* BulletsHitText;

	UPROPERTY(Category="Display", BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess="true"))
	UTextBlock* DamageDoneText;

public:
	void SetDamageDone(const int DamageAmount) const;
	void SetBulletsHit(const int BulletsHit) const;
};
