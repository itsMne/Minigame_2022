//*****************************************************************************
// RecoveryItem.cpp
// 回復アイテムブジェクトの管理
// Author : Mane
//*****************************************************************************
#include "RecoveryItem.h"
#include "TextureManager.h"
#include "Player.h"
#include "SceneInGame.h"
#include "Sound.h"

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
RecoveryItem::RecoveryItem(XMFLOAT2 Position) : GameObject()
{
	Init(Position);
}


RecoveryItem::~RecoveryItem()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化の関数
//引数：int, HitBox, float, int
//戻：void
//*****************************************************************************
void RecoveryItem::Init(XMFLOAT2 Position)
{
	InitSprite();
	SetType(RECOVER_TYPE);
	SetUse(true);
	gpTexture = GetTexture(TEXTURE_SOBA);
	SetSpriteSize(125, 82);
	SetWorldPosition(Position.x, Position.y);
	x2GOInitialPos = Position;
	hitbox = { 0,0,125,82 };
	fMirrorAnimation = 0.25f;
}

//*****************************************************************************
//Update関数
//処理のアップデートの関数
//引数：void
//戻：void
//*****************************************************************************
void RecoveryItem::Update()
{
	//回復のアイテム
	GameObject::Update();
	if (!bIsInUse)
		return;
	Player* pPlayer = GetPlayerPointer();
	SceneInGame* pGame = GetCurrentGame();
	Size.x += fMirrorAnimation;
	Size.y += fMirrorAnimation;
	if (Size.x > 125 + 5 || Size.x < 125 - 5)
		fMirrorAnimation *= -1;
	if (pPlayer)
	{
		if (IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), GetHitbox()) && pPlayer->GetPlayerState() != PLAYER_TELEPORTING)
		{
			pPlayer->RecoverHP(1);
			if (!pGame)
				return;
			pGame->AddToScore(30);
			bIsInUse = false;
			PlaySound(SOUND_LABEL_RECOVERED);
		}
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void RecoveryItem::Draw()
{
	GameObject::Draw();
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void RecoveryItem::End()
{
	GameObject::End();
}
