// Fill out your copyright notice in the Description page of Project Settings.


#include "./BasicBot.h"
#include "../CustomCompoenents/Health/HealthAndDamageComponent.h"
#include "../UI/UI_DamageDisplay.h"

#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

ABasicBot::ABasicBot(const class FObjectInitializer& PCIP) : Super(PCIP)
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

	N_CurrentBulletCount = 0;
	N_CurrentDamageTaken = 0;

	_damageUI->SetBulletsHit(N_CurrentBulletCount);
	_damageUI->SetDamageDone(N_CurrentDamageTaken);
}

void ABasicBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_resetTimer > 0)
	{
		_resetTimer -= DeltaTime;
		if (_resetTimer <= 0)
		{
			N_CurrentBulletCount = 0;
			N_CurrentDamageTaken = 0;

			_damageUI->SetBulletsHit(N_CurrentBulletCount);
			_damageUI->SetDamageDone(N_CurrentDamageTaken);
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
	N_CurrentBulletCount += 1;
	N_CurrentDamageTaken += DamageAmount;

	_damageUI->SetBulletsHit(N_CurrentBulletCount);
	_damageUI->SetDamageDone(N_CurrentDamageTaken);
}

void ABasicBot::OnDataFromNetwork() const
{
	_damageUI->SetBulletsHit(N_CurrentBulletCount);
	_damageUI->SetDamageDone(N_CurrentDamageTaken);
}

void ABasicBot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasicBot, N_CurrentBulletCount);
	DOREPLIFETIME(ABasicBot, N_CurrentDamageTaken);
}
