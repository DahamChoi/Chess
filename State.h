// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum class EActionEnum : uint8
{
	AE_NORMAL = 0,
	AE_TAKE,
	AE_EN_PASSANT,
	AE_PROMOTION,
	AE_CASTLING
};

class ActionStruct
{
public:
	EActionEnum ActionType;
	int32 Action;

public:
	ActionStruct(EActionEnum type, int32 action) : ActionType(type), Action(action)	{}
};

class Position
{
public:
	int32 X, Y;

public:
	Position(int32 x,int32 y) : X(x), Y(y) {}

	Position operator+(Position& ref)
	{
		return Position(this->X + ref.X, this->Y + ref.Y);
	}

	Position operator-(Position& ref)
	{
		return Position(this->X - ref.X, this->Y - ref.Y);
	}
};

static const int ACTION_KING_SIDE_CASTLING = 4097;
static const int ACTION_QUEEN_SIDE_CASTLING = 4098;
static const int ACTION_PROMOTION_WHITE_START = 4100;
static const int ACTION_PROMOTION_BLACK_START = 4108;

/**
 * 
 */
class CHESS_API State
{
public:
	TArray<int32>* mPieces;
	TArray<int32>* mEnemyPieces;

	TArray<int32>* mPrevPieces;
	TArray<int32>* mEnemyPrevPieces;

	int32 mDepth;

	bool mWhiteCastling;
	bool mBlackCastling;

	bool mWhiteKingMove;
	bool mWhiteLeftRookMove;
	bool mWhiteRightRookMove;
	bool mBlackKingMove;
	bool mBlackLeftRookMove;
	bool mBlackRightRookMove;

public:
	State();
	State(State* state);
	~State();

public:
	// 초기화
	void initGameBoard();

	// 선공여부
	bool isFirstPlayer();

	// 체크
	bool isCheck();

	// 체크메이트
	bool isCheckMate();
	
	// 50수 무승부
	bool isDraw();

	// 다음액션
	void next(int32 action);

	// 가능한 액션
	TArray<ActionStruct>* legalActions(int32 unitIndex);

	bool CanMoveEnemyUnitByIndex(int32 dst);

	// Position To Index
	static int32 positionToIndex(int32 x, int32 y)
	{
		return (y * 8) + x;
	}

	static Position indexToPosition(int32 index)
	{
		return Position(index % 8, index / 8);
	}

	static Position actionToIndex(int32 action)
	{
		int32 src = action / 64;
		int32 dst = action % 64;

		return Position(src, dst);
	}

	static int32 indexToAction(int32 src, int32 dst)
	{
		return (src * 64) + dst;
	}
};
