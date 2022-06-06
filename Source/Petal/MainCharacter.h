// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicEnemy.h"
#include "VisionOrb.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "MainCharacter.generated.h"

UCLASS()
class PETAL_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	float ChargeMultiplier = 0.5f;
	TArray<bool> BusyChecks;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		AVisionOrb* TargetingOrb;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USpringArmComponent* CameraArm;
	bool SaveAttack = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UUserWidget* AimingReticle;
	bool NextHeavy = false;
	int32 AttackCounter = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int32 SwingingForce = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float ScytheScale = 2.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float UpwardForce = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UAnimMontage*> AttackMontages;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		ABasicEnemy* ClosestTarget;
	UFUNCTION(BlueprintCallable)
		bool IsBusy(int32 checkValue);
	UFUNCTION(BlueprintCallable)
		bool IsBusyMulti(TArray<int32> checkValues);
	UFUNCTION(BlueprintCallable)
		void SetBusy(int32 valueType, bool newValue);
	UFUNCTION(BlueprintCallable)
		void StartPetalBurst(float forwardScale, float rightScale);
	UFUNCTION(BlueprintCallable)
		void PlayAttackMontage(bool isHeavy = false);
	UFUNCTION(BlueprintCallable)
		void PlayAttackCombo();
	UFUNCTION(BlueprintCallable)
		bool HeavyCharge(UAnimMontage* startAnim);
	UFUNCTION(BlueprintCallable)
		void LookUp(float axis);
	UFUNCTION(BlueprintCallable)
		void LookRight(float axis);
	UFUNCTION(BlueprintCallable)
		void StartAiming();
	UFUNCTION(BlueprintCallable)
		void StopAiming();
	UFUNCTION(BlueprintCallable)
		void LockOn();
	UFUNCTION(BlueprintCallable)
		void LockOff();
	void PlayAttackAnim(int AnimID, int counter, float playRate = 1.0f, float hitBoxScale = 2.0f, int swingingForce = 300, int forwardStep = 600, float upwardForce = 0.0f);

};
