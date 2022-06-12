//*****************************************************************************
// PunchStar.cpp
// ����鐯�̊Ǘ�
// Author : Mane
//*****************************************************************************
#include "PunchStar.h"
#include "Player.h"
#include "Camera.h"
#include "sound.h"
#include "TextureManager.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define PRINT_HITBOX false

//*****************************************************************************
// �R���X�g���N�^�ϐ�
//*****************************************************************************
PunchStar::PunchStar(XMFLOAT2 InitPos, XMFLOAT2 FinalPos) : GameObject()
{
	nWaitingFrames = 0;
	xm2StarMoveableInitialPos = { 0,0 };
	xm2StarMoveableFinalPos = { 0,0 };
	nFrameCounterMoving = 0;
	Init(InitPos, FinalPos, STATIC_ID);
}

PunchStar::PunchStar(XMFLOAT2 InitPos, XMFLOAT2 FinalPos, int nID) : GameObject()
{
	nWaitingFrames = 0;
	xm2StarMoveableInitialPos = { 0,0 };
	xm2StarMoveableFinalPos = { 0,0 };
	nFrameCounterMoving = 0;
	Init(InitPos, FinalPos, nID);
}

PunchStar::~PunchStar()
{
	End();
}

//*****************************************************************************
//Init�֐�
//�������̊֐�
//�����Fint
//�߁Fvoid
//*****************************************************************************
void PunchStar::Init(XMFLOAT2 InitPos, XMFLOAT2 FinalPos, int nID)
{
	InitSprite();
	bIsInUse = true;
	nStarID = nID;
	nType = PUNCHABLE_STAR_TYPE;
	gpTexture = GetTexture(TEXTURE_PUNCHABLE_STAR);
	nFrame_X = 3;
	nFrame_Y = 5;
	uv = { 0,0 };
	SetSpriteSize(STAR_SIZE, STAR_SIZE);
	hitbox = { 0,0,STAR_SIZE,STAR_SIZE, PRINT_HITBOX, nullptr };
	SetLocalPosition(InitPos.x, InitPos.y);
	xm2StarMoveableInitialPos = InitPos;
	xm2StarMoveableFinalPos = FinalPos;
}

