// Fill out your copyright notice in the Description page of Project Settings.


#include "./DefenceBuffBaseComponent.h"

UDefenceBuffBaseComponent::UDefenceBuffBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDefenceBuffBaseComponent::BeginPlay()
{
}

int UDefenceBuffBaseComponent::TakeDamage(const int DamageAmount)
{
	return DamageAmount;
}
