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
			RecoilResetCallback.Broadcast();
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

FRecoilOffset ABaseShootingWeapon::ShootWithRecoil(const bool IsMoving, const bool IsInAds)
{
	if (!CanShoot())
	{
		return {FVector2D::ZeroVector, FVector2D::ZeroVector};
	}

	// ADS or No ADS
	FVector2D defaultFiringError = DefaultFiringError;
	FVector2D movementFiringError = MovementFiringError;
	int horizontalRecoilStartBullet = HorizontalRecoilStartBullet;
	float hVOffsetAmount = HVOffsetAmount;
	float horizontalOffsetAmount = HorizontalOffsetAmount;
	float horizontalBulletSinMultiplier = HorizontalBulletSinMultiplier;
	float horizontalSinAmplitude = HorizontalSinAmplitude;
	int verticalRecoilStartBullet = VerticalRecoilStartBullet;
	float verticalOffsetAmount = VerticalOffsetAmount;
	UCurveFloat* cameraMultiplierX = CrossHairMultiplierX;
	UCurveFloat* cameraMultiplierY = CrossHairMultiplierY;
	UCurveFloat* crossHairOffsetMultiplierX = RaycastOffsetMultiplierX;
	UCurveFloat* crossHairOffsetMultiplierY = RaycastOffsetMultiplierY;
	if (IsInAds)
	{
		defaultFiringError = AdsDefaultFiringError;
		movementFiringError = AdsMovementFiringError;
		horizontalRecoilStartBullet = AdsHorizontalRecoilStartBullet;
		hVOffsetAmount = AdsHVOffsetAmount;
		horizontalOffsetAmount = AdsHorizontalOffsetAmount;
		horizontalBulletSinMultiplier = AdsHorizontalBulletSinMultiplier;
		horizontalSinAmplitude = AdsHorizontalSinAmplitude;
		verticalRecoilStartBullet = AdsVerticalRecoilStartBullet;
		verticalOffsetAmount = AdsVerticalOffsetAmount;
		cameraMultiplierX = AdsCrossHairMultiplierX;
		cameraMultiplierY = AdsCrossHairMultiplierY;
		crossHairOffsetMultiplierX = AdsRaycastOffsetMultiplierX;
		crossHairOffsetMultiplierY = AdsRaycastOffsetMultiplierY;
	}

	// Default/Movement Firing Error
	const FVector2D firingError = IsMoving ? movementFiringError : defaultFiringError;
	const FVector randomPointInSphere = FMath::VRand();
	FVector2D shootingOffset = FVector2D(randomPointInSphere.X * FMath::RandRange(-firingError.X, firingError.X),
	                                     randomPointInSphere.Z * FMath::RandRange(0.0f, firingError.Y));

	if (_bulletsShot >= horizontalRecoilStartBullet) // Check And Apply Horizontal Recoil
	{
		shootingOffset.Y = firingError.Y + FMath::RandRange(-hVOffsetAmount, hVOffsetAmount);
		shootingOffset.X += horizontalOffsetAmount;

		int horizontalBullets = _bulletsShot - horizontalRecoilStartBullet;
		horizontalBullets *= horizontalBulletSinMultiplier;
		horizontalBullets %= 360;
		const float sinAngle = FMath::DegreesToRadians(horizontalBullets);

		shootingOffset.X *= FMath::Sin(sinAngle) * horizontalSinAmplitude;
	}
	else if (_bulletsShot >= verticalRecoilStartBullet) // Check And Apply Vertical Recoil
	{
		shootingOffset.Y += verticalOffsetAmount;
	}

	// Calculate Camera Offset From Recoil
	FVector2D cameraOffset = FVector2D(shootingOffset.X, -shootingOffset.Y);
	cameraOffset.X *= cameraMultiplierX->GetFloatValue(_bulletsShot);
	cameraOffset.Y *= cameraMultiplierY->GetFloatValue(_bulletsShot);

	// Calculate Offset From CrossHair
	shootingOffset.X *= crossHairOffsetMultiplierX->GetFloatValue(_bulletsShot);
	shootingOffset.Y *= crossHairOffsetMultiplierY->GetFloatValue(_bulletsShot);

	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, "Bullets Shot: " + FString::SanitizeFloat(_bulletsShot));

	_bulletsShot += 1;
	_lastShotTime = UGameplayStatics::GetTimeSeconds(GetWorld());
	_currentRecoilResetTime = RecoilResetTime;

	return {cameraOffset, shootingOffset};
}

void ABaseShootingWeapon::PickupWeapon() const
{
	WeaponCollider->SetSimulatePhysics(false);
	WeaponCollider->SetCollisionProfileName("NoCollision");
}

void ABaseShootingWeapon::DropWeapon()
{
	WeaponCollider->SetCollisionProfileName("BlockAllDynamic");
	WeaponCollider->SetSimulatePhysics(true);

	RecoilResetCallback.Clear();
}
