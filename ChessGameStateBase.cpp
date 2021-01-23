// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessGameStateBase.h"

AChessGameStateBase::AChessGameStateBase()
{
	mState = new State();
}

AChessGameStateBase::~AChessGameStateBase()
{
	delete mState;
}
