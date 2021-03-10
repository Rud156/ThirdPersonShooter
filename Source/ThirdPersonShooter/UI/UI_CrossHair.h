// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_CrossHair.generated.h"

class UImage;

UCLASS()
class THIRDPERSONSHOOTER_API UUI_CrossHair : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(Category="CrossHair", BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = "true"))
	UImage* TopCH;

	UPROPERTY(Category="CrossHair", BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = "true"))
	UImage* BottomCH;

	UPROPERTY(Category="CrossHair", BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = "true"))
	UImage* LeftCH;

	UPROPERTY(Category="CrossHair", BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = "true"))
	UImage* RightCH;

	UPROPERTY(Category="CrossHair", BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = "true"))
	UImage* CenterDot;

public:
#pragma region Properties

	UPROPERTY(Category="CrossHair", EditAnywhere)
	int LineLength;

	UPROPERTY(Category="CrossHair", EditAnywhere)
	int LineThickness;

	UPROPERTY(Category="CrossHair", EditAnywhere)
	int LineOffset;

	UPROPERTY(Category="CrossHair", EditAnywhere)
	float LineOpacity;

	UPROPERTY(Category="CrossHair", EditAnywhere)
	int CircleRadius;

	UPROPERTY(Category="CrossHair", EditAnywhere)
	float CircleOpacity;

	UPROPERTY(Category="CrossHair", EditAnywhere)
	FSlateColor CrossHairColor;

#pragma endregion

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
