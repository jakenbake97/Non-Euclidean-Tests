// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"

#include "DrawDebugHelpers.h"
#include "NonEuclideanTestCharacter.h"
#include "NonEuclideanTestPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortal::APortal()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsActive = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->Mobility = EComponentMobility::Static;

	PortalRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PortalRootComponent"));
	PortalRootComponent->SetupAttachment(GetRootComponent());
	PortalRootComponent->SetRelativeLocation(FVector::ZeroVector);
	PortalRootComponent->SetRelativeRotation(FRotator::ZeroRotator);
	PortalRootComponent->Mobility = EComponentMobility::Movable;
}

void APortal::BeginPlay()
{
}

void APortal::Tick(float DeltaSeconds)
{
}

bool APortal::IsActive() const
{
	return bIsActive;
}

void APortal::SetActive(const bool bActiveStatus)
{
	bIsActive = bActiveStatus;
}

void APortal::ClearRenderTarget_Implementation()
{
}

void APortal::SetRenderTarget_Implementation(UTexture* RenderTexture)
{
}

void APortal::ForceTick_Implementation()
{
}

AActor* APortal::GetTarget() const
{
	return Target;
}

void APortal::SetTarget(AActor* NewTarget)
{
	Target = NewTarget;
}

bool APortal::IsPointInFrontOfPortal(FVector Point, FVector PortalLocation, FVector PortalNormal)
{
	const FPlane PortalPlane = FPlane(PortalLocation, PortalNormal);
	const float PortalDot = PortalPlane.PlaneDot(Point);

	return (PortalDot >= 0);
}

bool APortal::IsPointCrossingPortal(FVector Point, FVector PortalLocation, FVector PortalNormal)
{
	FVector IntersectionPoint;
	bool IsCrossing = false;
	const bool IsInFront = IsPointInFrontOfPortal(Point, PortalLocation, PortalNormal);

	const bool IsIntersect = FMath::SegmentPlaneIntersection(LastPosition, Point, FPlane(PortalLocation, PortalNormal),
	                                                         IntersectionPoint);

	// If the plane was intersected since last position then check the direction to make sure crossing only counts for the visible side
	if (IsIntersect && !IsInFront && LastInFront)
	{
		IsCrossing = true;
	}

	LastInFront = IsInFront;
	LastPosition = Point;

	return IsCrossing;
}

FVector APortal::ConvertLocationToActorSpace(const FVector Location, AActor* Reference, AActor* Target)
{
	if (!Reference || !Target)
	{
		return FVector::ZeroVector;
	}

	const FVector Direction = Location - Reference->GetActorLocation();
	const FVector TargetLocation = Target->GetActorLocation();

	FVector DotProducts;
	DotProducts.X = FVector::DotProduct(Direction, Reference->GetActorForwardVector());
	DotProducts.Y = FVector::DotProduct(Direction, Reference->GetActorRightVector());
	DotProducts.Z = FVector::DotProduct(Direction, Reference->GetActorUpVector());

	const FVector NewDirection = DotProducts.X * Target->GetActorForwardVector()
		+ DotProducts.Y * Target->GetActorRightVector()
		+ DotProducts.Z * Target->GetActorUpVector();

	return TargetLocation + NewDirection;
}

FRotator APortal::ConvertRotationToActorSpace(const FRotator Rotation, AActor* Reference, AActor* Target)
{
	if (!Reference || !Target)
	{
		return FRotator::ZeroRotator;
	}

	const FTransform SourceTransform = Reference->GetActorTransform();
	const FTransform TargetTransform = Target->GetActorTransform();
	const FQuat QuatRotation = FQuat(Rotation);

	const FQuat LocalQuat = SourceTransform.GetRotation().Inverse() * QuatRotation;
	const FQuat NewWorldQuat = TargetTransform.GetRotation() * LocalQuat;

	return NewWorldQuat.Rotator();
}

void APortal::TeleportActor(AActor* ActorToTeleport)
{
	if (!ActorToTeleport || !Target)
	{
		return;
	}

	// Save the velocity from the movement component
	FVector SavedVelocity = FVector::ZeroVector;
	ANonEuclideanTestCharacter* Character = nullptr;

	if (ActorToTeleport->IsA(ANonEuclideanTestCharacter::StaticClass()))
	{
		Character = Cast<ANonEuclideanTestCharacter>(ActorToTeleport);
		SavedVelocity = Character->GetCharacterMovement()->Velocity;
	}

	// Compute the new location
	FHitResult HitResult;
	const FVector ExitLocation = ConvertLocationToActorSpace(ActorToTeleport->GetActorLocation(), this, Target);

	ActorToTeleport->SetActorLocation(ExitLocation, false, &HitResult, ETeleportType::TeleportPhysics);

	// Compute the new rotation
	FRotator ExitRotation = ConvertRotationToActorSpace(ActorToTeleport->GetActorRotation(), this, Target);

	ActorToTeleport->SetActorRotation(ExitRotation);

	// if a character is being teleported then its controller needs to be updated as well
	if (ActorToTeleport->IsA(ANonEuclideanTestCharacter::StaticClass()))
	{
		ANonEuclideanTestPlayerController* CharacterPlayerController = Character->GetPlayerController();

		if (!CharacterPlayerController)
		{
			ExitRotation = ConvertRotationToActorSpace(CharacterPlayerController->GetControlRotation(), this, Target);

			CharacterPlayerController->SetControlRotation(ExitRotation);
		}

		// Reapply Velocity
		{
			FVector DotProducts;
			DotProducts.X = FVector::DotProduct(SavedVelocity, GetActorForwardVector());
			DotProducts.Y = FVector::DotProduct(SavedVelocity, GetActorRightVector());
			DotProducts.Z = FVector::DotProduct(SavedVelocity, GetActorUpVector());

			const FVector ExitVelocity = DotProducts.X * Target->GetActorForwardVector()
				+ DotProducts.Y * Target->GetActorRightVector()
				+ DotProducts.Z * Target->GetActorUpVector();

			Character->GetMovementComponent()->Velocity = ExitVelocity;
		}
	}

	LastPosition = ExitLocation;
}
