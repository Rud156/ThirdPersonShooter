// Fill out your copyright notice in the Description page of Project Settings.


#include "./TPPlayerController.h"
#include "./TPPlayer.h"

void ATPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ATPPlayerController::HandleJumpPressed);
	InputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ATPPlayerController::HandleJumpReleased);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ATPPlayerController::HandleSprintPressed);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ATPPlayerController::HandleCrouchPressed);
	InputComponent->BindAction("ShoulderSwap", EInputEvent::IE_Pressed, this, &ATPPlayerController::HandleShoulderSwapPressed);
	InputComponent->BindAction("Dive", EInputEvent::IE_Pressed, this, &ATPPlayerController::HandleDivePressed);
	InputComponent->BindAction("ADS", EInputEvent::IE_Pressed, this, &ATPPlayerController::HandleADSPressed);
	InputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &ATPPlayerController::HandleInteractPressed);
	InputComponent->BindAction("Drop", EInputEvent::IE_Pressed, this, &ATPPlayerController::HandleDropPressed);
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ATPPlayerController::HandleFirePressed);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ATPPlayerController::HandleFireReleased);

	InputComponent->BindAxis("Turn", this, &ATPPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &ATPPlayerController::LookUpRate);
	InputComponent->BindAxis("MoveForward", this, &ATPPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATPPlayerController::MoveRight);
}

void ATPPlayerController::MoveForward(const float Value)
{
	AActor* pawn = GetPawn();
	ATPPlayer* player = Cast<ATPPlayer>(pawn);

	if (player != nullptr)
	{
		player->Client_MoveForward(Value);
	}
}

void ATPPlayerController::MoveRight(const float Value)
{
	AActor* pawn = GetPawn();
	ATPPlayer* player = Cast<ATPPlayer>(pawn);

	if (player != nullptr)
	{
		player->Client_MoveRight(Value);
	}
}

void ATPPlayerController::TurnAtRate(const float Value)
{
	AActor* pawn = GetPawn();
	ATPPlayer* player = Cast<ATPPlayer>(pawn);

	if (player != nullptr)
	{
		player->Client_TurnAtRate(Value);
	}
}

void ATPPlayerController::LookUpRate(const float Value)
{
	AActor* pawn = GetPawn();
	ATPPlayer* player = Cast<ATPPlayer>(pawn);

	if (player != nullptr)
	{
		player->Client_LookUpRate(Value);
	}
}

void ATPPlayerController::HandleJumpPressed()
{
	AActor* pawn = GetPawn();
	ATPPlayer* player = Cast<ATPPlayer>(pawn);

	if (player != nullptr)
	{
		player->Client_HandleJumpPressed();
	}
}

void ATPPlayerController::HandleJumpReleased()
{
	AActor* pawn = GetPawn();
	ATPPlayer* player = Cast<ATPPlayer>(pawn);

	if (player != nullptr)
	{
		player->Client_HandleJumpReleased();
	}
}

void ATPPlayerController::HandleSprintPressed()
{
	AActor* pawn = GetPawn();
	ATPPlayer* player = Cast<ATPPlayer>(pawn);

	if (player != nullptr)
	{
		player->Client_HandleSprintPressed();
	}
}

void ATPPlayerController::HandleCrouchPressed()
{
	AActor* pawn = GetPawn();
	ATPPlayer* player = Cast<ATPPlayer>(pawn);

	if (player != nullptr)
	{
		player->Client_HandleCrouchPressed();
	}
}

void ATPPlayerController::HandleShoulderSwapPressed()
{
	AActor* pawn = GetPawn();
	ATPPlayer* player = Cast<ATPPlayer>(pawn);

	if (player != nullptr)
	{
		player->Client_HandleShoulderSwapPressed();
	}
}

void ATPPlayerController::HandleDivePressed()
{
	AActor* pawn = GetPawn();
	ATPPlayer* player = Cast<ATPPlayer>(pawn);

	if (player != nullptr)
	{
		player->Client_HandleDivePressed();
	}
}

void ATPPlayerController::HandleADSPressed()
{
	AActor* pawn = GetPawn();
	ATPPlayer* player = Cast<ATPPlayer>(pawn);

	if (player != nullptr)
	{
		player->Client_HandleADSPressed();
	}
}

void ATPPlayerController::HandleInteractPressed()
{
	AActor* pawn = GetPawn();
	ATPPlayer* player = Cast<ATPPlayer>(pawn);

	if (player != nullptr)
	{
		player->Client_HandleInteractPressed();
	}
}

void ATPPlayerController::HandleDropPressed()
{
	AActor* pawn = GetPawn();
	ATPPlayer* player = Cast<ATPPlayer>(pawn);

	if (player != nullptr)
	{
		player->Client_HandleDropPressed();
	}
}

void ATPPlayerController::HandleFirePressed()
{
	AActor* pawn = GetPawn();
	ATPPlayer* player = Cast<ATPPlayer>(pawn);

	if (player != nullptr)
	{
		player->Client_HandleFirePressed();
	}
}

void ATPPlayerController::HandleFireReleased()
{
	AActor* pawn = GetPawn();
	ATPPlayer* player = Cast<ATPPlayer>(pawn);

	if (player != nullptr)
	{
		player->Client_HandleFireReleased();
	}
}
