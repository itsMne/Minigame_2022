//*****************************************************************************
// Spike.cpp
// スパイクブジェクトの管理
// Author : Mane
//*****************************************************************************
#include "Spike.h"
#include "TextureManager.h"
#include "Player.h"
#include "sound.h"

//*****************************************************************************
// マクロ
//*****************************************************************************
#define PRINT_HITBOX false

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
Spike::Spike(XMFLOAT2 Pos, int SpikeNumber)
{
	Init(Pos, SpikeNumber);
}


Spike::~Spike()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化の関数
//引数：XMFLOAT2, int
//戻：void
//*****************************************************************************
void Spike::Init(XMFLOAT2 Pos, int SpikeNumber)
{
	InitSprite();
	nSpikeNumber = SpikeNumber;
	bIsInUse = true;
	gpTexture = GetTexture(TEXTURE_SPIKE);
	SetLocalPosition(Pos.x, Pos.y);
	x2GOInitialPos = Pos;
	SetSpriteSize((float)SPIKE_SIZE* nSpikeNumber, (float)SPIKE_SIZE_TALL);
	hitbox = { 0,0,Size.x,Size.y, PRINT_HITBOX, nullptr };
	fXTileSize = 85;
	nFrame_Y = 1;
	nFrame_X = 1;
	nType = SPIKE_TYPE;
	bRepeatableUV_X = true;
}

//*****************************************************************************
//GetSpikeNumber関数
//スパイクの数を戻す
//引数：void
//戻：int
//*****************************************************************************
int Spike::GetSpikeNumber()
{
	return nSpikeNumber;
}

//*****************************************************************************
//Update関数
//処理のアップデートの関数
//引数：void
//戻：void
//*****************************************************************************
void Spike::Update()
{
	GameObject::Update();
	//スパイクのオブジェクトの管理
	bRepeatableUV_X = true;
	uv = { 0,0 };
	if (hitbox.w == 0 && hitbox.h == 0)
		return;
	Player* pPlayer = GetPlayerPointer();
	if (pPlayer)
	{
		if (IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), GetHitbox()) && pPlayer->GetPlayerState() != PLAYER_TELEPORTING && pPlayer->GetPlayerState() != PLAYER_ATTACKED_STATE)
		{
			pPlayer->SetTeleportingDamageAfterJumping(1);
			PlaySound(SOUND_LABEL_SE_PLAYER_DAMAGED);
		}
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Spike::Draw()
{
	GameObject::Draw();
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Spike::End()
{
	GameObject::End();
}
