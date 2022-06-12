//*****************************************************************************
// Teleport.h
// Author : Mane
//*****************************************************************************
#ifndef TELEPORTER_H
#define	TELEPORTER_H

#include "GameObject.h"

//*****************************************************************************
// クラス
//*****************************************************************************
class Teleporter :
	public GameObject
{
private:
	//テレポートの変数
	XMFLOAT2	xm2Teleporter_NewPlayerPos;
	int			nTeleporterID;
	bool		bIsEndpoint;
	bool		bSwitchToResultScreen;
	int			nFrameCounterBeforeChangingStates;
public:
	Teleporter(XMFLOAT2 TeleporterPos, XMFLOAT2 NewPlayerPos);
	Teleporter(XMFLOAT2 TeleporterPos, XMFLOAT2 NewPlayerPos, int nID);
	~Teleporter();
	void Init(XMFLOAT2 TeleporterPos, XMFLOAT2 NewPlayerPos);
	void Update();
	void Draw();
	void End();
	void SetTeleporterID(int newID);
	void SetTeleporterAsEndpoint();
	int GetTeleporterID();
	bool IsEndPoint();
	void SetTeleporterDestination(XMFLOAT2 dest);
	XMFLOAT2 GetTeleporterDestination();
};

#endif