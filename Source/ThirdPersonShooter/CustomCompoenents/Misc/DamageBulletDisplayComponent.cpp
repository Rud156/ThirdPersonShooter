// Fill out your copyright notice in the Description page of Project Settings.


#include "./DamageBulletDisplayComponent.h"
#include "../Health/HealthAndDamageComponent.h"
#include "../../UI/UI_DamageDisplay.h"

#include "Net/UnrealNetwork.h"

UDamageBulletDisplayComponent::UDamageBulletDisplayComponent(const FObjectInitializer& PCIP) : Super(PCIP)
{
	SetWidgetClass(UUI_DamageDisplay::StaticClass());
}

void UDamageBulletDisplayComponent::BeginPlay()
{
	Super::BeginPlay();

	UActorComponent* actorComponent = GetOwner()->GetComponentByClass(UHealthAndDamageComponent::StaticClass());
	if (actorComponent != nullptr)
	{
		UHealthAndDamageComponent* healthAndDamageComp = Cast<UHealthAndDamageComponent>(actorComponent);
		healthAndDamageComp->OnDamageTaken.AddDynamic(this, &UDamageBulletDisplayComponent::HandleDamageTaken);

		_damageUI = Cast<UUI_DamageDisplay>(GetUserWidgetObject());

		N_CurrentBulletCount = 0;
		N_CurrentDamageTaken = 0;

		_damageUI->SetBulletsHit(N_CurrentBulletCount);
		_damageUI->SetDamageDone(N_CurrentDamageTaken);
	}
}

void UDamageBulletDisplayComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (_resetTimer > 0)
	{
		_resetTimer -= DeltaTime;
		if (_resetTimer <= 0)
		{
			N_CurrentBulletCount = 0;
			N_CurrentDamageTaken = 0;

			if (_damageUI != nullptr)
			{
				_damageUI->SetBulletsHit(N_CurrentBulletCount);
				_damageUI->SetDamageDone(N_CurrentDamageTaken);
			}
		}
	}
}

void UDamageBulletDisplayComponent::HandleDamageTaken(int DamageAmount)
{
	_resetTimer = DataResetDelay;
	N_CurrentBulletCount += 1;
	N_CurrentDamageTaken += DamageAmount;

	if (_damageUI != nullptr)
	{
		_damageUI->SetBulletsHit(N_CurrentBulletCount);
		_damageUI->SetDamageDone(N_CurrentDamageTaken);
	}
}


void UDamageBulletDisplayComponent::OnDataFromNetwork() const
{
	if (_damageUI != nullptr)
	{
		_damageUI->SetBulletsHit(N_CurrentBulletCount);
		_damageUI->SetDamageDone(N_CurrentDamageTaken);
	}
}

void UDamageBulletDisplayComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDamageBulletDisplayComponent, N_CurrentBulletCount);
	DOREPLIFETIME(UDamageBulletDisplayComponent, N_CurrentDamageTaken);
}
