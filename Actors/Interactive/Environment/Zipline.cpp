// Fill out your copyright notice in the Description page of Project Settings.


#include "Zipline.h"
#include "../Components/GameCodeTypes.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

AZipline::AZipline()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ZiplineRoot"));

	FirstPoleSMComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirstPoleSM"));
	FirstPoleSMComponent->SetupAttachment(RootComponent);

	SecondPoleSMComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondPoleSM"));
	SecondPoleSMComponent->SetupAttachment(RootComponent);

	CableSMComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cable"));
	CableSMComponent->SetupAttachment(RootComponent);
	
	InteractionVolume =CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionVolume"));
	
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(PawnCollisionProfileInteractionVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);

	SetObjectType();
}

bool AZipline::CanAttachToZipline(const FVector& ActorLocation)
{
	///offset to ensure that we grab something higher 
	float Offset = 40;

	if (FirstPoleSMComponent->GetComponentLocation().Z<SecondPoleSMComponent->GetComponentLocation().Z)
	{
		LowestPoleLocation = FirstPoleSMComponent->GetComponentLocation();
		ZiplineDownVector = SecondPoleSMComponent->GetComponentLocation() - LowestPoleLocation;
		
		ZiplineDownVector.Normalize(1.0f);
	}
	else
	{
		LowestPoleLocation = SecondPoleSMComponent->GetComponentLocation();

		ZiplineDownVector = LowestPoleLocation- FirstPoleSMComponent->GetComponentLocation() ;
		
		ZiplineDownVector.Normalize(1.0f);
	}

	if (LowestPoleLocation.Z + Offset < ActorLocation.Z )
	{
		CableHighestPoint= CableSMComponent->GetComponentLocation() - ZiplineDownVector * CableLength / 2;
		CableLowestPoint= CableSMComponent->GetComponentLocation() + ZiplineDownVector * CableLength / 2;
		return true;
	}
	return false;
}



void AZipline::BeginPlay()
{
	Super::BeginPlay();

	FVector DeltaVector = SecondPoleSMComponent->GetComponentLocation() - FirstPoleSMComponent->GetComponentLocation();
	CableLength= DeltaVector.Size();
}

void AZipline::SetObjectType()
{
	ObjectType = EInteractiveObjectType::Zipline;
}

void AZipline::OnConstruction(const FTransform& Transform)
{
	//@TODO change cable location to points

	checkf(IsValid(CableSMComponent), TEXT("void AZipline::OnConstruction CableSMComponent is not valid"));
	checkf(IsValid(FirstPoleSMComponent), TEXT("void AZipline::OnConstruction FirstPole is not valid"));
	checkf(IsValid(SecondPoleSMComponent), TEXT("void AZipline::OnConstruction SecondPole is not valid"));
	checkf(IsValid(InteractionVolume), TEXT("void AZipline::OnConstruction Interaction volume is not valid"));

	FTransform CableTransform;
	FBox PoleBox = FirstPoleSMComponent->GetStaticMesh()->GetBoundingBox();

	//placing the cable in the middle 

	FVector DeltaVector = SecondPoleSMComponent->GetComponentLocation() - FirstPoleSMComponent->GetComponentLocation();
	
	float TargetCableLength = DeltaVector.Size();
	float InitialCableLength = CableSMComponent->GetStaticMesh()->GetBoundingBox().GetExtent().X * 2;
	float TopOffset = 5.0f;

	CableLength = TargetCableLength;

	CableTransform.SetLocation((FirstPoleSMComponent->GetComponentLocation() + SecondPoleSMComponent->GetComponentLocation()) / 2);
	CableTransform.SetLocation(CableTransform.GetLocation() + (PoleBox.GetExtent().Z - TopOffset) * FVector::UpVector);

	CableTransform.SetScale3D(FVector(TargetCableLength / InitialCableLength, 1, 1));
	CableTransform.SetRotation(DeltaVector.ToOrientationRotator().Quaternion());

	CableSMComponent->SetWorldTransform(CableTransform);

	//using the same transform with slight adjustments for the interaction volume
	
	CableTransform.SetScale3D(FVector(1));
	InteractionVolume->SetWorldTransform(CableTransform);
	InteractionVolume->AddLocalRotation(FRotator(90, 0, 0));
	
	UCapsuleComponent* InteractionCapsule = Cast<UCapsuleComponent>(InteractionVolume);
	InteractionCapsule->SetCapsuleHalfHeight(TargetCableLength / 2);
	
}

void AZipline::OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CanAttachToZipline(OtherActor->GetActorLocation()))
	{
		Super::OnInteractionVolumeOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	}

	return;
}

void AZipline::ConstructZipline()
{
	OnConstruction(GetActorTransform());
}
