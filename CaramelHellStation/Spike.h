//*****************************************************************************
// Spike.h
// Author : Mane
//*****************************************************************************
#ifndef	SPIKE_H
#define SPIKE_H

#include "GameObject.h"

//*****************************************************************************
// ƒNƒ‰ƒX
//*****************************************************************************
class Spike :
	public GameObject
{
private:
	int			nSpikeNumber;
public:
	Spike(XMFLOAT2 Pos, int SpikeNumber);
	~Spike();
	void Init(XMFLOAT2 Pos, int SpikeNumber);
	int GetSpikeNumber();
	void Update();
	void Draw();
	void End();
};

#endif