// Fill out your copyright notice in the Description page of Project Settings.


#include "./DamageBuffBaseComponent.h"

UDamageBuffBaseComponent::UDamageBuffBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDamageBuffBaseComponent::BeginPlay()
{
}

int UDamageBuffBaseComponent::ApplyDamage(const int DamageAmount)
{
	return DamageAmount;
}
