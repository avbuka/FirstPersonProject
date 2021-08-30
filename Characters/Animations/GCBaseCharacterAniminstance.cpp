// Fill out your copyright notice in the Description page of Project Settings.


#include "GCBaseCharacterAniminstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Components/MovementComponents/GCBaseCharacterMovementComponent.h"

void UGCBaseCharacterAniminstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	checkf(TryGetPawnOwner()->IsA<AGCPlayerCharacter>(), 
		TEXT("UGCBaseCharacterAniminstance::NativeBeginPlay() can use only AGCBaseCharacter"));
	CachedBasedCharacter = StaticCast<AGCPlayerCharacter*>(TryGetPawnOwner());
}

void UGCBaseCharacterAniminstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!CachedBasedCharacter.IsValid())
	{
		return;
	}
	
	UGCBaseCharacterMovementComponent* CharacterMovementC = CachedBasedCharacter->GetCharacterBaseMovementComponent();

	
	bIsFalling		= CharacterMovementC->IsFalling();
	bIsCrouching	= CharacterMovementC->IsCrouching();
	bIsCrawling		= CharacterMovementC->IsCrawling();
	bIsSprinting	= CharacterMovementC->IsSprinting();
	bIsOutOfStamina = CharacterMovementC->IsOutOfStamina();
	bIsSwimming		= CharacterMovementC->IsSwimming();
	bIsClimbing		= CharacterMovementC->IsOnLadder();
	bIsZiplining	= CharacterMovementC->IsZiplining();
	bIsWallRunning  = CharacterMovementC->IsWallRunning();
	bIsSliding		= CharacterMovementC->IsSliding();

	WallrunningSide = CharacterMovementC->GetWallRunningSide();
	CharacterSpeed  = CharacterMovementC->Velocity.Size();
	
	if (bIsClimbing)
	{
		LadderSpeedRatio = CharacterMovementC->GetLadderSpeedRatio();
	}

	
	CharacterSpeed  = CharacterMovementC->Velocity.Size();
	IKHipOffset		= FVector(0.0f, 0.0f, -CachedBasedCharacter->GetIKHipOffset());

	LeftFootEffectorLocation  = FVector(CachedBasedCharacter->GetIKLeftFootOffset()  + IKHipOffset.Z,0,0);
	RightFootEffectorLocation = FVector(CachedBasedCharacter->GetIKRightFootOffset() + IKHipOffset.Z,0,0);
	
	//LeftArmEffectorLocation = FVector(0  , CachedBasedCharacter->GetIKLeftArmOffset(),0);
	LeftArmEffectorLocation = CachedBasedCharacter->GetLeftArmIK();
	RightArmEffectorLocation = CachedBasedCharacter->GetRightArmIK();
	
}
