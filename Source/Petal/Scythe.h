// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Petal/MainCharacter.h>
#include <Petal/BasicEnemy.h>
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Scythe.generated.h"

UCLASS()
class PETAL_API AScythe : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AScythe();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool CanHit = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<ABasicEnemy*> CurrentEnemies;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		AMainCharacter* ParentPlayer;
	UFUNCTION(BlueprintCallable)
		void ShootBullet(FVector playerPos, UAnimMontage* fireAnim, UParticleSystem* sparkFX);
};
