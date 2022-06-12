//*****************************************************************************
// Teleporter.cpp
// テレポートの管理
// Author : Mane
//*****************************************************************************
#include "Teleporter.h"
#include "TextureManager.h"
#include "Camera.h"
#include "Player.h"
#include "SceneInGame.h"
#include "SceneMainMenu.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PRINT_HITBOX false

//*****************************************************************************
// コンストラクタ変数
//*****************************************************************************
Teleporter::Teleporter(XMFLOAT2 TeleporterPos, XMFLOAT2 NewPlayerPos) : GameObject()
{
	bIsEndpoint = false;
	bSwitchToResultScreen = false;
	nFrameCounterBeforeChangingStates = 0;
	Init(TeleporterPos, NewPlayerPos);
	SetTeleporterID(STATIC_ID);
}

Teleporter::Teleporter(XMFLOAT2 TeleporterPos, XMFLOAT2 NewPlayerPos, int nID): GameObject()
{
	bIsEndpoint = false;
	bSwitchToResultScreen = false;
	nFrameCounterBeforeChangingStates = 0;
	Init(TeleporterPos, NewPlayerPos);
	SetTeleporterID(nID);
}


Teleporter::~Teleporter()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化の関数
//引数：int
//戻：void
//*****************************************************************************
void Teleporter::Init(XMFLOAT2 TeleporterPos, XMFLOAT2 NewPlayerPos)
{
	InitSprite();
	nType = TELEPORTER_TYPE;
	bIsInUse = true;
	gpTexture = GetTexture(TEXTURE_TELEPORTER);
	nFrame_Y = 15;
	nFrame_X = 1;
	uv = { 0,0 };
	xm2Teleporter_NewPlayerPos = NewPlayerPos;
	SetLocalPosition(TeleporterPos.x, TeleporterPos.y);
	x2GOInitialPos = TeleporterPos;
	SetSpriteSize(TELEPORTER_SIZE, TELEPORTER_SIZE);
	SetAlpha(0.75f);
	bMovementInProgress = false;
	hitbox = { 0,-TELEPORTER_SIZE * 0.015f,TELEPORTER_SIZE*0.5f,TELEPORTER_SIZE*0.40f, PRINT_HITBOX, nullptr };
}

//*****************************************************************************
//Update関数
//処理のアップデートの関数
//引数：void
//戻：void
//*****************************************************************************
void Teleporter::Update()
{
	GameObject::Update();
	if (!bIsInUse)
		return;
	Player* pPlayer = GetPlayerPointer();
	Weapon* pWeapon = pPlayer->GetCurrentWeapon();

	if (++n_uvTimer > 1)
	{
		n_uvTimer = 0;
		uv.V += fMirrorAnimation;
		if (uv.V == 0 || uv.V == nFrame_Y - 1)
			fMirrorAnimation *= -1;
	}
	if (bMovementInProgress) {
		Rotation.z += 20;
		if (pPlayer->GetPlayerState() != PLAYER_TELEPORTING)
		{
			bMovementInProgress = false;
			Rotation.z = 0;
		}
	}
	else {
		if (pWeapon && pPlayer)
		{
			if (pWeapon->IsWeaponAttacking() && IsInCollision(pWeapon->GetHitbox(), GetHitbox()) && pPlayer->GetPlayerState() != PLAYER_TELEPORTING)
			{
				pPlayer->SetPlayerTeleporting(xm2Teleporter_NewPlayerPos);
				bMovementInProgress = true;
				if (bIsEndpoint)
				{
					SetFocalPoint(this);
					if (!bSwitchToResultScreen) {//クリアの時、スコアを保存する
						SceneInGame* pGame = GetCurrentGame();
						if (!pGame)
							return;
						AddScoreToTopRankings(pGame->GetScore(), pGame->GetCurrentLevel());
						bSwitchToResultScreen = true;
					}
				}
			}
		}
	}
	if (bSwitchToResultScreen)
	{
		if (++nFrameCounterBeforeChangingStates > 120)
		{
			UIManager::ActivateFinalResults();
		}
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Teleporter::Draw()
{
	GameObject::Draw();
}

//*****************************************************************************
//End関数
//終了の関数
//引数：void
//戻：void
//*****************************************************************************
void Teleporter::End()
{
	GameObject::End();
}

//*****************************************************************************
//SetTeleporterID関数
//テレポートのオブジェクトのIDを設定する
//引数：int
//戻：void
//*****************************************************************************
void Teleporter::SetTeleporterID(int newID)
{
	nTeleporterID = newID;
}

//*****************************************************************************
//SetTeleporterAsEndpoint関数
//テレポートをステージのゴールとしてを設定する
//引数：void
//戻：void
//*****************************************************************************
void Teleporter::SetTeleporterAsEndpoint()
{
	gpTexture = GetTexture(TEXTURE_ENDPOINT_TELEPORTER);
	bIsEndpoint = true;
}

//*****************************************************************************
//GetTeleporterID関数
//テレポートのオブジェクトのIDを戻す
//引数：void
//戻：int
//*****************************************************************************
int Teleporter::GetTeleporterID()
{
	return nTeleporterID;
}

//*****************************************************************************
//IsEndPoint関数
//テレポートはゴールを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Teleporter::IsEndPoint()
{
	return bIsEndpoint;
}

//*****************************************************************************
//SetTeleporterDestination関数
//テレポートのファイナル拠点を設定する
//引数：XMFLOAT2
//戻：void
//*****************************************************************************
void Teleporter::SetTeleporterDestination(XMFLOAT2 dest)
{
	xm2Teleporter_NewPlayerPos = dest;
}

//*****************************************************************************
//GetTeleporterDestination関数
//テレポートのファイナル拠点を戻す
//引数：void 
//戻：XMFLOAT2
//*****************************************************************************
XMFLOAT2 Teleporter::GetTeleporterDestination()
{
	return xm2Teleporter_NewPlayerPos;
}