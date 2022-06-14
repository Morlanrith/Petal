// Copyright Epic Games, Inc. All Rights Reserved.


#include "PetalGameModeBase.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "Blueprint/UserWidget.h"

void APetalGameModeBase::EndGame(bool hasWon) {
	APlayerController* pControl = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	UUserWidget* uiObject;
	UBlueprint* castBlueprint = Cast<UBlueprint>(
		StaticLoadObject(
			UObject::StaticClass(), 
			nullptr, 
			TEXT("WidgetBlueprint'/Game/PetalContent/UI/GameOver_Menu.GameOver_Menu'")
		)
	);
	if (castBlueprint && castBlueprint->GeneratedClass->IsChildOf(UUserWidget::StaticClass()))
		uiObject = CreateWidget(pControl, *castBlueprint->GeneratedClass);
	else return;

	uiObject->AddToViewport();
	pControl->SetPause(true);
	FInputModeGameAndUI iParams;
	iParams.SetHideCursorDuringCapture(false);
	iParams.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	iParams.SetWidgetToFocus(uiObject->TakeWidget());
	pControl->SetInputMode(iParams);
	pControl->SetShowMouseCursor(true);
}