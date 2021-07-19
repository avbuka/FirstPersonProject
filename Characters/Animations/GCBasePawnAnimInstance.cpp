// Fill out your copyright notice in the Description page of Project Settings.


#include "GCBasePawnAnimInstance.h"
#include "../GameCodeBasePawn.h"
#include "../../Components/MovementComponents/GCBasicMovementComponent.h"

void UGCBasePawnAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	//checkf(TryGetPawnOwner()->IsA<AGameCodeBasePawn>(), TEXT("UGCBasePawnAnimInstance::NativeBeginPlay() only GameCodeBasePawn can work with void UGCBasePawnAnimInstance"))
	CachedBasePawn = StaticCast<AGameCodeBasePawn*>(TryGetPawnOwner());

	 
}

void UGCBasePawnAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!CachedBasePawn.IsValid())
	{
		return;
	}

	InputForward = CachedBasePawn->GetInputForward();
	InputRight = CachedBasePawn->GetInputRight();

	bIsInAir = CachedBasePawn->GetMovementComponent()->IsFalling();

}
