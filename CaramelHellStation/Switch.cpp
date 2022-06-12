//*****************************************************************************
// Switch.cpp
// �X�C�b�`�I�u�W�F�N�g�̊Ǘ�
// Author : Mane
//*****************************************************************************
#include "Switch.h"
#include "TextureManager.h"
#include "sound.h"
#include "Camera.h"
#include "Bullet.h"
#include "SceneInGame.h"
#include "GameInputManager.h"

//*****************************************************************************
// �}�N��
//*****************************************************************************
#define PRINT_HITBOX false
#define SWITCH_RESISTANCE 15.0f

//*****************************************************************************
// �R���X�g���N�^�֐�
//*****************************************************************************
SwitchGO::SwitchGO(XMFLOAT2 xPosition, int nID, bool bIsPunchable)
{
	bIsScaleInverterSwitch = false;
	Init(xPosition, nID, bIsPunchable);
}


SwitchGO::~SwitchGO()
{
	End();
}

//*****************************************************************************
//Init�֐�
//�������̊֐�
//�����FXMFLOAT2, int, bool
//�߁Fvoid
//*****************************************************************************
void SwitchGO::Init(XMFLOAT2 xPosition, int nID, bool bIsPunchable)
{
	InitSprite();
	bIsInUse = true;
	nSwitchID = nID;
	nType = BULB_SWITCH_TYPE;
	fAcceleration = 0;
	bPunchableSwitch = bIsPunchable;
	if (!bIsPunchable) {
		gpTexture = GetTexture(TEXTURE_SWITCH_SHOOT);
		hitbox = { 0,-324 / 3 + 40,125,324 / 2, PRINT_HITBOX, nullptr };
	}
	else {
		gpTexture = GetTexture(TEXTURE_SWITCH_PUNCH);
		hitbox = { 0,-324 / 2 + 130,125,324 / 2.7f, PRINT_HITBOX, nullptr };
	}
	nFrame_X = 5;
	nFrame_Y = 5;
	uv = { 0,0 };
	SetLocalPosition(xPosition.x, xPosition.y);
	x2GOInitialPos = xPosition;
	SetSpriteSize(160, 324);
	fMaxResistanceSwitch = fSwitchHitsForActivation = SWITCH_RESISTANCE;
	bSwitchisOn = false;
}

//*****************************************************************************
//Update�֐�
//�����̃A�b�v�f�[�g�̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void SwitchGO::Update()
{
	if (!bIsInUse)
		return;
	GameObject::Update();
	Bullet ** Bullets = GetBullets();
	Weapon * CurrentWeapon = GetPlayerPointer()->GetCurrentWeapon();
	if (!Bullets)return;
	if (bSwitchisOn)
	{
		uv.U++;
		if (uv.U == nFrame_X)
		{
			uv.U = 0;
			uv.V++;
			if (uv.V == nFrame_Y)
				uv.V = 0;
		}
		if (uv.U == 0 && uv.V == 0)
			uv.U = 1;
	}
	else {
		if (!bPunchableSwitch) {
			static int nSEFrameCoolDown = 0;
			fAcceleration += 0.0125f;
			for (int i = 0; i < MAX_BULLETS; i++)
			{
				if (!Bullets[i])continue;
				if (!(Bullets[i]->IsInUse()))continue;
				if (IsInCollision(Bullets[i]->GetHitbox(), GetHitbox()))
				{
					Bullets[i]->SetUse(false);
					if (--nSEFrameCoolDown <= 0) {
						PlaySound(SOUND_LABEL_SE_CHARGE);
						nSEFrameCoolDown = 1;
					}
					fAcceleration = 0;
					if (fSwitchHitsForActivation <= 0) {
						bSwitchisOn = true;
						fSwitchHitsForActivation = 0;
						uv.U = 1;
					}
					else
						fSwitchHitsForActivation--;
				}
			}
			fSwitchHitsForActivation += fAcceleration;
			if (fSwitchHitsForActivation > fMaxResistanceSwitch) {
				fSwitchHitsForActivation = fMaxResistanceSwitch;
				fAcceleration = 0;
			}
			SetSpriteSize(160, 324);
		}
		else {
			if (CurrentWeapon)
			{
				if (CurrentWeapon->IsWeaponAttacking() && IsInCollision(CurrentWeapon->GetHitbox(), GetHitbox()))
				{
					bSwitchisOn = true;
					SetZoomOffset(1.225f, 0.0095f, 15);
					PlaySound(SOUND_LABEL_SE_SWITCH_HIT);
					VibrateXinput(65535 / 2, 65535 / 2, 35);
				}
			}
			Bullets = GetBullets();
			for (int i = 0; i < MAX_BULLETS; i++)
			{
				
				if (!Bullets[i])
					continue;
				if (!(Bullets[i]->IsInUse()))continue;
				if (IsInCollision(Bullets[i]->GetHitbox(), GetHitbox()))
				{
					Bullets[i]->SetBulletSpeed({ 0, 75 });
				}
			}
		}
	}
	if (bIsScaleInverterSwitch)
	{
		if (bSwitchisOn && !Inverting)
		{
			Inverting = true;
			SceneInGame* pGame = GetCurrentGame();
			if (!pGame)
				return;
			newScale = pGame->GetLevelScale()*(-1);
			fScaleAcceleration = 0;
		}
		if (Inverting)
		{
			fScaleAcceleration += 0.0025f;
			float CurrentScale = GetUniversalScale();
			if (CurrentScale != newScale)
			{
				if (CurrentScale < newScale) {
					CurrentScale += fScaleAcceleration;
					if (CurrentScale >= newScale)
						CurrentScale = newScale;
				}
				if (CurrentScale > newScale) {
					CurrentScale -= fScaleAcceleration;
					if (CurrentScale <= newScale)
						CurrentScale = newScale;
				}
				SetUniversalScale(CurrentScale);
				SceneInGame* pGame = GetCurrentGame();
				if (!pGame)
					return;
				pGame->SetLevelScale(CurrentScale);
			}
			else {
				bSwitchisOn = false;
				Inverting = false;
			}
		}
	}
}

