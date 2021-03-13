// Fill out your copyright notice in the Description page of Project Settings.

#include "./BaseShootingWeapon.h"
#include "../CustomCompoenents/Misc/InteractionComponent.h"
#include "../Player/TPPlayer.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Misc/DefaultValueHelper.h"

ABaseShootingWeapon::ABaseShootingWeapon()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollider"));
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionCompoenent"));

	RootComponent = WeaponCollider;
	WeaponMesh->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
}

void ABaseShootingWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseShootingWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (_currentRecoilResetTime > 0)
	{
		_currentRecoilResetTime -= DeltaSeconds;

		if (_currentRecoilResetTime <= 0)
		{
			_bulletsShot = 0;

			if (_owningPlayer != nullptr)
			{
				_owningPlayer->ResetPreRecoilCamera();
			}
		}
	}
}

bool ABaseShootingWeapon::CanShoot() const
{
	const float currentTime = UGameplayStatics::GetTimeSeconds(GetWorld());
	const float diff = currentTime - _lastShotTime;

	if (diff > FireRate)
	{
		return true;
	}

	return false;
}

FRecoilOffset ABaseShootingWeapon::ShootWithRecoil(const bool IsMoving)
{
	if (!CanShoot())
	{
		return {FVector2D::ZeroVector, FVector2D::ZeroVector};
	}

	// Default/Movement Firing Error
	const FVector2D firingError = IsMoving ? MovementFiringError : DefaultFiringError;
	const FVector randomPointInSphere = FMath::VRand();
	FVector2D shootingOffset = FVector2D(randomPointInSphere.X * FMath::RandRange(-firingError.X, firingError.X),
	                                     randomPointInSphere.Z * FMath::RandRange(0.0f, firingError.Y));

	if (_bulletsShot >= HorizontalRecoilStartBullet) // Check And Apply Horizontal Recoil
	{
		shootingOffset.Y = firingError.Y + FMath::RandRange(-HVOffsetAmount, HVOffsetAmount);
		shootingOffset.X += HorizontalOffsetAmount;

		int horizontalBullets = _bulletsShot - HorizontalRecoilStartBullet;
		horizontalBullets *= HorizontalBulletSinMultiplier;
		horizontalBullets %= 360;
		const float sinAngle = FMath::DegreesToRadians(horizontalBullets);

		shootingOffset.X *= FMath::Sin(sinAngle) * HorizontalSinAmplitude;
	}
	else if (_bulletsShot >= VerticalRecoilStartBullet) // Check And Apply Vertical Recoil
	{
		shootingOffset.Y += VerticalOffsetAmount;
	}

	// Calculate Camera Offset From Recoil
	FVector2D cameraOffset = FVector2D(shootingOffset.X, -shootingOffset.Y);
	cameraOffset.X *= CameraMultiplierX->GetFloatValue(_bulletsShot);
	cameraOffset.Y *= CameraMultiplierY->GetFloatValue(_bulletsShot);

	// Calculate Offset From CrossHair
	shootingOffset.X *= CrossHairOffsetMultiplierX->GetFloatValue(_bulletsShot);
	shootingOffset.Y *= CrossHairOffsetMultiplierY->GetFloatValue(_bulletsShot);

	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, "Bullets Shot: " + FString::SanitizeFloat(_bulletsShot));

	_bulletsShot += 1;
	_lastShotTime = UGameplayStatics::GetTimeSeconds(GetWorld());
	_currentRecoilResetTime = RecoilResetTime;

	return {cameraOffset, shootingOffset};
}

void ABaseShootingWeapon::PickupWeapon(ATPPlayer* OwningPlayer)
{
	WeaponCollider->SetSimulatePhysics(false);
	WeaponCollider->SetCollisionProfileName("NoCollision");

	_owningPlayer = OwningPlayer;
}

void ABaseShootingWeapon::DropWeapon()
{
	WeaponCollider->SetCollisionProfileName("BlockAllDynamic");
	WeaponCollider->SetSimulatePhysics(true);

	_owningPlayer = nullptr;
}
