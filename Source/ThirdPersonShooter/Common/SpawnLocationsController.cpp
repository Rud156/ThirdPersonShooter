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

		TArray<FPlayerSpawnData> playerSpawns;
		for (int i = 0; i < playerStarts.Num(); i++)
		{
			const FVector startPosition = playerStarts[i]->GetActorLocation();
			const float distance = FVector::Dist(startPosition, centerOfMass);

			FPlayerSpawnData spawnData = {distance, false, playerStarts[i]};
			if (distance > MAX_VALID_DISTANCE)
			{
				// Do nothing here...
			}
			else
			{
				const FVector upwardOffset = FVector::UpVector * UP_OFFSET;
				for (int j = 0; j < players.Num(); j++)
				{
					AActor* player = players[j];
					const bool los = HasLineOfSight(player, upwardOffset, playerStarts[i], upwardOffset);
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

	return playerStarts[FMath::RandHelper(playerStarts.Num())];
}

AActor* ASpawnLocationsController::GetValidSpawnPointPlayer(AActor* Player) const
{
	TArray<AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), playerStarts);

	TArray<AActor*> players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATPPlayer::StaticClass(), players);

	if (players.Num() - 1 > 0)
	{
		FVector centerOfMass = FVector::ZeroVector;
		for (int i = 0; i < players.Num(); i++)
		{
			if (players[i] == Player)
			{
				continue;
			}

			const FVector playerPosition = players[i]->GetActorLocation();
			centerOfMass += playerPosition;
		}
		centerOfMass /= (players.Num() - 1);

		TArray<FPlayerSpawnData> playerSpawns;
		for (int i = 0; i < playerStarts.Num(); i++)
		{
			const FVector startPosition = playerStarts[i]->GetActorLocation();
			const float distance = FVector::Dist(startPosition, centerOfMass);

			FPlayerSpawnData spawnData = {distance, false, playerStarts[i]};
			if (distance > MAX_VALID_DISTANCE)
			{
				// Do nothing here...
			}
			else
			{
				const FVector upwardOffset = FVector::UpVector * UP_OFFSET;
				for (int j = 0; j < players.Num(); j++)
				{
					AActor* player = players[j];
					if (player == Player)
					{
						continue;
					}
					
					const bool los = HasLineOfSight(player, upwardOffset, playerStarts[i], upwardOffset);
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

	return playerStarts[FMath::RandHelper(playerStarts.Num())];
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
