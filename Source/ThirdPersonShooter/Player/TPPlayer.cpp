// Fill out your copyright notice in the Description page of Project Settings.


#include "./TPPlayer.h"

ATPPlayer::ATPPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATPPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void ATPPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATPPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}