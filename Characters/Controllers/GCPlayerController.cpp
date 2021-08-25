// Fill out your copyright notice in the Description page of Project Settings.


#include "GCPlayerController.h"
#include "../GCBaseCharacter.h"
#include "../GCPlayerCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
#include "../Components/MovementComponents/GCBaseCharacterMovementComponent.h"


void AGCPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	ChachedBaseCharacter = Cast<AGCPlayerCharacter>(InPawn);
}

void AGCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &AGCPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AGCPlayerController::MoveRight);
	InputComponent->BindAxis("SwimForward", this, &AGCPlayerController::SwimForward);
	InputComponent->BindAxis("SwimRight", this, &AGCPlayerController::SwimRight);	
	InputComponent->BindAxis("ClimbLadderUp", this, &AGCPlayerController::ClimbLadderUp);
	InputComponent->BindAxis("SwimUp", this, &AGCPlayerController::SwimUp);
	InputComponent->BindAxis("Turn", this, &AGCPlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &AGCPlayerController::LookUp);
	InputComponent->BindAxis("TurnAtRate", this, &AGCPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUpAtRate", this, &AGCPlayerController::LookUpAtRate);
	InputComponent->BindAction("Mantle", IE_Pressed, this, &AGCPlayerController::Mantle);
	InputComponent->BindAction("InteractWithObject", IE_Pressed, this, &AGCPlayerController::InteractWithIActor);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AGCPlayerController::Jump);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &AGCPlayerController::ChangeCrouchState);
	InputComponent->BindAction("Crawl", IE_Pressed, this, &AGCPlayerController::ChangeCrawlState);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AGCPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &AGCPlayerController::StopSprint);

}

void AGCPlayerController::MoveForward(float Value)
{
	if (ChachedBaseCharacter.IsValid())
	{
		if (!ChachedBaseCharacter->GetMovementComponent()->IsSwimming())
		{
			ChachedBaseCharacter->MoveForward(Value);

		}
	}
}

void AGCPlayerController::MoveRight(float Value)
{
	if (ChachedBaseCharacter.IsValid())
	{
		if (!ChachedBaseCharacter->GetMovementComponent()->IsSwimming())
		{
			ChachedBaseCharacter->MoveRight(Value);

		}
	}
}

void AGCPlayerController::Turn(float Value)
{
	if (ChachedBaseCharacter.IsValid())
	{
		ChachedBaseCharacter->Turn(Value);
	}
}

void AGCPlayerController::LookUp(float Value)
{
	if (ChachedBaseCharacter.IsValid())
	{
		ChachedBaseCharacter->LookUp(Value);
	}
}

void AGCPlayerController::TurnAtRate(float Value)
{
	if (ChachedBaseCharacter.IsValid())
	{
		ChachedBaseCharacter->TurnAtRate(Value);
	}
}

void AGCPlayerController::LookUpAtRate(float Value)
{
	if (ChachedBaseCharacter.IsValid())
	{
		ChachedBaseCharacter->LookUpAtRate(Value);
	}
}

void AGCPlayerController::Jump()
{
	if (ChachedBaseCharacter.IsValid())
	{
		if (ChachedBaseCharacter->GetMovementComponent()->IsCrouching())
		{
			ChangeCrouchState();
			return;
		}
		if (ChachedBaseCharacter->GetCharacterBaseMovementComponent()->IsCrawling())
		{
			ChachedBaseCharacter->GetCharacterBaseMovementComponent()->bWantsToStandUp = true;
			ChangeCrawlState();
			return;
		}
		if (ChachedBaseCharacter->GetCharacterBaseMovementComponent()->IsZiplining())
		{
			ChachedBaseCharacter->GetCharacterBaseMovementComponent()->DetachFromZipline();
			return;
		}

		ChachedBaseCharacter->Jump();
	}
}

void AGCPlayerController::Mantle()
{
	if (ChachedBaseCharacter.IsValid())
	{
		if (!ChachedBaseCharacter->GetCharacterBaseMovementComponent()->IsMantling() &&
			!ChachedBaseCharacter->GetCharacterBaseMovementComponent()->IsWallRunning())
		{
			ChachedBaseCharacter->Mantle();
		}
	}
}

void AGCPlayerController::SwimForward(float Value)
{
	if (ChachedBaseCharacter.IsValid())
	{
		if (ChachedBaseCharacter->GetMovementComponent()->IsSwimming())
		{
			ChachedBaseCharacter->SwimForward(Value);
		}
	}
}

void AGCPlayerController::SwimRight(float Value)
{
	if (ChachedBaseCharacter.IsValid())
	{
		if (ChachedBaseCharacter->GetMovementComponent()->IsSwimming())
		{
			ChachedBaseCharacter->SwimRight(Value);
		}
	}
}

void AGCPlayerController::SwimUp(float Value)
{
	if (ChachedBaseCharacter.IsValid())
	{
		if (ChachedBaseCharacter->GetMovementComponent()->IsSwimming())
		{
			ChachedBaseCharacter->SwimUp(Value);
		}
	}
}

void AGCPlayerController::ChangeCrouchState()
{
	if (ChachedBaseCharacter.IsValid())
	{
		if (!ChachedBaseCharacter->GetCharacterBaseMovementComponent()->IsCrawling())
		{
			ChachedBaseCharacter->ChangeCrouchState();
		}
	}
}

void AGCPlayerController::ChangeCrawlState()
{
	if (ChachedBaseCharacter.IsValid())
	{
		
		if (ChachedBaseCharacter->GetCharacterMovement()->IsCrouching() ||
			ChachedBaseCharacter->GetCharacterBaseMovementComponent()->IsCrawling())
		{
			ChachedBaseCharacter->ChangeCrawlState();
		}
	}
}


void AGCPlayerController::StartSprint()
{
	if (ChachedBaseCharacter.IsValid())
	{
		if (ChachedBaseCharacter->bIsCrouched)
		{
			ChachedBaseCharacter->UnCrouch();
		}
		if (!ChachedBaseCharacter->GetMovementComponent()->IsFalling() &&
				ChachedBaseCharacter->GetMovementComponent()->IsMovingOnGround()) 
		{
			ChachedBaseCharacter->StartSprint();		
		}
		
	}
}

void AGCPlayerController::StopSprint()
{
	if (ChachedBaseCharacter.IsValid())
	{
		ChachedBaseCharacter->StopSprint();
		
	}
}

void AGCPlayerController::InteractWithIActor()
{
	if (ChachedBaseCharacter.IsValid())
	{
		ChachedBaseCharacter->InteractWithIActor();
	}
}

void AGCPlayerController::ClimbLadderUp(float Value)
{
	if (ChachedBaseCharacter.IsValid())
	{
		ChachedBaseCharacter->ClimbLadderUp(Value);

	}
}

