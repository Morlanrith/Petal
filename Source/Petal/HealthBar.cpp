// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBar.h"

void UHealthBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(HealthTimer < 3.0f) HealthTimer += InDeltaTime;
	else if(thisHealthBar->IsVisible())
		thisHealthBar->SetVisibility(ESlateVisibility::Hidden);
}

void UHealthBar::AdjustBar(float newPercent) {
	HealthTimer = 0.0f;
	thisHealthBar->SetVisibility(ESlateVisibility::Visible);
	thisHealthBar->SetPercent(newPercent);
}