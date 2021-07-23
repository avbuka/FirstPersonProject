// Fill out your copyright notice in the Description page of Project Settings.


#include "GCBaseCharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Curves/CurveVector.h"
#include "GameFramework/CheatManager.h"
#include "DrawDebugHelpers.h"
#include "../Utils/ALSMathLibrary.h"

void UGCBaseCharacterMovementComponent::SetIsOutOfStamina(bool bIsOutOfStamina_In)
{
	bIsOutOfStamina = bIsOutOfStamina_In;
	if (bIsOutOfStamina)
	{
		StopSprint();
		SetJumpAllowed(false);
	}
	else
	{
		SetJumpAllowed(true);
	}
}

void UGCBaseCharacterMovementComponent::SetIsCrawling(bool bIsProning_In)
{
	bIsCrawling = bIsProning_In;
}

float UGCBaseCharacterMovementComponent::GetMaxSpeed() const
{	
	if (bIsSprinting)
	{
		return SprintSpeed;
	}
	else if (bIsOutOfStamina)
	{
		return OutOfStaminaMaxSpeed;
	}
	else if (bIsCrawling)
	{
		return CrawlSpeed;
	}		
	else if (IsOnLadder())
	{
		return ClimbingMaxSpeed;
	}
	else if (IsZiplining())
	{
		return ZipliningSpeed;
	}


	return Super::GetMaxSpeed();
}

void UGCBaseCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);

	// proxies get replicated crawl state.
	if (GCPlayerCharacter->GetLocalRole() != ROLE_SimulatedProxy)
	{
		//check for a change in crawl state. 

		if (bIsCrawling && (!bWantsToCrawl || !CanCrawlInCurrentState()))
		{
			UnCrawl();
		}
		else if (!bIsCrawling && bWantsToCrawl)
		{
			Crawl();
		}
		if (bWantsToMantle && CanMantleInCurrentState())
		{
			GCPlayerCharacter->Mantle();
		}
	}
}

void UGCBaseCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);

	// proxies get replicated crouch state 
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		//uncrawl if no longer allowed to crawl
		if (IsCrawling() && !CanCrawlInCurrentState())
		{
			UnCrawl();
		}
	}

}

void UGCBaseCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (bForceRotation)
	{
		// Accumulate a desired new rotation.
		const float AngleTolerance = 1e-3f;


		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
		CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

		FRotator DeltaRot = GetDeltaRotation(DeltaTime);
		DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));

		FRotator DesiredRotation = ForceTargetRotation;

		if (!CurrentRotation.Equals(DesiredRotation, AngleTolerance))
		{
			// PITCH
			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
			{
				DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
			{
				DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
			{
				DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
			}

			// Set the new rotation.
			DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
			MoveUpdatedComponent(FVector::ZeroVector, DesiredRotation, /*bSweep*/ false);
		}
		else
		{
			bForceRotation = false;
			ForceTargetRotation = FRotator::ZeroRotator;
		}
		
		return;
	}
	
	if (IsOnLadder())
	{
		return;
	}

	Super::PhysicsRotation(DeltaTime);
}

