//*****************************************************************************
// Gem.cpp
// プレイヤーのスポットライトの管理
// Author : Mane
//*****************************************************************************
#include "Gem.h"
#include "TextureManager.h"
#include "Player.h"
#include "SceneInGame.h"
#include "sound.h"

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
Gem::Gem(XMFLOAT2 Position)
{
	Init(Position);
}


Gem::~Gem()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化の関数
//引数：XMFLOAT2
//戻：void
//*****************************************************************************
void Gem::Init(XMFLOAT2 Position)
{
	InitSprite();
	SetType(GEM_TYPE);
	SetUse(true);
	gpTexture = GetTexture(TEXTURE_CRYSTAL);
	SetSpriteSize(50, 71);
	SetWorldPosition(Position.x, Position.y);
	x2GOInitialPos = Position;
	hitbox = { 0,0,70,91 };
	nFrame_Y = 4;
	nFrame_X = 5;
}

//*****************************************************************************
//Update関数
//処理のアップデートの関数
//引数：void
//戻：void
//*****************************************************************************
void Gem::Update()
{
	GameObject::Update();
	//ジェムのオブジェクト
	if (!bIsInUse)
		return;
	Player* pPlayer = GetPlayerPointer();
	SceneInGame* pGame = GetCurrentGame();
	RegularUVAnimationControl(3);
	if (pPlayer)
	{
		Weapon* cWeapon = pPlayer->GetCurrentWeapon();
		if ((IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), GetHitbox()) || (IsInCollision(cWeapon->GetHitbox(), GetHitbox()) && cWeapon->IsWeaponAttacking()))
			&& pPlayer->GetPlayerState() != PLAYER_TELEPORTING)
		{
			bIsInUse = false;
			if (!pGame)
				return;
			pGame->AddToScore(10);
			pGame->AddGem(1);
			PlaySound(SOUND_GEM_PICKUP);
		}
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Gem::Draw()
{
	GameObject::Draw();
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Gem::End()
{
	GameObject::End();
}
