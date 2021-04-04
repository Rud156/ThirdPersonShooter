// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TP_GameModeBase.generated.h"

class APlayerStart;
class ASpawnLocationsController;

UCLASS()
class THIRDPERSONSHOOTER_API ATP_GameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
#pragma region Properties

	UPROPERTY(Category="Spawn", EditAnywhere)
	TSubclassOf<class ASpawnLocationsController> SpawnControllerPrefab;

#pragma endregion
	virtual void Logout(AController* Exiting) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	virtual bool IsSpawnPointPreferred(APlayerStart* PlayerStart, AController* Controller);
};