void UGCBaseCharacterMovementComponent::Crawl()
{
	if (!HasValidData())
	{
		return;
	}
	if (!CanCrawlInCurrentState() || CrawlOverlapsWithSomething())
	{
		bWantsToCrawl = false;
		bWantsToCrouch = true;
		return;
	}

	//see if collision is already at desired size
	if (GCPlayerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == CrawlingHalfHeight)
	{

		bWantsToCrouch = true;
		bIsCrawling = true;
		GCPlayerCharacter->OnCrawlStart(0.0f, 0.0f);
		return;
	}

	//change collision size to crawling dimensions

	const float ComponentScale = GCPlayerCharacter->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = GCPlayerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledHalfRadius = GCPlayerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();

	GCPlayerCharacter->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledHalfRadius, CrawlingHalfHeight);

	//in case height was less than radius. the capsule will be a sphere
	float HalfHeightAdjust = (OldUnscaledHalfHeight - GCPlayerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;;


	// Crawling to a larger height? (this is rare)
	if (CrawlingHalfHeight > OldUnscaledHalfHeight)
	{
		FCollisionQueryParams CapsuleParams;
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);
		const bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(UpdatedComponent->GetComponentLocation() - FVector(0.f, 0.f, CrawlingHalfHeight), FQuat::Identity,
			UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);

		// If encroached, cancel
		if (bEncroached)
		{
			CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledHalfRadius, OldUnscaledHalfHeight);
			return;
		}
	}
	if (bCrawlMaintainsBaseLocation)
	{
		// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
		UpdatedComponent->MoveComponent(FVector(0.0f, 0.0f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	}
	bIsCrawling = true;



	bForceNextFloorCheck = true;

	//onstartCrawl takes the change from the default size, not the current one (though the are usually the same)
	const float MeshAdjust = HalfHeightAdjust;
	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	HalfHeightAdjust = OldUnscaledHalfHeight - CrawlingHalfHeight;
	HalfHeightAdjust *= ComponentScale;

	AdjustProxyCapsuleSize();
	GCPlayerCharacter->OnCrawlStart(HalfHeightAdjust, ScaledHalfHeightAdjust);


}

void UGCBaseCharacterMovementComponent::UnCrawl()
{

	if (!HasValidData())
	{
		return;
	}

	// See if collision is already at desired size.
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == CrouchedHalfHeight)
	{
		bIsCrawling = false;
		GCPlayerCharacter->OnCrawlEnd(0.f, 0.f);
		return;
	}

	const float CurrentCrawlingHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = CrouchedHalfHeight - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	// Grow to uncrawling size.
	check(CharacterOwner->GetCapsuleComponent());


	// Try to stay in place and see if the larger capsule fits. We use a slightly taller capsule to avoid penetration.
	const UWorld* MyWorld = GetWorld();
	const float SweepInflation = KINDA_SMALL_NUMBER * 10.f;
	FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleParams, ResponseParam);

	// Compensate for the difference between current capsule size and crouching size
	const FCollisionShape CrouchingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust); // Shrink by negative amount, so actually grow it.
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
	bool bEncrawled = true;

	if (!bCrawlMaintainsBaseLocation)
	{
		// Expand in place
		bEncrawled = MyWorld->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, CollisionChannel, CrouchingCapsuleShape, CapsuleParams, ResponseParam);

		if (bEncrawled)
		{
			// Try adjusting capsule position to see if we can avoid encroachment.
			if (ScaledHalfHeightAdjust > 0.f)
			{
				// Shrink to a short capsule, sweep down to base to find where that would hit something, and then try to stand up from there.
				float PawnRadius, PawnHalfHeight;
				CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
				const float ShrinkHalfHeight = PawnHalfHeight - PawnRadius;
				const float TraceDist = PawnHalfHeight - ShrinkHalfHeight;
				const FVector Down = FVector(0.f, 0.f, -TraceDist);

				FHitResult Hit(1.f);
				const FCollisionShape ShortCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, ShrinkHalfHeight);
				const bool bBlockingHit = MyWorld->SweepSingleByChannel(Hit, PawnLocation, PawnLocation + Down, FQuat::Identity, CollisionChannel, ShortCapsuleShape, CapsuleParams);
				if (Hit.bStartPenetrating)
				{
					bEncrawled = true;
				}
				else
				{
					// Compute where the base of the sweep ended up, and see if we can stand there
					const float DistanceToBase = (Hit.Time * TraceDist) + ShortCapsuleShape.Capsule.HalfHeight;
					const FVector NewLoc = FVector(PawnLocation.X, PawnLocation.Y, PawnLocation.Z - DistanceToBase + CrouchingCapsuleShape.Capsule.HalfHeight + SweepInflation + MIN_FLOOR_DIST / 2.f);
					bEncrawled = MyWorld->OverlapBlockingTestByChannel(NewLoc, FQuat::Identity, CollisionChannel, CrouchingCapsuleShape, CapsuleParams, ResponseParam);
					if (!bEncrawled)
					{
						// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
						UpdatedComponent->MoveComponent(NewLoc - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
					}
				}
			}
		}
	}
	else
	{
		// Expand while keeping base location the same.
		FVector CrouchingLocation = PawnLocation + FVector(0.f, 0.f, CrouchingCapsuleShape.GetCapsuleHalfHeight() - CurrentCrawlingHalfHeight);
		bEncrawled = MyWorld->OverlapBlockingTestByChannel(CrouchingLocation, FQuat::Identity, CollisionChannel, CrouchingCapsuleShape, CapsuleParams, ResponseParam);

		if (bEncrawled)
		{
			if (IsMovingOnGround())
			{
				// Something might be just barely overhead, try moving down closer to the floor to avoid it.
				const float MinFloorDist = KINDA_SMALL_NUMBER * 10.f;
				if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
				{
					CrouchingLocation.Z -= CurrentFloor.FloorDist - MinFloorDist;
					bEncrawled = MyWorld->OverlapBlockingTestByChannel(CrouchingLocation, FQuat::Identity, CollisionChannel, CrouchingCapsuleShape, CapsuleParams, ResponseParam);
				}
			}
		}

		if (!bEncrawled)
		{
			// Commit the change in location.
			UpdatedComponent->MoveComponent(CrouchingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
			bForceNextFloorCheck = true;
		}
	}

	// If still encroached then abort.
	if (bEncrawled)
	{
		return;
	}


	bIsCrawling = false;
	if (!bWantsToStandUp)
	{
		bWantsToCrouch = true;
		CharacterOwner->bIsCrouched = true;
	}
	else
	{
		bWantsToCrouch = false;
		CharacterOwner->bIsCrouched = true;
	}
	bWantsToStandUp = false;

	// Now call SetCapsuleSize() to cause touch/untouch events and actually grow the capsule
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), CrouchedHalfHeight, true);

	const float MeshAdjust = ScaledHalfHeightAdjust;
	AdjustProxyCapsuleSize();
	GCPlayerCharacter->OnCrawlEnd(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

UGCBaseCharacterMovementComponent::UGCBaseCharacterMovementComponent()
{
}

bool UGCBaseCharacterMovementComponent::CrawlOverlapsWithSomething()
{
	//perform the overlap test, using a character-sized box

	FVector BoxHalfSize(GCPlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
		GCPlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius(),
		GCPlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius()
	);

	float Offset = 3.0f;
	FVector OverlapLocation(GetActorLocation().X,
		GetActorLocation().Y,
		GetActorLocation().Z - GCPlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()
		+ BoxHalfSize.Z + Offset
	);

	FCollisionQueryParams CollisionParams;
	FCollisionResponseParams CollisionResponceParams;
	FCollisionShape CrawlShape = FCollisionShape::MakeBox(BoxHalfSize);
	InitCollisionParams(CollisionParams, CollisionResponceParams);
	CollisionParams.AddIgnoredActor(GetOwner());

	bool bOverlapsWithSomething = GetWorld()->OverlapBlockingTestByChannel(OverlapLocation, GCPlayerCharacter->GetActorRotation().Quaternion(), ECollisionChannel::ECC_Visibility, CrawlShape, CollisionParams, CollisionResponceParams);

	if (bOverlapsWithSomething)
	{
		return true;
	}
	return false;
}

bool UGCBaseCharacterMovementComponent::CanCrawlInCurrentState()
{
	if (!IsFalling() && IsMovingOnGround() && CanEverCrawl() && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics())
	{
		return true;
	}

	return false;
}

bool UGCBaseCharacterMovementComponent::IsMantling() const
{
	return (UpdatedComponent && (MovementMode == MOVE_Custom) && (CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Mantling));
}

bool UGCBaseCharacterMovementComponent::IsOnLadder() const
{
	return (UpdatedComponent && (MovementMode == MOVE_Custom) && (CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_ClimbingLadder));
}

void UGCBaseCharacterMovementComponent::StartSprint()
{
	bIsSprinting = true;
	bForceMaxAccel = 1;
}

void UGCBaseCharacterMovementComponent::StopSprint()
{
	bIsSprinting = false;
	bForceMaxAccel = 0;
	GCPlayerCharacter->TryChangeSprintState(GetWorld()->GetDeltaSeconds());
}

void UGCBaseCharacterMovementComponent::StartMantle(const FMantlingMovementParameters& MantlingParams)
{
	CurrentMantlingParameters = MantlingParams;
	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Mantling);
	
	//for debug DELETE later
	GCPlayerCharacter->ToggleSlowmo();

	
}

