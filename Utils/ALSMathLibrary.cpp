// Fill out your copyright notice in the Description page of Project Settings.


#include "ALSMathLibrary.h"

FTransform UALSMathLibrary::MantleComponentLocalToWorld(const UPrimitiveComponent* Comp,const FTransform& Transform)
{
	const FTransform& InverseTransform = Comp->GetComponentToWorld().Inverse();
	const FVector Location = InverseTransform.InverseTransformPosition(Transform.GetLocation());
	const FQuat Quat = InverseTransform.InverseTransformRotation(Transform.GetRotation());
	const FVector Scale = InverseTransform.InverseTransformPosition(Transform.GetScale3D());
	return { Quat, Location, Scale };
}
