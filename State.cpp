// Fill out your copyright notice in the Description page of Project Settings.


#include "State.h"

State::State()
{
	mPieces = new TArray<int32>();
	mEnemyPieces = new TArray<int32>();
	mPrevPieces = new TArray<int32>();
	mEnemyPrevPieces = new TArray<int32>();
}

State::State(State* state)
{
	mPieces = new TArray<int32>(*state->mPieces);
	mEnemyPieces = new TArray<int32>(*state->mEnemyPieces);
	mPrevPieces = new TArray<int32>(*state->mPrevPieces);
	mEnemyPrevPieces = new TArray<int32>(*state->mEnemyPrevPieces);

	mDepth = state->mDepth;
	mWhiteCastling = state->mWhiteCastling;
	mBlackCastling = state->mBlackCastling;
	
	mWhiteKingMove = state->mWhiteKingMove;
	mWhiteLeftRookMove = state->mWhiteLeftRookMove;
	mWhiteRightRookMove = state->mWhiteRightRookMove;
	mBlackKingMove = state->mBlackKingMove;
	mBlackLeftRookMove = state->mBlackLeftRookMove;
	mBlackRightRookMove = state->mBlackRightRookMove;
}

State::~State()
{
	delete mPieces;
	delete mEnemyPieces;

	delete mPrevPieces;
	delete mEnemyPrevPieces;
}

void State::initGameBoard()
{
	mDepth = 0;

	mWhiteCastling = false;
	mBlackCastling = false;

	mWhiteKingMove = false;
	mWhiteLeftRookMove = false;
	mWhiteRightRookMove = false;
	mBlackKingMove = false;
	mBlackLeftRookMove = false;
	mBlackRightRookMove = false;

	int32 PiecesInts[] = {
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		6,6,6,6,6,6,6,6,
		5,4,3,2,1,3,4,5
	};

	int32 EnemyPiecesInts[] = {
		5,4,3,2,1,3,4,5,
		6,6,6,6,6,6,6,6,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	};

	mPieces->Empty();
	mEnemyPieces->Empty();
	mPrevPieces->Empty();
	mEnemyPrevPieces->Empty();

	mPieces->AddUninitialized(64);
	FMemory::Memcpy(mPieces->GetData(), PiecesInts, 64 * sizeof(int32));

	mEnemyPieces->AddUninitialized(64);
	FMemory::Memcpy(mEnemyPieces->GetData(), EnemyPiecesInts, 64 * sizeof(int32));

	(*mPrevPieces) = (*mPieces);
	(*mEnemyPrevPieces) = (*mEnemyPieces);
}

bool State::isFirstPlayer()
{
	return (mDepth % 2 == 0);
}

bool State::isCheck()
{
	for (int i = 0; i < mPieces->Num(); i++)
	{
		if ((*mPieces)[i] == 1)
		{
			return CanMoveEnemyUnitByIndex(i);
		}
	}

	return false;
}

bool State::isCheckMate()
{
	for (int i = 0; i < mPieces->Num(); i++)
	{
		if ((*mPieces)[i] != 0)
		{
			TArray<ActionStruct> * actions = legalActions(i);
			if (actions->Num() > 0)
			{
				delete actions;
				return false;
			}
			delete actions;
		}
	}

	return true;
}

bool State::isDraw()
{
	return (mDepth >= 50);
}