void UGCBaseCharacterMovementComponent::EndMantle()
{
	SetMovementMode(MOVE_Walking);	
	
	//for debug DELETE later
	GCPlayerCharacter->ToggleSlowmo();
}

void UGCBaseCharacterMovementComponent::AttachToLadder(const class ALadder* Ladder)
{
	CurrentLadder = Ladder;
	FRotator TargetOrientationRotation = CurrentLadder->GetActorForwardVector().ToOrientationRotator();
	FVector NewCharacterLocation = FVector::ZeroVector;

	TargetOrientationRotation.Yaw += 180.0f;
	

	GetActorToCurrentLadderProjection(GetActorLocation());

	if (Ladder->GetIsCharacterOnTop())
	{
		NewCharacterLocation = CurrentLadder->GetAttachFromTopAnimMontageInitialOffset();
	}
	else
	{
		NewCharacterLocation = CurrentLadder->GetActorLocation() + GetActorToCurrentLadderProjection(GetActorLocation())
			* CurrentLadder->GetActorUpVector() + LadderToCharacterOffset * CurrentLadder->GetActorForwardVector();
	}
	
	

	GetOwner()->SetActorLocation(NewCharacterLocation);
	GetOwner()->SetActorRotation(TargetOrientationRotation);

	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_ClimbingLadder);
}

