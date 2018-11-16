// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"


/*
 * PUBLIC
 */

// Sets default values
ALabyrinth::ALabyrinth() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DeafultSceneRoot"));
	DefaultSceneRoot->bEditableWhenInherited = true;
	RootComponent = DefaultSceneRoot;

	Wall = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Wall"));
	Wall->SetupAttachment(DefaultSceneRoot);
	Wall->bEditableWhenInherited = true;

	Floor = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Floor"));
	Floor->SetupAttachment(DefaultSceneRoot);
	Floor->bEditableWhenInherited = true;

	X = 0;
	Y = 0;

}

// Called every frame
void ALabyrinth::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ALabyrinth::OnConstruction(const FTransform& Transform) {
	Generate();
}


/*
 * PROTECTED
 */

// Called when the game starts or when spawned
void ALabyrinth::BeginPlay() {
	Super::BeginPlay();
}

void ALabyrinth::Generate() {
	Floor->ClearInstances();
	Wall->ClearInstances();
	// Fill floor
	Wall->AddInstance(FTransform(FRotator::ZeroRotator.Quaternion(), FVector(Size * (X - 1) / 2, Size * (Y - 1) / 2, -Size), FVector(1.0 * X, 1.0 * Y, 1.0)));
	// Fill walls
	for (int32 j = 0; j < Y; ++j) {
		for (int32 i = 0; i < X; ++i) {
			if (rand() % 2) {
				Wall->AddInstance(FTransform(FRotator::ZeroRotator.Quaternion(), FVector(Size * i, Size * j, 0.0)));
			}
		}
	}
}