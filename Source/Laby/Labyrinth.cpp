// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include <set>
#include <map>

DEFINE_LOG_CATEGORY(LabyrinthLog);


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

	Walls = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Walls"));
	Walls->SetupAttachment(DefaultSceneRoot);
	Walls->bEditableWhenInherited = true;

	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	Floor->SetupAttachment(DefaultSceneRoot);
	Floor->bEditableWhenInherited = true;

	DesiredX = 5;
	DesiredY = 5;
	X = 0;
	Y = 0;
	BaseCubeSize = 100.0;
	WallWidthRelative = 0.1;
}

ALabyrinth::~ALabyrinth() {
	DeleteArrays();
}

// Called every frame
void ALabyrinth::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ALabyrinth::OnConstruction(const FTransform& Transform) {
	if (!HasActorBegunPlay()) {
		Recreate();
	}
}

void ALabyrinth::BeginDestroy() {
	Super::BeginDestroy();
	DeleteArrays();
}

void ALabyrinth::Recreate() {
	SetX(DesiredX);
	SetY(DesiredY);
	Generate();
	Draw();
}

void ALabyrinth::SetX(int32 NewX) {
	if (X != NewX) {
		DeleteArrays();
		X = NewX;
	}
}

void ALabyrinth::SetY(int32 NewY) {
	if (Y != NewY) {
		DeleteArrays();
		Y = NewY;
	}
}

int32 ALabyrinth::GetX() {
	return X;
}

int32 ALabyrinth::GetY() {
	return Y;
}


/*
 * PROTECTED
 */

// Called when the game starts or when spawned
void ALabyrinth::BeginPlay() {
	Super::BeginPlay();
	//Recreate();
}

void ALabyrinth::Draw() {
	UE_LOG(LabyrinthLog, Display, TEXT("Clearing instances of walls"));
	Walls->ClearInstances();
	// Adjust floor floor
	UE_LOG(LabyrinthLog, Display, TEXT("Transforming floor"));
	Floor->SetRelativeTransform(FTransform(FRotator::ZeroRotator.Quaternion(), FVector(0.0, 0.0, -BaseCubeSize / 2.0), FVector(WallWidthRelative + WallWidthRelative * X + X, WallWidthRelative + WallWidthRelative * Y + Y, 1.0)));
	// Fill horizontal border walls
	UE_LOG(LabyrinthLog, Display, TEXT("Adding horizontal border walls"));
	Walls->AddInstance(FTransform(FRotator::ZeroRotator.Quaternion(), FVector(0.0, -Y / 2.0 * BaseCubeSize * (1.0 + WallWidthRelative), BaseCubeSize / 2.0), FVector(WallWidthRelative + WallWidthRelative * X + X, WallWidthRelative, 1.0)));
	Walls->AddInstance(FTransform(FRotator::ZeroRotator.Quaternion(), FVector(0.0, Y / 2.0 * BaseCubeSize * (1.0 + WallWidthRelative), BaseCubeSize / 2.0), FVector(WallWidthRelative + WallWidthRelative * X + X, WallWidthRelative, 1.0)));
	// Fill horizontal inner walls
	// To optimize amount of meshes we need to merge walls in a row
	for (int32 j = 1; j < Y; ++j) {
		int32 groupped{ 0 };    // Amount of currently groupped walls
		int32 head_wall_i{ 0 }; // Index of the first wall in a row
		for (int32 i = 0; i < X; ++i) {
			if (walls_h[j][i]) {
				if (groupped == 0) {
					head_wall_i = i;
				}
				++groupped;
			}
			if (i > 0 && groupped > 0 && (!walls_h[j][i] || i == X - 1 )) {
				UE_LOG(LabyrinthLog, Display, TEXT("Adding horizontal wall: row %d, start index %d, size %d"), j, head_wall_i, groupped);
				Walls->AddInstance(FTransform(FRotator::ZeroRotator.Quaternion(), FVector(BaseCubeSize * (1.0 + WallWidthRelative) * (head_wall_i + (groupped - 1 - X) / 2.0 + 0.5), BaseCubeSize * (1.0 + WallWidthRelative) * (j - Y / 2.0), BaseCubeSize / 2.0), FVector(1.0 + (1.0 + WallWidthRelative) * (groupped - 1), WallWidthRelative, 1.0)));
				groupped = 0;
			}
		}
	}
	// Fill vertical border walls
	UE_LOG(LabyrinthLog, Display, TEXT("Adding vertical border walls"));
	Walls->AddInstance(FTransform(FRotator::ZeroRotator.Quaternion(), FVector(-X / 2.0 * BaseCubeSize * (1.0 + WallWidthRelative), 0.0, BaseCubeSize / 2.0), FVector(WallWidthRelative, WallWidthRelative + WallWidthRelative * Y + Y, 1.0)));
	Walls->AddInstance(FTransform(FRotator::ZeroRotator.Quaternion(), FVector(X / 2.0 * BaseCubeSize * (1.0 + WallWidthRelative), 0.0, BaseCubeSize / 2.0), FVector(WallWidthRelative, WallWidthRelative + WallWidthRelative * Y + Y, 1.0)));
	for (int32 i = 1; i < X; ++i) {
		int32 groupped{ 0 };    // Amount of currently groupped walls
		int32 head_wall_j{ 0 }; // Index of the first wall in a column
		for (int32 j = 0; j < Y; ++j) {
			if (walls_v[j][i]) {
				if (groupped == 0) {
					head_wall_j = j;
				}
				++groupped;
			}
			if (j > 0 && groupped > 0 && (!walls_v[j][i] || j == Y - 1)) {
				UE_LOG(LabyrinthLog, Display, TEXT("Adding vertical wall: column %d, start index %d, size %d"), i, head_wall_j, groupped);
				Walls->AddInstance(FTransform(FRotator::ZeroRotator.Quaternion(), FVector(BaseCubeSize * (1.0 + WallWidthRelative) * (i - X / 2.0), BaseCubeSize * (1.0 + WallWidthRelative) * (head_wall_j + (groupped - 1 - Y) / 2.0 + 0.5), BaseCubeSize / 2.0), FVector(WallWidthRelative, 1.0 + (1.0 + WallWidthRelative) * (groupped - 1), 1.0)));
				groupped = 0;
			}
		}
	}
}

