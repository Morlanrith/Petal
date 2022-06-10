// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStats.h"

// Sets default values for this component's properties

UCharacterStats::UCharacterStats()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}


// Called when the game starts
void UCharacterStats::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCharacterStats::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UCharacterStats::SetHealth(int healthAmount) {
	MaxHealth = healthAmount;
	CurrentHealth = healthAmount;
}

int32 UCharacterStats::GetHealth() {
	if(CurrentHealth >= 0) return CurrentHealth;
	return 0;
}

bool UCharacterStats::TakeDamage(int damageAmount) {
	CurrentHealth -= damageAmount;
	if (CurrentHealth <= 0) return false;
	return true;
}