void State::next(int32 action)
{
	Position pos = actionToIndex(action);
	int src = pos.X;
	int dst = pos.Y;

	(*mPrevPieces) = (*mEnemyPieces);
	(*mEnemyPrevPieces) = (*mPieces);

	if (isFirstPlayer() && src == 60 && (*mPieces)[60] == 1)
		mWhiteKingMove = true;
	if (isFirstPlayer() && src == 63 && (*mPieces)[63] == 5)
		mWhiteRightRookMove = true;
	if (isFirstPlayer() && src == 57 && (*mPieces)[57] == 5)
		mWhiteLeftRookMove = true;

	if (!isFirstPlayer() && src == 0 && (*mPieces)[0] == 5)
		mBlackLeftRookMove = true;
	if (!isFirstPlayer() && src == 7 && (*mPieces)[7] == 5)
		mBlackRightRookMove = true;
	if (!isFirstPlayer() && src == 4 && (*mPieces)[4] == 1)
		mBlackKingMove = true;

	
	if (action == ACTION_KING_SIDE_CASTLING)
	{
		if (this->isFirstPlayer())
		{
			(*mPieces)[63] = 0;
			(*mPieces)[60] = 0;
			(*mPieces)[61] = 5;
			(*mPieces)[62] = 1;
			mWhiteCastling = true;
		}
		else
		{
			(*mPieces)[7] = 0;
			(*mPieces)[4] = 0;
			(*mPieces)[6] = 1;
			(*mPieces)[5] = 5;
			mBlackCastling = true;
		}
	}
	else if (action == ACTION_QUEEN_SIDE_CASTLING)
	{
		if (this->isFirstPlayer())
		{
			(*mPieces)[56] = 0;
			(*mPieces)[60] = 0;
			(*mPieces)[58] = 1;
			(*mPieces)[59] = 5;
			mWhiteCastling = true;
		}
		else
		{
			(*mPieces)[4] = 0;
			(*mPieces)[0] = 0;
			(*mPieces)[2] = 1;
			(*mPieces)[3] = 5;
			mBlackCastling = true;
		}
	}
	else
	{
		// 앙파상
		if (FMath::Abs<int>(dst - src) != 8 && FMath::Abs<int>(dst - src) != 16 &&
			(*mPieces)[src] == 6 && (*mEnemyPieces)[dst] == 0)
		{
			int dstX = dst % 8;
			int dstY = dst / 8;
			int takePos = ((dstY + (isFirstPlayer() ? 8 : -8)) * 8) + dstX;

			if (isFirstPlayer())
			{
				(*mPieces)[dst] = (*mPieces)[src];
				(*mPieces)[src] = 0;
				(*mEnemyPieces)[takePos] = 0;
			}
		}
		else
		{
			(*mPieces)[dst] = (*mPieces)[src];
			(*mEnemyPieces)[dst] = 0;
			(*mPieces)[src] = 0;
		}
	}

	TArray<int32>* temp = this->mPieces;
	this->mPieces = this->mEnemyPieces;
	this->mEnemyPieces = temp;

	mDepth++;
}

