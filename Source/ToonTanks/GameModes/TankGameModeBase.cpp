// Fill out your copyright notice in the Description page of Project Settings.


#include "TankGameModeBase.h"
#include "ToonTanks/Pawns/PawnTank.h"
#include "ToonTanks/Pawns/PawnTurret.h"
#include "Kismet/GameplayStatics.h"
#include "ToonTanks/PlayerControllers/PlayerControllerBase.h"

void ATankGameModeBase::BeginPlay()
{
	Super::BeginPlay();



	HandleGameStart();
}

void ATankGameModeBase::HandleGameStart()
{
	TargetTurrets = GetTargetTurretsCount();
	PlayerTank = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
	PlayerController = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(this, 0));
	
	GameStart();

	if (PlayerController)
	{
		PlayerController->SetPlayerEnabledState(false);

		FTimerHandle PlayerEnabledHandle;
		FTimerDelegate PlayerEnabledDelegate = FTimerDelegate::CreateUObject(PlayerController, &APlayerControllerBase::SetPlayerEnabledState, true);

		GetWorld()->GetTimerManager().SetTimer(PlayerEnabledHandle, PlayerEnabledDelegate, StartDelay, false);
	}
}


void ATankGameModeBase::ActorDied(AActor* DeadActor)
{

	if (DeadActor == PlayerTank)
	{
		PlayerTank->HandleDestruction();
		HandleGameOver(false);

		if(PlayerController)
		{
			PlayerController->SetPlayerEnabledState(false);
		}
	}
	else if (APawnTurret* DestroyedTurret = Cast<APawnTurret>(DeadActor))
	{
		DestroyedTurret->HandleDestruction();
		--TargetTurrets;
		
		if(TargetTurrets <= 0)
		{
			HandleGameOver(true);
		}
	}
}			


void ATankGameModeBase::HandleGameOver(bool PlayerWon)
{
	GameOver(PlayerWon);
}

int32 ATankGameModeBase::GetTargetTurretsCount() const
{
	TArray<AActor*> TurretActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawnTurret::StaticClass(), TurretActors);
	return TurretActors.Num();
}