void ALabyrinth::Generate() {
	UE_LOG(LabyrinthLog, Display, TEXT("Start generating labyrinth"));
	if (walls_h == nullptr && walls_v == nullptr) {
		AllocateArrays();
	}

	// Edge borders
	for (int32 i = 0; i < X; ++i) {
		walls_h[0][i] = true;
		walls_h[Y][i] = true;
	}
	for (int32 i = 0; i < Y; ++i) {
		walls_v[i][0] = true;
		walls_v[i][X] = true;
	}
	// In this case generation is not needed
	if (X < 2 || Y < 2) {
		UE_LOG(LabyrinthLog, Display, TEXT("Generating complete (too small)"));
		return;
	}
	/***********\
	* First row *
	\***********/
	int32 *row = new int32 [X];
	int32 n_multi { 1 }; // Index of the last multiplicity created
	for (int32 i = 0; i < X; ++i) {
		if (i > 0) {
			walls_v[0][i] = RandBool();
			if (walls_v[0][i]) {
				++n_multi;
			}
		}
		walls_h[1][i] = RandBool();
		row[i] = n_multi;
	}
	std::set <int> multies(row, row + X); // A set of multies on current row
	std::map <int, int> multi_cells;       // Amount of cells of each multi
	std::map <int, int> multi_borders;     // Amount of lower borders of each multi
	for (auto m : multies) {
		multi_cells[m] = 0;
		multi_borders[m] = 0;
	}
	// Count cells and lower borders for each multi
	for (int32 i = 0; i < X; ++i) {
		++multi_cells[row[i]];
		if (walls_h[1][i]) {
			++multi_borders[row[i]];
		}
	}
	// If amount of lower borders is equal to cells of each multi delete one
	// single random lower border of each multi
	for (auto m : multies) {
		if (multi_borders[m] == multi_cells[m]) {
			int32 i_deleting_border { rand() % multi_borders[m] + 1 };
			int32 i_border { 0 };
			for (int32 i = 0; i < X; ++i) {
				if (row[i] == m) {
					++i_border;
				}
				if (i_border == i_deleting_border) {
					walls_h[1][i] = false;
					break;
				}
			}
		}
	}
	/**************\
	* 2 - y-1 rows *
	\**************/
	for (int32 j = 1; j < Y - 1; ++j) {
		// Set new multies to the cells with borders above
		for (int32 i = 0; i < X; ++i) {
			if (walls_h[j][i]) {
				row[i] = ++n_multi;
			}
		}
		// Set vertical borders
		for (int32 i = 0; i < X - 1; ++i) {
			if (row[i] == row[i + 1]) {
				walls_v[j][i + 1] = true;
			}
			else {
				walls_v[j][i + 1] = RandBool();
				// All cells with the same multi should be changed
				if (!walls_v[j][i + 1]) {
					int32 multi_to_merge { row[i + 1] };
					for (int32 k = 0; k < X; ++k) {
						if (row[k] == multi_to_merge) {
							row[k] = row[i];
						}
					}
				}
			}
		}
		// Set horizontal borders
		for (int32 i = 0; i < X; ++i) {
			walls_h[j + 1][i] = RandBool();
		}
		multies.clear();
		for (int32 i = 0; i < X; ++i) {
			multies.insert(row[i]);
		}
		for (auto m : multies) {
			multi_cells[m] = 0;
			multi_borders[m] = 0;
		}
		// Count cells and lower borders for each multi
		for (int32 i = 0; i < X; ++i) {
			++multi_cells[row[i]];
			if (walls_h[j + 1][i]) {
				++multi_borders[row[i]];
			}
		}
		// If amount of lower borders as equal to cells of each multi delete one
		// single random lower border of each multi
		for (auto m : multies) {
			if (multi_borders[m] == multi_cells[m]) {
				int32 i_deleting_border { rand() % multi_borders[m] + 1 };
				int32 i_border { 0 };
				for (int32 i = 0; i < X; ++i) {
					if (row[i] == m) {
						++i_border;
					}
					if (i_border == i_deleting_border) {
						walls_h[j + 1][i] = false;
						break;
					}
				}
			}
		}
	}
	/**************\
	* The last row *
	\**************/
	// Set new multies to the cells with borders above
	for (int32 i = 0; i < X; ++i) {
		if (walls_h[Y - 1][i]) {
			row[i] = ++n_multi;
		}
	}
	// Set vertical borders
	for (int32 i = 0; i < X - 1; ++i) {
		if (row[i] == row[i + 1]) {
			walls_v[Y - 1][i + 1] = true;
		}
		else {
			walls_v[Y - 1][i + 1] = RandBool();
			// All cells with the same multi should be changed
			if (!walls_v[Y - 1][i + 1]) {
				int32 multi_to_merge { row[i + 1] };
				for (int32 k = 0; k < X; ++k) {
					if (row[k] == multi_to_merge) {
						row[k] = row[i];
					}
				}
			}
		}
	}
	// Remove borders between different multies and merge them
	for (int32 i = 0; i < X - 1; ++i) {
		if (row[i] != row[i + 1]) {
			walls_v[Y - 1][i + 1] = false;
			int32 multi_to_merge { row[i + 1] };
			for (int32 k = i + 1; k < X; ++k) {
				if (row[k] == multi_to_merge) {
					row[k] = row[i];
				}
			}
		}
	}

	delete[] row;
	UE_LOG(LabyrinthLog, Display, TEXT("Generating complete"));
}

