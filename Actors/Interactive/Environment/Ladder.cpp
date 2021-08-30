// Fill out your copyright notice in the Description page of Project Settings.


#include "Ladder.h"
#include "Components/BoxComponent.h"
#include "../Components/GameCodeTypes.h"

ALadder::ALadder()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LadderRoot"));

	LeftRailMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftRail"));
	LeftRailMeshComponent->SetupAttachment(RootComponent);
	
	RightRailMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightRail"));
	RightRailMeshComponent->SetupAttachment(RootComponent);
	
	StepsMeshComponent= CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Steps"));
	StepsMeshComponent->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(PawnCollisionProfileInteractionVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);
	
	TopInteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TopInteractionVolume"));
	TopInteractionVolume->SetupAttachment(RootComponent);
	TopInteractionVolume->SetCollisionProfileName(PawnCollisionProfileInteractionVolume);
	TopInteractionVolume->SetGenerateOverlapEvents(true);

	SetObjectType();
}

void ALadder::BeginPlay()
{
	Super::BeginPlay();
	TopInteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnInteractionVolumeOverlapBegin);
	TopInteractionVolume->OnComponentEndOverlap.AddDynamic(this, &ALadder::OnInteractionVolumeOverlapEnd);
	StepsCount = (FMath::FloorToInt((GetLadderHeight() - GetBottomStepOffset()) / GetStepsInterval()));

}

FVector ALadder::GetAttachFromTopAnimMontageInitialOffset() const
{
	FRotator OrientationRotation = GetActorForwardVector().ToOrientationRotator();
	FVector Offset = OrientationRotation.RotateVector(AttachFromTopAnimMontageInitialOffset);
	FVector LadderTop = GetActorLocation() + GetLadderHeight()*GetActorUpVector();
	return LadderTop + Offset;
}

void ALadder::SetObjectType()
{
	ObjectType = EInteractiveObjectType::Ladder;
}

void ALadder::OnConstruction(const FTransform& Transform)
{	
	LeftRailMeshComponent->SetRelativeLocation(FVector(0.0f, -GetLadderWidth() * 0.5, GetLadderHeight()*0.5));
	RightRailMeshComponent->SetRelativeLocation(FVector(0.0f, GetLadderWidth() * 0.5, GetLadderHeight() * 0.5));

	if (LeftRailMeshComponent->GetStaticMesh())
	{
		float MeshHeight = LeftRailMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			LeftRailMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, GetLadderHeight() / MeshHeight));
		}	
	}
	
	if (RightRailMeshComponent->GetStaticMesh())
	{
		float MeshHeight = RightRailMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			RightRailMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, GetLadderHeight() / MeshHeight));
		}
	}

	if (StepsMeshComponent->GetStaticMesh())
	{
		float MeshWidth = StepsMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize().Y;
		StepsMeshComponent->SetRelativeScale3D(FVector(1.0f, GetLadderWidth() / MeshWidth, 1.0f));
	}
	StepsMeshComponent->ClearInstances();

	StepsCount=(FMath::FloorToInt((GetLadderHeight() - GetBottomStepOffset()) / GetStepsInterval()));

	for (int i = 0; i < StepsCount; i++)
	{
		FTransform InstanceTransform(FVector(1.0f, 1.0f, GetBottomStepOffset() + i * GetStepsInterval()));
		StepsMeshComponent->AddInstance(InstanceTransform);
	}
	
	//setting interactive volume size
	float VolumeDepth = GetLadderInteractionBox()->GetUnscaledBoxExtent().X;

	GetLadderInteractionBox()->SetBoxExtent(FVector(VolumeDepth, GetLadderWidth() * 0.5, GetLadderHeight() * 0.5));
	GetLadderInteractionBox()->SetRelativeLocation(FVector(VolumeDepth, 0.0f, GetLadderHeight() * 0.5));

	FVector TopInteractionVolumeExtent = TopInteractionVolume->GetUnscaledBoxExtent();
	TopInteractionVolume->SetBoxExtent(FVector(TopInteractionVolumeExtent.X, GetLadderWidth()*0.5, TopInteractionVolumeExtent.Z));
	TopInteractionVolume->SetRelativeLocation(FVector(-TopInteractionVolumeExtent.X, 0.0f, GetLadderHeight()));
}

void ALadder::OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnInteractionVolumeOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OverlappedComponent == TopInteractionVolume)
	{
		bIsCharacterOnTop = true;
	}
}

void ALadder::OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnInteractionVolumeOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	
	if (OverlappedComponent == TopInteractionVolume)
	{
		bIsCharacterOnTop = false;

	}
}

class UBoxComponent* ALadder::GetLadderInteractionBox() const
{
	return StaticCast<UBoxComponent*>(InteractionVolume);
}
