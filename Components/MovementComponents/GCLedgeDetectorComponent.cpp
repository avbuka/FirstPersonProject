// Fill out your copyright notice in the Description page of Project Settings.

#include "GCLedgeDetectorComponent.h"

#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "../Utils/GCTraceUtils.h"
#include "../Characters/GCBaseCharacter.h"
#include "GCGameInstance.h"
#include "../Subsystems/DebugSubsystem.h"
#include "../Characters/GCPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Utils/ALSMathLibrary.h"



// Called when the game starts
void UGCLedgeDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(GetOwner()->IsA<ACharacter>(), TEXT("UGCLedgeDetectorComponent::BeginPlay() only ACharacter can use UGCLedgeDetector Component"));
	CachedCharacterOwner = StaticCast<ACharacter*>(GetOwner());

	AGCPlayerCharacter* DefaultCharacter = Cast<AGCPlayerCharacter>(CachedCharacterOwner);
	OverlapCapsuleHalfHeight= DefaultCharacter->GetUnchrouchedHalfHeight();
	
}

UGCLedgeDetectorComponent::UGCLedgeDetectorComponent()
{
}

bool UGCLedgeDetectorComponent::DetectLedge(OUT FLedgeDescription& LedgeDescription)
{
	float BottomOffset = 2.0f;
	float DebugDrawTime = 3.0f;
	
	
#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnabled = DebugSubSystem->IsCategoryEnabled(DebugCategoryLedgeDetection);
#else
	bool bIsDebugEnabled = false;
#endif

	FHitResult ForwardCheckHitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());

	UCapsuleComponent* CapsuleComponent = CachedCharacterOwner->GetCapsuleComponent();
	FVector CharacterBottom = CapsuleComponent->GetComponentLocation() -
		( CapsuleComponent->GetScaledCapsuleHalfHeight()- BottomOffset)* FVector::UpVector;

	//1. Forward check
	float ForwardCheckCapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	float ForwardCheckCapsuleHalfHeight = (MaxLedgeHeight - MinLedgeHeight) * 0.5;

	FCollisionShape ForwardCaplsuleShape = FCollisionShape::MakeCapsule(ForwardCheckCapsuleRadius, ForwardCheckCapsuleHalfHeight);
	
	FVector ForwardStartLocation = CharacterBottom + (MinLedgeHeight + ForwardCheckCapsuleHalfHeight) * FVector::UpVector;
	FVector ForwardEndLocation = ForwardStartLocation + (CachedCharacterOwner->GetActorForwardVector() * ForwardCheckDistance);
	
	if (!GCTraceUtils::SweepCapsuleSingleByChannel(GetWorld(), ForwardCheckHitResult, ForwardStartLocation, ForwardEndLocation, ForwardCheckCapsuleRadius, ForwardCheckCapsuleHalfHeight, FQuat::Identity, ECC_Climbing, QueryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnabled, DebugDrawTime))
	{
		return false;

	}

	//2. Downward check 

	float EdgeOffset = 5.0f;
	float DownwardCheckSphereRadius = 5;

	FHitResult DownwardCheckHitResult;
	FCollisionShape DownwardSphereShape = FCollisionShape::MakeSphere(DownwardCheckSphereRadius);
	FVector DownwardCheckStartLocation = ForwardCheckHitResult.ImpactPoint - ForwardCheckHitResult.ImpactNormal* EdgeOffset;
	DownwardCheckStartLocation.Z = CharacterBottom.Z + MaxLedgeHeight + DownwardCheckSphereRadius+MinLedgeHeight;

	FVector DownwardCheckEndLocation(DownwardCheckStartLocation.X, DownwardCheckStartLocation.Y, CharacterBottom.Z);
	
	if (!GCTraceUtils::SweepSphereSingleByChannel(GetWorld(), DownwardCheckHitResult, DownwardCheckStartLocation, DownwardCheckEndLocation, DownwardCheckSphereRadius, ECC_Climbing, QueryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnabled, DebugDrawTime))
	{
		return false;
	}
	
	if (DownwardCheckHitResult.ImpactPoint.Z- CharacterBottom.Z < MinLedgeHeight)
	{
		return false;
	}

	//3. Overlap check

	float OverlapCapsuleRadius = CapsuleComponent->GetUnscaledCapsuleRadius();
	FVector OverlapLocation = DownwardCheckHitResult.ImpactPoint + (OverlapCapsuleHalfHeight + EdgeOffset) * FVector::UpVector;

	if (GCTraceUtils::OverlapBlockingTestByProfile(GetWorld(), OverlapLocation, OverlapCapsuleRadius, OverlapCapsuleHalfHeight, FQuat::Identity, PawnCollisionProfile, QueryParams, bIsDebugEnabled, DebugDrawTime))
	{
		return false;
	}
	LedgeDescription.LedgeComponent = DownwardCheckHitResult.Component;
	
	LedgeDescription.TranformWS = FTransform((ForwardCheckHitResult.ImpactNormal * FVector(-1.0, -1.0, 0)).ToOrientationRotator(), OverlapLocation-EdgeOffset*FVector::UpVector, FVector::OneVector);

	//4. Sweep check 
	
	// Starting position slightly behind to account for very close actor location to the ledge
	FQuat CapsuleRotation = (OverlapLocation - ForwardStartLocation).ToOrientationQuat();
	FVector AnimationStartPosition = ForwardCheckHitResult.ImpactNormal * OverlapCapsuleRadius * 2.0f + ForwardCheckHitResult.ImpactPoint;
	
	if (GCTraceUtils::SweepCapsuleSingleByChannel(GetWorld(), ForwardCheckHitResult, AnimationStartPosition, OverlapLocation, OverlapCapsuleRadius, OverlapCapsuleRadius, CapsuleRotation, ECC_Visibility, QueryParams, FCollisionResponseParams(), bIsDebugEnabled, DebugDrawTime))
	{
		return false;
	}

	
	return true;
}

