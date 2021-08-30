// Fill out your copyright notice in the Description page of Project Settings.


#include "FPBaseCharacterAniminstance.h"
#include "../FPPlayerCharacter.h"

void UFPBaseCharacterAniminstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AFPPlayerCharacter>(), TEXT("void UFPBaseCharacterAniminstance::NativeBeginPlay() only FPPlayer allowed in this AnimInstance"));
	ChachedFPCharacterOwner = StaticCast<AFPPlayerCharacter*>(TryGetPawnOwner());
}

void UFPBaseCharacterAniminstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!ChachedFPCharacterOwner.IsValid())
	{
		return;
	}
	APlayerController* Controller = ChachedFPCharacterOwner->GetController<APlayerController>();

	if (IsValid(Controller))
	{
		PlayerCamerPitchAngle = Controller->GetControlRotation().Pitch;
	}
}
