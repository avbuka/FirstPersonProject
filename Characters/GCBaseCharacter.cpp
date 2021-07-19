// Fill out your copyright notice in the Description page of Project Settings.


#include "GCBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "../Components/MovementComponents/GCLedgeDetectorComponent.h"
#include "Curves/CurveVector.h"
#include "DrawDebugHelpers.h"
#include "../Utils/ALSMathLibrary.h"
#include "../Actors/Interactive/Environment/Zipline.h"

AGCBaseCharacter::AGCBaseCharacter(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer.SetDefaultSubobjectClass<UGCBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	GCBaseCharacterMovementComponent = StaticCast<UGCBaseCharacterMovementComponent*>(GetCharacterMovement());

	IKScale = GetActorScale3D().Z;
	IKTraceDistance = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * IKScale;
	LedgeDetector = CreateDefaultSubobject<class UGCLedgeDetectorComponent>(TEXT("LedgeDetector"));
}

void AGCBaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TryChangeSprintState(DeltaSeconds);
	UpdateStamina(DeltaSeconds);
	UpdateIKSettings(DeltaSeconds);
}

void AGCBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	GCBaseCharacterMovementComponent->GCPlayerCharacter = StaticCast<AGCPlayerCharacter*>(this);
	CurrentStamina = MaxStamina;
}



const FMantlingSettings& AGCBaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	return LedgeHeight > LowMantleHeight ? HighMantleSettings : LowMantleSettings;
}

float AGCBaseCharacter::GetIKFootOffset(const FName& SocketName)
{
	float Result = 0;

	FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);
	FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
	FVector TraceEnd(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z - IKTraceDistance);

	FHitResult HitResult;

	ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, TraceTypeQuery, true, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true))
	{

		Result = (TraceEnd.Z - HitResult.Location.Z) / IKScale;

	}
	else
	{
		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceEnd, TraceEnd - LineTraceExtendDistance * FVector::UpVector, TraceTypeQuery, true, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true, FLinearColor::Blue))
		{
			Result = (TraceEnd.Z - HitResult.Location.Z) / IKScale;

		}
	}
	return Result;
}

void AGCBaseCharacter::UpdateStamina(float DeltaSeconds)
{
	CurrentStamina += StaminaRestoreVelocity * DeltaSeconds;
	CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);

	if (CurrentStamina < MaxStamina)
	{
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Stamina: %.2f"), CurrentStamina));

		if (FMath::IsNearlyZero(CurrentStamina, 0.5f))
		{
			GetCharacterBaseMovementComponent()->SetIsOutOfStamina(true);

		}
	}
	else
	{
		if (GetCharacterBaseMovementComponent()->IsOutOfStamina())
		{
			GetCharacterBaseMovementComponent()->SetIsOutOfStamina(false);
		}
	}
}

void AGCBaseCharacter::RegisterInteractiveActor(AInteractiveActor* IActor)
{
	InteractiveActors.AddUnique(IActor);
}

void AGCBaseCharacter::UnRegisterInteractiveActor(AInteractiveActor* IActor)
{
	InteractiveActors.Remove(IActor);
}

void AGCBaseCharacter::InteractWithLadder()
{
	if (GetCharacterBaseMovementComponent()->IsOnLadder())
	{
		GetCharacterBaseMovementComponent()->DetachFromLadder(EDetachFromLadderMethod::Jump);
	}
	else
	{
		const ALadder* Lad = GetAvailableLadder();
		if (IsValid(Lad))
		{
			GetCharacterBaseMovementComponent()->AttachToLadder(Lad);

			if (Lad->GetIsCharacterOnTop())
			{
				PlayAnimMontage(Lad->GetAttachFromTopAnimMontage());
			}
		}
	}
}

void AGCBaseCharacter::InteractWithZipline()
{
	if (GetCharacterBaseMovementComponent()->IsZiplining())
	{
		GetCharacterBaseMovementComponent()->DetachFromZipline();
	}
	else
	{
		const AZipline* Zip = GetAvailableZipline();
		if (IsValid(Zip))
		{
			GetCharacterBaseMovementComponent()->AttachToZipline(Zip);
		}
		//animations are needed here
	}
}

void AGCBaseCharacter::InteractWithIActor()
{
	if (InteractiveActors.Num() > 0)
	{
		// it is a good idea to find the closes actor if we have several and use it. in future
		// also OOP ???
		AInteractiveActor* IActor = InteractiveActors.Last();

		switch (IActor->GetObjectType())
		{
		case EInteractiveObjectType::Ladder:
		{
			InteractWithLadder();
			break;
		}
		case EInteractiveObjectType::Zipline:
		{
			InteractWithZipline();
			break;
		}
		default:
			break;
		}
	}

}

const class ALadder* AGCBaseCharacter::GetAvailableLadder()
{	
	for (const AInteractiveActor* IActor : InteractiveActors)
	{
		if (IActor->IsA<ALadder>())
		{
			return StaticCast<const ALadder*>(IActor);
			break;
		}
	}
	return nullptr;
}

const class AZipline* AGCBaseCharacter::GetAvailableZipline()
{
	for (const AInteractiveActor* IActor : InteractiveActors)
	{
		if (IActor->IsA<AZipline>())
		{
			return StaticCast<const AZipline*>(IActor);
			break;
		}
	}
	return nullptr;
}

