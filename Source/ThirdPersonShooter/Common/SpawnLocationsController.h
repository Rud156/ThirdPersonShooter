// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnLocationsController.generated.h"

UCLASS()
class THIRDPERSONSHOOTER_API ASpawnLocationsController : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	ASpawnLocationsController();
	virtual void Tick(float DeltaTime) override;
};
