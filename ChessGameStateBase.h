// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "State.h"
#include "ChessGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API AChessGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AChessGameStateBase();
	~AChessGameStateBase();

public:
	State* mState;
};
