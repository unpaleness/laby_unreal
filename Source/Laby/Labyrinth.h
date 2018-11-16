// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Labyrinth.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LabyrinthLog, Log, All);


UCLASS(Blueprintable)
class LABY_API ALabyrinth : public AActor {
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ALabyrinth();

	// Destructor to clean memory
	~ALabyrinth();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * Called when an instance of this class is placed (in editor) or spawned.
	 * @param	Transform			The transform the actor was constructed at.
	 */
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginDestroy() override;

	/** Setter for X */
	void SetX(int32 NewX);

	/** Setter for Y */
	void SetY(int32 NewY);

	/** Getter for X */
	int32 GetX();

	/** Getter for Y */
	int32 GetY();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Recreates Wall and Floor instances according to generated labyrinth */
	UFUNCTION(BlueprintCallable, Category = "Tools")
		void Draw();

	/** Generates labyrinth walls and stores them in inner arrays */
	void Generate();

	/** Allocates memory to arrays with walls */
	void AllocateArrays();

	/** Deletes memory from arrays with walls */
	void DeleteArrays();

	/**
	 * Generates pseudo-randomized boolean value
	 * @return Pseudo-randomized boolean value
	 */
	bool RandBool();


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

	/** Desired number of cells of labyrinth along X axis */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties")
		int32 DesiredX;

	/** Desired number of cells of labyrinth along Y axis */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties")
		int32 DesiredY;

	/** Size of each cell */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Properties")
		float Size;

	/** Actual number of cells of labyrinth along X axis */
	int32 X;

	/** Actual number of cells of labyrinth along Y axis */
	int32 Y;

	/** Vertical walls */
	bool **walls_v { nullptr };

	/** Horizontal_walls */
	bool **walls_h { nullptr };
};
