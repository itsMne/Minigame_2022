//*****************************************************************************
// RankManager.cpp
// �����N���Ǘ�����
// Author : Mane
//*****************************************************************************
#include "RankManager.h"
#include "Player.h"
#include <stdio.h>
#include <windows.h>

//*****************************************************************************
// �G�i����`
//*****************************************************************************
enum ADDED_TO_RANK_RESULT
{
	MOVE_ADDED,
	MOVE_REPEATED,
	MOVE_NOT_ADDED = -1,
	MAX_RESULT
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
int		nCurrentRank;
int		MovesInMeter[MAX_MOVES];
int		RepeatedMoves[MAX_MOVES*2];
int		AddResult;
int		nMoveStrength;
float	fRankFrames;
int		nFramesForFreezingRank;

//*****************************************************************************
//Init�֐�
//�������̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void RankManager::Init()
{
	nCurrentRank = 1;
	AddResult = MOVE_NOT_ADDED;
	fRankFrames = 0;
	nFramesForFreezingRank = 0;
	nMoveStrength = 0;
	for (int i = 0; i < MAX_MOVES; i++)
	{
		MovesInMeter[i] = -1;
		RepeatedMoves[i] = -1;
	}
}

//*****************************************************************************
//Update�֐�
//�A�b�v�f�[�g�̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void RankManager::Update()
{
	Player* pPlayer = GetPlayerPointer();
	if (!pPlayer)
		return;
	if (pPlayer->GetPlayerState() == PLAYER_TELEPORTING || IsPlayerPaused())
		return;
	if (AddResult == MOVE_ADDED) {
		fRankFrames += nMoveStrength;
		nFramesForFreezingRank = 15;
	}
	else if (AddResult == MOVE_REPEATED)
	{
		nFramesForFreezingRank = 10;
	}
	if (fRankFrames > MAX_FRAMES_FOR_RANKINGUP*nCurrentRank)
	{
		nCurrentRank++;
		if (nCurrentRank > MAX_STYLE_RANK) {
			nCurrentRank = MAX_STYLE_RANK;
			fRankFrames = (float)(MAX_FRAMES_FOR_RANKINGUP * nCurrentRank);
		}
	}
	else if (fRankFrames == 0) {
		nCurrentRank--;
		if (nCurrentRank < 1)
			nCurrentRank = 1;
		fRankFrames = 50;
		nFramesForFreezingRank = 5;
	}
	if (nFramesForFreezingRank > 20)
		nFramesForFreezingRank = 20;
	if (nFramesForFreezingRank <= 0) {
		if (fRankFrames > 0) {
			fRankFrames--;
			if (nCurrentRank >= 4)
				fRankFrames-=0.5f;
		}
	}
	else
	{
		nFramesForFreezingRank--;
	}
	if (fRankFrames < 0)
		fRankFrames = 0;
	AddResult = MOVE_NOT_ADDED;
}

//*****************************************************************************
//AddMoveToRankMeter�֐�
//�����������Ń����N���v�Z����
//�����Fint, int
//�߁Fvoid
//*****************************************************************************
void AddMoveToRankMeter(int nMoveToAdd, int MoveStrength)
{
	if (AddResult != MOVE_NOT_ADDED)
		return;
	//printf("%s, L%d: %d\n", __FILE__, __LINE__, nMoveToAdd);
	nMoveStrength = MoveStrength;
	int countSameMove = 0;
	for (int i = 0; i < MAX_MOVES; i++)
	{
		if (MovesInMeter[i] == nMoveToAdd)
			countSameMove++;
	}
	if (countSameMove <= 1) {
		for (int i = MAX_MOVES-1; i >= 0; i--)
		{
			MovesInMeter[i] = MovesInMeter[i - 1];
		}
		MovesInMeter[0] = nMoveToAdd;
		AddResult = MOVE_ADDED;
		return;
	}
	countSameMove = 0;
	for (int i = 0; i < MAX_MOVES*2; i++)
	{
		if (RepeatedMoves[i] == nMoveToAdd)
			countSameMove++;
	}
	if (countSameMove <= 8)
	{
		for (int i = (MAX_MOVES * 2)-1; i >= 0; i--)
		{
			RepeatedMoves[i] = RepeatedMoves[i-1];
		}
		RepeatedMoves[0] = nMoveToAdd;
		AddResult = MOVE_REPEATED;
		return;
	}
	AddResult = MOVE_NOT_ADDED;
}

//*****************************************************************************
//AddFreezeRankFrames�֐�
//������������t���[���̂Ń����N���~�܂�
//�����Fint
//�߁Fvoid
//*****************************************************************************
void AddFreezeRankFrames(int nFramesToAdd)
{
	nFramesForFreezingRank += nFramesToAdd;
}

//*****************************************************************************
//ResetRanks�֐�
//�����N�����Z�b�g����
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void ResetRanks()
{
	nCurrentRank = 1;
	AddResult = MOVE_NOT_ADDED;
	fRankFrames = 0;
	nFramesForFreezingRank = 0;
	nMoveStrength = 0;
	for (int i = 0; i < MAX_MOVES; i++)
	{
		MovesInMeter[i] = -1;
		RepeatedMoves[i] = -1;
	}
}

//*****************************************************************************
//GetRank�֐�
//�����N��߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int GetRank()
{
	return nCurrentRank;
}

//*****************************************************************************
//GetRankFrames�֐�
//�����N�̃t���[���J�E���^�[��߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int GetRankFrames()
{
	return (int)fRankFrames;
}
