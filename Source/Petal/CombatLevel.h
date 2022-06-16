// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemySpawner.h"
#include "Engine/LevelScriptActor.h"
#include "CombatLevel.generated.h"

/**
 * 
 */

UCLASS()
class PETAL_API ACombatLevel : public ALevelScriptActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	AEnemySpawner* Spawner;
	int WaveNumber = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FWaveGroupStruct> LevelWaves;
	void SpawnSetWave();
public:
	void SpawnNextWave();
};
