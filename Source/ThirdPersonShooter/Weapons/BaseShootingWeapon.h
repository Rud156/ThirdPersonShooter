// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseShootingWeapon.generated.h"

UCLASS()
class THIRDPERSONSHOOTER_API ABaseShootingWeapon : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(Category="Mesh", VisibleDefaultsOnly)
	class USceneComponent* WeaponRoot;

	UPROPERTY(Category="Mesh", VisibleDefaultsOnly)
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(Category="Collider", VisibleDefaultsOnly)
	class UBoxComponent* WeaponCollider;

	UPROPERTY(Category="Interaction", VisibleDefaultsOnly)
	class UInteractionComponent* InteractionComponent;

protected:
	virtual void BeginPlay() override;

public:
	ABaseShootingWeapon();
	virtual void Tick(float DeltaTime) override;
};
