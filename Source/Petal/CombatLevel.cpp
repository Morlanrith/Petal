// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatLevel.h"
#include "PetalGameModeBase.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

// Called when the game starts or when spawned
void ACombatLevel::BeginPlay()
{
	Super::BeginPlay();
	Spawner = Cast<AEnemySpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemySpawner::StaticClass()));
	Spawner->Player = Cast<AMainCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainCharacter::StaticClass()));
	SpawnSetWave();
}

void ACombatLevel::SpawnSetWave() {
	TArray<FWaveGroupStruct> currentWave;
	for (FWaveGroupStruct group : LevelWaves) {
		if (group.WaveNumber < WaveNumber) continue;
		if (group.WaveNumber == WaveNumber) currentWave.Add(group);
		else break;
	}
	if(currentWave.Num()) Spawner->SpawnWave(currentWave);
	else ((APetalGameModeBase*)GetWorld()->GetAuthGameMode())->EndGame(true);
}

void ACombatLevel::SpawnNextWave() {
	WaveNumber++;
	SpawnSetWave();
}