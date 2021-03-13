// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Utils/Structs.h"
#include "BaseShootingWeapon.generated.h"

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

	ATPPlayer* _owningPlayer;

protected:
	virtual void BeginPlay() override;

public:
#pragma region Properties

	UPROPERTY(Category="Weapon|Shoot", EditAnywhere)
	float FireRate;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
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

#pragma endregion

	bool CanShoot() const;
	FRecoilOffset ShootWithRecoil(const bool IsMoving);

	void PickupWeapon(ATPPlayer* OwningPlayer);
	void DropWeapon();

	ABaseShootingWeapon();
	virtual void Tick(float DeltaSeconds) override;
};
