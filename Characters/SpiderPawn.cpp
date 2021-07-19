// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderPawn.h"
#include "Kismet/KismetSystemLibrary.h"

ASpiderPawn::ASpiderPawn()
{
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Spider mesh"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);

	IKScale = GetActorScale3D().Z;

	IKTraceDistance = CollisionSphereRadius * IKScale;
}

void ASpiderPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	

	IKLeftRearFootOffset = FMath::FInterpTo(IKLeftRearFootOffset, GetIKOffsetForASocket(LeftRearFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKLeftFrontFootOffset= FMath::FInterpTo(IKLeftFrontFootOffset, GetIKOffsetForASocket(LeftFrontFootSocketName), DeltaSeconds, IKInterpSpeed); 
	IKRightFrontFootOffset= FMath::FInterpTo(IKRightFrontFootOffset, GetIKOffsetForASocket(RightFrontFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKRightRearFootOffset= FMath::FInterpTo(IKRightRearFootOffset, GetIKOffsetForASocket(RightRearFootSocketName), DeltaSeconds, IKInterpSpeed);
}

float ASpiderPawn::GetIKOffsetForASocket(const FName& SocketName)
{
	float Result = 0.0f;

	FVector SocketLocation = SkeletalMeshComponent->GetSocketLocation(SocketName);
	FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
	FVector TraceEnd = TraceStart - IKTraceDistance * FVector::UpVector;

	FHitResult HitResult;
	ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, TraceTypeQuery, true, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true))
	{
		Result = (TraceEnd.Z - HitResult.Location.Z)/IKScale;
	}
	else if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceEnd,TraceEnd-IKTraceExtendDistance*FVector::UpVector , TraceTypeQuery, true, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true))
	{
		Result = (TraceEnd.Z - HitResult.Location.Z)/IKScale;
	}
	

	return Result;
}
