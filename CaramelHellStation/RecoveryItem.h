//*****************************************************************************
// RecoveryItem.h
// Author : Mane
//*****************************************************************************
#ifndef	RECOVERYITEM_H
#define RECOVERYITEM_H

#include "GameObject.h"

//*****************************************************************************
// ƒNƒ‰ƒX
//*****************************************************************************
class RecoveryItem :
	public GameObject
{
public:
	RecoveryItem(XMFLOAT2 Position);
	~RecoveryItem();
	void Init(XMFLOAT2 Position);
	void Update();
	void Draw();
	void End();
};

#endif