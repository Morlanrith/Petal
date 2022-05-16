// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsCharging && ChargeMultiplier < 3.0f) ChargeMultiplier += DeltaTime;
	if (IsDashing) {
		AddMovementInput(UKismetMathLibrary::GetForwardVector(FRotator(0.0f, GetMesh()->GetComponentRotation().Yaw + 90.0f, 0.0f)));
	}
	else if (IsLockedOn) {
		if (!ClosestTarget || !TargetingOrb->CurrentEnemies.Contains(ClosestTarget)) {
			IsLockedOn = false;
			return;
		}
		FRotator targetTurningDirection = FRotator(GetMesh()->GetComponentRotation().Pitch, // Pitch
													UKismetMathLibrary::FindLookAtRotation(
														GetMesh()->GetComponentLocation(), 
														ClosestTarget->GetActorLocation()
													).Yaw -90.0f, // Yaw
													GetMesh()->GetComponentRotation().Roll); // Roll

		GetMesh()->SetWorldRotation(UKismetMathLibrary::RInterpTo(GetMesh()->GetComponentRotation(), targetTurningDirection, DeltaTime, 10.0f));


		FRotator cameraTurningDirection = FRotator(0.0f, // Pitch
													UKismetMathLibrary::FindLookAtRotation(
														CameraArm->GetComponentLocation(),
														ClosestTarget->GetActorLocation()
													).Yaw, // Yaw
													0.0f); // Roll

		CameraArm->SetWorldRotation(UKismetMathLibrary::RInterpTo(CameraArm->GetComponentRotation(), cameraTurningDirection, DeltaTime, 10.0f));

		ClosestTarget->TargetingReticle->SetVisibility(true);
	}
	else if (GetCharacterMovement()->GetLastInputVector() != FVector(0,0,0)) {
		GetMesh()->SetWorldRotation(UKismetMathLibrary::RInterpTo(GetMesh()->GetComponentRotation(), FRotator(0.0f, GetCharacterMovement()->Velocity.Rotation().Yaw - 90.0f, 0.0f), DeltaTime, IsCharging? 10.0f : 20.0f));
	}	
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMainCharacter::PlayAttackMontage(bool isHeavy) {
	if (IsCharging) {
		if (isHeavy) {
			IsAttacking = true;
			PlayAttackAnim(4, -1, 1.25f, 2.0f + (0.5f * ChargeMultiplier), 350 * ChargeMultiplier, 0, 350.0f);
		}
		return;
	}
	else if (IsAttacking) {
		SaveAttack = true;
		NextHeavy = isHeavy;
		return;
	}
	else {
		IsAttacking = true;
		if (this->GetMovementComponent()->IsFalling()) {
			this->LaunchCharacter(FVector(0.0f, 0.0f, 500.0f), true, true);
			if (!isHeavy) PlayAttackAnim(9, 4, 2.5f, 2.5f, 0, 0, 600.0f);
			else PlayAttackAnim(12, -1, 2.0f, 3.0f, 0, 0, -1500.0f);
		}
		else if (IsSprinting) {
			isHeavy ? PlayAttackAnim(5, -1, 1.5f, 2.5f, 800, 0, 350.0f) : PlayAttackAnim(3, -1, 2.0f, 2.5f, 600, 6000, 250.0f);
		}
		else PlayAttackAnim(0, 1, 2.0f);
	}
}

void AMainCharacter::PlayAttackCombo() {
	if (SaveAttack && AttackCounter != -1) {
		switch (AttackCounter) {
			case 1:
				NextHeavy ? PlayAttackAnim(7, 3, 1.75f, 2.0f, 0, 0, 750.0f) : PlayAttackAnim(1, 2, 2.0f);
				return;
			case 2:
				NextHeavy ? PlayAttackAnim(6, -1, 2.0f, 2.5f, 800, 600, 250.0f) : PlayAttackAnim(2, -1, 2.0f, 3.0f, 800);
				return;
			case 3:
				if (NextHeavy) {
					this->LaunchCharacter(FVector(0.0f, 0.0f, 1000.0f), false, false);
					PlayAttackAnim(8, -1, 2.0f, 2.5f, 0, 0, 500.0f);
					return;
				}
				else break;
			case 4:
				if (!this->GetMovementComponent()->IsFalling()) break;
				this->LaunchCharacter(FVector(0.0f, 0.0f, 500.0f), true, true);
				NextHeavy ? PlayAttackAnim(12, -1, 2.0f, 3.0f, 0, 0, -1500.0f) : PlayAttackAnim(10, 5, 1.5f, 2.5f, 0, 0, 600.0f);
				return;
			case 5:
				if (!this->GetMovementComponent()->IsFalling()) break;
				this->LaunchCharacter(FVector(0.0f, 0.0f, 500.0f), true, true);
				NextHeavy ? PlayAttackAnim(12, -1, 2.0f, 3.0f, 0, 0, -1500.0f) : PlayAttackAnim(11, -1, 2.0f, 2.5f, 800, 0, 600.0f);
				return;
		}
	}
	AttackCounter = 0;
	ChargeMultiplier = 0.5f;
	IsAttacking = false;
	SaveAttack = false;
	NextHeavy = false;
}

void AMainCharacter::PlayAttackAnim(int AnimID, int counter, float playRate, float hitBoxScale, int swingingForce, int forwardStep, float upwardForce) {
	SaveAttack = false;
	SwingingForce = swingingForce;
	AttackCounter = counter;
	UpwardForce = upwardForce;
	SwordScale = hitBoxScale;
	if(forwardStep > 0) this->LaunchCharacter(UKismetMathLibrary::GetForwardVector(FRotator(0.0f, GetMesh()->GetComponentRotation().Yaw+90.0f, 0.0f)) * forwardStep, false, false);
	this->PlayAnimMontage(AttackMontages[AnimID], playRate);
}

void AMainCharacter::LookUp(float axis) {
	if (IsLockedOn) return;
	FRotator currentRotation = CameraArm->GetComponentRotation();
	if ((currentRotation.Pitch > -20.0f && axis > 0.0f) || (currentRotation.Pitch < 8.0f && axis < 0.0f)) { // Prevent looking too far up or down
		currentRotation.Pitch -= axis;
		CameraArm->SetWorldRotation(currentRotation);
	}
}

void AMainCharacter::LookRight(float axis) {
	if (IsLockedOn) return;
	FRotator currentRotation = CameraArm->GetComponentRotation();
	currentRotation.Yaw += axis;
	CameraArm->SetWorldRotation(currentRotation);
}

void AMainCharacter::HeavyCharge(UAnimMontage* startAnim) {
	if (AttackCounter <= 0 && IsAttacking) return;
	if (AttackCounter > 0 || this->GetMovementComponent()->IsFalling() || IsSprinting) {
		PlayAttackMontage(true);
		return;
	}
	this->PlayAnimMontage(startAnim, 1.5f);
	IsCharging = true;
	IsAttacking = true;
}

