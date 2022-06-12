//*****************************************************************************
// Bullet.h
// Author : Mane
//*****************************************************************************
#ifndef BULLET_H
#define BULLET_H

#include "GameObject.h"

//*****************************************************************************
// クラス
//*****************************************************************************
class Bullet :
	public GameObject
{
private:
	//弾の変数
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