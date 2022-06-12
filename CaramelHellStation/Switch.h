//*****************************************************************************
// Switch.h
// Author : Mane
//*****************************************************************************
#ifndef	SWITCH_H
#define SWITCH_H

#include "GameObject.h"

//*****************************************************************************
// クラス
//*****************************************************************************
class SwitchGO :
	public GameObject
{
private:
	//スイッチの変数
	bool		bSwitchisOn;
	float		fSwitchHitsForActivation;
	float		fMaxResistanceSwitch;
	int			nSwitchID;
	bool		bPunchableSwitch;
	bool		bIsScaleInverterSwitch;
	float		newScale = 0;
	float		fScaleAcceleration = 0;
	bool		Inverting = false;
public:
	SwitchGO(XMFLOAT2 xPosition, int nID, bool bIsPunchable);
	~SwitchGO();
	void Init(XMFLOAT2 xPosition, int nID, bool bIsPunchable);
	void Update();
	void Draw();
	void SetSwitchResistance(float fSwitchmax);
	void SetSwitchOn(bool isOn);
	void SetSwitchAsScaleInverter();
	int GetSwitchID();
	bool IsSwitchOn();
	void End();
};

#endif