// Fill out your copyright notice in the Description page of Project Settings.


#include "GCPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

#include "Components/CapsuleComponent.h"
#include "CollisionShape.h"
#include "GameFramework/MovementComponent.h"
#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../Components/MovementComponents/GCBaseCharacterMovementComponent.h"

AGCPlayerCharacter::AGCPlayerCharacter(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = 1;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	SetUnchrouchedHalfHeight(GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	DefaultSpringArmTargetLength= SpringArmComponent->TargetArmLength;

}


void AGCPlayerCharacter::MoveForward(float Value)
{
	InputForward = Value;

	if (GetCharacterMovement()->IsMovingOnGround()|| GetCharacterBaseMovementComponent()->IsWallRunning() || GetCharacterMovement()->IsFalling()
		&& (!FMath::IsNearlyZero(Value, 1e-6f)))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);	
		AddMovementInput(ForwardVector, Value);

	}
}

void AGCPlayerCharacter::MoveRight(float Value)
{
	if (GetCharacterMovement()->IsMovingOnGround() ||GetCharacterBaseMovementComponent()->IsWallRunning() ||GetCharacterMovement()->IsFalling()
		&& (!FMath::IsNearlyZero(Value, 1e-6f)))
	{
		InputRight = Value;

		if (!FMath::IsNearlyZero(Value, 1e-6f))
		{
			FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
			FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
			AddMovementInput(RightVector, Value);
		}
	}	
}

void AGCPlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AGCPlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AGCPlayerCharacter::LookUpAtRate(float Value)
{
	AddControllerPitchInput(Value * BaseLookUpRate* GetWorld()->DeltaTimeSeconds);
}

void AGCPlayerCharacter::TurnAtRate(float Value)
{
	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->DeltaTimeSeconds);

}

void AGCPlayerCharacter::SwimForward(float Value)
{
	if (!GetCharacterMovement()->IsSwimming())
	{
		return;
	}

	InputForward = Value;

	if (!FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator PitchYawRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = PitchYawRotator.RotateVector(FVector::ForwardVector);
		//AddMovementInput(GetActorForwardVector(), Value);
		AddMovementInput(ForwardVector, Value);

	}
}

void AGCPlayerCharacter::SwimRight(float Value)
{
	if (!GetCharacterMovement()->IsSwimming())
	{
		return;
	}
	InputRight = Value;

	if (!FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void AGCPlayerCharacter::SwimUp(float Value)
{
	if (!GetCharacterMovement()->IsSwimming())
	{
		return;
	}
	if (!FMath::IsNearlyEqual(Value, 1e-6f))
	{
		AddMovementInput(FVector::UpVector, Value);
	}
}

void AGCPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void AGCPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void AGCPlayerCharacter::OnSlideStartEnd(float ScaledHalfHeightAdjust)
{
	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z += ScaledHalfHeightAdjust;
	}
	else
	{
		//changing mesh translation
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + ScaledHalfHeightAdjust;
	}
	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, ScaledHalfHeightAdjust);
}

void AGCPlayerCharacter::RecalculateBaseEyeHeight()
{
	if (!GetCharacterBaseMovementComponent()->IsCrawling() || bIsCrouched)
	{		
		Super::RecalculateBaseEyeHeight();
	}
	else
	{
		BaseEyeHeight = CrawledEyeHeight;
	}
}
void AGCPlayerCharacter::OnCrawlStart(float HeightAdjust, float ScaledHeightAdjust)
{
	//TODO combine this and OnCrawlEnd
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		//	MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HeightAdjust;
		MeshRelativeLocation.Z += HeightAdjust;
		//BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		//changing mesh translation
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HeightAdjust;
	}
	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, ScaledHeightAdjust);
	//K2_OnCrawlStart(HeightAdjust, ScaledHeightAdjust);
}
void AGCPlayerCharacter::OnCrawlEnd(float HeightAdjust, float ScaledHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z -= HeightAdjust;
		//BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		//changing mesh translation
		BaseTranslationOffset.Z += HeightAdjust;
	}
	SpringArmComponent->TargetOffset -=FVector(0.0f, 0.0f, ScaledHeightAdjust);
	//	K2_OnStartCrouch(HeightAdjust, ScaledHeightAdjust);
}


void AGCPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	FString Line = USpringArmSprintCurve->GetFullName();
	
	

	if (IsValid(USpringArmSprintCurve))
	{
		TimeLine = FTimeline();
		InterpFunction.BindUObject(this, &AGCPlayerCharacter::UpdateSprintCamera);
		TimeLine.AddInterpFloat(USpringArmSprintCurve, InterpFunction);
	}
	else
	{		
		UE_LOG(LogTemp, Warning, TEXT("USpringArmSprintCurve is not valid"));
	}
	
}

void AGCPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TimeLine.TickTimeline(DeltaSeconds);

}

void AGCPlayerCharacter::OnSprintStart_Implementation()
{

	BeginSprintCamera();
}

void AGCPlayerCharacter::OnSprintStop_Implementation()
{
	EndSprintCamera();
}

void AGCPlayerCharacter::UpdateSprintCamera(float Value)
{
	SpringArmComponent->TargetArmLength= FMath::Lerp(DefaultSpringArmTargetLength, SprintSpringArmTargetLength, Value);
}


bool AGCPlayerCharacter::CanJumpInternal_Implementation() const
{
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();


	float CurrentCrouchedHalfHeight = CapsuleComp->GetScaledCapsuleHalfHeight();
	float ComponentScale = CapsuleComp->GetShapeScale();
	float OldUnscaledHalfHeight = GetUnchrouchedHalfHeight();
	float CollisionOffset = 40;

	FCollisionQueryParams CollisionParams;
	FCollisionResponseParams ResponseParms;

	GetCharacterMovement()->InitCollisionParams(CollisionParams, ResponseParms);
	CollisionParams.AddIgnoredActor(GetOwner());
	CollisionParams.AddIgnoredComponent(CapsuleComp);

	FCollisionShape StandingCapsuleShape = CapsuleComp->GetCollisionShape();
	FVector PawnLocation = CapsuleComp->GetComponentLocation();
	PawnLocation.Z += OldUnscaledHalfHeight;

	ECollisionChannel CollisionChannel = CapsuleComp->GetCollisionObjectType();

	bool bCanJump = GetWorld()->OverlapAnyTestByChannel(PawnLocation, PawnLocation.Rotation().Quaternion(), CollisionChannel, StandingCapsuleShape, CollisionParams, ResponseParms);

	return (Super::CanJumpInternal_Implementation())&&!bCanJump;
}

void AGCPlayerCharacter::OnJumped_Implementation()
{
	
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
}

