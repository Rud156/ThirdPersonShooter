// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRDPERSONSHOOTER_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	bool _interactionActive;
	bool _canInteract;
	float _currentInteractionTime;

protected:
	virtual void BeginPlay() override;

public:
#pragma region Properties

	UPROPERTY(Category="Interaction", EditAnywhere)
	float InteractionTime;

	UPROPERTY(Category="Interaction", EditAnywhere)
	bool InitialInteractState;

#pragma endregion

	void StartInteraction();
	void EndInteraction();
	void SetCanInteract(const bool CanInteract);

	float GetInteractionDuration() const;
	float GetCurrentInteractionTime() const;
	bool IsInteractionComplete() const;
	bool IsInteractionActive() const;
	bool CanInteract() const;

	UInteractionComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
