// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Basic_UMG_Creator.generated.h"

class UUI_CrossHair;
class UUI_Interaction;
class UUI_PlayerHealthBar;

UCLASS()
class THIRDPERSONSHOOTER_API ABasic_UMG_Creator : public AActor
{
	GENERATED_BODY()

private:
	UUI_CrossHair* _crossHairWidget;
	UUI_Interaction* _interactionWidget;
	UUI_PlayerHealthBar* _playerHealthWidget;

protected:
	virtual void BeginPlay() override;

public:
#pragma region Properties

	UPROPERTY(Category="UI|Prefabs", EditAnywhere)
	TSubclassOf<class UUI_CrossHair> CrossHairPrefab;

	UPROPERTY(Category="UI|Prefabs", EditAnywhere)
	TSubclassOf<class UUI_Interaction> InteractionPrefab;

	UPROPERTY(Category="UI|Prefabs", EditAnywhere)
	TSubclassOf<class UUI_PlayerHealthBar> PlayerHealthPrefab;

#pragma endregion

	ABasic_UMG_Creator(const class FObjectInitializer& PCIP);

	UUI_CrossHair* GetCrossHairWidget() const;
	UUI_Interaction* GetInteractionWidget() const;
	UUI_PlayerHealthBar* GetPlayerHealthWidget() const;
};