//*****************************************************************************
//Update�֐�
//�����̃A�b�v�f�[�g�̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void PunchStar::Update()
{
	GameObject::Update();
	if (!bIsInUse)
		return;
	Player* pPlayer = GetPlayerPointer();
	Weapon* pWeapon = pPlayer->GetCurrentWeapon();
	if (!pPlayer || !pWeapon)
		return;

	if (bMovementInProgress) {
		if (++n_uvTimer > 2)
		{
			n_uvTimer = 0;
			uv.U++;
			if (uv.U == nFrame_X - 1)
			{
				uv.U = 0;
				uv.V++;
				if (uv.V == nFrame_Y - 1) {
					uv.V = 0;
				}
			}
		}
	}
	XMFLOAT3 xm2PlayerTranslation = { 0,0,0 };
	int nCurrentAttack = pWeapon->GetCurrentAttackPlaying();
	if (pWeapon->IsWeaponAttacking() && IsInCollision(pWeapon->GetHitbox(), GetHitbox()) && nFrameCounterMoving <= 0 && pPlayer->GetPlayerState() != PLAYER_TELEPORTING)
	{
		SetZoomOffset(1.115f, 0.0095f, 20);
		if (nCurrentAttack == HAND_DOWNSTOGROUND_STATE) {
			PlaySound(SOUND_LABEL_SE_STAR_HIT);
			nFrameCounterMoving = 6;
		}
		else {
			if (nCurrentAttack == DOWN_DASH_ATTACK || nCurrentAttack == HAND_AIR_DASH) {
				pWeapon->ResetWeaponState();
				pPlayer->SetLockInputs(false);
				pPlayer->SetPlayerState(PLAYER_IDLE_STATE);
			}
			XMFLOAT3 StarPos = Position;
			StarPos.x -= 175 * pPlayer->GetDirection();
			pPlayer->SetWorldPosition(StarPos);
			PlaySound(SOUND_LABEL_SE_STAR_HIT);
			nFrameCounterMoving = 12;
		}
	}

	if (--nFrameCounterMoving > 0)
	{
		fAcceleration++;
		if (fAcceleration > 25.5f)
			fAcceleration = 25.5f;
		bMovementInProgress = true;
		if (Position.x < xm2StarMoveableFinalPos.x)
		{
			Position.x += fAcceleration;
			if (Position.x > xm2StarMoveableFinalPos.x)
				Position.x = xm2StarMoveableFinalPos.x;
			else
				xm2PlayerTranslation.x += fAcceleration;
		}
		if (Position.x > xm2StarMoveableFinalPos.x)
		{
			Position.x -= fAcceleration;
			if (Position.x < xm2StarMoveableFinalPos.x)
				Position.x = xm2StarMoveableFinalPos.x;
			else
				xm2PlayerTranslation.x -= fAcceleration;
		}

		if (Position.y < xm2StarMoveableFinalPos.y)
		{
			Position.y += fAcceleration;
			if (Position.y > xm2StarMoveableFinalPos.y)
				Position.y = xm2StarMoveableFinalPos.y;
			else
				xm2PlayerTranslation.y += fAcceleration;
		}
		if (Position.y > xm2StarMoveableFinalPos.y)
		{
			Position.y -= fAcceleration;
			if (Position.y < xm2StarMoveableFinalPos.y)
				Position.y = xm2StarMoveableFinalPos.y;
			else
				xm2PlayerTranslation.y -= fAcceleration;
		}
		if (nCurrentAttack != HAND_DOWNSTOGROUND_STATE) {
			pPlayer->SetPlayerTranslation(xm2PlayerTranslation.x, xm2PlayerTranslation.y);
			pPlayer->SetGravityCancelFrames(23);
		}
		nWaitingFrames = 120;
	}
	else {
		if (--nWaitingFrames <= 0) {
			if (Position.x == xm2StarMoveableInitialPos.x && Position.y == xm2StarMoveableInitialPos.y)
				fAcceleration = 0;
			fAcceleration++;
			if (fAcceleration > 35.5f)
				fAcceleration = 35.5f;
			bMovementInProgress = false;

			if (Position.x < xm2StarMoveableInitialPos.x)
			{
				Position.x += fAcceleration;
				if (Position.x > xm2StarMoveableInitialPos.x)
					Position.x = xm2StarMoveableInitialPos.x;
			}
			if (Position.x > xm2StarMoveableInitialPos.x)
			{
				Position.x -= fAcceleration;
				if (Position.x < xm2StarMoveableInitialPos.x)
					Position.x = xm2StarMoveableInitialPos.x;
			}

			if (Position.y < xm2StarMoveableInitialPos.y)
			{
				Position.y += fAcceleration;
				if (Position.y > xm2StarMoveableInitialPos.y)
					Position.y = xm2StarMoveableInitialPos.y;
			}
			if (Position.y > xm2StarMoveableInitialPos.y)
			{
				Position.y -= fAcceleration;
				if (Position.y < xm2StarMoveableInitialPos.y)
					Position.y = xm2StarMoveableInitialPos.y;
			}
		}
	}
}

//*****************************************************************************
//GetStarID�֐�
//���̃I�u�W�F�N�g��ID��߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int PunchStar::GetStarID()
{
	return nStarID;
}

//*****************************************************************************
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void PunchStar::Draw()
{
	GameObject::Draw();
}

//*****************************************************************************
//End�֐�
//�I���̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void PunchStar::End()
{
	GameObject::End();
}

//*****************************************************************************
//SetStarDestination�֐�
//���̃t�@�C�i�����_��ݒ肷��
//�����FXMFLOAT2
//�߁Fvoid
//*****************************************************************************
void PunchStar::SetStarDestination(XMFLOAT2 dest)
{
	xm2StarMoveableFinalPos = dest;
}

//*****************************************************************************
//GetStarInitialPos�֐�
//���̍ŏ����_��߂�
//�����Fvoid
//�߁FXMFLOAT2
//*****************************************************************************
XMFLOAT2 PunchStar::GetStarInitialPos()
{
	return xm2StarMoveableInitialPos;
}

//*****************************************************************************
//GetStarFinalPos�֐�
//���̃t�@�C�i�����_��߂�
//�����Fvoid
//�߁FXMFLOAT2
//*****************************************************************************
XMFLOAT2 PunchStar::GetStarFinalPos()
{
	return xm2StarMoveableFinalPos;
}