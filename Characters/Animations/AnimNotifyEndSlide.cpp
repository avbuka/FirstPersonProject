// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyEndSlide.h"
#include "../GCPlayerCharacter.h"

void UAnimNotifyEndSlide::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AGCPlayerCharacter* BaseCharacter = Cast<AGCPlayerCharacter>(MeshComp->GetOwner());

	if (IsValid(BaseCharacter))
	{
		BaseCharacter->ToggleSlide();
	}
}
