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

}

void AScythe::ShootBullet(UAnimMontage* fireAnim, UParticleSystem* sparkFX) {
	if (ParentPlayer->IsPetalBursting || ParentPlayer->IsAttacking || ParentPlayer->IsCharging || ParentPlayer->IsDashing || ParentPlayer->IsShooting) return;
	ParentPlayer->IsShooting = true;
	ParentPlayer->PlayAnimMontage(fireAnim);

	FHitResult hResult;
	FCollisionQueryParams params = FCollisionQueryParams();
	params.AddIgnoredActor(ParentPlayer);
	FVector startLocation, startForward;

	if (ParentPlayer->IsAiming) {
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

