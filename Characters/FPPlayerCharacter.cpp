// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GCPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Components/GameCodeTypes.h"
#include "Controllers/GCPlayerController.h"


AFPPlayerCharacter::AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	FirstPersonMeshComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -128.0f));
	FirstPersonMeshComponent->CastShadow = true;
	FirstPersonMeshComponent->bCastDynamicShadow = false;
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, SocketFPCamera);
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->bCastHiddenShadow = false;
	GetMesh()->CastShadow = true;

	CameraComponent->SetActive(false);
	SpringArmComponent->SetActive(false);
	SpringArmComponent->bUsePawnControlRotation = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = false;

	bUseControllerRotationYaw = true;


}



void AFPPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsFPMontagePlaying())
	{
		float BlendSpeed = 20.0f;
		FRotator TargetControlRotation = GetGCPlayerController()->GetControlRotation();

		TargetControlRotation.Pitch = 0.0f;

		TargetControlRotation = FMath::RInterpTo(GetGCPlayerController()->GetControlRotation(), TargetControlRotation, DeltaSeconds, BlendSpeed);
		GetGCPlayerController()->SetControlRotation(TargetControlRotation);
	}
}

void AFPPlayerCharacter::OnStartMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTIme)
{
	Super::OnStartMantle(MantlingSettings, MantlingAnimationStartTIme);

	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();

	AGCPlayerController* PC = Cast<AGCPlayerController>(Controller);
	
	if (IsValid(PC))
	{
		PC->SetIgnoreLookInput(true);
		PC->SetIgnoreMoveInput(true);
	}

	if (IsValid(FPAnimInstance) && MantlingSettings.FPMantleMontage)
	{
		FPAnimInstance->Montage_Play(MantlingSettings.FPMantleMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingAnimationStartTIme);
	}
}

void AFPPlayerCharacter::OnEndMantle()
{
	Super::OnEndMantle();

	AGCPlayerController* PC = Cast<AGCPlayerController>(Controller);

	if (IsValid(PC))
	{
		PC->SetIgnoreLookInput(false);
		PC->SetIgnoreMoveInput(false);
	}
}

FRotator AFPPlayerCharacter::GetViewRotation() const
{
	FRotator Result = Super::GetViewRotation();
	if (IsFPMontagePlaying())
	{
		FRotator SocketRotation = FirstPersonMeshComponent->GetSocketRotation(SocketFPCamera);
		Result.Pitch += SocketRotation.Pitch;
		Result.Yaw = SocketRotation.Yaw;
		Result.Roll = SocketRotation.Roll;
	}

	return Result;
}

bool AFPPlayerCharacter::IsFPMontagePlaying() const
{

	return IsValid(FirstPersonMeshComponent->GetAnimInstance()) && FirstPersonMeshComponent->GetAnimInstance()->IsAnyMontagePlaying();	
}

void AFPPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());
	FVector& FPMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();
	FPMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z ;
}

void AFPPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());
	FVector& FPMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();
	FPMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z + HalfHeightAdjust;
}

