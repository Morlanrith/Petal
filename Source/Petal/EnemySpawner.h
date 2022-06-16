// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicEnemy.h"
#include "MainCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"


USTRUCT(BlueprintType)
struct FWaveGroupStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Group Struct")
		TSubclassOf<ABasicEnemy> EnemyType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Group Struct")
		int32 EnemyAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Group Struct")
		int32 WaveNumber;

	FWaveGroupStruct();
};

UCLASS()
class PETAL_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	int WaveTotal = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UBoxComponent* SpawnArea;
	void SpawnEnemy(TSubclassOf<ABasicEnemy> type, FVector startLocation, FRotator startRotation);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void KillEnemy();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		AMainCharacter* Player;
	UFUNCTION(BlueprintCallable)
		void SpawnWave(TArray<FWaveGroupStruct> wave);

};
