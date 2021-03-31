// Fill out your copyright notice in the Description page of Project Settings.


#include "./Basic_UMG_Creator.h"
#include "../UI/UI_CrossHair.h"
#include "../UI/UI_Interaction.h"
#include "../UI/UI_PlayerHealthBar.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"

ABasic_UMG_Creator::ABasic_UMG_Creator(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABasic_UMG_Creator::BeginPlay()
{
	Super::BeginPlay();

	const auto playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (playerController != nullptr)
	{
		auto crossHair = CreateWidget<UUI_CrossHair>(playerController, CrossHairPrefab);
		auto interaction = CreateWidget<UUI_Interaction>(playerController, InteractionPrefab);
		auto playerHealth = CreateWidget<UUI_PlayerHealthBar>(playerController, PlayerHealthPrefab);

		crossHair->AddToViewport();
		interaction->AddToViewport();
		playerHealth->AddToViewport();

		_crossHairWidget = crossHair;
		_interactionWidget = interaction;
		_playerHealthWidget = playerHealth;
	}
}

UUI_CrossHair* ABasic_UMG_Creator::GetCrossHairWidget() const
{
	return _crossHairWidget;
}

UUI_Interaction* ABasic_UMG_Creator::GetInteractionWidget() const
{
	return _interactionWidget;
}

UUI_PlayerHealthBar* ABasic_UMG_Creator::GetPlayerHealthWidget() const
{
	return _playerHealthWidget;
}
