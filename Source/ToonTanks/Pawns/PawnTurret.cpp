// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnTurret.h"
#include <Kismet/GameplayStatics.h>
#include "PawnTank.h"
#include "DrawDebugHelpers.h"

// Sets default values
APawnTurret::APawnTurret()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APawnTurret::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &APawnTurret::CheckFireCondition, FireRate, true);

	PlayerPawn = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void APawnTurret::HandleDestruction()
{
	Super::HandleDestruction();

	Destroy();
}

void APawnTurret::CheckFireCondition()
{
	if(!PlayerPawn || !PlayerPawn->GetIsPlayerAlive())
	{
		return;
	}

	if(ReturnDistanceToPlayer() <= FireRange && IsPlayerVisible())
	{
		Fire();
	}
}

float APawnTurret::ReturnDistanceToPlayer() const
{
	if (!PlayerPawn)
	{
		return 0.f;
	}

	return FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());
}

bool APawnTurret::IsPlayerVisible() const
{
	if (!PlayerPawn)
	{
		return 0.f;
	}
	
	FVector Start = ProjectileSpawnPoint->GetComponentLocation();
	FVector End = PlayerPawn->GetActorLocation();
	
	FCollisionQueryParams TraceParams;
	FHitResult Hit;

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

	auto color = Hit.GetActor() == PlayerPawn ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), Start, End, color, false, 1.f, 0, 3.f);
	
	if(Hit.GetActor() == PlayerPawn)
	{
		return true;
	}
	
	return false;
}

// Called every frame
void APawnTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!PlayerPawn || ReturnDistanceToPlayer() > FireRange)
	{
		return;
	}

	RotateTurret(PlayerPawn->GetActorLocation());
}
