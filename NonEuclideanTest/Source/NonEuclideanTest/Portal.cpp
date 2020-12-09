// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"


#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Chaos/ChaosDebugDraw.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();

	TInlineComponentArray<UArrowComponent*> ArrowComps;
	GetComponents(ArrowComps);

	for(auto Arrow : ArrowComps)
	{
		if (Arrow->GetName() == "Visible")
		{
			VisibleArrowComponent = Arrow;
		}
		else if (Arrow->GetName() == "Invisible")
		{
			InvisibleArrowComponent = Arrow;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Arrow Component not Visible or Invisible"));
		}
	}

	const auto Player = UGameplayStatics::GetPlayerPawn(this, 0);
	PlayerCam = Player->FindComponentByClass<UCameraComponent>();
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto Transform = VisibleArrowComponent->GetComponentTransform();
	const FVector VecToPlayer = PlayerCam->GetComponentLocation() - GetActorLocation();
	auto TransformedVec = Transform.TransformVector(PlayerCam->GetComponentLocation());
	auto InversedArrowTransform = Transform.Inverse().TransformVector(PlayerCam->GetComponentLocation());
	

	UE_LOG(LogTemp, Warning, TEXT("VecToPlayer: %s | ArrowTransform: %s| InversedArrowTransform: %s" ), *VecToPlayer.ToString(), *TransformedVec.ToString(), *InversedArrowTransform.ToString());
}

void APortal::SetSceneCaptureRenderTarget()
{
	CaptureCamera = ExitPortal->FindComponentByClass<USceneCaptureComponent2D>();
	if (CaptureCamera != nullptr)
	{
		if (RenderTarget != nullptr)
		{
			CaptureCamera->TextureTarget = RenderTarget;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Render Target is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CaptureCamera is null"));
	}

	SetMaterialInstanceOnStaticMesh();
}

void APortal::SetMaterialInstanceOnStaticMesh()
{
	PlaneMesh = this->FindComponentByClass<UStaticMeshComponent>();
	
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, PlaneMesh, FName("Generated Material"));
	if (DynamicMaterial != nullptr)
	{
		PlaneMesh->SetMaterial(0, DynamicMaterial);

		DynamicMaterial->SetTextureParameterValue(TEXT("RenderTargetTexture"), RenderTarget);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create dynamic material on %s"), *PlaneMesh->GetName());
	}
}

