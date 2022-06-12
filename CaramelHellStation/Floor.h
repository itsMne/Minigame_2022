//*****************************************************************************
// Floor.h
// Author : Mane
//*****************************************************************************
#ifndef	FLOOR_H
#define FLOOR_H

#include "GameObject.h"
#include "Switch.h"

//*****************************************************************************
// ÉNÉâÉX
//*****************************************************************************
class Floor :
	public GameObject
{
private:
	//ìÆÇØÇÈè∞ÇÃïœêî
	int			nFloorID;
	bool		bSafeFloor;
	int			nFloorTexture;
	SwitchGO* ResetSwitch;
	SwitchGO* MoveForwardSwitch;
	float		fMovingFloorAcceleration;
	bool		bControllableMoveFloor;
public:
	Floor();
	Floor(int FloorTextureID, HitBox hbox, float TileSize, int nID);
	~Floor();
	void Init(int FloorTextureID, HitBox hbox, float TileSize, int nID);
	void Update();
	void Draw();
	void End();
	void SetFloorAsSafe(bool bIsSafe);
	bool IsSafeFloor();
	int GetFloorID();
	void SetUpMoveFloorControlledBySwitch(XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, XMFLOAT2 ResetSwitchPosition);
	int GetFloorTexture();
	SwitchGO * GetResetSwitchForMoveablePlatform();
	SwitchGO * GetControlSwitchForMoveablePlatform();
};

#endif