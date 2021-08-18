#include "GCTraceUtils.h"

bool GCTraceUtils::SweepCapsuleSingleByChannel(const UWorld* InWorld, struct FHitResult& OutHit, const FVector& Start, const FVector& End, float CapsuleRadius, float CapsuleHalfHeight, const FQuat& Rot, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params /*= FCollisionQueryParams::DefaultQueryParam*/, const FCollisionResponseParams& ResponseParam /*= FCollisionResponseParams::DefaultResponseParam*/, bool bDrawDebug/*=true*/, float DrawTime/*=-1.0f*/, FColor TraceColor/*=FColor::Black*/, FColor HitColor/*=FColor::Red*/)
{
	bool bResult = false;
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);

	bResult = InWorld->SweepSingleByChannel(OutHit, Start, End, Rot, TraceChannel, CollisionShape, Params, ResponseParam);
#if ENABLE_DRAW_DEBUG

	if(bDrawDebug)
	{
		DrawDebugLine(InWorld, Start, End, TraceColor, false,DrawTime);
		DrawDebugCapsule(InWorld, End,CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, TraceColor, false, DrawTime);

		if (bResult)
		{
			DrawDebugPoint(InWorld, OutHit.ImpactPoint, 10.f, HitColor, false, DrawTime);
		}

	}

#endif
	return bResult;
}

bool GCTraceUtils::SweepSphereSingleByChannel(const UWorld* InWorld, struct FHitResult& OutHit, const FVector& Start, const FVector& End, float Radius, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params /*= FCollisionQueryParams::DefaultQueryParam*/, const FCollisionResponseParams& ResponseParam /*= FCollisionResponseParams::DefaultResponseParam*/, bool bDrawDebug/*=true*/, float DrawTime/*=-1.0f*/, FColor TraceColor/*=FColor::Black*/, FColor HitColor/*=FColor::Red*/)
{
	bool bResult = false;
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(Radius);

	bResult = InWorld->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, TraceChannel, CollisionShape, Params, ResponseParam);

#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		FVector DebugCenter = (Start + End) * 0.5f;
		FVector TraceVector = End - Start;

		float DebugCapsuleHalfHeight = TraceVector.Size() * 0.5f;
		FQuat DebugCapsuleRotation = FRotationMatrix::MakeFromZ(TraceVector).ToQuat();

		DrawDebugCapsule(InWorld, DebugCenter, DebugCapsuleHalfHeight, Radius, DebugCapsuleRotation, FColor::Purple, false, DrawTime);
		
		if (bResult)
		{

			DrawDebugSphere(InWorld, OutHit.Location, Radius, 16, HitColor, false, DrawTime);
			DrawDebugPoint(InWorld, OutHit.ImpactPoint, 10.f, HitColor, false, DrawTime);


		}
	}
	
#endif
	return bResult;
}

bool GCTraceUtils::OverlapCapsuleAnyByProfile(const UWorld* InWorld, const FVector& Pos, float CapsuleRadius, float CapsuleHalfHeight, FQuat Rotation, FName ProfileName, const FCollisionQueryParams QueryParams, bool bDrawDebug /*= true*/, float DrawTime /*= 5*/, FColor TraceColor /*= FColor::Transparent*/, FColor HitColor /*= FColor::Red*/)
{
	bool bResult = false;
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);

	bResult = InWorld->OverlapAnyTestByProfile(Pos, Rotation, ProfileName, CollisionShape, QueryParams);
#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		DrawDebugCapsule(InWorld, Pos, CapsuleHalfHeight, CapsuleRadius, Rotation, bResult ? HitColor : TraceColor, false, DrawTime);
	}
#endif

	return bResult;
}

bool GCTraceUtils::OverlapBlockingTestByProfile(const UWorld* InWorld, const FVector& Pos, float CapsuleRadius, float CapsuleHalfHeight, FQuat Rotation, FName ProfileName, const FCollisionQueryParams QueryParams, bool bDrawDebug /*= true*/, float DrawTime /*= 5*/, FColor TraceColor /*= FColor::Transparent*/, FColor HitColor /*= FColor::Red*/)
{
	bool bResult = false;
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);

	bResult = InWorld->OverlapBlockingTestByProfile(Pos, Rotation, ProfileName, CollisionShape, QueryParams);
#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		DrawDebugCapsule(InWorld, Pos, CapsuleHalfHeight, CapsuleRadius, Rotation, bResult ? HitColor : TraceColor, false, DrawTime);
	}
#endif

	return bResult;
}
