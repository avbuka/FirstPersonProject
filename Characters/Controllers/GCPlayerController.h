// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../GCBaseCharacter.h"
#include "GCPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class XYZPROJECT_API AGCPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void SetPawn(APawn* InPawn) override;

protected:
	virtual void SetupInputComponent() override;

private:
	TSoftObjectPtr<class AGCBaseCharacter> ChachedBaseCharacter;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void TurnAtRate(float Value);
	void LookUpAtRate(float Value);
	void Jump();
	void Mantle();
	void Slide();

	void SwimForward(float Value);
	void SwimRight(float Value);
	void SwimUp(float Value);

	void ChangeCrouchState();
	void ChangeCrawlState();
	void StartSprint();
	void StopSprint();

	void InteractWithIActor();
	void ClimbLadderUp(float Value);
	
};
