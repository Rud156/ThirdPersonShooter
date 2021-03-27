// Fill out your copyright notice in the Description page of Project Settings.


#include "./BasicBot.h"
#include "../CustomCompoenents/Health/HealthAndDamageComponent.h"
#include "../UI/UI_DamageDisplay.h"

#include "Components/WidgetComponent.h"

ABasicBot::ABasicBot()
{
	HealthAndDamage = CreateDefaultSubobject<UHealthAndDamageComponent>(TEXT("HealthAndDamage"));
	DamageWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageWidget"));

	PrimaryActorTick.bCanEverTick = true;
}

void ABasicBot::BeginPlay()
{
	Super::BeginPlay();
	HealthAndDamage->OnDamageTaken.AddDynamic(this, &ABasicBot::HandleDamageTaken);

	_damageUI = Cast<UUI_DamageDisplay>(DamageWidget->GetUserWidgetObject());

	_currentBulletCount = 0;
	_currentDamageTaken = 0;

	_damageUI->SetBulletsHit(_currentBulletCount);
	_damageUI->SetDamageDone(_currentDamageTaken);
}

void ABasicBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_resetTimer > 0)
	{
		_resetTimer -= DeltaTime;
		if (_resetTimer <= 0)
		{
			_currentBulletCount = 0;
			_currentDamageTaken = 0;

			_damageUI->SetBulletsHit(_currentBulletCount);
			_damageUI->SetDamageDone(_currentDamageTaken);
		}
	}
}

void ABasicBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABasicBot::HandleDamageTaken(int DamageAmount)
{
	_resetTimer = DataResetDelay;
	_currentBulletCount += 1;
	_currentDamageTaken += DamageAmount;

	_damageUI->SetBulletsHit(_currentBulletCount);
	_damageUI->SetDamageDone(_currentDamageTaken);
}
