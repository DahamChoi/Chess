// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessUnitActor.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AChessUnitActor::AChessUnitActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UnitStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitStaticMesh"));

	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;
	UnitStaticMesh->SetupAttachment(DummyRoot);
	UnitStaticMesh->SetWorldScale3D(FVector(0.15f, 0.15f, 0.15f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_CHESS_KING(
		TEXT("/Game/BoardGames_Vol1/Meshes/SM_Chess_King.SM_Chess_King"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_CHESS_QUEEN(
		TEXT("/Game/BoardGames_Vol1/Meshes/SM_Chess_Queen.SM_Chess_Queen"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_CHESS_BISHOP(
		TEXT("/Game/BoardGames_Vol1/Meshes/SM_Chess_Bishop.SM_Chess_Bishop"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_CHESS_KNIGHT(
		TEXT("/Game/BoardGames_Vol1/Meshes/SM_Chess_Knight.SM_Chess_Knight"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_CHESS_ROOK(
		TEXT("/Game/BoardGames_Vol1/Meshes/SM_Chess_Rook.SM_Chess_Rook"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_CHESS_PAWN(
		TEXT("/Game/BoardGames_Vol1/Meshes/SM_Chess_Pawn.SM_Chess_Pawn"));

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> M_BLACK(
		TEXT("/Game/BoardGames_Vol1/Materials/M_ChessPieces_Marble_Black_Inst.M_ChessPieces_Marble_Black_Inst"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> M_WHITE(
		TEXT("/Game/BoardGames_Vol1/Materials/M_ChessPieces_Marble_White_Inst.M_ChessPieces_Marble_White_Inst"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> M_BLACK_OPACITY(
		TEXT("/Game/BoardGames_Vol1/Materials/M_ChessPieces_Marble_Black_Opacity.M_ChessPieces_Marble_Black_Opacity"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> M_WHITE_OPACITY(
		TEXT("/Game/BoardGames_Vol1/Materials/M_ChessPieces_Marble_White_Opacity.M_ChessPieces_Marble_White_Opacity"));

	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveF(
		TEXT("/Game/Chess/Others/MovementFloatCurve.MovementFloatCurve"));
	
	if(CurveF.Succeeded())
		CurveFloat = CurveF.Object;

	if(SM_CHESS_KING.Succeeded())
		ChessKingStaticMesh = SM_CHESS_KING.Object;

	if (SM_CHESS_QUEEN.Succeeded())
		ChessQueenStaticMesh = SM_CHESS_QUEEN.Object;

	if (SM_CHESS_BISHOP.Succeeded())
		ChessBishopStaticMesh = SM_CHESS_BISHOP.Object;

	if (SM_CHESS_KNIGHT.Succeeded())
		ChessKnightStaticMesh = SM_CHESS_KNIGHT.Object;

	if (SM_CHESS_ROOK.Succeeded())
		ChessRookStaticMesh = SM_CHESS_ROOK.Object;

	if (SM_CHESS_PAWN.Succeeded())
		ChessPawnStaticMesh = SM_CHESS_PAWN.Object;

	if (M_WHITE_OPACITY.Succeeded())
		ChessWhiteOpacityMaterial = M_WHITE_OPACITY.Object;

	if (M_WHITE.Succeeded())
		ChessWhiteMaterial = M_WHITE.Object;

	if (M_BLACK_OPACITY.Succeeded())
		ChessBlackOpacityMaterial = M_BLACK_OPACITY.Object;

	if (M_BLACK.Succeeded())
		ChessBlackMaterial = M_BLACK.Object;

	TargetLocation = ActorInitialLocation = GetActorLocation();
}

void AChessUnitActor::InitializeActor(EUnitEnum type, bool white, bool opacity)
{
	this->UnitType = type;
	this->isWhite = white;
	this->isOpacity = opacity;

	switch (UnitType)
	{
	case EUnitEnum::UE_KING:
		UnitStaticMesh->SetStaticMesh(ChessKingStaticMesh);
		break;
	case EUnitEnum::UE_QUEEN:
		UnitStaticMesh->SetStaticMesh(ChessQueenStaticMesh);
		break;
	case EUnitEnum::UE_BISHOP:
		UnitStaticMesh->SetStaticMesh(ChessBishopStaticMesh);
		break;
	case EUnitEnum::UE_KNIGHT:
		UnitStaticMesh->SetStaticMesh(ChessKnightStaticMesh);
		break;
	case EUnitEnum::UE_ROOK:
		UnitStaticMesh->SetStaticMesh(ChessRookStaticMesh);
		break;
	case EUnitEnum::UE_PAWN:
		UnitStaticMesh->SetStaticMesh(ChessPawnStaticMesh);
		break;
	default:
		break;
	}

	if (isWhite && isOpacity)
	{
		UnitStaticMesh->SetMaterial(0, ChessWhiteOpacityMaterial);
	}
	else if (isWhite)
	{
		UnitStaticMesh->SetMaterial(0, ChessWhiteMaterial);
	}
	else if (isOpacity)
	{
		UnitStaticMesh->SetMaterial(0, ChessBlackOpacityMaterial);
	}
	else
	{
		UnitStaticMesh->SetMaterial(0, ChessBlackMaterial);
	}
}

void AChessUnitActor::LerpMoveToTarget(FVector target)
{
	FOnTimelineFloat ProgressFunction;

	ProgressFunction.BindUFunction(this, FName("HandleProgress"));

	MyTimeline.AddInterpFloat(CurveFloat, ProgressFunction);

	ActorInitialLocation = GetActorLocation();
	TargetLocation = target;

	MyTimeline.PlayFromStart();
}

void AChessUnitActor::LerpMoveToTargetReverse()
{
	FVector temp = TargetLocation;
	TargetLocation = ActorInitialLocation;
	ActorInitialLocation = temp;

	MyTimeline.PlayFromStart();
}

// Called when the game starts or when spawned
void AChessUnitActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChessUnitActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MyTimeline.TickTimeline(DeltaTime);
}

void AChessUnitActor::HandleProgress(float value)
{
	//Setting up the new location of our actor
	FVector NewLocation = FMath::Lerp(ActorInitialLocation, TargetLocation, value);
	SetActorLocation(NewLocation);

	/*
	if (GetActorLocation().Equals(TargetLocation))
	{
		MyTimeline.Stop();
	}
	*/
}

