// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnLocationsController.generated.h"

UCLASS()
class THIRDPERSONSHOOTER_API ASpawnLocationsController : public AActor
{
	GENERATED_BODY()
private:
	const float MAX_VALID_DISTANCE = 3000.0f;
	const float UP_OFFSET = 100.0f;

	struct FPlayerSpawnData
	{
		float Distance;
		bool IsInLineOfSight;
		AActor* PlayerStart;
	};

public:
	ASpawnLocationsController();

	AActor* GetValidSpawnPoint() const;
	AActor* GetValidSpawnPointPlayer(AActor* Player) const;
	AActor* CalculateFarthestValidSpawnPoint() const;

	bool HasLineOfSight(AActor* StartActor, const FVector StartActorOffset, AActor* TargetActor, const FVector TargetActorOffset, const float HalfLookAngle = 45) const;
};