void UGCBaseCharacterMovementComponent::AttachToZipline(const class AZipline* Zipline)
{
	//GCPlayerCharacter->ToggleSlowmo();

	CurrentZipline = Zipline;
	bIsZiplining = true;
	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Ziplining);
}

float UGCBaseCharacterMovementComponent::GetActorToCurrentLadderProjection(const FVector& Location) const
{
	checkf(IsValid(CurrentLadder), TEXT("float UGCBaseCharacterMovementComponent::GetCharacterToCurrentLadderProjection() Current ladder is null"));

	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderToCharacterDistance = Location - CurrentLadder->GetActorLocation();

	return FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);
}

float UGCBaseCharacterMovementComponent::GetLadderSpeedRatio() const
{
	checkf(IsValid(CurrentLadder), TEXT("float UGCBaseCharacterMovementComponent::GetLadderSpeedRatio() The ladder is null"));

	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	return FVector::DotProduct(LadderUpVector, Velocity) / ClimbingMaxSpeed;
}

void UGCBaseCharacterMovementComponent::DetachFromLadder(EDetachFromLadderMethod DMethod/*= Fall*/)
{
	switch (DMethod)
	{
	case EDetachFromLadderMethod::Fall:
	{
		
		SetMovementMode(MOVE_Falling);
		break;
	}
	case EDetachFromLadderMethod::ReachingTheTop:
	{
		
		GCPlayerCharacter->Mantle(true);
		break;
	}
	case EDetachFromLadderMethod::ReachingTheBottom:
	{
		
		SetMovementMode(MOVE_Walking);
		break;
	}
	case EDetachFromLadderMethod::Jump:
	{
		
		FVector JumpDirection = CurrentLadder->GetActorForwardVector();
		FVector JumpVelocity = JumpDirection * JumpOffSpeed;
		ForceTargetRotation= JumpDirection.ToOrientationRotator();
		bForceRotation = true;
		SetMovementMode(MOVE_Falling);
		Launch(JumpVelocity);
		break;
	}
	default:
		break;
	}
}

void UGCBaseCharacterMovementComponent::DetachFromZipline()
{
	bIsZiplining = false;
	//GCPlayerCharacter->ToggleSlowmo();
	SetMovementMode(MOVE_Falling);
}

void UGCBaseCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if (MovementMode==MOVE_Swimming)
	{
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(SwimmingCapsuleRadius, SwimmingCapsuleHeight);

	}
	else if (PreviousMovementMode == MOVE_Swimming)
	{		 		
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);

	}

	if (MovementMode == MOVE_Custom)
	{
		switch (CustomMovementMode)
		{
			case (uint8)ECustomMovementMode::CMOVE_Mantling:
			{						
				
				GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &UGCBaseCharacterMovementComponent::EndMantle, CurrentMantlingParameters.Duration, false);
				break;
			}
	

		default:
			break;
		}
	}

	if (MovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_ClimbingLadder)
	{
		CurrentLadder = nullptr;
		
	}
}

void UGCBaseCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
	case (uint8)ECustomMovementMode::CMOVE_Mantling:
	{
		PhysMantling(deltaTime,Iterations);

		break;
	}
	case (uint8)ECustomMovementMode::CMOVE_ClimbingLadder:
	{
		PhysClimbing(deltaTime, Iterations);

		break;
	}
	case  (uint8)ECustomMovementMode::CMOVE_Ziplining:
	{
		PhysZiplining(deltaTime, Iterations);
		break;
	}
	default:
		break;
	}

	Super::PhysCustom(deltaTime, Iterations);
}

