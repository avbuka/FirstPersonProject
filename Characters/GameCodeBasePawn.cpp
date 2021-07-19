// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCodeBasePawn.h"
#include "Components/SphereComponent.h"
#include "Engine/CollisionProfile.h"
#include "../Components/MovementComponents/GCBasicMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"


// Sets default values
AGameCodeBasePawn::AGameCodeBasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Comp"));
	CollisionComponent ->SetSphereRadius(CollisionSphereRadius);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	RootComponent = CollisionComponent;

	//MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("Movement component"));
	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UGCBasiMovementComponent>(TEXT("Movement component"));
	MovementComponent->SetUpdatedComponent(CollisionComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->bUsePawnControlRotation = 1;
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

#if WITH_EDITORONLY_DATA
	ArrowComponent=CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(RootComponent);
#endif

	
}



// Called to bind functionality to input
void AGameCodeBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("MoveForward", this, &AGameCodeBasePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGameCodeBasePawn::MoveRight);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AGameCodeBasePawn::Jump);


}

void AGameCodeBasePawn::MoveForward(float Value)
{
	InputForward = Value;

	if (Value != 0)
	{
		AddMovementInput(CurrentActor->GetActorForwardVector(), Value);
	}

}

void AGameCodeBasePawn::MoveRight(float Value)
{

	InputRight = Value;
	if (Value != 0)
	{
		AddMovementInput(CurrentActor->GetActorRightVector(), Value);
	}
}

void AGameCodeBasePawn::Jump()
{
	checkf(MovementComponent->IsA<UGCBasiMovementComponent>(), TEXT("Jump can only work with UGCBasePawnComponent"));

	UGCBasiMovementComponent* BaseMovement = StaticCast<UGCBasiMovementComponent*>(MovementComponent);
	BaseMovement->JumpStart();
}

void AGameCodeBasePawn::BeginPlay()
{
	Super::BeginPlay();
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	CurrentActor = CameraManager->GetViewTarget();
	CameraManager->OnBlendComplete().AddUFunction(this,FName("OnBlendCompleted"));

}

void AGameCodeBasePawn::OnBlendCompleted()
{
	CurrentActor = GetController()->GetViewTarget();
	UE_LOG(LogTemp, Log, TEXT("AGameCodeBasePawn::OnBlendCompleted() Current view actor: %s"), *CurrentActor->GetName());

}

