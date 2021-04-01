// Fill out your copyright notice in the Description page of Project Settings.


#include "./SpawnLocationsController.h"

ASpawnLocationsController::ASpawnLocationsController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpawnLocationsController::BeginPlay()
{
	Super::BeginPlay();
}

void ASpawnLocationsController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
