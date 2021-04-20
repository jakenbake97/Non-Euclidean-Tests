// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

UCLASS()
class NONEUCLIDEANTEST_API APortal : public AActor
{
    GENERATED_BODY()

    APortal();
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
   virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintPure, Category="Portal")
    bool IsActive() const;

    UFUNCTION(BlueprintCallable, Category="Portal")
    void SetActive(bool bActiveStatus);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Portal")
    void ClearRenderTarget();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Portal")
    void SetRenderTarget(UTexture* RenderTexture);

    UFUNCTION(BlueprintNativeEvent, Category="Portal")
    void ForceTick();

    UFUNCTION(BlueprintPure, Category="Portal")
    AActor* GetTarget() const;

    UFUNCTION(BlueprintCallable, Category="Portal")
    void SetTarget(AActor* NewTarget);

    UFUNCTION(BlueprintCallable, Category="Portal")
    bool IsPointInFrontOfPortal(FVector Point, FVector PortalLocation, FVector PortalNormal);

    UFUNCTION(BlueprintCallable, Category="Portal")
    bool IsPointCrossingPortal(FVector Point, FVector PortalLocation, FVector PortalNormal);

    static FVector ConvertLocationToActorSpace(FVector Location, AActor* Reference, AActor* Target);

    static FRotator ConvertRotationToActorSpace(FRotator Rotation, AActor* Reference, AActor* Target);

    UFUNCTION(BlueprintCallable, Category="Portal")
    void TeleportActor(AActor* ActorToTeleport);

protected:
    UPROPERTY(BlueprintReadOnly)
    USceneComponent* PortalRootComponent;

private:
    bool bIsActive;

    UPROPERTY()
    AActor* Target;

    FVector LastPosition;

    bool LastInFront;
};
