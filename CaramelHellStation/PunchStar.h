#ifndef PUNCHSTAR_H
#define PUNCHSTAR_H

#include "GameObject.h"

//*****************************************************************************
// ÉNÉâÉX
//*****************************************************************************
class PunchStar :
	public GameObject
{
private:
	//êØÇÃïœêî
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