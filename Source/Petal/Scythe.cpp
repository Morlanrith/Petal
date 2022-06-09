// Fill out your copyright notice in the Description page of Project Settings.


#include "Scythe.h"

// Sets default values
AScythe::AScythe()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AScythe::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AScythe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ParentPlayer->IsBusy(1) && ChargeMultiplier < 5.0f) ChargeMultiplier += DeltaTime; // If the player is charging an attack, increase its attack multiplier
	else if (!ParentPlayer->IsBusy(0) && CurrentEnemies.Num() > 0) {
		CurrentEnemies.Empty();
		ChargeMultiplier = 1.0f;
	}
}

void AScythe::ShootBullet(UAnimMontage* fireAnim, UParticleSystem* sparkFX) {
	// Will not work if the player chracter is attacking, charging, dashing, petal bursting, or shooting
	if (ParentPlayer->IsBusyMulti({ 0, 1, 3, 5, 6 })) return;
	ParentPlayer->SetBusy(6, true); // Shooting is now true
	ParentPlayer->PlayAnimMontage(fireAnim);

	FHitResult hResult;
	FCollisionQueryParams params = FCollisionQueryParams();
	params.AddIgnoredActor(ParentPlayer);
	FVector startLocation, startForward;

	if (ParentPlayer->IsBusy(7)) { // If the player is aiming
		FVector cameraLocation = ParentPlayer->CameraArm->GetChildComponent(0)->GetComponentLocation();
		FVector cameraForward = ParentPlayer->CameraArm->GetChildComponent(0)->GetForwardVector();
		startLocation = cameraLocation + (150.0f * cameraForward);
		startForward = cameraForward;
	}
	else {
		startLocation = GetActorLocation();
		startForward = ParentPlayer->GetMesh()->GetRightVector();
	}

	if (GetWorld()->LineTraceSingleByObjectType(
		hResult,
		startLocation,
		startLocation + (startForward*5000.0f),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_Pawn),
		params)) {

		if (!Cast<ABasicEnemy>(hResult.Actor)) return;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), sparkFX, FTransform(hResult.Location));
	}
}

