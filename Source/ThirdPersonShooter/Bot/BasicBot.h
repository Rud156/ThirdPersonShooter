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
	class UDamageBulletDisplayComponent* DamageBulletDisplay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UHealthAndDamageComponent* HealthAndDamage;

public:
	ABasicBot(const class FObjectInitializer& PCIP);
};
