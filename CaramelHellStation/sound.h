//*****************************************************************************
// GameObject.h
// Author : Mane
//*****************************************************************************
#ifndef _SOUND_H_
#define _SOUND_H_

#include "C_DXWindow.h"
#include <xaudio2.h>

// Windows7の場合は上記を削除して以下に置き換え.
//#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>

//*****************************************************************************
// サウンドファイル　//　エナム
//*****************************************************************************
typedef enum
{
	SOUND_LABEL_BGM_MENU = 0,		// BGM0
	SOUND_LABEL_BGM_TITLESCREEN,	// BGM0
	SOUND_LABEL_BGM_DEBUGROOM,		// BGM1
	SOUND_LABEL_BGM_LEVEL1ROOM,		// BGM1
	SOUND_LABEL_BGM_TUTORIALROOM,	// BGM1
	SOUND_LABEL_BGM_GAMEOVER,		// BGM2
	SOUND_LABEL_SE_HIT,					// 殴るSE
	SOUND_LABEL_SE_ALTHIT,				// 殴るSE
	SOUND_LABEL_SE_MEDIUMHIT,			// 殴るSE
	SOUND_LABEL_SE_BIGHIT,				// 殴るSE
	SOUND_LABEL_SE_SELECTOPTION,		// 殴るSE
	SOUND_LABEL_SE_SELECTED,			// 殴るSE
	SOUND_LABEL_SE_SA_BROKEN,			// 殴るSE
	SOUND_LABEL_SE_HITBYBULLET,			// 殴るSE
	SOUND_LABEL_SE_TELEPORTIN,			// 殴るSE
	SOUND_LABEL_SE_TELEPORTOUT,			// 殴るSE
	SOUND_LABEL_SE_PLAYER_DAMAGED,			// 殴るSE
	SOUND_LABEL_SE_PLAYER_DEAD,			// 殴るSE
	SOUND_LABEL_SE_ENEMY_DEAD,			// 殴るSE
	SOUND_LABEL_SE_ENEMY_ISDYING,			// 殴るSE
	SOUND_LABEL_SE_COLLISION_WALL,			// 殴るSE
	SOUND_LABEL_SE_SPOTLIGHT,			// 殴るSE
	SOUND_LABEL_SE_REMOVE_STICKER,			// 殴るSE
	SOUND_LABEL_SE_PASTE_STICKER,			// 殴るSE
	SOUND_LABEL_SE_SWITCH_HIT,			// 殴るSE
	SOUND_LABEL_SE_CHARGE,			// 殴るSE
	SOUND_LABEL_SE_JUMP,			// 殴るSE
	SOUND_LABEL_SE_FLOWER_A,		// 殴るSE
	SOUND_LABEL_SE_FLOWER_B,		// 殴るSE
	SOUND_LABEL_SE_STAR_HIT,		// 殴るSE
	SOUND_LABEL_SE_ON,				// 殴るSE
	SOUND_LABEL_RECOVERED,			// 殴るSE
	SOUND_GEM_PICKUP,				// 登録数の自動設定
	SOUND_AIR_DASH,					// 登録数の自動設定
	SOUND_DOWNDASH,					// 登録数の自動設定
	SOUND_REGULAR_PUNCH,			// 登録数の自動設定
	SOUND_BIG_PUNCH,				// 登録数の自動設定
	SOUND_LABEL_MAX,				// 登録数の自動設定
} SOUND_LABEL;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSound(HWND hWnd);
void	UninitSound(void);
HRESULT PlaySound(SOUND_LABEL label);
void	StopSound(SOUND_LABEL label);
void	StopSound(void);
void	SetVolume(float fVol);

#endif
