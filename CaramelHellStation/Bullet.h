//*****************************************************************************
// Bullet.h
// Author : Mane
//*****************************************************************************
#ifndef BULLET_H
#define BULLET_H

#include "GameObject.h"

//*****************************************************************************
// ÉNÉâÉX
//*****************************************************************************
class Bullet :
	public GameObject
{
private:
	//íeÇÃïœêî
	XMFLOAT2	xm2BulletSpeed;
	int			nBulletType;
public:
	Bullet(int);
	~Bullet();
	void Init(int);
	void Update();
	void Draw();
	void End();
	void SetBulletSpeed(XMFLOAT2 Speed);
};

#endif