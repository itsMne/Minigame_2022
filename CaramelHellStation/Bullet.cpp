//*****************************************************************************
// Bullet.cpp
// 弾丸オブジェクトの管理
// Author : Mane
//*****************************************************************************
#include "Bullet.h"
#include "TextureManager.h"
#include "Camera.h"

//*****************************************************************************
// マクロ
//*****************************************************************************
#define PRINT_HITBOX false

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
Bullet::Bullet(int nBulletType) :GameObject()
{
	Init(nBulletType);
}


Bullet::~Bullet()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化の関数
//引数：int
//戻：void
//*****************************************************************************
void Bullet::Init(int nBulletType)
{
	xm2BulletSpeed = { 0,0 };
	Position = { 0,0,0 };
	GameObject::Init(GetTexture(TEXTURE_COMMON_BULLETS), nBulletType);
	if (nBulletType == PLAYER_BULLET_TYPE) {
		gpTexture = GetTexture(TEXTURE_COMMON_BULLETS);
		hitbox = { 0,0,50,50, PRINT_HITBOX, nullptr };
		SetSpriteSize(50, 50);
	}
	if (nBulletType == PLAYER_CATCHBULLET_TYPE)
	{
		gpTexture = GetTexture(TEXTURE_CATCH_BULLETS);
		hitbox = { 0,0,80,80, PRINT_HITBOX, nullptr };
		SetSpriteSize(100, 100);
	}

	bIsInUse = false;

	nFrame_X = 10;
	nFrame_Y = 1;
}

//*****************************************************************************
//Update関数
//処理のアップデートの関数
//引数：void
//戻：void
//*****************************************************************************
void Bullet::Update()
{
	GameObject::Update();
	if (nType == PLAYER_BULLET_TYPE)
	{
		hitbox = { 0,0,50,50 };
		SetSpriteSize(50, 50);
	}
	if (++n_uvTimer > 2)
	{
		n_uvTimer = 0;
		uv.U++;
		if (uv.U == nFrame_X)
			uv.U = 0;
	}
	Position.x += xm2BulletSpeed.x + x2Acceleration.x;
	Position.y += xm2BulletSpeed.y + x2Acceleration.y;
	x2Acceleration.x += x2Acceleration.x;
	x2Acceleration.y += x2Acceleration.y;
	if (Position.x > GetCameraLimitRight() + 10 || Position.x < GetCameraLimitLeft() - 10 || Position.y > GetCameraLimitUp() + 150 || Position.y < GetCameraLimitDown())
	{
		bIsInUse = false;
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Bullet::Draw()
{
	GameObject::Draw();
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Bullet::End()
{
	GameObject::End();
}

//*****************************************************************************
//SetBulletSpeed関数
//弾丸の速さを設定する
//引数：XMFLOAT2
//戻：void
//*****************************************************************************
void Bullet::SetBulletSpeed(XMFLOAT2 Speed)
{
	xm2BulletSpeed = Speed;
}