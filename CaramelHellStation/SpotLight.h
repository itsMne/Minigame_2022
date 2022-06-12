//*****************************************************************************
// SpotLight.h
// Author : Mane
//*****************************************************************************
#ifndef	SPOTLIGHT_H
#define SPOTLIGHT_H

#include "GameObject.h"

//*****************************************************************************
// クラス
//*****************************************************************************
class SpotLight :
	public GameObject
{
private:
	//スポットライト
	int			nSpotlightCoolDownFrames;
	Sprite*		BorderTop;
	Sprite*		BorderBottom;
	Sprite*		BorderRight;
	Sprite*		BorderLeft;
public:
	SpotLight();
	~SpotLight();
	void Init();
	void Update();
	void Draw();
	void End();
	void SpotLightBordersSetUp();
};

#endif