void ALabyrinth::AllocateArrays() {
	if (Y > 0 && X > 0) {
		UE_LOG(LabyrinthLog, Display, TEXT("Allocating vertical walls"));
		walls_v = new bool *[Y];
		for (int32 i = 0; i < Y; ++i) {
			walls_v[i] = new bool[X + 1];
		}
		UE_LOG(LabyrinthLog, Display, TEXT("Allocating horizontal walls"));
		walls_h = new bool *[Y + 1];
		for (int32 i = 0; i < Y + 1; ++i) {
			walls_h[i] = new bool[X];
		}
	} else {
		UE_LOG(LabyrinthLog, Warning, TEXT("Cannot allocate arrays: X = %i, Y = %i"), X, Y);
	}
}

void ALabyrinth::DeleteArrays() {
	if (walls_v != nullptr) {
		UE_LOG(LabyrinthLog, Display, TEXT("Deleting vertical walls"));
		for (int32 i = 0; i < Y; ++i) {
			delete[] walls_v[i];
		}
		delete[] walls_v;
		walls_v = nullptr;
	}
	if (walls_h != nullptr) {
		UE_LOG(LabyrinthLog, Display, TEXT("Deleting horizontal walls"));
		for (int32 i = 0; i < Y + 1; ++i) {
			delete[] walls_h[i];
		}
		delete[] walls_h;
		walls_h = nullptr;
	}
}

bool ALabyrinth::RandBool() {
	return rand() % 2 ? true : false;
}
