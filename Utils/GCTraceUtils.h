#pragma once
#include "DrawDebugHelpers.h"
#include "UObject/NoExportTypes.h"


namespace GCTraceUtils
{
	bool SweepCapsuleSingleByChannel(const UWorld* InWorld, struct FHitResult& OutHit, const FVector& Start, const FVector& End, float CapsuleRadius, float CapsuleHalfHeight,const FQuat& Rot, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam, bool bDrawDebug=true,float DrawTime=-1.0f,FColor TraceColor=FColor::Black,FColor HitColor=FColor::Red);
	
	bool SweepSphereSingleByChannel(const UWorld* InWorld, struct FHitResult& OutHit, const FVector& Start, const FVector& End, float Radius, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam, bool bDrawDebug=true,float DrawTime=-1.0f,FColor TraceColor=FColor::Black,FColor HitColor=FColor::Red);

	bool OverlapCapsuleAnyByProfile(const UWorld* InWorld, const FVector& Pos, float CapsuleRadius, float CapsuleHalfHeight, FQuat Rotation, FName ProfileName, const FCollisionQueryParams QueryParams, bool bDrawDebug = true, float DrawTime = 5, FColor TraceColor = FColor::Transparent, FColor HitColor = FColor::Red);

	bool OverlapBlockingTestByProfile(const UWorld* InWorld, const FVector& Pos, float CapsuleRadius, float CapsuleHalfHeight, FQuat Rotation, FName ProfileName, const FCollisionQueryParams QueryParams, bool bDrawDebug = true, float DrawTime = 5, FColor TraceColor = FColor::Transparent, FColor HitColor = FColor::Red);
}