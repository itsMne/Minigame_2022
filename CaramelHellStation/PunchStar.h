#ifndef PUNCHSTAR_H
#define PUNCHSTAR_H

#include "GameObject.h"

//*****************************************************************************
// �N���X
//*****************************************************************************
class PunchStar :
	public GameObject
{
private:
	//���̕ϐ�
	XMFLOAT2	xm2StarMoveableInitialPos;
	XMFLOAT2	xm2StarMoveableFinalPos;
	int			nWaitingFrames;
	int			nStarID;
	int			nFrameCounterMoving;
public:
	PunchStar(XMFLOAT2 InitPos, XMFLOAT2 FinalPos);
	PunchStar(XMFLOAT2 InitPos, XMFLOAT2 FinalPos, int nID);
	~PunchStar();
	void Init(XMFLOAT2 InitPos, XMFLOAT2 FinalPos, int ID);
	void Update();
	int GetStarID();
	void Draw();
	void End();
	void SetStarDestination(XMFLOAT2 dest);
	XMFLOAT2 GetStarInitialPos();
	XMFLOAT2 GetStarFinalPos();
};

#endif