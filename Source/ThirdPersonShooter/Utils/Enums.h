// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.generated.h"

UENUM(BlueprintType)
enum class EPlayerMovementState : uint8
{
	None,
	Walk,
	Run,
	Crouch,
	Dive
};