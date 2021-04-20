// Fill out your copyright notice in the Description page of Project Settings.


#include "NonEuclideanLocalPlayer.h"

UNonEuclideanLocalPlayer::UNonEuclideanLocalPlayer()
{
	bCameraCut = false;
}

FSceneView* UNonEuclideanLocalPlayer::CalcSceneView(FSceneViewFamily* ViewFamily, FVector& OutViewLocation,
	FRotator& OutViewRotation, FViewport* Viewport, FViewElementDrawer* ViewDrawer, EStereoscopicPass StereoPass)
{
	FSceneView* View = Super::CalcSceneView(ViewFamily, OutViewLocation, OutViewRotation, Viewport, ViewDrawer, StereoPass);

	if (bCameraCut)
	{
		View->bCameraCut = true;
		bCameraCut = false;
	}

	return View;
}

void UNonEuclideanLocalPlayer::PerformCameraCut()
{
	bCameraCut = true;
}
