// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_UI.h"

void UPlayer_UI::AdjustHealth(float newPercent) {
	healthBar->SetPercent(newPercent);
}