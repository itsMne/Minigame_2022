//*****************************************************************************
// GameObject.h
// Author : Mane
//*****************************************************************************
#ifndef _SOUND_H_
#define _SOUND_H_

#include "C_DXWindow.h"
#include <xaudio2.h>

// Windows7ÌêÍãLðíµÄÈºÉu«·¦.
//#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>

//*****************************************************************************
// TEht@C@//@Gi
//*****************************************************************************
typedef enum
{
	SOUND_LABEL_BGM_MENU = 0,		// BGM0
	SOUND_LABEL_BGM_TITLESCREEN,	// BGM0
	SOUND_LABEL_BGM_DEBUGROOM,		// BGM1
	SOUND_LABEL_BGM_LEVEL1ROOM,		// BGM1
	SOUND_LABEL_BGM_TUTORIALROOM,	// BGM1
	SOUND_LABEL_BGM_GAMEOVER,		// BGM2
	SOUND_LABEL_SE_HIT,					// £éSE
	SOUND_LABEL_SE_ALTHIT,				// £éSE
	SOUND_LABEL_SE_MEDIUMHIT,			// £éSE
	SOUND_LABEL_SE_BIGHIT,				// £éSE
	SOUND_LABEL_SE_SELECTOPTION,		// £éSE
	SOUND_LABEL_SE_SELECTED,			// £éSE
	SOUND_LABEL_SE_SA_BROKEN,			// £éSE
	SOUND_LABEL_SE_HITBYBULLET,			// £éSE
	SOUND_LABEL_SE_TELEPORTIN,			// £éSE
	SOUND_LABEL_SE_TELEPORTOUT,			// £éSE
	SOUND_LABEL_SE_PLAYER_DAMAGED,			// £éSE
	SOUND_LABEL_SE_PLAYER_DEAD,			// £éSE
	SOUND_LABEL_SE_ENEMY_DEAD,			// £éSE
	SOUND_LABEL_SE_ENEMY_ISDYING,			// £éSE
	SOUND_LABEL_SE_COLLISION_WALL,			// £éSE
	SOUND_LABEL_SE_SPOTLIGHT,			// £éSE
	SOUND_LABEL_SE_REMOVE_STICKER,			// £éSE
	SOUND_LABEL_SE_PASTE_STICKER,			// £éSE
	SOUND_LABEL_SE_SWITCH_HIT,			// £éSE
	SOUND_LABEL_SE_CHARGE,			// £éSE
	SOUND_LABEL_SE_JUMP,			// £éSE
	SOUND_LABEL_SE_FLOWER_A,		// £éSE
	SOUND_LABEL_SE_FLOWER_B,		// £éSE
	SOUND_LABEL_SE_STAR_HIT,		// £éSE
	SOUND_LABEL_SE_ON,				// £éSE
	SOUND_LABEL_RECOVERED,			// £éSE
	SOUND_GEM_PICKUP,				// o^Ì©®Ýè
	SOUND_AIR_DASH,					// o^Ì©®Ýè
	SOUND_DOWNDASH,					// o^Ì©®Ýè
	SOUND_REGULAR_PUNCH,			// o^Ì©®Ýè
	SOUND_BIG_PUNCH,				// o^Ì©®Ýè
	SOUND_LABEL_MAX,				// o^Ì©®Ýè
} SOUND_LABEL;

//*****************************************************************************
// vg^Cvé¾
//*****************************************************************************
HRESULT InitSound(HWND hWnd);
void	UninitSound(void);
HRESULT PlaySound(SOUND_LABEL label);
void	StopSound(SOUND_LABEL label);
void	StopSound(void);
void	SetVolume(float fVol);

#endif
