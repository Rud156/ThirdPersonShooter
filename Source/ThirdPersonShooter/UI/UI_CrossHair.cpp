// Fill out your copyright notice in the Description page of Project Settings.


#include "./UI_CrossHair.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

void UUI_CrossHair::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	// Set Color
	TopCH->SetBrushTintColor(CrossHairColor);
	BottomCH->SetBrushTintColor(CrossHairColor);
	LeftCH->SetBrushTintColor(CrossHairColor);
	RightCH->SetBrushTintColor(CrossHairColor);
	CenterDot->SetBrushTintColor(CrossHairColor);

	// Set Center Dot
	CenterDot->SetOpacity(CircleOpacity);
	auto conCenterDot = Cast<UCanvasPanelSlot>(CenterDot->Slot);
	const float centerDotPosition = -CircleRadius / 2.0f;
	conCenterDot->SetSize(FVector2D(CircleRadius, CircleRadius));
	conCenterDot->SetPosition(FVector2D(centerDotPosition, centerDotPosition));

	// Line Opacity
	TopCH->SetOpacity(LineOpacity);
	BottomCH->SetOpacity(LineOpacity);
	LeftCH->SetOpacity(LineOpacity);
	RightCH->SetOpacity(LineOpacity);

	// Line Size and Position
	auto topLine = Cast<UCanvasPanelSlot>(TopCH->Slot);
	auto bottomLine = Cast<UCanvasPanelSlot>(BottomCH->Slot);
	auto leftLine = Cast<UCanvasPanelSlot>(LeftCH->Slot);
	auto rightLine = Cast<UCanvasPanelSlot>(RightCH->Slot);

	// Size
	topLine->SetSize(FVector2D(LineLength, LineThickness));
	bottomLine->SetSize(FVector2D(LineLength, LineThickness));
	leftLine->SetSize(FVector2D(LineThickness, LineLength));
	rightLine->SetSize(FVector2D(LineThickness, LineLength));

	// Position
	const float modifiedThickness = -(LineLength / 2.0f);
	topLine->SetPosition(FVector2D(modifiedThickness, -(LineThickness + LineOffset)));
	bottomLine->SetPosition(FVector2D(modifiedThickness, LineOffset));
	leftLine->SetPosition(FVector2D(-(LineThickness + LineOffset), modifiedThickness));
	rightLine->SetPosition(FVector2D(LineOffset, modifiedThickness));
}
