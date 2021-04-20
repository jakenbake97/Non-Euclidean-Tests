// Fill out your copyright notice in the Description page of Project Settings.

// From the "Creating Seamless Portals in Unreal Engine"
// https://www.froyok.fr/blog/2019-03-creating-seamless-portals-in-unreal-engine-4

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "NonEuclideanLocalPlayer.generated.h"

/**
 * 
 */
UCLASS()
class NONEUCLIDEANTEST_API UNonEuclideanLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

	UNonEuclideanLocalPlayer();

public:

	virtual FSceneView* CalcSceneView(class FSceneViewFamily* ViewFamily, FVector& OutViewLocation, FRotator& OutViewRotation,
	                                  FViewport* Viewport, class FViewElementDrawer* ViewDrawer,
	                                  EStereoscopicPass StereoPass) override;

	void PerformCameraCut();

	private:
	bool bCameraCut;
};
