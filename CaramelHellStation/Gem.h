//*****************************************************************************
// Gem.h
// Author : Mane
//*****************************************************************************
#ifndef	GEM_H
#define GEM_H

#include "GameObject.h"

//*****************************************************************************
// ƒNƒ‰ƒX
//*****************************************************************************
class Gem :
	public GameObject
{
public:
	Gem(XMFLOAT2 Position);
	~Gem();
	void Init(XMFLOAT2 Position);
	void Update();
	void Draw();
	void End();
};
#endif
