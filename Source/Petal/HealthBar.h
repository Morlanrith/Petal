// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "HealthBar.generated.h"


UCLASS()
class PETAL_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
protected:
	float HealthTimer = 3.0f;
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UProgressBar* thisHealthBar;
	void AdjustBar(float newPercent);
};
