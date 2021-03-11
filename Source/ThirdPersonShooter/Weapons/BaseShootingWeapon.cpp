// Fill out your copyright notice in the Description page of Project Settings.

#include "./BaseShootingWeapon.h"
#include "../CustomCompoenents/Misc/InteractionComponent.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"

ABaseShootingWeapon::ABaseShootingWeapon()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollider"));
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionCompoenent"));

	RootComponent = WeaponCollider;
	WeaponMesh->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = false;
}

void ABaseShootingWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseShootingWeapon::PickupWeapon() const
{
	WeaponCollider->SetCollisionProfileName("NoCollision");
	WeaponCollider->SetSimulatePhysics(false);
}

void ABaseShootingWeapon::DropWeapon() const
{
	WeaponCollider->SetCollisionProfileName("BlockAllDynamic");
	WeaponCollider->SetSimulatePhysics(true);
}
