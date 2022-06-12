//*****************************************************************************
// Weapon.cpp
// 床を管理する
// Author : Mane
//*****************************************************************************
#include "Floor.h"
#include "Bullet.h"
#include "TextureManager.h"
#include "UniversalVariables.h"
#include "Camera.h"
#include "Weapon.h"
#include "Sound.h"
#include "Player.h"

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
Floor::Floor(): GameObject()
{
	ResetSwitch = nullptr;
	MoveForwardSwitch = nullptr;
	fMovingFloorAcceleration = 0;
	bControllableMoveFloor = false;
	InitSprite();
	nType = FLOOR_TYPE;
	nFloorID = STATIC_ID;
}

Floor::Floor(int FloorTextureID, HitBox hbox, float TileSize, int nID) : GameObject()
{
	ResetSwitch = nullptr;
	MoveForwardSwitch = nullptr;
	fMovingFloorAcceleration = 0;
	bControllableMoveFloor = false;
	InitSprite();
	Init(FloorTextureID, hbox, TileSize, nID);
}


Floor::~Floor()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化の関数
//引数：int, HitBox, float, int
//戻：void
//*****************************************************************************
void Floor::Init(int FloorTextureID, HitBox hbox, float TileSize, int nID)
{
	nFloorTexture = FloorTextureID;
	SetTexture(GetTexture(FloorTextureID));
	nType = FLOOR_TYPE;
	hitbox = hbox;
	fXTileSize = TileSize;
	hitbox.print = SHOW_HITBOXES;
	bIsInUse = true;
	nFloorID = nID;
	bRepeatableUV_X = true;
	bSafeFloor = true;
}

