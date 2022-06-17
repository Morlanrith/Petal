// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Wave_UI.generated.h"

/**
 * 
 */
UCLASS()
class PETAL_API UWave_UI : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UBorder* waveBorder;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UTextBlock* nextText;
	void HideBorder();
public:
	void StartWave(int waveNumber);
	void WaveCountdown();
};
