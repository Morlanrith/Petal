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

void AScythe::ShootBullet(FVector playerPos, UAnimMontage* fireAnim, UParticleSystem* sparkFX) {
	if (ParentPlayer->IsPetalBursting || ParentPlayer->IsAttacking || ParentPlayer->IsCharging || ParentPlayer->IsDashing || ParentPlayer->IsShooting || ParentPlayer->GetMovementComponent()->IsFalling()) return;
	ParentPlayer->IsShooting = true;
	ParentPlayer->PlayAnimMontage(fireAnim);
	FHitResult hResult;
	FCollisionQueryParams params = FCollisionQueryParams();
	params.AddIgnoredActor(ParentPlayer);
	if (GetWorld()->LineTraceSingleByObjectType(
		hResult,
		GetActorTransform().GetLocation(),
		GetActorTransform().GetLocation() + (playerPos*5000.0f),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_Pawn),
		params)) {

		if (!Cast<ABasicEnemy>(hResult.Actor)) return;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), sparkFX, FTransform(hResult.Location));
	}
}

