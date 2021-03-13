// Fill out your copyright notice in the Description page of Project Settings.

#include "./BaseShootingWeapon.h"
#include "../CustomCompoenents/Misc/InteractionComponent.h"
#include "../Player/TPPlayer.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Misc/DefaultValueHelper.h"
#include "ThirdPersonShooter/Player/TPPlayer.h"

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
	                                     randomPointInSphere.Z * FMath::RandRange(-firingError.Y, firingError.Y));

	if (_bulletsShot >= HorizontalRecoilStartBullet) // Check And Apply Horizontal Recoil
	{
		if (_randomLRBulletCounter == 0)
		{
			if (_recoilLeft)
			{
				_randomLRBulletCounter = FMath::RandRange(HorizontalMinLeftBulletCount, HorizontalMaxLeftBulletCount);
			}
			else
			{
				_randomLRBulletCounter = FMath::RandRange(HorizontalMinRightBulletCount, HorizontalMaxRightBulletCount);
			}
		}

		if (_recoilLeft)
		{
			shootingOffset.X += HorizontalOffsetAmount;
		}
		else
		{
			shootingOffset.X -= HorizontalOffsetAmount;
		}

		_randomLRBulletCounter -= 1;
		if (_randomLRBulletCounter <= 0)
		{
			_recoilLeft = !_recoilLeft;
		}
	}
	else if (_bulletsShot >= VerticalRecoilStartBullet) // Check And Apply Vertical Recoil
	{
		shootingOffset.Y += VerticalOffsetAmount;
	}

	// Calculate Camera Offset From Recoil
	const float xOffset = CameraOffsetMultiplierX->GetFloatValue(_bulletsShot) * shootingOffset.X;
	const float yOffset = -CameraOffsetMultiplierY->GetFloatValue(_bulletsShot) * shootingOffset.Y;

	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, "Bullets Shot: " + FString::SanitizeFloat(_bulletsShot));

	_bulletsShot += 1;
	_lastShotTime = UGameplayStatics::GetTimeSeconds(GetWorld());
	_currentRecoilResetTime = RecoilResetTime;

	return {FVector2D(xOffset, yOffset), shootingOffset};
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
