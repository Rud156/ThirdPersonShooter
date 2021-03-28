// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BasicBot.generated.h"

class UUI_DamageDisplay;

UCLASS()
class THIRDPERSONSHOOTER_API ABasicBot : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* DamageWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UHealthAndDamageComponent* HealthAndDamage;

	UUI_DamageDisplay* _damageUI;

	float _resetTimer;
	
	UFUNCTION()
	void HandleDamageTaken(int DamageAmount);

protected:
	virtual void BeginPlay() override;

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

	UPROPERTY(Category="Bot|Data", EditAnywhere)
	float DataResetDelay;

#pragma endregion

	ABasicBot(const class FObjectInitializer& PCIP);
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
