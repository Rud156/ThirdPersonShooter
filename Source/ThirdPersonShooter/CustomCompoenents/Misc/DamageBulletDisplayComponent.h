// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageBulletDisplayComponent.generated.h"

class UUI_DamageDisplay;

UCLASS()
class THIRDPERSONSHOOTER_API UDamageBulletDisplayComponent : public UWidgetComponent
{
	GENERATED_BODY()

private:
	UUI_DamageDisplay* _damageUI;
	float _resetTimer;

	UFUNCTION()
	void HandleDamageTaken(int DamageAmount);

public:
#pragma region Networked Data

	UPROPERTY(ReplicatedUsing=OnDataFromNetwork)
	int N_CurrentBulletCount;

	UPROPERTY(ReplicatedUsing=OnDataFromNetwork)
	int N_CurrentDamageTaken;

	UFUNCTION()
	void OnDataFromNetwork() const;

#pragma endregion

#pragma region Properties

	UPROPERTY(Category="DamageBullet|Data", EditAnywhere)
	float DataResetDelay;

#pragma endregion

	UDamageBulletDisplayComponent(const class FObjectInitializer& PCIP);
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
