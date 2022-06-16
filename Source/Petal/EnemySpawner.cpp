// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include <Petal/CombatLevel.h>
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

FWaveGroupStruct::FWaveGroupStruct()
{
	EnemyAmount = 1;
	WaveNumber = 1;
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawner::SpawnEnemy(TSubclassOf<ABasicEnemy> type, FVector startLocation, FRotator startRotation)
{
	// Creates spawn parameters the ensure the enemy always spawns, but attempting to adjust if there are conflicts
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	// Spawns the enemy
	ABasicEnemy* enemy = GetWorld()->SpawnActor<ABasicEnemy>(type, startLocation, startRotation, SpawnInfo);
	enemy->SetParent(this);
	enemy->SpawnDefaultController(); // Makes sure the enemy's default controller is also created
}

void AEnemySpawner::SpawnWave(TArray<FWaveGroupStruct> wave)
{
	if (WaveTotal) return;
	for (FWaveGroupStruct group : wave) {
		for (int i = 0; i < group.EnemyAmount; i++) {
			FVector location = UKismetMathLibrary::RandomPointInBoundingBox(SpawnArea->GetComponentLocation(),SpawnArea->GetScaledBoxExtent());
			float yawRotation = UKismetMathLibrary::FindLookAtRotation(location,Player->GetMesh()->GetComponentLocation()).Yaw;
			SpawnEnemy(group.EnemyType, location, FRotator(0.0f, yawRotation, 0.0f));
		}
		WaveTotal += group.EnemyAmount;
	}
}

void AEnemySpawner::KillEnemy() {
	WaveTotal--;
	if (!WaveTotal) Cast<ACombatLevel>(GetWorld()->GetLevelScriptActor())->SpawnNextWave();
}
