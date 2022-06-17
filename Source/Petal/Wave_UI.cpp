// Fill out your copyright notice in the Description page of Project Settings.


#include "Wave_UI.h"

void UWave_UI::StartWave(int waveNumber) {
	nextText->SetVisibility(ESlateVisibility::Hidden);
	Cast<UTextBlock>(waveBorder->GetChildAt(0))->SetText(
		FText::FromString(
			waveNumber == -1 ? "Final Wave" : "Wave " + FString::FromInt(waveNumber)
		)
	);
	waveBorder->SetVisibility(ESlateVisibility::Visible);
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UWave_UI::HideBorder, 5, false);
}

void UWave_UI::WaveCountdown() {
	nextText->SetVisibility(ESlateVisibility::Visible);
}

void UWave_UI::HideBorder() {
	waveBorder->SetVisibility(ESlateVisibility::Hidden);
}
