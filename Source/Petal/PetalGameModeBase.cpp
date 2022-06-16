// Copyright Epic Games, Inc. All Rights Reserved.


#include "PetalGameModeBase.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include <Petal/MainCharacter.h>
#include "Blueprint/UserWidget.h"

void APetalGameModeBase::EndGame(bool hasWon) {
	APlayerController* pControl = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AMainCharacter* player = pControl->GetPawn<AMainCharacter>();
	UUserWidget* uiObject;
	UBlueprint* castBlueprint = Cast<UBlueprint>(
		StaticLoadObject(
			UObject::StaticClass(), 
			nullptr, 
			hasWon? TEXT("WidgetBlueprint'/Game/PetalContent/UI/Victory_Menu.Victory_Menu'" ) : TEXT("WidgetBlueprint'/Game/PetalContent/UI/GameOver_Menu.GameOver_Menu'")
		)
	);
	if (castBlueprint && castBlueprint->GeneratedClass->IsChildOf(UUserWidget::StaticClass()))
		uiObject = CreateWidget(pControl, *castBlueprint->GeneratedClass);
	else return;

	player->PlayerUI->RemoveFromViewport();
	uiObject->AddToViewport();
	if (!hasWon) pControl->SetPause(true);
	else player->DisableInput(pControl);
	FInputModeGameAndUI iParams;
	iParams.SetHideCursorDuringCapture(false);
	iParams.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	iParams.SetWidgetToFocus(uiObject->TakeWidget());
	pControl->SetInputMode(iParams);
	pControl->SetShowMouseCursor(true);
}