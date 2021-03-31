// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthAndDamageComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitDied, AActor*, Unit);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthChanged, int, NewHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDamageTaken, int, DamageAmount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDataFromServer);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRDPERSONSHOOTER_API UHealthAndDamageComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
#pragma region Networked Data

	UPROPERTY(ReplicatedUsing=OnDataFromNetwork)
	int N_CurrentHealth;

	UPROPERTY(Category="Health", BlueprintReadOnly, EditAnywhere, Replicated)
	int N_MaxHealth;

	UPROPERTY(Category="Delegates", BlueprintAssignable)
	FUnitDied OnUnitDied;

	UPROPERTY(Category="Delegates", BlueprintAssignable)
	FHealthChanged OnHealthChanged;

	UPROPERTY(Category="Delegates", BlueprintAssignable)
	FDamageTaken OnDamageTaken;

	UPROPERTY(Category="Delegates", BlueprintAssignable)
	FDataFromServer OnDataFromServer;

	UFUNCTION()
	void OnDataFromNetwork();

#pragma endregion

	UHealthAndDamageComponent(const class FObjectInitializer& PCIP);

	UFUNCTION(Category = "Health", BlueprintPure, BlueprintCallable)
	int GetCurrentHealth();

	void SetMaxHealth(const int HealthAmount, const bool ResetCurrentHealth = true);
	void SetHealth(const int HealthAmount);
	void AddHealth(const int HealthAmount);
	void TakeDamage(const int DamageAmount);

	UFUNCTION(Server, Reliable)
	void Server_TakeDamage(const int DamageAmount);
	UFUNCTION(Server, Reliable)
	void Server_AddHealth(const int HealthAmount);
	UFUNCTION(Server, Reliable)
	void Server_SetMaxHealth(const int HealthAmount, const bool ResetCurrentHealth = true);
	UFUNCTION(Server, Reliable)
	void Server_SetHealth(const int HealthAmount);
};
