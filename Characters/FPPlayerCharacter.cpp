// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GCPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Components/GameCodeTypes.h"

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
