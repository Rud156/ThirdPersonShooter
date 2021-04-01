// Fill out your copyright notice in the Description page of Project Settings.


#include "./SpawnLocationsController.h"
#include "../Player/TPPlayer.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ASpawnLocationsController::ASpawnLocationsController()
{
	PrimaryActorTick.bCanEverTick = true;
}

AActor* ASpawnLocationsController::GetValidSpawnPoint() const
{
	TArray<APlayerStart*> preferredSpawns;

	TArray<AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), playerStarts);

	TArray<AActor*> players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATPPlayer::StaticClass(), players);

	if (players.Num() > 0)
	{
		FVector centerOfMass = FVector::ZeroVector;
		for (int i = 0; i < players.Num(); i++)
		{
			const FVector playerPosition = players[i]->GetActorLocation();
			centerOfMass += playerPosition;
		}
		centerOfMass /= players.Num();

		float maxDistance = TNumericLimits<float>::Min();
		AActor* farthestPlayerStart = nullptr;
		for (int i = 0; i < playerStarts.Num(); i++)
		{
			const FVector location = playerStarts[i]->GetActorLocation();
			const float distance = FVector::Dist(location, centerOfMass);

			if (distance > maxDistance)
			{
				maxDistance = distance;
				farthestPlayerStart = playerStarts[i];
			}
		}
		preferredSpawns.Add(Cast<APlayerStart>(farthestPlayerStart));
	}
	else
	{
		for (int i = 0; i < playerStarts.Num(); i++)
		{
			preferredSpawns.Add(Cast<APlayerStart>(playerStarts[i]));
		}
	}

	APlayerStart* bestStart = nullptr;
	if (preferredSpawns.Num() > 0)
	{
		bestStart = preferredSpawns[FMath::RandHelper(preferredSpawns.Num())];
	}

	UE_LOG(LogTemp, Warning, TEXT("Hello World"));
	return bestStart;
}
