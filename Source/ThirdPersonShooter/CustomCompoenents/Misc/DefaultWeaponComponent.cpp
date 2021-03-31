// Fill out your copyright notice in the Description page of Project Settings.


#include "./DefaultWeaponComponent.h"
#include "../../Weapons/BaseShootingWeapon.h"
#include "../../Player/TPPlayer.h"

UDefaultWeaponComponent::UDefaultWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDefaultWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	ATPPlayer* player = Cast<ATPPlayer>(GetOwner());

	if (player != nullptr && player->HasAuthority())
	{
		AActor* weapon = GetWorld()->SpawnActor(DefaultWeapon, &FVector::ZeroVector, &FRotator::ZeroRotator);
		ABaseShootingWeapon* shootingWeapon = Cast<ABaseShootingWeapon>(weapon);
		player->PickupWeapon(shootingWeapon);
	}
}

void UDefaultWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
