// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnLocationsController.generated.h"

class APlayerStart;

UCLASS()
class THIRDPERSONSHOOTER_API ASpawnLocationsController : public AActor
{
	GENERATED_BODY()

public:
	ASpawnLocationsController();
	
	AActor* GetValidSpawnPoint() const;
	AActor* GetValidSpawnPointPlayer(AActor* Player) const;
};
