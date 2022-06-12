//*****************************************************************************
// Wall.cpp
// 壁と壁の当たり判定を管理する
// Author : Mane
//*****************************************************************************
#include "Wall.h"
#include "Player.h"
#include "TextureManager.h"

//*****************************************************************************
// コンストラクタ変数
//*****************************************************************************
Wall::Wall(int TextureID, int n_Type, HitBox hbox, int WallTileSize, int nActivation) : GameObject()
{
	nWallID = STATIC_ID;
	nWallScaleActivation = nActivation;
	nWallTexture = TextureID;
	Init(TextureID, n_Type, hbox, WallTileSize);
	switch (nWallScaleActivation)
	{
	case WALL_SCALE_POSITIVE:
		SetWallDeactivateableWhenScaleIsNegative(true);
		break;
	case WALL_SCALE_NEGATIVE:
		SetWallDeactivateableWhenScaleIsPositive(true);
		break;
	default:
		break;
	}
}


Wall::~Wall()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void Wall::Init(int TextureID, int n_Type, HitBox hbox, int WallTileSize)
{
	GameObject::Init(GetTexture(TextureID), n_Type);
	hitbox = hbox;
	hitbox.print = SHOW_HITBOXES;
	fYTileSize = WallTileSize;
	bIsInUse = true;
	bRepeatableUV_Y = true;
	Parent = nullptr;
	bDeactivateWhenScaleIsNegative = false;
	bDeactivateWhenScaleIsPositive = false;
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void Wall::Update()
{
	if (!bIsInUse)
		return;
	GameObject::Update();
	uv.U = 1;
	Player* pPlayer = GetPlayerPointer();
	if (bDeactivateWhenScaleIsNegative)
	{
		if (GetUniversalScale() < 0)
		{
			hitbox = { 0,0,0,0 };
			SetAlpha(0.35f);
		}
		else if (GetUniversalScale() > 0)
		{
			hitbox = hbOriginalHitbox;
			SetAlpha(1);
		}
	}
	if (bDeactivateWhenScaleIsPositive)
	{
		if (GetUniversalScale() > 0)
		{
			hitbox = { 0,0,0,0 };
			SetAlpha(0.35f);
		}
		else if (GetUniversalScale() < 0)
		{
			hitbox = hbOriginalHitbox;
			SetAlpha(1);
		}
	}
	if (!pPlayer)
		return;
	if (pPlayer->GetPlayerState() == PLAYER_TELEPORTING)
		return;
	int nPlayerDirection = pPlayer->GetDirection();
	while (IsInCollision(pPlayer->GetPlayerHitbox(HB_LEFT_LIMIT), GetHitbox()))
	{
		XMFLOAT3 x3PlayerPos = pPlayer->GetWorldPosition();
		x3PlayerPos.x++;
		pPlayer->SetWorldPosition(x3PlayerPos);
	}
	while (IsInCollision(pPlayer->GetPlayerHitbox(HB_RIGHT_LIMIT), GetHitbox()))
	{
		XMFLOAT3 x3PlayerPos = pPlayer->GetWorldPosition();
		x3PlayerPos.x--;
		pPlayer->SetWorldPosition(x3PlayerPos);
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Wall::Draw()
{
	GameObject::Draw();
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Wall::End()
{
	GameObject::End();
}

//*****************************************************************************
//SetWallID関数
//壁のIDを設定する
//引数：int
//戻：void
//*****************************************************************************
void Wall::SetWallID(int newID)
{
	nWallID = newID;
}

//*****************************************************************************
//SetWallDeactivateableWhenScaleIsNegative関数
//壁はスケールが普通で頼っていることを設定する
//引数：bool
//戻：void
//*****************************************************************************
void Wall::SetWallDeactivateableWhenScaleIsNegative(bool deact)
{
	bDeactivateWhenScaleIsNegative = deact;
	bDeactivateWhenScaleIsPositive = !deact;
	hbOriginalHitbox = hitbox;
}

//*****************************************************************************
//SetWallDeactivateableWhenScaleIsPositive関数
//壁はスケールが逆さで頼っていることを設定する
//引数：bool
//戻：void
//*****************************************************************************
void Wall::SetWallDeactivateableWhenScaleIsPositive(bool act)
{
	bDeactivateWhenScaleIsPositive = act;
	bDeactivateWhenScaleIsNegative = !act;
	hbOriginalHitbox = hitbox;
}

//*****************************************************************************
//GetWallID関数
//IDを戻す
//引数：void
//戻：int
//*****************************************************************************
int Wall::GetWallID()
{
	return nWallID;
}

//*****************************************************************************
//GetWallTexture関数
//テクスチャを戻す
//引数：void
//戻：int
//*****************************************************************************
int Wall::GetWallTexture()
{
	return nWallTexture;
}

//*****************************************************************************
//GetScaleActivation関数
//壁の種類を戻す
//引数：void
//戻：int
//*****************************************************************************
int Wall::GetTypeScaleActivation()
{
	return nWallScaleActivation;
}
