// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_Interaction.generated.h"

class UImage;

UCLASS()
class THIRDPERSONSHOOTER_API UUI_Interaction : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(Category="Interaction", BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = "true"))
	UImage* InteractionRing;

public:
#pragma region Properties

	UPROPERTY(Category="Interaction", EditAnywhere)
	FName AlphaScalarName;

#pragma endregion

	virtual void NativeConstruct() override;
	void SetInteractionProgress(const float Amount) const;
};
