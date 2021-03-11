// Fill out your copyright notice in the Description page of Project Settings.

#include "./BaseShootingWeapon.h"
#include "../CustomCompoenents/Misc/InteractionComponent.h"

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
	LoadRecoilData(RecoilData->Text);
}

void ABaseShootingWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (_currentRecoilResetTime > 0)
	{
		_currentRecoilResetTime -= DeltaSeconds;

		if (_currentRecoilResetTime <= 0)
		{
			_currentRecoilIndex = 0;
		}
	}
}

void ABaseShootingWeapon::LoadRecoilData(const FText InputRecoilData)
{
	_recoilOffset = TArray<FRecoilOffset>();

	int rowIndex = 0;
	int columnIndex = 0;
	FString numberString = "";

	const FString recoilString = InputRecoilData.ToString();

	for (int i = 0; i < recoilString.Len(); i++)
	{
		const auto letter = recoilString[i];

		if (letter == '\r')
		{
			rowIndex += 1;
			columnIndex = 0;
			i += 1;
		}
		else if (letter == '\n')
		{
			rowIndex += 1;
			columnIndex = 0;
		}
		else if (letter == ',')
		{
			int outValue = 0;
			const bool parseSuccess = FDefaultValueHelper::ParseInt(numberString, outValue);

			if (parseSuccess)
			{
				FRecoilOffset recoilOffset = {outValue, rowIndex, columnIndex, FVector2D::ZeroVector};
				_recoilOffset.Add(recoilOffset);
			}

			numberString = "";
			columnIndex += 1;
		}
		else
		{
			numberString += letter;
		}
	}

	_recoilOffset.Sort(FSortRecoil());
	for (int i = 1; i < _recoilOffset.Num(); i++)
	{
		const FRecoilOffset lastIndexData = _recoilOffset[i - 1];
		const FRecoilOffset currentIndexData = _recoilOffset[i];

		const int rowDiff = lastIndexData.RowIndex - currentIndexData.RowIndex;
		const int columnDiff = lastIndexData.ColumnIndex - currentIndexData.ColumnIndex;

		const FVector2D offset = FVector2D(rowDiff * RecoilOffsetMultiplier, columnDiff * RecoilOffsetMultiplier);
		_recoilOffset[i].Offset = offset;
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

FVector2D ABaseShootingWeapon::ShootWithRecoil()
{
	if (!CanShoot())
	{
		return FVector2D::ZeroVector;
	}

	const FRecoilOffset recoilOffset = _recoilOffset[_currentRecoilIndex];

	_currentRecoilIndex += 1;
	if (_currentRecoilIndex >= _recoilOffset.Num())
	{
		_currentRecoilIndex = MidRecoilIndex;
	}
	_currentRecoilResetTime = RecoilResetTime;

	_lastShotTime = UGameplayStatics::GetTimeSeconds(GetWorld());
	return recoilOffset.Offset;
}

void ABaseShootingWeapon::PickupWeapon() const
{
	WeaponCollider->SetSimulatePhysics(false);
	WeaponCollider->SetCollisionProfileName("NoCollision");
}

void ABaseShootingWeapon::DropWeapon() const
{
	WeaponCollider->SetCollisionProfileName("BlockAllDynamic");
	WeaponCollider->SetSimulatePhysics(true);
}
