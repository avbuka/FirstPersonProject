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

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("DirectionArrow"));
	ArrowComponent->SetupAttachment(RootComponent);

	SetObjectType();
}

bool AZipline::CanAttachToZipline(const FVector& ActorLocation)
{
	///offset to ensure that we grab something higher 
	float Offset = 40;

	if (LowestPoleLocation.Z + Offset < ActorLocation.Z )
	{

		return true;
	}
	return false;
}



void AZipline::SetLowestPopleAndZDownVector()
{
	FVector DeltaVector = SecondPoleSMComponent->GetComponentLocation() - FirstPoleSMComponent->GetComponentLocation();
	CableLength = DeltaVector.Size();

	if (FirstPoleSMComponent->GetComponentLocation().Z < SecondPoleSMComponent->GetComponentLocation().Z)
	{
		LowestPoleLocation = FirstPoleSMComponent->GetComponentLocation();		
		ZiplineDownVector = GetPoleTopLocation(FirstPoleSMComponent) -GetPoleTopLocation(SecondPoleSMComponent);

		ArrowComponent->SetWorldLocation(GetPoleTopLocation(SecondPoleSMComponent));		
	}
	else
	{
		LowestPoleLocation = SecondPoleSMComponent->GetComponentLocation();
		ZiplineDownVector = GetPoleTopLocation(SecondPoleSMComponent) - GetPoleTopLocation(FirstPoleSMComponent);

		ArrowComponent->SetWorldLocation(GetPoleTopLocation(FirstPoleSMComponent));
	}

	ZiplineDownVector.Normalize(1.0f);
	CableHighestPoint = CableSMComponent->GetComponentLocation() - ZiplineDownVector * CableLength / 2;
	CableLowestPoint  = CableSMComponent->GetComponentLocation() + ZiplineDownVector * CableLength / 2;
	ArrowComponent->SetWorldRotation(ZiplineDownVector.ToOrientationQuat());
}

FVector AZipline::GetPoleTopLocation(const UStaticMeshComponent* Mesh)
{
	return (Mesh->GetComponentLocation() + Mesh->GetUpVector() * Mesh->GetStaticMesh()->GetBoundingBox().GetExtent().Z * Mesh->GetComponentScale().Z);
}

void AZipline::BeginPlay()
{
	Super::BeginPlay();

	SetLowestPopleAndZDownVector();

}

void AZipline::SetObjectType()
{
	ObjectType = EInteractiveObjectType::Zipline;
}

void AZipline::OnConstruction(const FTransform& Transform)
{


	checkf(IsValid(CableSMComponent), TEXT("void AZipline::OnConstruction CableSMComponent is not valid"));
	checkf(IsValid(FirstPoleSMComponent), TEXT("void AZipline::OnConstruction FirstPole is not valid"));
	checkf(IsValid(SecondPoleSMComponent), TEXT("void AZipline::OnConstruction SecondPole is not valid"));
	checkf(IsValid(InteractionVolume), TEXT("void AZipline::OnConstruction Interaction volume is not valid"));

	FTransform CableTransform;
	FBox FirstPoleBox = FirstPoleSMComponent->GetStaticMesh()->GetBoundingBox();
	FBox SecondPoleBox = SecondPoleSMComponent->GetStaticMesh()->GetBoundingBox();

	//placing the cable in the middle 

	FVector DeltaVector = GetPoleTopLocation(SecondPoleSMComponent) - GetPoleTopLocation(FirstPoleSMComponent);
	
	float TargetCableLength = DeltaVector.Size();
	float InitialCableLength = CableSMComponent->GetStaticMesh()->GetBoundingBox().GetExtent().X * 2;
	float TopOffset = 5.0f;

	CableLength = TargetCableLength;

	CableTransform.SetLocation(GetPoleTopLocation(SecondPoleSMComponent)+GetPoleTopLocation(FirstPoleSMComponent));
	CableTransform.SetLocation(CableTransform.GetLocation() / 2);

	CableTransform.SetScale3D(FVector(TargetCableLength / InitialCableLength, 1, 1));
	CableTransform.SetRotation(DeltaVector.ToOrientationRotator().Quaternion());

	CableSMComponent->SetWorldTransform(CableTransform);

	//using the same transform with slight adjustments for the interaction volume
	
	CableTransform.SetScale3D(FVector(1));
	InteractionVolume->SetWorldTransform(CableTransform);
	InteractionVolume->AddLocalRotation(FRotator(90, 0, 0));
	
	UCapsuleComponent* InteractionCapsule = Cast<UCapsuleComponent>(InteractionVolume);
	InteractionCapsule->SetCapsuleHalfHeight(TargetCableLength / 2);
	
	SetLowestPopleAndZDownVector();

	

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
