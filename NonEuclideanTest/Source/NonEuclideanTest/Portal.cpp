// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"



#include "DrawDebugHelpers.h"
#include "NonEuclideanTestPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/DrawFrustumComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APortal::APortal()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->Mobility = EComponentMobility::Static;

	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMeshComponent"));
	PlaneMesh->SetupAttachment(RootComponent);
	PlaneMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	LocalCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	LocalCaptureComponent->SetupAttachment(RootComponent);
	LocalCaptureComponent->bCaptureEveryFrame = false;
	LocalCaptureComponent->bCaptureOnMovement = false;
	LocalCaptureComponent->bEnableClipPlane = true;
	LocalCaptureComponent->bUseCustomProjectionMatrix = false;
	LocalCaptureComponent->CaptureSource = SCS_SceneColorHDRNoAlpha;
	LocalCaptureComponent->LODDistanceFactor = 3;

	// Scene Capture post processing settings from:
	// https://www.froyok.fr/blog/2019-03-creating-seamless-portals-in-unreal-engine-4/#portals_how_do_they_work
	FPostProcessSettings CaptureSettings;
	CaptureSettings.bOverride_AmbientOcclusionQuality = true;
	CaptureSettings.bOverride_MotionBlurAmount = true;
	CaptureSettings.bOverride_SceneFringeIntensity = true;
	CaptureSettings.bOverride_GrainIntensity = true;
	CaptureSettings.bOverride_ScreenSpaceReflectionQuality = true;

	CaptureSettings.AmbientOcclusionQuality = 0.0f; //0=lowest quality..100=maximum quality
	CaptureSettings.MotionBlurAmount = 0.0f; //0 = disabled
	CaptureSettings.SceneFringeIntensity = 0.0f; //0 = disabled
	CaptureSettings.GrainIntensity = 0.0f; //0 = disabled
	CaptureSettings.ScreenSpaceReflectionQuality = 0.0f; //0 = disabled

	CaptureSettings.bOverride_ScreenPercentage = true;
	CaptureSettings.ScreenPercentage = 100.0f;

	LocalCaptureComponent->PostProcessSettings = CaptureSettings;
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();

	VisibleNormal = GetActorForwardVector();

	const APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	PlayerCam = Player->FindComponentByClass<UCameraComponent>();
	
	PlayerController = Cast<ANonEuclideanTestPlayerController>(Player->GetController());
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ExitPortal)
	{
		UE_LOG(LogTemp, Error, TEXT("No Exit Portal defined for %s"), *GetName());
		return;
	}

	const FVector VecToPlayer = PlayerCam->GetComponentLocation() - GetActorLocation();

	// From the ConvertLocationToActorSpace function on the creating seamless portals tutorial
	// https://www.froyok.fr/blog/2019-03-creating-seamless-portals-in-unreal-engine-4

	FVector RelativeDotVector;
	RelativeDotVector.X = FVector::DotProduct(VecToPlayer, GetActorForwardVector());
	RelativeDotVector.Y = FVector::DotProduct(VecToPlayer, GetActorRightVector());
	RelativeDotVector.Z = FVector::DotProduct(VecToPlayer, GetActorUpVector());

	const FVector CameraPosition = RelativeDotVector.X * ExitPortal->GetActorForwardVector()
		+ RelativeDotVector.Y * ExitPortal->GetActorRightVector()
		+ RelativeDotVector.Z * ExitPortal->GetActorUpVector();

	LocalCaptureComponent->SetWorldLocation(CameraPosition + ExitPortal->GetActorLocation());

	const FQuat LocalQuat = GetActorTransform().GetRotation().Inverse() * PlayerCam->GetComponentTransform().GetRotation();
	const FQuat NewWorldQuat = ExitPortal->GetActorTransform().GetRotation() * LocalQuat;
	LocalCaptureComponent->SetWorldRotation(NewWorldQuat);

	LocalCaptureComponent->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(LocalCaptureComponent->GetComponentLocation(), ExitPortal->GetActorLocation()));

	LocalCaptureComponent->ClipPlaneNormal = ExitPortal->GetActorForwardVector();
	LocalCaptureComponent->ClipPlaneBase = ExitPortal->GetActorLocation() + (LocalCaptureComponent->ClipPlaneNormal * 10.5f);
	LocalCaptureComponent->CustomNearClippingPlane = FVector::Dist(LocalCaptureComponent->GetComponentLocation(), ExitPortal->GetActorLocation());
	
	LocalCaptureComponent->CustomProjectionMatrix = PlayerController->GetCameraProjectionMatrix();

	LocalCaptureComponent->CaptureScene();
}

void APortal::SetSceneCaptureRenderTarget()
{
	if (!ExitPortal)
	{
		UE_LOG(LogTemp, Error, TEXT("No Exit Portal defined for %s"), *GetName());
		return;
	}

	CaptureCamera = ExitPortal->LocalCaptureComponent;
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
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(
		BaseMaterial, PlaneMesh, FName("Generated Material"));
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
