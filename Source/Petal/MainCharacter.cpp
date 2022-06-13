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
}

FAttackStruct::FAttackStruct()
{
	AttackCounter = -1;
	Damage = 1;
	PlayRate = 1.0f;
	SwingingForce = 300;
	ForwardStep = 600;
	UpwardForce = 0.0f;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerStats->SetHealth(MaxHealth);
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsBusy(8)) return;
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
	if (IsBusyMulti({ 5, 6, 7, 8 })) return; // Will not work if the player is petal bursting, shooting, aiming or being hit
	else if (IsBusy(1)) { // If charging
		if (isHeavy) {
			SetBusy(0, true);
			PlayAttackAnim(4);
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
			PlayAttackAnim(isHeavy ? 12 : 9);
		}
		else if (IsBusy(2)) { // If the player is sprinting
			PlayAttackAnim(isHeavy ? 5 : 3);
		}
		else PlayAttackAnim(0);
	}
}

void AMainCharacter::PlayAttackCombo() {
	if (SaveAttack && AttackCounter != -1 && !IsBusy(8)) {
		switch (AttackCounter) {
			case 1:
				PlayAttackAnim(NextHeavy ? 7 : 1);
				return;
			case 2:
				PlayAttackAnim(NextHeavy ? 6 : 2);
				return;
			case 3:
				if (NextHeavy) {
					this->LaunchCharacter(FVector(0.0f, 0.0f, 1000.0f), false, false);
					PlayAttackAnim(8);
					return;
				}
				else break;
			case 4:
				if (!this->GetMovementComponent()->IsFalling()) break;
				this->LaunchCharacter(FVector(0.0f, 0.0f, 500.0f), true, true);
				PlayAttackAnim(NextHeavy ? 12 : 10);
				return;
			case 5:
				if (!this->GetMovementComponent()->IsFalling()) break;
				this->LaunchCharacter(FVector(0.0f, 0.0f, 500.0f), true, true);
				PlayAttackAnim(NextHeavy ? 12 : 11);
				return;
		}
	}
	AttackCounter = 0;
	SetBusy(0, false); // Attacking is now false
	SaveAttack = false;
	NextHeavy = false;
}

void AMainCharacter::StartPetalBurst(float forwardScale, float rightScale) {
	// Will not work if the player is currently attacking, charging, dashing, petal bursting, shooting, or jumping
	if (IsBusyMulti({ 0, 1, 3, 5, 6, 8 }) || this->GetMovementComponent()->IsFalling()) return;
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

void AMainCharacter::PlayAttackAnim(int index) {
	SaveAttack = false;
	CurrentAttack = index;
	//if (index == 4) SwingingForce *= ChargeMultiplier;
	AttackCounter = PlayerAttacks[index].AttackCounter;
	if (PlayerAttacks[index].ForwardStep > 0) this->LaunchCharacter(UKismetMathLibrary::GetForwardVector(FRotator(0.0f, GetMesh()->GetComponentRotation().Yaw + 90.0f, 0.0f)) * PlayerAttacks[index].ForwardStep, false, false);
	this->PlayAnimMontage(PlayerAttacks[index].Anim, PlayerAttacks[index].PlayRate);
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
	if (IsBusyMulti({ 0, 1, 3, 4, 5, 6, 8 })) return;
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

FAttackStruct AMainCharacter::GetCurrentAttack() {
	return PlayerAttacks[CurrentAttack];
}

void AMainCharacter::DamagePlayer(int32 damage, FVector attackLoc, float swingingForce, float upwardsForce) {
	if (!damage || IsBusy(5)) return;
	for(int i = 0; i<7; i++) SetBusy(i, false);
	StopAiming();
	PlayerStats->ReduceHealth(damage);
	PlayerUI->AdjustHealth((float)PlayerStats->GetHealth() / MaxHealth);
	if (upwardsForce > 0.0f || this->GetMovementComponent()->IsFalling())
		this->StopAnimMontage();
	else 
		this->PlayAnimMontage(LoadObject<UAnimMontage>(NULL, UTF8_TO_TCHAR("AnimMontage'/Game/PetalContent/Animation/Player/Animations/Movement/xbot_Impact.xbot_Impact'")), 1.5f);

	FVector xyVelocity = UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::FindLookAtRotation(attackLoc, GetMesh()->GetComponentLocation())) * swingingForce;
	this->LaunchCharacter(FVector(xyVelocity.X, xyVelocity.Y, upwardsForce), true, upwardsForce > 0.0f);
	GetMesh()->SetWorldRotation(FRotator(0.0f, UKismetMathLibrary::FindLookAtRotation(GetMesh()->GetComponentLocation(), attackLoc).Yaw - 90.0f, 0.0f));

	SetBusy(8, true);
}
