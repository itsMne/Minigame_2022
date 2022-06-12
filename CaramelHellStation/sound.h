//*****************************************************************************
// GameObject.h
// Author : Mane
//*****************************************************************************
#ifndef _SOUND_H_
#define _SOUND_H_

#include "C_DXWindow.h"
#include <xaudio2.h>

// Windows7�̏ꍇ�͏�L���폜���Ĉȉ��ɒu������.
//#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>

//*****************************************************************************
// �T�E���h�t�@�C���@//�@�G�i��
//*****************************************************************************
typedef enum
{
	SOUND_LABEL_BGM_MENU = 0,		// BGM0
	SOUND_LABEL_BGM_TITLESCREEN,	// BGM0
	SOUND_LABEL_BGM_DEBUGROOM,		// BGM1
	SOUND_LABEL_BGM_LEVEL1ROOM,		// BGM1
	SOUND_LABEL_BGM_TUTORIALROOM,	// BGM1
	SOUND_LABEL_BGM_GAMEOVER,		// BGM2
	SOUND_LABEL_SE_HIT,					// ����SE
	SOUND_LABEL_SE_ALTHIT,				// ����SE
	SOUND_LABEL_SE_MEDIUMHIT,			// ����SE
	SOUND_LABEL_SE_BIGHIT,				// ����SE
	SOUND_LABEL_SE_SELECTOPTION,		// ����SE
	SOUND_LABEL_SE_SELECTED,			// ����SE
	SOUND_LABEL_SE_SA_BROKEN,			// ����SE
	SOUND_LABEL_SE_HITBYBULLET,			// ����SE
	SOUND_LABEL_SE_TELEPORTIN,			// ����SE
	SOUND_LABEL_SE_TELEPORTOUT,			// ����SE
	SOUND_LABEL_SE_PLAYER_DAMAGED,			// ����SE
	SOUND_LABEL_SE_PLAYER_DEAD,			// ����SE
	SOUND_LABEL_SE_ENEMY_DEAD,			// ����SE
	SOUND_LABEL_SE_ENEMY_ISDYING,			// ����SE
	SOUND_LABEL_SE_COLLISION_WALL,			// ����SE
	SOUND_LABEL_SE_SPOTLIGHT,			// ����SE
	SOUND_LABEL_SE_REMOVE_STICKER,			// ����SE
	SOUND_LABEL_SE_PASTE_STICKER,			// ����SE
	SOUND_LABEL_SE_SWITCH_HIT,			// ����SE
	SOUND_LABEL_SE_CHARGE,			// ����SE
	SOUND_LABEL_SE_JUMP,			// ����SE
	SOUND_LABEL_SE_FLOWER_A,		// ����SE
	SOUND_LABEL_SE_FLOWER_B,		// ����SE
	SOUND_LABEL_SE_STAR_HIT,		// ����SE
	SOUND_LABEL_SE_ON,				// ����SE
	SOUND_LABEL_RECOVERED,			// ����SE
	SOUND_GEM_PICKUP,				// �o�^���̎����ݒ�
	SOUND_AIR_DASH,					// �o�^���̎����ݒ�
	SOUND_DOWNDASH,					// �o�^���̎����ݒ�
	SOUND_REGULAR_PUNCH,			// �o�^���̎����ݒ�
	SOUND_BIG_PUNCH,				// �o�^���̎����ݒ�
	SOUND_LABEL_MAX,				// �o�^���̎����ݒ�
} SOUND_LABEL;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSound(HWND hWnd);
void	UninitSound(void);
HRESULT PlaySound(SOUND_LABEL label);
void	StopSound(SOUND_LABEL label);
void	StopSound(void);
void	SetVolume(float fVol);

#endif
