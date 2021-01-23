// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "ChessGameModeBase.h"
#include "ChessPawn.h"
#include "ChessGameStateBase.h"
#include "ChessPlayerController.h"

AChessGameModeBase::AChessGameModeBase()
{
	// no pawn by default
	DefaultPawnClass = AChessPawn::StaticClass();

	// use our own player controller class
	PlayerControllerClass = AChessPlayerController::StaticClass();

	// use our own Game state base
	GameStateClass = AChessGameStateBase::StaticClass();
}