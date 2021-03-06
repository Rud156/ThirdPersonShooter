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
	const float VERTICAL_RECOIL_OFFSET = 10;
	const float HORIZONTAL_RECOIL_OFFSET = 10;

	UPROPERTY(Category="Mesh", VisibleDefaultsOnly)
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(Category="Collider", VisibleDefaultsOnly)
	class UBoxComponent* WeaponCollider;

	UPROPERTY(Category="Audio", VisibleDefaultsOnly)
	class UAudioComponent* WeaponAudio;

	UPROPERTY(Category="Interaction", VisibleDefaultsOnly)
	class UInteractionComponent* InteractionComponent;

	int _bulletsShot;
	float _currentRecoilResetTime;
	float _lastShotTime;
	float _lastShotRemainderTime;

	ATPPlayer* _currentOwner;

protected:
	virtual void BeginPlay() override;

public:
#pragma region Properties

	UPROPERTY(Category="Weapon|Shoot", EditAnywhere)
	float FireRate;

	UPROPERTY(Category="Weapon|Shoot", EditAnywhere)
	float RecoilResetDelay;

	UPROPERTY(Category="Weapon|Shoot", EditAnywhere)
	UCurveFloat* RecoilResetLerpSpeed;

	UPROPERTY(Category="Weapon|Shoot", EditAnywhere)
	float RecoilShootLerpSpeed;

	UPROPERTY(Category="Weapon|Shoot", EditAnywhere)
	UCurveFloat* RecoilLerpCurve;

	UPROPERTY(Category="Weapon|Damage", EditAnywhere)
	int BodyDamageAmount;

	UPROPERTY(Category="Weapon|Damage", EditAnywhere)
	TArray<FString> BodyBonesName;

	UPROPERTY(Category="Weapon|Damage", EditAnywhere)
	int HeadDamageAmount;

	UPROPERTY(Category="Weapon|Damage", EditAnywhere)
	TArray<FString> HeadBonesName;

	UPROPERTY(Category="Weapon|Damage", EditAnywhere)
	int LegDamageAmount;

	UPROPERTY(Category="Weapon|Damage", EditAnywhere)
	TArray<FString> LegBonesName;

#pragma region Reocil

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	FVector2D DefaultFiringError;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	FVector2D MovementFiringError;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	int HorizontalRecoilStartBullet;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	float HorizontalSinAmplitude;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	float HorizontalBulletSinMultiplier;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	float HVOffsetAmount;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	UCurveFloat* RaycastOffsetMultiplierX;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	UCurveFloat* RaycastOffsetMultiplierY;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	UCurveFloat* CrossHairMultiplierX;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	UCurveFloat* CrossHairMultiplierY;

#pragma endregion

#pragma region ADS Recoil

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	FVector2D AdsDefaultFiringError;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	FVector2D AdsMovementFiringError;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	int AdsHorizontalRecoilStartBullet;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	float AdsHorizontalSinAmplitude;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	float AdsHorizontalBulletSinMultiplier;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	float AdsHVOffsetAmount;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	UCurveFloat* AdsRaycastOffsetMultiplierX;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	UCurveFloat* AdsRaycastOffsetMultiplierY;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	UCurveFloat* AdsCrossHairMultiplierX;

	UPROPERTY(Category="Weapon|RecoilADS", EditAnywhere)
	UCurveFloat* AdsCrossHairMultiplierY;

#pragma endregion

	FRecoilResetDelegate RecoilResetCallback;

#pragma endregion

	USkeletalMeshComponent* GetMesh() const;
	
	int GetShootCountAndSaveRemainder();
	void ShootingSetComplete();
	FRecoilOffset ShootWithRecoil(const bool IsMoving, const bool IsInAds);
	void ResetRecoilData(const int BulletsShot);

	bool PickupWeapon(ATPPlayer* CurrentOwner);
	bool DropWeapon(ATPPlayer* CurrentOwner);

	void ShowWeapon() const;
	void HideWeapon() const;

	void PlayAudio() const;
	int GetCurrentBulletCount() const;
	int GetDamageForBone(const FString BoneName) const;

	ABaseShootingWeapon(const class FObjectInitializer& PCIP);
	virtual void Tick(float DeltaSeconds) override;
};
