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
}

FVector ALadder::GetAttachFromTopAnimMontageInitialOffset() const
{
	FRotator OrientationRotation = GetActorForwardVector().ToOrientationRotator();
	FVector Offset = OrientationRotation.RotateVector(AttachFromTopAnimMontageInitialOffset);
	FVector LadderTop = GetActorLocation() + LadderHeight*GetActorUpVector();
	return LadderTop + Offset;
}

void ALadder::SetObjectType()
{
	ObjectType = EInteractiveObjectType::Ladder;
}

void ALadder::OnConstruction(const FTransform& Transform)
{	
	checkf(IsValid(LeftRailMeshComponent), TEXT("void ALadder::OnConstruction LeftRail mesh is not valid"));
	checkf(IsValid(RightRailMeshComponent), TEXT("void ALadder::OnConstruction RightRail mesh is not valid"));
	checkf(IsValid(StepsMeshComponent), TEXT("void ALadder::OnConstruction Steps mesh is not valid"));
	checkf(IsValid(InteractionVolume) && IsValid(TopInteractionVolume), 
			TEXT("void ALadder::OnConstruction Interaction volume is not valid"));

	LeftRailMeshComponent->SetRelativeLocation(FVector(0.0f, -LadderWidth * 0.5, GetLadderHeight()*0.5));
	RightRailMeshComponent->SetRelativeLocation(FVector(0.0f, LadderWidth * 0.5, GetLadderHeight() * 0.5));

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
		StepsMeshComponent->SetRelativeScale3D(FVector(1.0f, LadderWidth / MeshWidth, 1.0f));
	}
	StepsMeshComponent->ClearInstances();

	uint16 StepsCount = FMath::FloorToInt((GetLadderHeight() - BottomStepOffset) / StepsInterval);

	for (int i = 0; i < StepsCount; i++)
	{
		FTransform InstanceTransform(FVector(1.0f, 1.0f, BottomStepOffset + i * StepsInterval));
		StepsMeshComponent->AddInstance(InstanceTransform);
	}
	
	//setting interactive volume size
	float VolumeDepth = GetLadderInteractionBox()->GetUnscaledBoxExtent().X;

	GetLadderInteractionBox()->SetBoxExtent(FVector(VolumeDepth, LadderWidth * 0.5, GetLadderHeight() * 0.5));
	GetLadderInteractionBox()->SetRelativeLocation(FVector(VolumeDepth, 0.0f, GetLadderHeight() * 0.5));

	FVector TopInteractionVolumeExtent = TopInteractionVolume->GetUnscaledBoxExtent();
	TopInteractionVolume->SetBoxExtent(FVector(TopInteractionVolumeExtent.X, LadderWidth*0.5, TopInteractionVolumeExtent.Z));
	TopInteractionVolume->SetRelativeLocation(FVector(-TopInteractionVolumeExtent.X, 0.0f, LadderHeight));
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
