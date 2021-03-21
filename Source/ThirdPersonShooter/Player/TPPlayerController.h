// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPPlayerController.generated.h"

UCLASS()
class THIRDPERSONSHOOTER_API ATPPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	void MoveForward(const float Value);
	void MoveRight(const float Value);
	void TurnAtRate(const float Value);
	void LookUpRate(const float Value);
	void HandleJumpPressed();
	void HandleJumpReleased();
	void HandleSprintPressed();
	void HandleCrouchPressed();
	void HandleShoulderSwapPressed();
	void HandleDivePressed();
	void HandleADSPressed();
	void HandleInteractPressed();
	void HandleDropPressed();
	void HandleFirePressed();
	void HandleFireReleased();

protected:
	virtual void SetupInputComponent() override;
};
