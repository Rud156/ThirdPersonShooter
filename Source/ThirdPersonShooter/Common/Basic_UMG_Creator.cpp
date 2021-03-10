// Fill out your copyright notice in the Description page of Project Settings.


#include "./Basic_UMG_Creator.h"
#include "../UI/UI_CrossHair.h"
#include "../UI/UI_Interaction.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ABasic_UMG_Creator::ABasic_UMG_Creator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABasic_UMG_Creator::BeginPlay()
{
	Super::BeginPlay();

	const auto playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	auto crossHair = CreateWidget<UUI_CrossHair>(playerController, CrossHairPrefab);
	crossHair->AddToViewport();

	auto interaction = CreateWidget<UUI_Interaction>(playerController, InteractionPrefab);
	interaction->AddToViewport();

	_crossHairWidget = crossHair;
	_interactionWidget = interaction;
}

UUI_CrossHair* ABasic_UMG_Creator::GetCrossHairWidget() const
{
	return _crossHairWidget;
}

UUI_Interaction* ABasic_UMG_Creator::GetInteractionWidget() const
{
	return _interactionWidget;
}