void AGCBaseCharacter::UpdateIKSettings(float DeltaSeconds)
{
	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, GetIKFootOffset(RightFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, GetIKFootOffset(LeftFootSocketName), DeltaSeconds, IKInterpSpeed);

	if (!FMath::IsNearlyEqual(IKLeftFootOffset, IKRightFootOffset))
	{
		IKHipOffset = FMath::FInterpTo(IKHipOffset, IKRightFootOffset + IKLeftFootOffset, DeltaSeconds, 10);
	}
	else
	{
		IKHipOffset = FMath::FInterpTo(IKHipOffset, 0, DeltaSeconds, IKInterpSpeed);
	}
}

void AGCBaseCharacter::ClimbLadderUp(float Value)
{
	if (GetCharacterBaseMovementComponent()->IsOnLadder() && !FMath::IsNearlyZero(Value))
	{
		FVector LadderUpVector= GetCharacterBaseMovementComponent()->GetCurrentLadder()->GetActorUpVector();

		AddMovementInput(LadderUpVector, Value);
	}
}

void AGCBaseCharacter::TryChangeSprintState(float DeltaSeconds)
{
	if (!GetCharacterMovement()->IsFalling() && GetCharacterMovement()->IsMovingOnGround()
		&&!GetCharacterBaseMovementComponent()->IsCrawling())
	{
		if (GCBaseCharacterMovementComponent->IsSprinting())
		{
			CurrentStamina -= SprintStaminaConsumprionVelocity * DeltaSeconds;
			CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);

			if (!bIsSprintRequested)
			{
				OnSprintStop();
				GCBaseCharacterMovementComponent->StopSprint();
				return;

			}
		}
		if ((bIsSprintRequested && (!GCBaseCharacterMovementComponent->IsSprinting()) && CanSprint()))
		{
			OnSprintStart();
			GCBaseCharacterMovementComponent->StartSprint();
			return;
		}

	
		if (GCBaseCharacterMovementComponent->IsOutOfStamina())
		{
			OnSprintStop();
			return;
		}
	}
	else
	{
		if (GCBaseCharacterMovementComponent->IsSprinting()) 
		{
			OnSprintStop();
			GCBaseCharacterMovementComponent->StopSprint();
		}

	}
	
}

void AGCBaseCharacter::ChangeCrouchState()
{
	
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		if (GetCharacterBaseMovementComponent()->CanCrouchInCurrentState() && GetCharacterMovement()->IsMovingOnGround())
		{
			Crouch();
		}
	}
}

void AGCBaseCharacter::ChangeCrawlState()
{			
	if (GetCharacterMovement()->IsCrouching())
	{
			
		GetCharacterMovement()->bWantsToCrouch = false;
		GetCharacterBaseMovementComponent()->bWantsToCrawl = true;
		return;
	}
	else
	{
		GetCharacterBaseMovementComponent()->bWantsToCrawl = false;
		return;
		
	}
	
	return;	
}


void AGCBaseCharacter::Mantle(bool bForce )
{
	if (!CanMantle()&&!bForce &&(!GetCharacterBaseMovementComponent()->IsMantling()))
	{
		return;
	}

	FLedgeDescription LedgeDescription;

	if (LedgeDetector->DetectLedge(LedgeDescription))
	{
		if (GetCharacterBaseMovementComponent()->IsCrawling() || GetCharacterBaseMovementComponent()->IsCrouching())
		{
			GetCharacterBaseMovementComponent()->bWantsToStandUp = true;
			GetCharacterBaseMovementComponent()->bWantsToMantle = true;
			return;
		}

		const FMantlingSettings MantlingSettings = GetMantlingSettings((LedgeDescription.TranformWS.GetLocation() - GetActorLocation()).Z);

		FMantlingMovementParameters MantlingParams;
		MantlingParams.MantlingCurve = MantlingSettings.MantlingCurve;
		MantlingParams.InitialLocation = GetActorLocation();
		MantlingParams.InitialRotation = GetActorRotation();
		MantlingParams.LedgeComponent = LedgeDescription.LedgeComponent;
		MantlingParams.TargetTranformLS = LedgeDescription.TranformWS * LedgeDescription.LedgeComponent->GetComponentToWorld().Inverse();

		float MinRange = 0.0f;
		float MaxRange = 0.0f;
		

		MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);
		MantlingParams.Duration = MaxRange - MinRange;

		float MantlingHight = (LedgeDescription.TranformWS.GetLocation() - MantlingParams.InitialLocation).Z;
		

		FVector2D SourceRange(MantlingSettings.MinHight, MantlingSettings.MaxHight);
		FVector2D TargetRange(MantlingSettings.MinHightStartTime, MantlingSettings.MaxHightStartTime);
		MantlingParams.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHight);

		
		MantlingParams.InitialAnimationLocation = LedgeDescription.TranformWS.GetLocation() - MantlingSettings.AnimationCorrectionZ * FVector::UpVector + MantlingSettings.AnimationCorrecitonXY * (LedgeDescription.TranformWS.Rotator().Vector() * -1);
		
		GetCharacterBaseMovementComponent()->StartMantle(MantlingParams);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantleMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingParams.StartTime);

		GetCharacterBaseMovementComponent()->bWantsToMantle = false;
	}
	//GetCharacterBaseMovementComponent()->bWantsToMantle = false;
	//Jump();

}

bool AGCBaseCharacter::CanMantle()
{
	return !GetCharacterBaseMovementComponent()->IsOnLadder();
}

bool AGCBaseCharacter::CanJumpInternal_Implementation() const
{	
	return Super::CanJumpInternal_Implementation()&&!GetCharacterBaseMovementComponent()->IsMantling();	
}

void AGCBaseCharacter::StartSprint()
{
	bIsSprintRequested = true;
}

void AGCBaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}




bool AGCBaseCharacter::CanSprint()
{
	if (GetCharacterBaseMovementComponent()->IsOutOfStamina())
	{
		return false;
	}
	return true;
}

