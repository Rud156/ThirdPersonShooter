// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_PlayerHealthBar.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class THIRDPERSONSHOOTER_API UUI_PlayerHealthBar : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(Category="Display", BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess="true"))
	UProgressBar* HealthProgress;

	UPROPERTY(Category="Display", BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess="true"))
	UTextBlock* CurrentHealthText;

	UPROPERTY(Category="Display", BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess="true"))
	UTextBlock* MaxHealthText;

public:
#pragma region Properties

	UPROPERTY(Category="Color", EditAnywhere)
	FLinearColor MinHealthColor;

	UPROPERTY(Category="Color", EditAnywhere)
	FLinearColor MaxHealthColor;

	UPROPERTY(Category="Color", EditAnywhere)
	float MinHealthPercent;

#pragma endregion

	void SetHealthRatio(const int CurrentHealth, const int MaxHealth) const;
};
