// Fill out your copyright notice in the Description page of Project Settings.


#include "GCBasicMovementComponent.h"

void UGCBasiMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector PendingInput = GetPendingInputVector().GetClampedToMaxSize(1.0f);
	Velocity = PendingInput * MaxSpeed ;

	ConsumeInputVector();

	bool bWasFallisng = bIsFalling;
	if (bEnableGravity)
	{
		FHitResult HitResult;
		FVector StartPoint = UpdatedComponent->GetComponentLocation();
		float TraceDepth = 1.0f;
		float SphereRadius = 50.0f;
		
		FVector EndPoint = StartPoint - TraceDepth * FVector::UpVector;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(GetOwner());

		FCollisionShape Sphere = FCollisionShape::MakeSphere(SphereRadius);
		bIsFalling= !GetWorld()->SweepSingleByChannel(HitResult, StartPoint, EndPoint, FQuat::Identity, ECC_Visibility,Sphere,CollisionParams);

		if (bIsFalling)
		{
			VerticalVelocity += GetGravityZ() * FVector::UpVector * DeltaTime;
			
		}
		else if (bWasFallisng)
		{
			VerticalVelocity = FVector::ZeroVector;
		}

		Velocity += VerticalVelocity;
	}
	
	FVector Delta = Velocity * DeltaTime;
	
	if (!Delta.IsNearlyZero(1e-6f))
	{
		FQuat Rot = UpdatedComponent->GetComponentQuat();
		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(Delta, Rot, true, Hit);

		
		if (Hit.IsValidBlockingHit())
		{
			HandleImpact(Hit, DeltaTime, Delta);
			// Try to slide the remaining distance along the surface.
			SlideAlongSurface(Delta, 1.f-Hit.Time, Hit.Normal, Hit, true);
		}

	}

	UpdateComponentVelocity();
	
}

void UGCBasiMovementComponent::JumpStart()
{
	VerticalVelocity = InitialJumpVelocity * FVector::UpVector;
}

bool UGCBasiMovementComponent::IsFalling() const
{
	return bIsFalling;
}