TArray<ActionStruct>* State::legalActions(int32 unitIndex)
{
	TArray<ActionStruct> *actions = new TArray<ActionStruct>();

	int32 srcPosX = unitIndex % 8;
	int32 srcPosY = unitIndex / 8;

	int32 pieceType = (*mPieces)[unitIndex];
	int32 dstIndex = unitIndex;
	int32 dstPosX = srcPosX;
	int32 dstPosY = srcPosY;

	int32 pdist[3] = { -1,0,1 };
	int32 enemyCount[3];

	static int32 distArrayNight[8][2] = {
			{2,1},{-2,1},{2,-1},{-2,-1},{1,2},{-1,2},{1,-2},{-1,-2}
	};

	static int32 distArray[][8][2] = {
			{{-1,0},{1,0},{0,-1},{0,1},{-1,-1},{-1,1},{1,-1},{1,1}},
			{{-1,-1},{-1,1},{1,-1},{1,1},{0,0},{0,0},{0,0},{0,0}},
			{{-1,0},{1,0},{0,-1},{0,1},{0,0},{0,0},{0,0},{0,0}}
	};

	switch (pieceType)
	{
		// 킹
	case 1:
		// White Castling
		if (this->isFirstPlayer() && !this->mWhiteCastling && !this->mWhiteKingMove &&
			!this->mWhiteLeftRookMove && !mWhiteRightRookMove)
		{
			if ((*mPieces)[61] == 0 && (*mPieces)[62] == 0 && (*mPieces)[63] == 5)
			{
				for (int i = 60; i <= 61; i++)
				{
					if (this->CanMoveEnemyUnitByIndex(i))
						break;
				}
				actions->Add(ActionStruct(EActionEnum::AE_CASTLING, ACTION_KING_SIDE_CASTLING));
			}

			if ((*mPieces)[56] == 5 && (*mPieces)[57] == 0 && (*mPieces)[58] == 0 && (*mPieces)[59] == 0)
			{
				for (int i = 57; i <= 59; i++)
				{
					if (this->CanMoveEnemyUnitByIndex(i))
						break;
				}
				actions->Add(ActionStruct(EActionEnum::AE_CASTLING, ACTION_QUEEN_SIDE_CASTLING));
			}
		}
		// Black Castling
		else if (!this->isFirstPlayer() && !this->mBlackCastling && !this->mBlackKingMove &&
			!this->mBlackLeftRookMove && !this->mBlackRightRookMove)
		{
			if ((*mPieces)[5] == 0 && (*mPieces)[6] == 0 && (*mPieces)[7] == 5)
			{
				for (int i = 5; i <= 6; i++)
				{
					if (this->CanMoveEnemyUnitByIndex(i))
						break;
				}
				actions->Add(ActionStruct(EActionEnum::AE_CASTLING, ACTION_KING_SIDE_CASTLING));
			}

			if ((*mPieces)[0] == 5 && (*mPieces)[1] == 0 && (*mPieces)[2] == 0 && (*mPieces)[3] == 0)
			{
				for (int i = 1; i <= 3; i++)
				{
					if (this->CanMoveEnemyUnitByIndex(i))
						break;
				}
				actions->Add(ActionStruct(EActionEnum::AE_CASTLING, ACTION_QUEEN_SIDE_CASTLING));
			}
		}

		for (int ix = -1; ix <= 1; ix++)
		{
			for (int iy = -1; iy <= 1; iy++)
			{
				int dx = (srcPosX + ix);
				int dy = (srcPosY + iy);
				dstIndex = dy * 8 + dx;

				if (dx >= 0 && dx < 8 && dy >= 0 && dy < 8)
				{
					if ((*this->mPieces)[dstIndex] == 0)
					{
						actions->Add(ActionStruct(
							(*this->mEnemyPieces)[dstIndex] != 0 ? EActionEnum::AE_TAKE : EActionEnum::AE_NORMAL,
							indexToAction(unitIndex, dstIndex)));
					}
				}
			}
		}
		break;
	// 퀸, 비숍, 룩
	case 2:
	case 3:
	case 5:
		for (int i = 0; i < (pieceType == 2 ? 8 : 4); i++)
		{
			int distIndex = (pieceType == 2 ? 0 : (pieceType == 3 ? 1 : 2));
			int dx = distArray[distIndex][i][0];
			int dy = distArray[distIndex][i][1];
			int px = (unitIndex % 8);
			int py = (unitIndex / 8);

			while (true)
			{
				py += dy;
				px += dx;
				dstIndex = py * 8 + px;

				if (px < 0 || px >= 8 || py < 0 || py >= 8)
					break;

				if ((*this->mPieces)[dstIndex] == 0)
				{
					if ((*this->mEnemyPieces)[dstIndex] == 0)
						actions->Add(ActionStruct(EActionEnum::AE_NORMAL, this->indexToAction(unitIndex, dstIndex)));
					else
					{
						actions->Add(ActionStruct(EActionEnum::AE_TAKE, this->indexToAction(unitIndex, dstIndex)));
						break;
					}
				}
				else
					break;
			}
		}
		break;
		// 나이트
	case 4:
		for (int i = 0; i < 8; i++)
		{
			int px = (unitIndex % 8) + distArrayNight[i][0];
			int py = (unitIndex / 8) + distArrayNight[i][1];
			dstIndex = py * 8 + px;

			if (px >= 0 && px < 8 && py >= 0 && py < 8)
			{
				if ((*this->mPieces)[dstIndex] == 0)
				{
					actions->Add(ActionStruct(
						(*this->mEnemyPieces)[dstIndex] != 0 ? EActionEnum::AE_TAKE : EActionEnum::AE_NORMAL,
						indexToAction(unitIndex, dstIndex)));
				}
			}
		}

		break;
	// PAWN
	case 6:
		// 앞으로 한 칸 전진
		dstPosY = this->isFirstPlayer() ? srcPosY - 1 : srcPosY + 1;

		// 대각선에 적 유닛이 있을 경우
		for (int i = 0; i < 3; i++)
		{
			int px = srcPosX + pdist[i];
			if (px >= 0 && px < 8 && dstPosY >= 0 && dstPosY < 8)
			{
				if ((*this->mEnemyPieces)[this->positionToIndex(px, dstPosY)] != 0)
				{
					enemyCount[i] = 1;
					continue;
				}
			}

			enemyCount[i] = 0;
		}

		// 전방 모든방위에 적 유닛이 있을 경우 앞으로 이동하지 못함
		if (enemyCount[0] && enemyCount[2])
		{
			actions->Add(ActionStruct(EActionEnum::AE_TAKE, 
				this->positionToIndex(dstPosX - 1, dstPosY)));
			actions->Add(ActionStruct(EActionEnum::AE_TAKE, 
				this->positionToIndex(dstPosX + 1, dstPosY)));
		}
		// 대각선 이동
		else
		{
			if (enemyCount[0])
			{
				actions->Add(ActionStruct(EActionEnum::AE_TAKE,
					this->positionToIndex(dstPosX - 1, dstPosY)));
			}
			if (enemyCount[2])
			{
				actions->Add(ActionStruct(EActionEnum::AE_TAKE,
					this->positionToIndex(dstPosX + 1, dstPosY)));
			}

			// 전방에 적이 없을 때 한칸 전진 가능
			if (!enemyCount[1])
			{
				// 프로모션
				if (dstPosY == 7 || dstPosY == 0)
				{
					actions->Add(ActionStruct(EActionEnum::AE_PROMOTION,
						indexToAction(unitIndex, this->positionToIndex(dstPosX, dstPosY))));
				}
				else
				{
					// 처음 이동시 두칸 이동 가능
					if ((this->isFirstPlayer() && srcPosY == 6) ||
						(!this->isFirstPlayer() && srcPosY == 1))
					{
						int spaceY = this->isFirstPlayer() ? 5 : 2;
						int dstY = this->isFirstPlayer() ? 4 : 3;

						dstIndex = this->positionToIndex(dstPosX, dstY);
						if ((*this->mPieces)[this->positionToIndex(dstPosX, spaceY)] == 0 &&
							(*this->mEnemyPieces)[this->positionToIndex(dstPosX, spaceY)] == 0 &&
							(*this->mEnemyPieces)[dstIndex] == 0 &&
							(*this->mPieces)[dstIndex] == 0)
						{
							actions->Add(ActionStruct(EActionEnum::AE_NORMAL,indexToAction(unitIndex, dstIndex)));
						}
					}

					actions->Add(ActionStruct(EActionEnum::AE_NORMAL,
						indexToAction(unitIndex, this->positionToIndex(dstPosX, dstPosY))));
				}
			}
		}

		// 앙파상
		if (dstPosY == 2 && isFirstPlayer())
		{
			for (int i = -1; i <= 1; i += 2)
			{
				int px = srcPosX + i;
				if (px >= 0 && px < 8)
				{
					if ((*this->mEnemyPrevPieces)[8 + px] == 6 &&
						(*this->mEnemyPieces)[24 + px] == 6)
					{
						int p = dstPosY * 8 + px;
						actions->Add(ActionStruct(EActionEnum::AE_EN_PASSANT, indexToAction(unitIndex, p)));
					}
				}
			}
		}
		else if (dstPosY == 5 && !isFirstPlayer())
		{
			for (int i = -1; i <= 1; i += 2)
			{
				int px = srcPosX + i;
				if (px >= 0 && px < 8)
				{
					if ((*this->mEnemyPrevPieces)[48 + px] == 6 &&
						(*this->mEnemyPieces)[32 + px] == 6)
					{
						int p = dstPosY * 8 + px;
						actions->Add(ActionStruct(EActionEnum::AE_EN_PASSANT, indexToAction(unitIndex, p)));
					}
				}
			}
		}
		break;
	default:
		break;
	}

	if (isCheck())
	{
		int i = 0;

		while(true)
		{
			if (actions->Num() <= i)
				break;

			State* state = new State(this);

			state->next((*actions)[i].Action);
			state->mDepth += 1;
			TArray<int32>* temp = state->mPieces;
			state->mPieces = state->mEnemyPieces;
			state->mEnemyPieces = temp;

			if (state->isCheck())
			{
				actions->RemoveAt(i);
				i = 0;
			}

			++i;
			delete state;
		}
	}

	return actions;
}

