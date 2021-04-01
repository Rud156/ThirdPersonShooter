// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnLocation.generated.h"

UCLASS()
class THIRDPERSONSHOOTER_API ASpawnLocation : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	ASpawnLocation();
	virtual void Tick(float DeltaTime) override;
};