void UGCBaseCharacterMovementComponent::PhysMantling(float deltaTime, int32 Iterations)
{
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer) + CurrentMantlingParameters.StartTime;

	FVector MantlingCurveValue = CurrentMantlingParameters.MantlingCurve->GetVectorValue(ElapsedTime);
	float PositionAlpha = MantlingCurveValue.X;
	float XYCorrectionAlpha = MantlingCurveValue.Y;
	float ZCorrectionAlpha = MantlingCurveValue.Z;


	//convert local to global transform credit goes to ALS 
	FTransform TargetTransformWS = UALSMathLibrary::MantleComponentLocalToWorld(CurrentMantlingParameters.LedgeComponent.Get(), CurrentMantlingParameters.TargetTranformLS);

	FVector CorrectedInitialLocation = FMath::Lerp(CurrentMantlingParameters.InitialLocation, CurrentMantlingParameters.InitialAnimationLocation, XYCorrectionAlpha);
	CorrectedInitialLocation.X += 1;
	CorrectedInitialLocation.Z = FMath::Lerp(CurrentMantlingParameters.InitialLocation.Z, CurrentMantlingParameters.InitialAnimationLocation.Z, ZCorrectionAlpha);

	FVector NewLocation = FMath::Lerp(CorrectedInitialLocation, TargetTransformWS.GetLocation(), PositionAlpha);
	FRotator NewRotation = FMath::Lerp(CurrentMantlingParameters.InitialRotation, TargetTransformWS.GetRotation().Rotator(), PositionAlpha);

	FVector Delta = NewLocation - GetActorLocation();
	FHitResult HitResult;

	SafeMoveUpdatedComponent(Delta, NewRotation, false, HitResult);
}

void UGCBaseCharacterMovementComponent::PhysClimbing(float deltaTime, int32 Iterations)
{
	CalcVelocity(deltaTime, 1.0f, false, ClimbingDeceleration);
	FVector Delta = Velocity * deltaTime;

	if (HasAnimRootMotion())
	{

		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
		return;
	}

	FVector NewPos = GetActorLocation() + Delta;
	float NewPosProjection = GetActorToCurrentLadderProjection(NewPos);

	if (NewPosProjection < MinClimbBottomOffset && Delta.Z<0)
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheBottom);
	}
	else if(NewPosProjection> (CurrentLadder->GetLadderHeight()- MaxClimbTopOffset))
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheTop);

	}

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
}

void UGCBaseCharacterMovementComponent::PhysZiplining(float deltaTime, int32 Iterations)
{
	
	FVector HandLocation = GCPlayerCharacter->GetMesh()->GetSocketLocation(GCPlayerCharacter->GetGrabbingHandSocketName());
	FVector ZiplineVector = GetCurrentZipline()->GetZiplineDownVector();	
	FVector Delta = deltaTime * (ZiplineVector * ZipliningSpeed);
	FVector TargetDelta = CurrentZipline->GetLowerPoleLocation()-GetActorLocation();
	FRotator TargetRotation = deltaTime * (ZiplineVector.ToOrientationRotator() - GCPlayerCharacter->GetActorRotation()) + GCPlayerCharacter->GetActorRotation();

	TargetRotation.Pitch = 0;

	
	if (TargetDelta.Size()< CurrentZipline->GetZiplineJumpOffThreshold())
	{
		DetachFromZipline();
	}

	FVector CableVector = CurrentZipline->GetCableLowestPoint() - CurrentZipline->GetCableHighestPoint();
	FVector CharacterVector = HandLocation - CurrentZipline->GetCableHighestPoint();

	FVector Projection = CurrentZipline->GetCableHighestPoint() + FVector::DotProduct(CharacterVector, CableVector) / FVector::DotProduct(CableVector, CableVector) * CableVector;

	float MSpeed = 10;
	float CapsuleHalfHeight = 50;

	FVector DeltaHand = (Projection - HandLocation) / MSpeed ;

	if (HandLocation.Z < GetActorLocation().Z + CapsuleHalfHeight)
	{
		DeltaHand = FVector::ZeroVector;
	}

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta+DeltaHand, TargetRotation, false, Hit);
}

bool UGCBaseCharacterMovementComponent::CanMantleInCurrentState()
{
	return !IsMantling() && !IsCrouching() && !IsCrawling();
}

