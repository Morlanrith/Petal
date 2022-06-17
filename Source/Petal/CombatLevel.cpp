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
	WaveAmount = LevelWaves.Last(0).WaveNumber;
	SpawnNextWave();
}

void ACombatLevel::SpawnSetWave() {
	TArray<FWaveGroupStruct> currentWave;
	for (FWaveGroupStruct group : LevelWaves) {
		if (group.WaveNumber < WaveNumber) continue;
		if (group.WaveNumber == WaveNumber) currentWave.Add(group);
		else break;
	}
	waveUI->StartWave(WaveNumber == WaveAmount ? -1 : WaveNumber);
	Spawner->SpawnWave(currentWave);
}

void ACombatLevel::SpawnNextWave() {
	WaveNumber++;
	if(WaveAmount < WaveNumber)((APetalGameModeBase*)GetWorld()->GetAuthGameMode())->EndGame(true);
	else {
		waveUI->WaveCountdown();
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACombatLevel::SpawnSetWave, 5, false);
	}
}