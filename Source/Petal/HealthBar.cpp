// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBar.h"

void UHealthBar::AdjustBar(float newPercent) {
	thisHealthBar->SetVisibility(ESlateVisibility::Visible);
	thisHealthBar->SetPercent(newPercent);
}