bool State::CanMoveEnemyUnitByIndex(int32 dst)
{
	TArray<int32>* pieces = mEnemyPieces;
	TArray<int32>* enemyPieces = mPieces;

	static int32 distArrayNight[8][2] = {
		{2,1},{-2,1},{2,-1},{-2,-1},{1,2},{-1,2},{1,-2},{-1,-2}
	};

	static int32 distArray[][8][2] = {
			{{-1,0},{1,0},{0,-1},{0,1},{-1,-1},{-1,1},{1,-1},{1,1}},
			{{-1,-1},{-1,1},{1,-1},{1,1},{0,0},{0,0},{0,0},{0,0}},
			{{-1,0},{1,0},{0,-1},{0,1},{0,0},{0,0},{0,0},{0,0}}
	};

	int dstX = dst % 8;
	int dstY = dst / 8;

	// 킹
	for (int ix = -1; ix <= 1; ix++)
	{
		for (int iy = -1; iy <= 1; iy++)
		{
			if (ix == 0 && iy == 0)
				continue;

			int dx = (dstX + ix);
			int dy = (dstY + iy);
			int pIndex = dy * 8 + dx;

			if (dx >= 0 && dx < 8 && dy >= 0 && dy < 8)
			{
				if ((*pieces)[pIndex] == 1)
					return true;
			}
		}
	}

	// 퀸
	// 비숍
	// 룩
	for (int j = 0; j < 3; j++)
	{
		int pieceType = j == 0 ? 2 : (j == 1) ? 3 : 5;
		for (int i = 0; i < (pieceType == 2 ? 8 : 4); i++)
		{
			int dx = distArray[j][i][0];
			int dy = distArray[j][i][1];
			int px = (dst % 8);
			int py = (dst / 8);

			while (true)
			{
				py += dy;
				px += dx;
				int pIndex = py * 8 + px;

				if (px < 0 || px >= 8 || py < 0 || py >= 8)
					break;

				if ((*pieces)[pIndex] == pieceType)
					return true;
				else if ((*pieces)[pIndex] != 0 || (*enemyPieces)[pIndex] != 0)
					break;
			}
		}
	}

	// 나이트
	for (int i = 0; i < 8; i++)
	{
		int px = (dst % 8) + distArrayNight[i][0];
		int py = (dst / 8) + distArrayNight[i][1];
		int pIndex = py * 8 + px;

		if (px >= 0 && px < 8 && py >= 0 && py < 8)
		{
			if ((*pieces)[pIndex] == 5)
				return true;
		}
	}

	// 폰
	int py = (this->isFirstPlayer() ? (dstY - 1) : (dstY + 1));

	for (int i = -1; i <= 1; i += 2)
	{
		int px = dstX + i;

		if (py >= 0 && py < 8 && px >= 0 && px < 8)
		{
			if ((*pieces)[py * 8 + px] == 6)
				return true;
		}
	}

	// 아무것도 해당되지 않으면
	return false;
}
