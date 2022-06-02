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
	if (IsDashing || IsPetalBursting) {
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
	else if (IsAiming) {
		GetMesh()->SetWorldRotation(UKismetMathLibrary::RInterpTo(GetMesh()->GetComponentRotation(), FRotator(0.0f, CameraArm->GetComponentRotation().Yaw - 90.0f, 0.0f), DeltaTime, 20.0f));
	}
	else if (GetCharacterMovement()->GetLastInputVector() != FVector(0,0,0)) {
		GetMesh()->SetWorldRotation(UKismetMathLibrary::RInterpTo(GetMesh()->GetComponentRotation(), FRotator(0.0f, GetCharacterMovement()->Velocity.Rotation().Yaw - 90.0f, 0.0f), DeltaTime, IsCharging? 10.0f : 20.0f));
	}
	CameraArm->TargetArmLength = UKismetMathLibrary::FInterpTo(CameraArm->TargetArmLength, IsAiming ? 150.0f : 450.0f, DeltaTime, 30.0f);
	CameraArm->SocketOffset.Z = UKismetMathLibrary::FInterpTo(CameraArm->SocketOffset.Z, IsAiming ? 150.0f : 120.0f, DeltaTime, 30.0f);
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMainCharacter::PlayAttackMontage(bool isHeavy) {
	if (IsPetalBursting || IsShooting || IsAiming) return;
	else if (IsCharging) {
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

void AMainCharacter::StartPetalBurst(float forwardScale, float rightScale) {
	if (IsPetalBursting || IsAttacking || IsCharging || IsDashing || IsShooting || this->GetMovementComponent()->IsFalling()) return;
	IsPetalBursting = true;
	this->PlayAnimMontage(LoadObject<UAnimMontage>(NULL, UTF8_TO_TCHAR("AnimMontage'/Game/PetalContent/Animation/Player/Animations/Movement/xbot_PetalBurst.xbot_PetalBurst'")), 2.0f);
	if (forwardScale || rightScale) {
		float newYaw = CameraArm->GetComponentRotation().Yaw - 90.0f;
		if (forwardScale < 0.0f) newYaw += 180.0f;
		if (!forwardScale) newYaw += (rightScale > 0.0f ? 90.0f : -90.0f);
		else if(rightScale) newYaw += UKismetMathLibrary::DegAtan(rightScale / forwardScale);
		GetMesh()->SetWorldRotation(FRotator(0.0f, newYaw, 0.0f));
	}
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
	currentRotation.Pitch -= axis;
	// Prevent looking too far up or down
	if (currentRotation.Pitch < -20.0f) currentRotation.Pitch = -20.0f;
	else if (currentRotation.Pitch > 8.0f) currentRotation.Pitch = 8.0f;
	CameraArm->SetWorldRotation(currentRotation);
}

void AMainCharacter::LookRight(float axis) {
	if (IsLockedOn) return;
	FRotator currentRotation = CameraArm->GetComponentRotation();
	currentRotation.Yaw += axis;
	CameraArm->SetWorldRotation(currentRotation);
}

bool AMainCharacter::HeavyCharge(UAnimMontage* startAnim) {
	if (AttackCounter <= 0 && IsAttacking) return false;
	if (AttackCounter > 0 || this->GetMovementComponent()->IsFalling() || IsSprinting || IsPetalBursting || IsShooting || IsAiming) {
		PlayAttackMontage(true);
		return false;
	}
	this->PlayAnimMontage(startAnim, 1.5f);
	IsCharging = true;
	IsAttacking = true;
	return true;
}

void AMainCharacter::StartAiming() {
	if (IsPetalBursting || IsAttacking || IsCharging || IsDashing || IsShooting || this->GetMovementComponent()->IsFalling()) return;
	IsAiming = true;
	AimingReticle->AddToViewport();
}

void AMainCharacter::StopAiming() {
	if (!IsAiming) return;
	IsAiming = false;
	AimingReticle->RemoveFromViewport();
}