//*****************************************************************************
//Update関数
//処理のアップデートの関数
//引数：void
//戻：void
//*****************************************************************************
void Floor::Update()
{
	if (!bIsInUse)
		return;
	GameObject::Update();
	uv.U = 1;
	if (bControllableMoveFloor)
	{
		bIsMoveAble = false;//床は動けるなら、自動の動く関数を使うことは要りません
		if (ResetSwitch && MoveForwardSwitch)
		{
			Player* pPlayer = GetPlayerPointer();
			if (pPlayer->GetParent() != this) {
				if (pPlayer->GetFloor() == this) {
					pPlayer->SetParent(this);
				}
			}
			else
			{
				if (pPlayer->GetWorldPosition().x < this->GetWorldPosition().x - this->GetHitbox().w / 2
					|| pPlayer->GetWorldPosition().x > this->GetWorldPosition().x + this->GetHitbox().w / 2)
				{
					pPlayer->RemoveParent();
				}
			}
			XMFLOAT3 FloorPos = this->GetLocalPosition();
			if (ResetSwitch->IsSwitchOn())
			{
				fMovingFloorAcceleration++;
				if (FloorPos.x != xm2InitialMovingPos.x)
				{
					if (FloorPos.x < xm2InitialMovingPos.x) {
						FloorPos.x += fMovingFloorAcceleration;
						if (FloorPos.x > xm2InitialMovingPos.x)
							FloorPos.x = xm2InitialMovingPos.x;
					}
					else if (FloorPos.x > xm2InitialMovingPos.x) {
						FloorPos.x -= fMovingFloorAcceleration;
						if (FloorPos.x < xm2InitialMovingPos.x)
							FloorPos.x = xm2InitialMovingPos.x;
					}
				}
				if (FloorPos.y != xm2InitialMovingPos.y)
				{
					if (FloorPos.y < xm2InitialMovingPos.y) {
						FloorPos.y += fMovingFloorAcceleration;
						if (FloorPos.y > xm2InitialMovingPos.y)
							FloorPos.y = xm2InitialMovingPos.y;
					}
					else if (FloorPos.y > xm2InitialMovingPos.y) {
						FloorPos.y -= fMovingFloorAcceleration;
						if (FloorPos.y < xm2InitialMovingPos.y)
							FloorPos.y = xm2InitialMovingPos.y;
					}
				}
				if (FloorPos.y == xm2InitialMovingPos.y && FloorPos.x == xm2InitialMovingPos.x) {
					fMovingFloorAcceleration = 0;
					ResetSwitch->SetSwitchOn(false);
				}
				this->SetLocalPosition(FloorPos.x, FloorPos.y);
			}
			if (MoveForwardSwitch->IsSwitchOn())
			{
				static int nSEFrameCoolDown = 0;
				Bullet** goBullets = GetBullets();
				nSEFrameCoolDown--;
				if (goBullets)
				{
					for (int i = 0; i < MAX_BULLETS; i++)
					{
						if (!goBullets[i])
							continue;
						if (IsInCollision(goBullets[i]->GetHitbox(), MoveForwardSwitch->GetHitbox()))
						{
							goBullets[i]->SetUse(false);
							if (nSEFrameCoolDown <= 0) {
								PlaySound(SOUND_LABEL_SE_CHARGE);
								nSEFrameCoolDown = 5;
							}
							fMovingFloorAcceleration = 30.0f;
						}
					}
				}
				XMFLOAT2 fSpeedMove = xm2MovingPlatformSpeed;
				if (FloorPos.x != xm2MoveableFinalPos.x)
				{
					if (FloorPos.x < xm2MoveableFinalPos.x) {
						FloorPos.x += fSpeedMove.x;
						if (FloorPos.x > xm2MoveableFinalPos.x)
							FloorPos.x = xm2MoveableFinalPos.x;
					}
					else if (FloorPos.x > xm2MoveableFinalPos.x) {
						FloorPos.x -= fSpeedMove.x;
						if (FloorPos.x < xm2MoveableFinalPos.x)
							FloorPos.x = xm2MoveableFinalPos.x;
					}
				}
				if (FloorPos.y != xm2MoveableFinalPos.y)
				{
					if (FloorPos.y < xm2MoveableFinalPos.y) {
						FloorPos.y += fSpeedMove.y;
						if (FloorPos.y > xm2MoveableFinalPos.y)
							FloorPos.y = xm2MoveableFinalPos.y;
					}
					else if (FloorPos.y > xm2MoveableFinalPos.y) {
						FloorPos.y -= fSpeedMove.y;
						if (FloorPos.y < xm2MoveableFinalPos.y)
							FloorPos.y = xm2MoveableFinalPos.y;
					}
				}
				if (FloorPos.y == xm2MoveableFinalPos.y && FloorPos.x == xm2MoveableFinalPos.x)
					MoveForwardSwitch->SetSwitchOn(false);
				else
					SetFocalPoint(this);
				this->SetLocalPosition(FloorPos.x, FloorPos.y);
				fMovingFloorAcceleration--;
				if (fMovingFloorAcceleration <= 0)
				{
					fMovingFloorAcceleration = 0;
					MoveForwardSwitch->SetSwitchOn(false);
				}

			}
			else
			{
				if (GetFocalPoint() == this)
					SetFocalPoint(GetPlayerPointer());
			}
		}
		MoveForwardSwitch->Update();
		ResetSwitch->Update();
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Floor::Draw()
{
	GameObject::Draw();
	if (ResetSwitch)
		ResetSwitch->Draw();
	if (MoveForwardSwitch)
		MoveForwardSwitch->Draw();
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Floor::End()
{
	GameObject::End();
	SAFE_DELETE(ResetSwitch);
	SAFE_DELETE(MoveForwardSwitch);
}

//*****************************************************************************
//SetFloorAsSafe関数
//安全な床を設定する
//引数：bool
//戻：void
//*****************************************************************************
void Floor::SetFloorAsSafe(bool bIsSafe)
{
	bSafeFloor = bIsSafe;
}

//*****************************************************************************
//IsSafeFloor関数
//安全な床を確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Floor::IsSafeFloor()
{
	return bSafeFloor;
}

//*****************************************************************************
//GetFloorID関数
//床のIDを確認する
//引数：void
//戻：int
//*****************************************************************************
int Floor::GetFloorID()
{
	return nFloorID;
}

//*****************************************************************************
//SetUpMoveFloorControlledBySwitch関数
//床の動き方を設定する
//引数：XMFLOAT2, XMFLOAT2, XMFLOAT2, XMFLOAT2
//戻：void
//*****************************************************************************
void Floor::SetUpMoveFloorControlledBySwitch(XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, XMFLOAT2 ResetSwitchPosition)
{
	if (!ResetSwitch)
	{
		ResetSwitch = new SwitchGO({ 0,0 }, STATIC_ID, true);
		ResetSwitch->SetWorldPosition(ResetSwitchPosition.x, ResetSwitchPosition.y);
	}
	if (!MoveForwardSwitch)
	{
		MoveForwardSwitch = new SwitchGO({ 0,0 }, STATIC_ID, false);
		MoveForwardSwitch->SetParent(this, false);
		MoveForwardSwitch->SetSwitchResistance(1);
		while (IsInCollision(MoveForwardSwitch->GetHitbox(), GetHitbox())) {
			XMFLOAT3 LocalPosSwitch = MoveForwardSwitch->GetLocalPosition();
			MoveForwardSwitch->SetLocalPosition(LocalPosSwitch.x, LocalPosSwitch.y + 1);
		}
	}
	fMovingFloorAcceleration = 0;
	bControllableMoveFloor = true;
	xm2MovingPlatformSpeed = MovingSpeed;
	xm2InitialMovingPos = InitialPos;
	xm2MoveableFinalPos = FinalPos;
	SetFloorAsSafe(false);
}

//*****************************************************************************
//GetFloorTexture関数
//床のテクスチャを戻す
//引数：void
//戻：int
//*****************************************************************************
int Floor::GetFloorTexture()
{
	return nFloorTexture;
}

//*****************************************************************************
//GetResetSwitchForMoveablePlatform関数
//動ける床のスイッチのアドレスを戻す
//引数：void
//戻：SwitchGO*
//*****************************************************************************
SwitchGO* Floor::GetResetSwitchForMoveablePlatform()
{
	return ResetSwitch;
}

//*****************************************************************************
//GetControlSwitchForMoveablePlatform関数
//動ける床の呼ぶスイッチのアドレスを戻す
//引数：void
//戻：SwitchGO*
//*****************************************************************************
SwitchGO* Floor::GetControlSwitchForMoveablePlatform()
{
	return MoveForwardSwitch;
}
