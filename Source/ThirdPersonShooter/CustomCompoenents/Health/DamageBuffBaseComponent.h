// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageBuffBaseComponent.generated.h"

class ATPPlayer;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRDPERSONSHOOTER_API UDamageBuffBaseComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UDamageBuffBaseComponent();
	virtual int ApplyDamage(const int DamageAmount);
};
