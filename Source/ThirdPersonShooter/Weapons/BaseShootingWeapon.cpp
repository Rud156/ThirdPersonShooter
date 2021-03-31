// Fill out your copyright notice in the Description page of Project Settings.

#include "./BaseShootingWeapon.h"
#include "../CustomCompoenents/Misc/InteractionComponent.h"
#include "../Player/TPPlayer.h"

#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Misc/DefaultValueHelper.h"

#include "Net/UnrealNetwork.h"

ABaseShootingWeapon::ABaseShootingWeapon(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollider"));
	WeaponAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("WeaponAudio"));
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionCompoenent"));

	RootComponent = WeaponCollider;
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponAudio->SetupAttachment(RootComponent);

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

void ABaseShootingWeapon::PlayAudio() const
{
	WeaponAudio->Play();
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
	float horizontalBulletSinMultiplier = HorizontalBulletSinMultiplier;
	float horizontalSinAmplitude = HorizontalSinAmplitude;
	UCurveFloat* crossHairMultiplierX = CrossHairMultiplierX;
	UCurveFloat* crossHairMultiplierY = CrossHairMultiplierY;
	UCurveFloat* raycastOffsetMultiplierX = RaycastOffsetMultiplierX;
	UCurveFloat* raycastOffsetMultiplierY = RaycastOffsetMultiplierY;
	if (IsInAds)
	{
		defaultFiringError = AdsDefaultFiringError;
		movementFiringError = AdsMovementFiringError;
		horizontalRecoilStartBullet = AdsHorizontalRecoilStartBullet;
		hVOffsetAmount = AdsHVOffsetAmount;
		horizontalBulletSinMultiplier = AdsHorizontalBulletSinMultiplier;
		horizontalSinAmplitude = AdsHorizontalSinAmplitude;
		crossHairMultiplierX = AdsCrossHairMultiplierX;
		crossHairMultiplierY = AdsCrossHairMultiplierY;
		raycastOffsetMultiplierX = AdsRaycastOffsetMultiplierX;
		raycastOffsetMultiplierY = AdsRaycastOffsetMultiplierY;
	}

	// Default/Movement Firing Error
	const FVector2D firingError = IsMoving ? movementFiringError : defaultFiringError;
	const FVector randomPointInSphere = FMath::VRand();
	FVector2D rayCastOffset = FVector2D(randomPointInSphere.X * FMath::RandRange(-firingError.X, firingError.X),
	                                    FMath::Abs(randomPointInSphere.Z) * FMath::RandRange(0.0f, firingError.Y));
	FVector2D crossHairOffset = FVector2D::ZeroVector;

	bool multNegative = false;
	if (_bulletsShot >= horizontalRecoilStartBullet) // Check And Apply Horizontal Recoil
	{
		rayCastOffset.Y = firingError.Y + FMath::RandRange(-hVOffsetAmount, hVOffsetAmount);
		rayCastOffset.X = FMath::Abs(rayCastOffset.X);
		rayCastOffset.X += HORIZONTAL_RECOIL_OFFSET;

		int sinBulletAngle = _bulletsShot - horizontalRecoilStartBullet;
		sinBulletAngle *= horizontalBulletSinMultiplier;
		sinBulletAngle %= 360;
		const float sinAngle = FMath::DegreesToRadians(sinBulletAngle);

		rayCastOffset.X *= FMath::Sin(sinAngle) * horizontalSinAmplitude;
		if (sinBulletAngle > 90 && sinBulletAngle < 270)
		{
			multNegative = true;
		}
	}

	// Always add Vertical Recoil. It is ultimately multiplied by the Curve to obtain final value
	rayCastOffset.Y += VERTICAL_RECOIL_OFFSET;
	crossHairOffset.Y += VERTICAL_RECOIL_OFFSET;

	// Always add Horizontal Recoil. It is ultimately multiplied by the Curve to obtain final value
	crossHairOffset.X += HORIZONTAL_RECOIL_OFFSET;

	// Calculate Camera Offset From Recoil
	crossHairOffset.X = FMath::Abs(crossHairOffset.X);
	if (multNegative)
	{
		crossHairOffset.X = -crossHairOffset.X;
	}
	crossHairOffset.Y = -FMath::Abs(crossHairOffset.Y);
	crossHairOffset.X *= crossHairMultiplierX->GetFloatValue(_bulletsShot);
	crossHairOffset.Y *= crossHairMultiplierY->GetFloatValue(_bulletsShot);

	// Calculate Offset From CrossHair
	rayCastOffset.X *= raycastOffsetMultiplierX->GetFloatValue(_bulletsShot);
	rayCastOffset.Y *= raycastOffsetMultiplierY->GetFloatValue(_bulletsShot);

	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, "Bullets Shot: " + FString::SanitizeFloat(_bulletsShot));

	_bulletsShot += 1;
	_currentRecoilResetTime = RecoilResetDelay;
	_lastShotTime = UGameplayStatics::GetTimeSeconds(GetWorld());

	// GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Raycast Offset: " + rayCastOffset.ToString());
	// GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "CrossHair Offset: " + crossHairOffset.ToString());

	return {crossHairOffset, rayCastOffset};
}

void ABaseShootingWeapon::ResetRecoilData(const int BulletsShot)
{
	if (BulletsShot < 0)
	{
		_bulletsShot = 0;
	}
	else
	{
		_bulletsShot = BulletsShot;
	}
}

void ABaseShootingWeapon::PickupWeapon()
{
	WeaponCollider->SetEnableGravity(false);
	WeaponCollider->SetSimulatePhysics(false);
	WeaponCollider->SetCollisionProfileName("NoCollision");
	SetReplicateMovement(false);
}

void ABaseShootingWeapon::DropWeapon()
{
	WeaponCollider->SetCollisionProfileName("BlockAllDynamic");
	WeaponCollider->SetSimulatePhysics(true);
	WeaponCollider->SetEnableGravity(true);

	RecoilResetCallback.Clear();
	SetReplicateMovement(true);
}

void ABaseShootingWeapon::ShowWeapon() const
{
	WeaponMesh->SetHiddenInGame(false);
}

void ABaseShootingWeapon::HideWeapon() const
{
	WeaponMesh->SetHiddenInGame(true);
}

int ABaseShootingWeapon::GetCurrentBulletCount() const
{
	return _bulletsShot;
}

int ABaseShootingWeapon::GetDamageForBone(const FString BoneName) const
{
	for (int i = 0; i < BodyBonesName.Num(); i++)
	{
		if (BoneName == BodyBonesName[i])
		{
			return BodyDamageAmount;
		}
	}

	for (int i = 0; i < HeadBonesName.Num(); i++)
	{
		if (BoneName == HeadBonesName[i])
		{
			return HeadDamageAmount;
		}
	}

	for (int i = 0; i < LegBonesName.Num(); i++)
	{
		if (BoneName == LegBonesName[i])
		{
			return LegDamageAmount;
		}
	}

	return 0;
}

USkeletalMeshComponent* ABaseShootingWeapon::GetMesh() const
{
	return WeaponMesh;
}
