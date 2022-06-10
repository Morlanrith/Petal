// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Player_UI.generated.h"

/**
 * 
 */
UCLASS()
class PETAL_API UPlayer_UI : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UProgressBar* healthBar;
public:
	void AdjustHealth(float newPercent);
};
