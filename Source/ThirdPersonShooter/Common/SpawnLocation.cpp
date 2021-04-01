// Fill out your copyright notice in the Description page of Project Settings.


#include "./SpawnLocation.h"

ASpawnLocation::ASpawnLocation()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpawnLocation::BeginPlay()
{
	Super::BeginPlay();
}

void ASpawnLocation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
