// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs.generated.h"

USTRUCT(BlueprintType)
struct FRecoilOffset
{
	GENERATED_BODY()

	int Index;
	int RowIndex;
	int ColumnIndex;
	FVector2D Offset;
};
