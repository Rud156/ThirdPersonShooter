// Fill out your copyright notice in the Description page of Project Settings.


#include "./InteractionComponent.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UInteractionComponent::CanInteract() const
{
	return true;
}
