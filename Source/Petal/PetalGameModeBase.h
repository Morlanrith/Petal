// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PetalGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PETAL_API APetalGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	void EndGame(bool hasWon = false);
};
