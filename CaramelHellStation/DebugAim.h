//*****************************************************************************
// DebugAim.h
// Author : Mane
//*****************************************************************************
#ifndef DEBUGAIM_H
#define DEBUGAIM_H

#include "GameObject.h"

//*****************************************************************************
// エナム定義
//*****************************************************************************
enum DEBUG_CREATOR_AIM
{
	DA_AIM = 0,
	DA_GEM,
	DA_SOBA,
	DA_FLOOR_DEBUG,
	DA_FLOOR_REGULAR_PURPLE,
	DA_WALL_PURPLEFACE,
	DA_SPIKE,
	DA_ENEMY_BUNBUN,
	DA_TELEPORT,
	DA_ENDPOINT,
	DA_GENERAL_POLYGON,
	DA_STAR,
	DA_MAX
};
enum eSelectNumOptions
{
	DAO_LEVEL_SLOT = 0,
	DAO_OBJECT_SPEED,
	DAO_OBJECT_DELAY,
	MAX_SELECT_NUM_OPTIONS
};

//*****************************************************************************
// クラス
//*****************************************************************************
class DebugAim :
	public GameObject
{
private:
	int nTypeObjectToCreate;
	int nFramesCooldownForSave;
	int nSaveSlot;
	bool bSetStarTeleportDestination;
	GameObject* goLastAdded;
	int nMoveableObjectSpeed;
	int nDelaySeconds;
	int SelectNumChange;
public:
	DebugAim();
	~DebugAim();
	void Init();
	void Update();
	void Draw();
	void End();
	int GetSaveSlot();
	int GetMoveableObjectSpeed();
	int GetDelayObjects();
	int GetSelectNum();
};

#endif