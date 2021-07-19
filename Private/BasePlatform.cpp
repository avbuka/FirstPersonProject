// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlatform.h"
#include "PlatformInvocator.h"
	

// Sets default values
ABasePlatform::ABasePlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Platform root"));
	RootComponent = DefaultPlatformRoot;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT(
	"Platform mesh"));
	PlatformMesh->SetupAttachment(DefaultPlatformRoot);

}



// Called when the game starts or when spawned
void ABasePlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = PlatformMesh->GetRelativeLocation();

	if (IsValid(TimelineCurve))
	{
		FOnTimelineFloatStatic PlatformMovementTimelineUpdate;
		PlatformMovementTimelineUpdate.BindUObject(this, &ABasePlatform::PlatformTimelineUpdate);
		PlatformTimeline.AddInterpFloat(TimelineCurve, PlatformMovementTimelineUpdate);
		
		FOnTimelineEventStatic PlatformMovementTimelineEnded;
		
		switch (PlatformBehavior)
		{
		case EPlatformMovementType::OnDemand:
		{
			PlatformTimeline.Play();
			break;
		}
			
		case EPlatformMovementType::Loop:
		{
			PlatformMovementTimelineEnded.BindUFunction(this, FName("PlatformTimelineEnded"));
			PlatformTimeline.SetTimelineFinishedFunc(PlatformMovementTimelineEnded);
			PlatformTimeline.Play();
			break;
		}
			
		default:
			break;
		}
		
		if(IsValid(PlatformInvocator))
			PlatformInvocator->OnInvocatorActivated.AddUObject(this,&ABasePlatform::OnPlatformInvoked);
	}
}

// Called every frame
void ABasePlatform::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlatformTimeline.TickTimeline(DeltaTime);	
}

void ABasePlatform::PlatformTimelineUpdate( float Alpha)
{
	
	const FVector PlatformTargetLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	PlatformMesh->SetRelativeLocation(PlatformTargetLocation);
}



void ABasePlatform::PlatformTimelineEnded(float Alpha)
{

	if (PlatformMesh->GetRelativeLocation().Equals(EndLocation,3.0f))
	{
		PlatformTimeline.Reverse();
	}
	else
	{
		PlatformTimeline.Play();
	}
}

void ABasePlatform::ReversePlatformMovementDirection()
{

	if (PlatformTimeline.IsReversing() || (PlatformMesh->GetRelativeLocation().Equals(StartLocation)))
	{
		PlatformTimeline.Play();
		return;
	}
	if ((PlatformMesh->GetRelativeLocation().Equals(EndLocation)) || PlatformTimeline.IsPlaying() )
	{
		PlatformTimeline.Reverse();	
		return;
	}
}



void ABasePlatform::OnPlatformInvoked()
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, "Reversing");
	ReversePlatformMovementDirection();
}

