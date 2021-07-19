// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveCameraActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AInteractiveCameraActor::AInteractiveCameraActor()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Camera interaction volume"));
	BoxComponent->SetBoxExtent(FVector(500.f, 500.f, 500.f));
	BoxComponent->SetCollisionObjectType(ECC_WorldDynamic);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComponent->SetupAttachment(RootComponent);

}

void AInteractiveCameraActor::BeginPlay()
{
	Super::BeginPlay();

	//BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveCameraActor::OnBeginOverlap);

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveCameraActor::OnBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AInteractiveCameraActor::OnEndOverlap);
}

void AInteractiveCameraActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FViewTargetTransitionParams TransitionParams;
	TransitionParams.BlendTime = TransitionTime;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetViewTarget(this, TransitionParams);
}

void AInteractiveCameraActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	

	FViewTargetTransitionParams TransitionToPawnParams;
	TransitionToPawnParams.BlendTime = TransitionBackTime;
	
	//TransitionToPawnParams.bLockOutgoing = true;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APawn* Pawn = PlayerController->GetPawn();
	PlayerController->SetViewTarget(Pawn, TransitionToPawnParams);

}
