// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TextAsset/Public/TextAsset.h"
#include "../Utils/Structs.h"
#include "BaseShootingWeapon.generated.h"

struct FSortRecoil
{
	bool operator()(const FRecoilOffset A, const FRecoilOffset B) const
	{
		return A.Index < B.Index;
	}
};

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

	float _currentRecoilResetTime;
	int _currentRecoilIndex;

	float _lastShotTime;

	TArray<FRecoilOffset> _recoilOffset;
	void LoadRecoilData(const FText InputRecoilData);

protected:
	virtual void BeginPlay() override;

public:
#pragma region Properties

	UPROPERTY(Category="Weapon|Shoot", EditAnywhere)
	float FireRate;

	UPROPERTY(Category="Weapon|Recoil", BlueprintReadOnly, EditAnywhere)
	UTextAsset* RecoilData;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	float RecoilOffsetMultiplier;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	int MidRecoilIndex;

	UPROPERTY(Category="Weapon|Recoil", EditAnywhere)
	float RecoilResetTime;

#pragma endregion

	bool CanShoot() const;
	FVector2D ShootWithRecoil();

	void PickupWeapon() const;
	void DropWeapon() const;

	ABaseShootingWeapon();
	virtual void Tick(float DeltaSeconds) override;
};
