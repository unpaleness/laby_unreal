// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Labyrinth.generated.h"

UCLASS(Blueprintable)
class LABY_API ALabyrinth : public AActor {
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ALabyrinth();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * Called when an instance of this class is placed (in editor) or spawned.
	 * @param	Transform			The transform the actor was constructed at.
	 */
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Clears labyrinth and generates it anew */
	UFUNCTION(BlueprintCallable, Category = "Tools")
		void Generate();

protected:
	/** Root component */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Components")
		USceneComponent *DefaultSceneRoot;

	/** This component is a wall of the whole labyrinth */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Components")
		UInstancedStaticMeshComponent *Wall;

	/** This component is a floor of the whole labyrinth */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Components")
		UInstancedStaticMeshComponent *Floor;

	/** Number of cells of labyrinth along X axis */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties")
		int32 X;

	/** Number of cells of labyrinth along Y axis */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties")
		int32 Y;

	/** Size of each cell */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties")
		float Size;
};
