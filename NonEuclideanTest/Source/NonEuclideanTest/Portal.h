// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

UCLASS()
class NONEUCLIDEANTEST_API APortal : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    APortal();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category="Portal Scene Caputre")
    void SetSceneCaptureRenderTarget();

    void SetMaterialInstanceOnStaticMesh();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere)
    APortal* ExitPortal = nullptr;

    UPROPERTY(EditAnywhere)
    UMaterial* BaseMaterial = nullptr;

    USceneCaptureComponent2D* CaptureCamera = nullptr;

    UStaticMeshComponent* PlaneMesh = nullptr;

    UPROPERTY(BlueprintReadWrite)
    UTextureRenderTarget2D* RenderTarget = nullptr;

    FTransform VisibleNormal;
    FTransform InvisibleNormal;

    private:
    UCameraComponent* PlayerCam;
    UArrowComponent* VisibleArrowComponent = nullptr;
    UArrowComponent* InvisibleArrowComponent = nullptr;
};
