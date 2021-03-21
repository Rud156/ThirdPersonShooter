// Fill out your copyright notice in the Description page of Project Settings.


#include "./HealthAndDamageComponent.h"
#include "./DefenceBuffBaseComponent.h"

#include "Net/UnrealNetwork.h"

UHealthAndDamageComponent::UHealthAndDamageComponent(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	N_MaxHealth = 100;

	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthAndDamageComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority())
	{
		N_CurrentHealth = N_MaxHealth;
	}
}

void UHealthAndDamageComponent::Server_TakeDamage_Implementation(const int DamageAmount)
{
	int damageAmount = DamageAmount;
	TArray<UActorComponent*> actorComponents = GetOwner()->GetComponentsByClass(UDefenceBuffBaseComponent::StaticClass());
	for (int i = 0; i < actorComponents.Num(); i++)
	{
		UDefenceBuffBaseComponent* damageDebuff = Cast<UDefenceBuffBaseComponent>(actorComponents[i]);
		damageAmount = damageDebuff->TakeDamage(DamageAmount);
	}

	const int lastHealth = N_CurrentHealth;
	N_CurrentHealth -= damageAmount;

	if (lastHealth != N_CurrentHealth)
	{
		OnHealthChanged.Broadcast(N_CurrentHealth);
	}

	if (N_CurrentHealth <= 0)
	{
		OnUnitDied.Broadcast(GetOwner());
	}
}

void UHealthAndDamageComponent::Server_AddHealth_Implementation(const int HealthAmount)
{
	N_CurrentHealth += HealthAmount;
	if (N_CurrentHealth > N_MaxHealth)
	{
		N_CurrentHealth = N_MaxHealth;
	}

	OnHealthChanged.Broadcast(N_CurrentHealth);
}

void UHealthAndDamageComponent::Server_SetMaxHealth_Implementation(const int HealthAmount, const bool ResetCurrentHealth)
{
	N_MaxHealth = HealthAmount;
	if (ResetCurrentHealth)
	{
		N_CurrentHealth = HealthAmount;
	}
}

void UHealthAndDamageComponent::SetMaxHealth(const int HealthAmount, const bool ResetCurrentHealth)
{
	Server_SetMaxHealth(HealthAmount, ResetCurrentHealth);
}

void UHealthAndDamageComponent::AddHealth(const int HealthAmount)
{
	Server_AddHealth(HealthAmount);
}

void UHealthAndDamageComponent::TakeDamage(const int DamageAmount)
{
	Server_TakeDamage(DamageAmount);
}

int UHealthAndDamageComponent::GetCurrentHealth()
{
	return N_CurrentHealth;
}

void UHealthAndDamageComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthAndDamageComponent, N_MaxHealth);
	DOREPLIFETIME(UHealthAndDamageComponent, N_CurrentHealth);
}
