//*****************************************************************************
// Weapon.cpp
// �����Ǘ�����
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
// �R���X�g���N�^�֐�
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
//Init�֐�
//�������̊֐�
//�����Fint, HitBox, float, int
//�߁Fvoid
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
//Update�֐�
//�����̃A�b�v�f�[�g�̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Floor::Update()
{
	if (!bIsInUse)
		return;
	GameObject::Update();
	uv.U = 1;
	if (bControllableMoveFloor)
	{
		bIsMoveAble = false;//���͓�����Ȃ�A�����̓����֐����g�����Ƃ͗v��܂���
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
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
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
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Floor::End()
{
	GameObject::End();
	SAFE_DELETE(ResetSwitch);
	SAFE_DELETE(MoveForwardSwitch);
}

//*****************************************************************************
//SetFloorAsSafe�֐�
//���S�ȏ���ݒ肷��
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void Floor::SetFloorAsSafe(bool bIsSafe)
{
	bSafeFloor = bIsSafe;
}

//*****************************************************************************
//IsSafeFloor�֐�
//���S�ȏ����m�F����
//�����Fvoid
//�߁Fbool
//*****************************************************************************
bool Floor::IsSafeFloor()
{
	return bSafeFloor;
}

//*****************************************************************************
//GetFloorID�֐�
//����ID���m�F����
//�����Fvoid
//�߁Fint
//*****************************************************************************
int Floor::GetFloorID()
{
	return nFloorID;
}

//*****************************************************************************
//SetUpMoveFloorControlledBySwitch�֐�
//���̓�������ݒ肷��
//�����FXMFLOAT2, XMFLOAT2, XMFLOAT2, XMFLOAT2
//�߁Fvoid
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
//GetFloorTexture�֐�
//���̃e�N�X�`����߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int Floor::GetFloorTexture()
{
	return nFloorTexture;
}

//*****************************************************************************
//GetResetSwitchForMoveablePlatform�֐�
//�����鏰�̃X�C�b�`�̃A�h���X��߂�
//�����Fvoid
//�߁FSwitchGO*
//*****************************************************************************
SwitchGO* Floor::GetResetSwitchForMoveablePlatform()
{
	return ResetSwitch;
}

//*****************************************************************************
//GetControlSwitchForMoveablePlatform�֐�
//�����鏰�̌ĂԃX�C�b�`�̃A�h���X��߂�
//�����Fvoid
//�߁FSwitchGO*
//*****************************************************************************
SwitchGO* Floor::GetControlSwitchForMoveablePlatform()
{
	return MoveForwardSwitch;
}
