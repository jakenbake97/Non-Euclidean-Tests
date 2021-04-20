// Fill out your copyright notice in the Description page of Project Settings.


#include "NonEuclideanTestPlayerController.h"

#include "NonEuclideanLocalPlayer.h"

void ANonEuclideanTestPlayerController::BeginPlay()
{
	Super::BeginPlay();

	LocalPlayer = Cast<UNonEuclideanLocalPlayer>(GetLocalPlayer());
}

void ANonEuclideanTestPlayerController::PerformCameraCut() const
{
	if (LocalPlayer)
	{
		LocalPlayer->PerformCameraCut();
	}
}

FMatrix ANonEuclideanTestPlayerController::GetCameraProjectionMatrix() const
{
	FMatrix ProjectionMatrix;

	if (LocalPlayer)
	{
		FSceneViewProjectionData PlayerProjectionData;

		LocalPlayer->GetProjectionData(LocalPlayer->ViewportClient->Viewport, EStereoscopicPass::eSSP_FULL,
		                               PlayerProjectionData);

		ProjectionMatrix = PlayerProjectionData.ProjectionMatrix;
	}

	return ProjectionMatrix;
}
