// Fill out your copyright notice in the Description page of Project Settings.


#include "./InteractionComponent.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	SetCanInteract(InitialInteractState);
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (_interactionActive)
	{
		_currentInteractionTime += DeltaTime;
	}
}

void UInteractionComponent::StartInteraction()
{
	if (!_canInteract)
	{
		return;
	}

	_interactionActive = true;
	_currentInteractionTime = 0;
}

void UInteractionComponent::EndInteraction()
{
	_interactionActive = false;
}

void UInteractionComponent::SetCanInteract(const bool CanInteract)
{
	_canInteract = CanInteract;
}

float UInteractionComponent::GetInteractionDuration() const
{
	return InteractionTime;
}

float UInteractionComponent::GetCurrentInteractionTime() const
{
	return _currentInteractionTime;
}

bool UInteractionComponent::IsInteractionComplete() const
{
	return _canInteract && _interactionActive && _currentInteractionTime >= InteractionTime;
}

bool UInteractionComponent::IsInteractionActive() const
{
	return _interactionActive;
}

bool UInteractionComponent::CanInteract() const
{
	return _canInteract;
}
