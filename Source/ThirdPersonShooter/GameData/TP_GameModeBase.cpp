// Fill out your copyright notice in the Description page of Project Settings.


#include "./TP_GameModeBase.h"
#include "../Player/TPPlayer.h"
#include "../Player/TPPlayerController.h"
#include "../Common/SpawnLocationsController.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerStart.h"

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

void ATP_GameModeBase::Logout(AController* Exiting)
{
	ATPPlayerController* playerController = Cast<ATPPlayerController>(Exiting);
	if (playerController != nullptr)
	{
		ATPPlayer* player = Cast<ATPPlayer>(playerController->GetPawn());
		if (player != nullptr)
		{
			player->DropCurrentWeapon();
		}
	}
}

AActor* ATP_GameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	AActor* spawnControllerActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnLocationsController::StaticClass());
	if (spawnControllerActor == nullptr)
	{
		spawnControllerActor = GetWorld()->SpawnActor(SpawnControllerPrefab, &FVector::ZeroVector, &FRotator::ZeroRotator);
	}

	ASpawnLocationsController* spawnController = Cast<ASpawnLocationsController>(spawnControllerActor);

	AActor* bestStart = spawnController->GetValidSpawnPoint();
	return bestStart ? bestStart : Super::ChoosePlayerStart_Implementation(Player);
}

AActor* ATP_GameModeBase::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	return Super::FindPlayerStart_Implementation(Player, IncomingName);
}

bool ATP_GameModeBase::ShouldSpawnAtStartSpot(AController* Player)
{
	return Super::ShouldSpawnAtStartSpot(Player);
}

bool ATP_GameModeBase::IsSpawnPointPreferred(APlayerStart* SpawnPoint, AController* Controller)
{
	if (SpawnPoint)
	{
		const FVector spawnLocation = SpawnPoint->GetActorLocation();
		for (TActorIterator<ATPPlayer> iterator(GetWorld()); iterator; ++iterator)
		{
			ATPPlayer* otherPawn = *iterator;
			if (otherPawn)
			{
				const float CombinedHeight = (SpawnPoint->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() +
					otherPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) * 2.0f;
				const float CombinedWidth = SpawnPoint->GetCapsuleComponent()->GetScaledCapsuleRadius() + otherPawn->GetCapsuleComponent()->GetScaledCapsuleRadius();
				const FVector OtherLocation = otherPawn->GetActorLocation();

				// Check if player overlaps the Player Start
				if (FMath::Abs(spawnLocation.Z - OtherLocation.Z) < CombinedHeight && (spawnLocation - OtherLocation).Size2D() < CombinedWidth)
				{
					return false;
				}
			}
		}

		return true;
	}

	return false;
}
