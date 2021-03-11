// Fill out your copyright notice in the Description page of Project Settings.

#include "./BaseShootingWeapon.h"
#include "../CustomCompoenents/Misc/InteractionComponent.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"

ABaseShootingWeapon::ABaseShootingWeapon()
{
	WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollider"));
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionCompoenent"));

	RootComponent = WeaponRoot;
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponCollider->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
}

void ABaseShootingWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseShootingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
