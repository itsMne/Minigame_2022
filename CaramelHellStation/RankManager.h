//*****************************************************************************
// RankManager.h
// Author : Mane
//*****************************************************************************
#ifndef RANKMANAGER_H
#define RANKMANAGER_H

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_STYLE_RANK			 5
#define MAX_MOVES				 4
#define MAX_FRAMES_FOR_RANKINGUP 90

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
namespace RankManager
{
	void Init();
	void Update();
};
void AddMoveToRankMeter(int MoveToAdd, int nMoveStrength);
void AddFreezeRankFrames(int nFramesToAdd);
void ResetRanks();
int  GetRank();
int	 GetRankFrames();
#endif  //RANKMANAGER_H