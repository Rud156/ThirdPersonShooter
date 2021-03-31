// Fill out your copyright notice in the Description page of Project Settings.


#include "./HealthDisplayComponent.h"
#include "../../Common/Basic_UMG_Creator.h"
#include "./HealthAndDamageComponent.h"
#include "../../UI/UI_PlayerHealthBar.h"
#include "../../Player/TPPlayer.h"

#include "Kismet/GameplayStatics.h"

UHealthDisplayComponent::UHealthDisplayComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthDisplayComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* basicUMGActor = UGameplayStatics::GetActorOfClass(GetWorld(), ABasic_UMG_Creator::StaticClass());
	if (basicUMGActor != nullptr)
	{
		_basicUMGCreator = Cast<ABasic_UMG_Creator>(basicUMGActor);
	}

	UActorComponent* healthDmgComponent = GetOwner()->GetComponentByClass(UHealthAndDamageComponent::StaticClass());
	if (healthDmgComponent != nullptr)
	{
		_healthComponent = Cast<UHealthAndDamageComponent>(healthDmgComponent);
		_healthComponent->OnDataFromServer.AddDynamic(this, &UHealthDisplayComponent::HandleDataFromServer);
	}
}

void UHealthDisplayComponent::HandleDataFromServer()
{
	ATPPlayer* player = Cast<ATPPlayer>(GetOwner());
	if (_basicUMGCreator != nullptr && player->IsLocallyControlled())
	{
		const int currentHealth = _healthComponent->GetCurrentHealth();
		const int maxHealth = _healthComponent->N_MaxHealth;

		_basicUMGCreator->GetPlayerHealthWidget()->SetHealthRatio(currentHealth, maxHealth);
	}
}
