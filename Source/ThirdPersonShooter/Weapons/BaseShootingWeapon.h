// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Utils/Structs.h"
#include "BaseShootingWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRecoilResetDelegate);

class ATPPlayer;

UCLASS()
class THIRDPERSONSHOOTER_API ABaseShootingWeapon : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(Category="Mesh", VisibleDefaultsOnly)
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(Category="Collider", VisibleDefaultsOnly)
	class UBoxComponent* WeaponCollider;

	UPROPERTY(Category="Interaction", VisibleDefaultsOnly)
	class UInteractionComponent* InteractionComponent;

	int _bulletsShot;

	float _currentRecoilResetTime;
	float _lastShotTime;

protected:
	virtual void BeginPlay() override;

public:
#pragma region Properties

	UPROPERTY(Category="Weapon|Shoot", EditAnywhere)
	float FireRate;

	UPROPERTY(Category="Weapon|Shoot", EditAnywhere)
	float RecoilResetTime;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	FVector2D DefaultFiringError;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	FVector2D MovementFiringError;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	int VerticalRecoilStartBullet;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	float VerticalOffsetAmount;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	int HorizontalRecoilStartBullet;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	float HorizontalOffsetAmount;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	float HorizontalSinAmplitude;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	float HorizontalBulletSinMultiplier;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	float HVOffsetAmount;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	UCurveFloat* CrossHairOffsetMultiplierX;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	UCurveFloat* CrossHairOffsetMultiplierY;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	UCurveFloat* CameraMultiplierX;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	UCurveFloat* CameraMultiplierY;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	FVector2D AdsDefaultFiringError;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	FVector2D AdsMovementFiringError;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	int AdsVerticalRecoilStartBullet;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	float AdsVerticalOffsetAmount;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	int AdsHorizontalRecoilStartBullet;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	float AdsHorizontalOffsetAmount;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	float AdsHorizontalSinAmplitude;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	float AdsHorizontalBulletSinMultiplier;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	float AdsHVOffsetAmount;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	UCurveFloat* AdsCrossHairOffsetMultiplierX;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	UCurveFloat* AdsCrossHairOffsetMultiplierY;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	UCurveFloat* AdsCameraMultiplierX;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	UCurveFloat* AdsCameraMultiplierY;

	FRecoilResetDelegate RecoilResetCallback;

#pragma endregion

	bool CanShoot() const;
	FRecoilOffset ShootWithRecoil(const bool IsMoving, const bool IsInAds);

	void PickupWeapon() const;
	void DropWeapon();

	ABaseShootingWeapon();
	virtual void Tick(float DeltaSeconds) override;
};
