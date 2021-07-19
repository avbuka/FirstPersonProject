// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderPawnAnimInstance.h"
#include "../SpiderPawn.h"

void USpiderPawnAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<ASpiderPawn>(), TEXT("USpiderPawnAnimInstance::NativeBeginPlay() Can only use ASpiderPawn"));
	CachedSpiderPawnOwner = StaticCast<ASpiderPawn*>(TryGetPawnOwner());
}

void USpiderPawnAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!CachedSpiderPawnOwner.IsValid())
	{
		return;
	}

	RightFrontFootEffectorLocation = FVector(CachedSpiderPawnOwner->GetIKRightFrontFoorOffset(), 0.f, 0.f);
	RightRearFootEffectorLocation = FVector(CachedSpiderPawnOwner->GetIKRightRearFoorOffset(), 0.f, 0.f);	
	
	LeftFrontFootEffectorLocation = FVector(CachedSpiderPawnOwner->GetIKLeftFrontFoorOffset(), 0.f, 0.f);
	LeftRearFootEffectorLocation = FVector(CachedSpiderPawnOwner->GetIKLeftRearFoorOffset(), 0.f, 0.f);
}
