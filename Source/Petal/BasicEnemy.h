// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterStats.h"
#include "GameFramework/Character.h"
#include <Runtime/UMG/Public/Components/WidgetComponent.h>
#include "BasicEnemy.generated.h"

UCLASS()
class PETAL_API ABasicEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasicEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	AActor* ParentSpawner;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UCharacterStats* EnemyStats;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundBase* HitSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetComponent* HealthBar;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool IsAttacking = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool HitPlayer = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void SetParent(AActor* parent);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetComponent* TargetingReticle;
	UFUNCTION(BlueprintCallable)
		void HitReaction(FVector backwardsVelocity, int32 damage, bool melee = true);

};
