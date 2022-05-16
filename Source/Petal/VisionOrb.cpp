// Fill out your copyright notice in the Description page of Project Settings.


#include "VisionOrb.h"
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>

// Sets default values
AVisionOrb::AVisionOrb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVisionOrb::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVisionOrb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ABasicEnemy* AVisionOrb::FindClosestEnemy(FVector playerLocation)
{
	ABasicEnemy* currentClosest = nullptr;
	float closestDistance = 3000.0f;
	for(ABasicEnemy* e : CurrentEnemies) {
		float newDistance = UKismetMathLibrary::Vector_Distance2D(playerLocation, e->GetActorLocation());
		if (newDistance < closestDistance) {
			currentClosest = e;
			closestDistance = newDistance;
		}
	}
	return currentClosest;
}
