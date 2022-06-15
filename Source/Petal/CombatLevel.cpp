// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatLevel.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

// Called when the game starts or when spawned
void ACombatLevel::BeginPlay()
{
	Super::BeginPlay();
	Spawner = Cast<AEnemySpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemySpawner::StaticClass()));
	Spawner->Player = Cast<AMainCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainCharacter::StaticClass()));
}

void ACombatLevel::SpawnSetWave(int32 waveNumber) {
	TArray<FWaveGroupStruct> currentWave;
	for (FWaveGroupStruct group : LevelWaves) {
		if (group.WaveNumber < waveNumber) continue;
		if (group.WaveNumber == waveNumber) currentWave.Add(group);
		else break;
	}
	Spawner->SpawnWave(currentWave);
}