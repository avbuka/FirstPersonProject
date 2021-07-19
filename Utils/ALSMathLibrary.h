// Project:         Advanced Locomotion System V4 on C++
// Copyright:       Copyright (C) 2021 Doğa Can Yanıkoğlu
// License:         MIT License (http://www.opensource.org/licenses/mit-license.php)
// Source Code:     https://github.com/dyanikoglu/ALSV4_CPP
// Original Author: Doğa Can Yanıkoğlu
// Contributors:    Achim Turan


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "ALSMathLibrary.generated.h"

class UCapsuleComponent;

/**
 * Math library functions for ALS
 */
UCLASS()
class XYZPROJECT_API UALSMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:


	UFUNCTION(BlueprintCallable, Category = "ALS|Math Utils")
		static FTransform TransfromSub(const FTransform& T1, const FTransform& T2)
	{
		return FTransform(T1.GetRotation().Rotator() - T2.GetRotation().Rotator(),
			T1.GetLocation() - T2.GetLocation(), T1.GetScale3D() - T2.GetScale3D());
	}

	UFUNCTION(BlueprintCallable, Category = "ALS|Math Utils")
		static FTransform TransfromAdd(const FTransform& T1, const FTransform& T2)
	{
		return FTransform(T1.GetRotation().Rotator() + T2.GetRotation().Rotator(),
			T1.GetLocation() + T2.GetLocation(), T1.GetScale3D() + T2.GetScale3D());
	}
	
	UFUNCTION(BlueprintCallable, Category = "ALS|Math Utils")
	static FTransform MantleComponentLocalToWorld(const UPrimitiveComponent* Comp, const FTransform& Transform);

};
