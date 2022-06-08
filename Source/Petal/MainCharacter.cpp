// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	/*
	The following initialises BusyChecks to hold the boolean values for each of the players possible actions.
	The index representations are as follows:
		0 = Attacking
		1 = Charging
		2 = Sprinting
		3 = Dashing
		4 = Locked On
		5 = Petal Bursting
		6 = Shooting
		7 = Aiming
		8 = Recovering
	*/
	BusyChecks.Init(false, 9);
	PlayerStats = CreateDefaultSubobject<UCharacterStats>(TEXT("Player Stats"));
	AddOwnedComponent(PlayerStats);
	PlayerStats->SetHealth(10);
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
	if (IsBusy(1) && ChargeMultiplier < 3.0f) ChargeMultiplier += DeltaTime; // If the player is charging an attack, increase its attack multiplier
	if (IsBusyMulti({ 3, 5 })) { // If the player is either dashing or petal bursting
		AddMovementInput(UKismetMathLibrary::GetForwardVector(FRotator(0.0f, GetMesh()->GetComponentRotation().Yaw + 90.0f, 0.0f)));
	}
	else if (IsBusy(4)) { // If the player is locked on
		if (!ClosestTarget || !TargetingOrb->CurrentEnemies.Contains(ClosestTarget)) {
			SetBusy(4, false); // Disables lock on
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
	else if(IsBusy(7)) { // If the player is aiming
		// Adjust the player rotation to match that of the camera
		GetMesh()->SetWorldRotation(UKismetMathLibrary::RInterpTo(GetMesh()->GetComponentRotation(), FRotator(0.0f, CameraArm->GetComponentRotation().Yaw - 90.0f, 0.0f), DeltaTime, 20.0f));
	}
	else if (GetCharacterMovement()->GetLastInputVector() != FVector(0,0,0)) {
		// Adjust the player rotation to match the current direction they are moving in, with interpolation time depending on if the player is charging
		GetMesh()->SetWorldRotation(UKismetMathLibrary::RInterpTo(GetMesh()->GetComponentRotation(), FRotator(0.0f, GetCharacterMovement()->Velocity.Rotation().Yaw - 90.0f, 0.0f), DeltaTime, IsBusy(1)? 10.0f : 20.0f));
	}
	// Adjusts the camera position depending on whether the player is aiming or not
	CameraArm->TargetArmLength = UKismetMathLibrary::FInterpTo(CameraArm->TargetArmLength, IsBusy(7)? 150.0f : 450.0f, DeltaTime, 30.0f);
	CameraArm->SocketOffset.Z = UKismetMathLibrary::FInterpTo(CameraArm->SocketOffset.Z, IsBusy(7)? 150.0f : 120.0f, DeltaTime, 30.0f);
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

/* Checks to see if the player is busy doing the specified action. */
bool AMainCharacter::IsBusy(int32 checkValue) {
	if (BusyChecks[checkValue]) return true;
	return false;
}

/* Checks to see if the player is busy doing any of the specified actions. */
bool AMainCharacter::IsBusyMulti(TArray<int32> checkValues) {
	for (int32 check : checkValues)
		if (BusyChecks[check]) return true;
	return false;
}

/* Sets a busy check to help determine if the player is busy performing a certain action. */
void AMainCharacter::SetBusy(int32 valueType, bool newValue) {
	BusyChecks[valueType] = newValue;
}

void AMainCharacter::PlayAttackMontage(bool isHeavy) {
	if (IsBusyMulti({ 5,6,7 })) return; // Will not work if the player is petal bursting, shooting, or aiming
	else if (IsBusy(1)) { // If charging
		if (isHeavy) {
			SetBusy(0, true);
			PlayAttackAnim(4, -1, 2, 1.25f, 2.0f + (0.5f * ChargeMultiplier), 350 * ChargeMultiplier, 0, 350.0f);
		}
		return;
	}
	else if (IsBusy(0)) { // If attacking
		SaveAttack = true;
		NextHeavy = isHeavy;
		return;
	}
	else {
		SetBusy(0, true); // Attacking is now true
		if (this->GetMovementComponent()->IsFalling()) {
			this->LaunchCharacter(FVector(0.0f, 0.0f, 500.0f), true, true);
			if (!isHeavy) PlayAttackAnim(9, 4, 1, 2.5f, 2.5f, 0, 0, 600.0f);
			else PlayAttackAnim(12, -1, 2, 2.0f, 3.0f, 0, 0, -1500.0f);
		}
		else if (IsBusy(2)) { // If the player is sprinting
			isHeavy ? PlayAttackAnim(5, -1, 2, 1.5f, 2.5f, 800, 0, 350.0f) : PlayAttackAnim(3, -1, 1, 2.0f, 2.5f, 600, 6000, 250.0f);
		}
		else PlayAttackAnim(0, 1, 1, 2.0f);
	}
}

void AMainCharacter::PlayAttackCombo() {
	if (SaveAttack && AttackCounter != -1) {
		switch (AttackCounter) {
			case 1:
				NextHeavy ? PlayAttackAnim(7, 3, 1, 1.75f, 2.0f, 0, 0, 750.0f) : PlayAttackAnim(1, 2, 1, 2.0f);
				return;
			case 2:
				NextHeavy ? PlayAttackAnim(6, -1, 3, 2.0f, 2.5f, 800, 600, 250.0f) : PlayAttackAnim(2, -1, 2, 2.0f, 3.0f, 800);
				return;
			case 3:
				if (NextHeavy) {
					this->LaunchCharacter(FVector(0.0f, 0.0f, 1000.0f), false, false);
					PlayAttackAnim(8, -1, 2, 2.0f, 2.5f, 0, 0, 500.0f);
					return;
				}
				else break;
			case 4:
				if (!this->GetMovementComponent()->IsFalling()) break;
				this->LaunchCharacter(FVector(0.0f, 0.0f, 500.0f), true, true);
				NextHeavy ? PlayAttackAnim(12, -1, 2, 2.0f, 3.0f, 0, 0, -1500.0f) : PlayAttackAnim(10, 5, 1, 1.5f, 2.5f, 0, 0, 600.0f);
				return;
			case 5:
				if (!this->GetMovementComponent()->IsFalling()) break;
				this->LaunchCharacter(FVector(0.0f, 0.0f, 500.0f), true, true);
				NextHeavy ? PlayAttackAnim(12, -1, 2, 2.0f, 3.0f, 0, 0, -1500.0f) : PlayAttackAnim(11, -1, 2, 2.0f, 2.5f, 800, 0, 600.0f);
				return;
		}
	}
	AttackCounter = 0;
	ChargeMultiplier = 0.5f;
	SetBusy(0, false); // Attacking is now false
	SaveAttack = false;
	NextHeavy = false;
}

void AMainCharacter::StartPetalBurst(float forwardScale, float rightScale) {
	// Will not work if the player is currently attacking, charging, dashing, petal bursting, shooting, or jumping
	if (IsBusyMulti({ 0, 1, 3, 5, 6 }) || this->GetMovementComponent()->IsFalling()) return;
	SetBusy(5, true); // Petal bursting is now true
	this->PlayAnimMontage(LoadObject<UAnimMontage>(NULL, UTF8_TO_TCHAR("AnimMontage'/Game/PetalContent/Animation/Player/Animations/Movement/xbot_PetalBurst.xbot_PetalBurst'")), 2.0f);
	if (forwardScale || rightScale) {
		float newYaw = CameraArm->GetComponentRotation().Yaw - 90.0f;
		if (forwardScale < 0.0f) newYaw += 180.0f;
		if (!forwardScale) newYaw += (rightScale > 0.0f ? 90.0f : -90.0f);
		else if(rightScale) newYaw += UKismetMathLibrary::DegAtan(rightScale / forwardScale);
		GetMesh()->SetWorldRotation(FRotator(0.0f, newYaw, 0.0f));
	}
}

void AMainCharacter::PlayAttackAnim(int AnimID, int counter, int damage, float playRate, float hitBoxScale, int swingingForce, int forwardStep, float upwardForce) {
	SaveAttack = false;
	CurrentDamage = damage;
	SwingingForce = swingingForce;
	AttackCounter = counter;
	UpwardForce = upwardForce;
	ScytheScale = hitBoxScale;
	if(forwardStep > 0) this->LaunchCharacter(UKismetMathLibrary::GetForwardVector(FRotator(0.0f, GetMesh()->GetComponentRotation().Yaw+90.0f, 0.0f)) * forwardStep, false, false);
	this->PlayAnimMontage(AttackMontages[AnimID], playRate);
}

void AMainCharacter::LookUp(float axis) {
	if (IsBusy(4)) return; // If the player is locked on, return
	FRotator currentRotation = CameraArm->GetComponentRotation();
	currentRotation.Pitch -= axis;
	// Prevent looking too far up or down
	if (currentRotation.Pitch < -20.0f) currentRotation.Pitch = -20.0f;
	else if (currentRotation.Pitch > 8.0f) currentRotation.Pitch = 8.0f;
	CameraArm->SetWorldRotation(currentRotation);
}

void AMainCharacter::LookRight(float axis) {
	if (IsBusy(4)) return; // If the player is locked on, return
	FRotator currentRotation = CameraArm->GetComponentRotation();
	currentRotation.Yaw += axis;
	CameraArm->SetWorldRotation(currentRotation);
}

bool AMainCharacter::HeavyCharge(UAnimMontage* startAnim) {
	if (AttackCounter <= 0 && IsBusy(0)) return false; // If the player is currently attacking while the counter is default, return
	if (AttackCounter > 0 || this->GetMovementComponent()->IsFalling() || IsBusyMulti({ 2, 5, 6, 7 })) {
		PlayAttackMontage(true);
		return false;
	}
	this->PlayAnimMontage(startAnim, 1.5f);
	SetBusy(1, true); // Charging is now true
	SetBusy(0, true); // Attacking is now true
	return true;
}

void AMainCharacter::StartAiming() {
	if (IsBusyMulti({ 0, 1, 3, 4, 5, 6 })) return;
	SetBusy(7, true); // Aiming is now true
	AimingReticle->AddToViewport();
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	SetBusy(2, false); // Sprinting is now false
}

void AMainCharacter::StopAiming() {
	if (!IsBusy(7)) return; // If not currently aiming, then return
	SetBusy(7, false); // Aiming is now false
	AimingReticle->RemoveFromViewport();
}

void AMainCharacter::LockOn() {
	if (IsBusy(7)) return; // If currently aiming, then return
	ClosestTarget = TargetingOrb->FindClosestEnemy(GetCapsuleComponent()->GetComponentLocation());
	SetBusy(4, true); // Locked on is now true
}

void AMainCharacter::LockOff() {
	SetBusy(4, false); // Locked on is now false
	if (ClosestTarget) ClosestTarget->TargetingReticle->SetVisibility(false);
}
