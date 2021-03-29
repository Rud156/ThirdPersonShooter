// Fill out your copyright notice in the Description page of Project Settings.


#include "./BasicBot.h"
#include "../CustomCompoenents/Health/HealthAndDamageComponent.h"
#include "../CustomCompoenents/Misc/DamageBulletDisplayComponent.h"

#include "Net/UnrealNetwork.h"

ABasicBot::ABasicBot(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	HealthAndDamage = CreateDefaultSubobject<UHealthAndDamageComponent>(TEXT("HealthAndDamage"));
	
	DamageBulletDisplay = CreateDefaultSubobject<UDamageBulletDisplayComponent>(TEXT("DamageBulletDisplay"));
	DamageBulletDisplay->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
}
