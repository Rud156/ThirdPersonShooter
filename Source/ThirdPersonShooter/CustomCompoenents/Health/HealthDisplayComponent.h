// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthDisplayComponent.generated.h"

class ABasic_UMG_Creator;
class UHealthAndDamageComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRDPERSONSHOOTER_API UHealthDisplayComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void HandleDataFromServer();
	
	ABasic_UMG_Creator* _basicUMGCreator;
	UHealthAndDamageComponent* _healthComponent;

protected:
	virtual void BeginPlay() override;

public:
	UHealthDisplayComponent();
};
