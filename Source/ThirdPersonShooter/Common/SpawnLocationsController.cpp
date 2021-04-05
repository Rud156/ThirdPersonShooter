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
	TArray<AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), playerStarts);

	TArray<AActor*> players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATPPlayer::StaticClass(), players);

	return CalculateValidSpawnPoint(players, playerStarts);
}

AActor* ASpawnLocationsController::GetValidSpawnPointPlayer(AActor* Player) const
{
	TArray<AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), playerStarts);

	TArray<AActor*> players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATPPlayer::StaticClass(), players);

	for (int i = 0; i < players.Num(); i++)
	{
		if (players[i] == Player)
		{
			players.RemoveAt(i);
			break;
		}
	}

	return CalculateValidSpawnPoint(players, playerStarts);
}

AActor* ASpawnLocationsController::CalculateValidSpawnPoint(TArray<AActor*> Players, TArray<AActor*> PlayerStarts) const
{
	if (Players.Num() > 0)
	{
		FVector centerOfMass = FVector::ZeroVector;
		for (int i = 0; i < Players.Num(); i++)
		{
			const FVector playerPosition = Players[i]->GetActorLocation();
			centerOfMass += playerPosition;
		}
		centerOfMass /= Players.Num();

		TArray<FPlayerSpawnData> playerSpawns;
		for (int i = 0; i < PlayerStarts.Num(); i++)
		{
			const FVector startPosition = PlayerStarts[i]->GetActorLocation();
			const float distance = FVector::Dist(startPosition, centerOfMass);

			FPlayerSpawnData spawnData = {distance, false, PlayerStarts[i]};
			if (distance > MAX_VALID_DISTANCE)
			{
				// Do nothing here...
			}
			else
			{
				const FVector upwardOffset = FVector::UpVector * UP_OFFSET;
				for (int j = 0; j < Players.Num(); j++)
				{
					AActor* player = Players[j];
					const bool los = HasLineOfSight(player, upwardOffset, PlayerStarts[i], upwardOffset);
					if (los)
					{
						spawnData.IsInLineOfSight = true;
						break;
					}
				}
			}

			playerSpawns.Add(spawnData);
		}

		playerSpawns.Sort([](const FPlayerSpawnData& spawnA, const FPlayerSpawnData& spawnB) -> bool
		{
			return spawnA.Distance > spawnB.Distance;
		});

		for (int i = 0; i < playerSpawns.Num(); i++)
		{
			if (!playerSpawns[i].IsInLineOfSight)
			{
				return playerSpawns[i].PlayerStart;
			}
		}

		return playerSpawns[0].PlayerStart;
	}

	return PlayerStarts[FMath::RandHelper(PlayerStarts.Num())];
}

bool ASpawnLocationsController::HasLineOfSight(AActor* StartActor, const FVector StartActorOffset, AActor* TargetActor, const FVector TargetActorOffset,
                                               const float HalfLookAngle) const
{
	// General Collision Params Used for All Collisions
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(StartActor);
	collisionParams.AddIgnoredActor(TargetActor);

	const FVector startLocation = StartActor->GetActorLocation() + StartActorOffset;
	const FVector endLocation = TargetActor->GetActorLocation() + TargetActorOffset;

	FHitResult hitResult;
	const bool hit = GetWorld()->LineTraceSingleByChannel(hitResult, startLocation, endLocation, ECollisionChannel::ECC_Visibility, collisionParams);
	if (hit)
	{
		return false;
	}

	const FVector forwardVector = StartActor->GetActorForwardVector();
	const FVector directionVector = TargetActor->GetActorLocation() - StartActor->GetActorLocation();

	const float angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(forwardVector, directionVector)));
	if (angle > HalfLookAngle)
	{
		return false;
	}

	return true;
}