//*****************************************************************************
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void SwitchGO::Draw()
{
	if (!bIsInUse)
		return;
	SetLocalPosition(Position.x, Position.y);
	SetSpriteFrameSize(1.0f / nFrame_X, 1.0f / nFrame_Y);
	SetSpriteUV(uv.U / (float)nFrame_X, uv.V / (float)nFrame_Y);
	if (nType == BULB_SWITCH_TYPE)
	{
		if (!bSwitchisOn) {
			SetAlpha(1);
			SetSpriteUV(1 / (float)nFrame_X, 0 / (float)nFrame_Y);
			DrawSprite(GetDeviceContext());
			SetAlpha(fSwitchHitsForActivation / fMaxResistanceSwitch);
			SetSpriteUV(0 / (float)nFrame_X, 0 / (float)nFrame_Y);
			DrawSprite(GetDeviceContext());
			return;
		}
		else {
			SetAlpha(1);
			SetSpriteUV(uv.U / (float)nFrame_X, uv.V / (float)nFrame_Y);
			DrawSprite(GetDeviceContext());
		}
	}
}

//*****************************************************************************
//SetSwitchResistance�֐�
//�X�C�b�`�̒e�ۑ΂ɒ�R
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void SwitchGO::SetSwitchResistance(float fSwitchmax)
{
	fMaxResistanceSwitch = fSwitchmax;
}

//*****************************************************************************
//SetSwitchOn�֐�
//�X�C�b�`��L������
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void SwitchGO::SetSwitchOn(bool isOn)
{
	bSwitchisOn = isOn;
}

//*****************************************************************************
//SetSwitchAsScaleInverter�֐�
//�X�P�[���𔽑΂��邱��
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void SwitchGO::SetSwitchAsScaleInverter()
{
	bIsScaleInverterSwitch = true;
}

//*****************************************************************************
//GetSwitchID�֐�
//�X�C�b�`��ID��߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int SwitchGO::GetSwitchID()
{
	return nSwitchID;
}

//*****************************************************************************
//IsSwitchOn�֐�
//�X�C�b�`�̂͗L�����m�F����
//�����Fvoid
//�߁Fbool
//*****************************************************************************
bool SwitchGO::IsSwitchOn()
{
	return bSwitchisOn;
}

//*****************************************************************************
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void SwitchGO::End()
{
	GameObject::End();
}
