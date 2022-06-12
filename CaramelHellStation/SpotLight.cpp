//*****************************************************************************
// SpotLight.cpp
// �v���C���[�̃X�|�b�g���C�g�̊Ǘ�
// Author : Mane
//*****************************************************************************
#include "SpotLight.h"
#include "TextureManager.h"
#include "Player.h"
#include "RankManager.h"
#include "sound.h"

//*****************************************************************************
// �}�N��
//*****************************************************************************
#define SPOTLIGHT_ALPHA 0.5f

//*****************************************************************************
// �R���X�g���N�^�֐�
//*****************************************************************************
SpotLight::SpotLight()
{
	BorderTop = nullptr;
	nSpotlightCoolDownFrames = 0;
	BorderRight = nullptr;
	BorderLeft = nullptr;
	BorderBottom = nullptr;
	BorderTop = nullptr;
	Init();
}

SpotLight::~SpotLight()
{
	End();
}

//*****************************************************************************
//Init�֐�
//�������̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void SpotLight::Init()
{
	InitSprite();
	SetType(SPOTLIGHT_TYPE);
	gpTexture = GetTexture(TEXTURE_SPOTLIGHT);
	nFrame_X = 5;
	nFrame_Y = 6;
	uv = { 0,0 };
	SetSpriteSize(270, 324);
	SetUse(true);
	BorderTop = new Sprite();
	BorderTop->InitSprite();
	BorderTop->SetSpriteSize(WINDOW_WIDTH + 500, WINDOW_HEIGHT / 2);
	BorderTop->gpTexture = GetTexture(TEXTURE_BLACKSPOT);
	BorderBottom = new Sprite();
	BorderBottom->InitSprite();
	BorderBottom->SetSpriteSize(WINDOW_WIDTH + 500, WINDOW_HEIGHT / 2);
	BorderBottom->gpTexture = GetTexture(TEXTURE_BLACKSPOT);

	BorderRight = new Sprite();
	BorderRight->InitSprite();
	BorderRight->SetSpriteSize(WINDOW_WIDTH - (270 * 2), 324);
	BorderRight->gpTexture = GetTexture(TEXTURE_BLACKSPOT);

	BorderLeft = new Sprite();
	BorderLeft->InitSprite();
	BorderLeft->SetSpriteSize(WINDOW_WIDTH - (270 * 2), 324);
	BorderLeft->gpTexture = GetTexture(TEXTURE_BLACKSPOT);
	SetAlpha(0);
	BorderTop->SetAlpha(0);
	BorderBottom->SetAlpha(0);
	BorderLeft->SetAlpha(0);
	BorderRight->SetAlpha(0);
}

//*****************************************************************************
//Update�֐�
//�����̃A�b�v�f�[�g�̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void SpotLight::Update()
{
	if (!BorderTop || !BorderBottom || !BorderLeft || !BorderRight)
		return;
	if (BorderTop->GetParent() == nullptr) {
		Player* pPlayer = GetPlayerPointer();
		if (!pPlayer)
			return;
		//�e�̐ݒ�
		BorderTop->SetParent(pPlayer);
		BorderBottom->SetParent(pPlayer);
		BorderRight->SetParent(pPlayer);
		BorderLeft->SetParent(pPlayer);
		//�X�|�b�g���C�g�̐ݒ�
		SpotLightBordersSetUp();
	}
	float CurrentAlpha = color.w;
	if (CurrentAlpha == 0)
		uv = { 0,0 };
	if (GetRank() == MAX_STYLE_RANK)
	{
		nSpotlightCoolDownFrames = 15;
		CurrentAlpha += 0.01f;
		if (CurrentAlpha > SPOTLIGHT_ALPHA)
			CurrentAlpha = SPOTLIGHT_ALPHA;

	}
	else
	{
		if (--nSpotlightCoolDownFrames <= 0) {
			CurrentAlpha -= 0.005f;
			if (CurrentAlpha < 0)
				CurrentAlpha = 0;
		}
	}
	SetAlpha(CurrentAlpha);
	if ((uv.U != nFrame_X - 1 || uv.V != nFrame_Y - 1) && CurrentAlpha == SPOTLIGHT_ALPHA)
	{
		uv.U++;
		if (uv.U == nFrame_X - 1 && uv.V != nFrame_Y - 1)
		{
			uv.U = 0;
			uv.V++;
		}
		if (uv.U == nFrame_X - 2 && uv.V == nFrame_Y - 1)
			PlaySound(SOUND_LABEL_SE_SPOTLIGHT);
	}
	XMFLOAT3 wPos = GetWorldPosition();

	BorderTop->SetAlpha(CurrentAlpha);
	BorderBottom->SetAlpha(CurrentAlpha);
	BorderRight->SetAlpha(CurrentAlpha);
	BorderLeft->SetAlpha(CurrentAlpha);
}

//*****************************************************************************
//MoveControl�֐�
//�����R���g���[��
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void GameObject::MoveControl()
{
	XMFLOAT3 CurrentPos = GetWorldPosition();
	Player* pPlayer = GetPlayerPointer();
	if (pPlayer->GetParent() != this) {
		if (pPlayer->GetFloor() == this) {
			pPlayer->SetParent(this);
		}
	}
	else if (pPlayer->GetParent() == this && pPlayer->GetFloor() != this) {
		pPlayer->RemoveParent();
	}
	if (++nFrameDelayCounter < nDelayFrames)
	{
		return;
	}
	if (!bReversingToInitialPos)
	{
		if (CurrentPos.x != xm2MoveableFinalPos.x)
		{
			if (CurrentPos.x < xm2MoveableFinalPos.x)
			{
				CurrentPos.x += xm2MovingPlatformSpeed.x;
				if (CurrentPos.x >= xm2MoveableFinalPos.x)
					CurrentPos.x = xm2MoveableFinalPos.x;
			}
			else if (CurrentPos.x > xm2MoveableFinalPos.x)
			{
				CurrentPos.x -= xm2MovingPlatformSpeed.x;
				if (CurrentPos.x <= xm2MoveableFinalPos.x)
					CurrentPos.x = xm2MoveableFinalPos.x;
			}
		}
		if (CurrentPos.y != xm2MoveableFinalPos.y)
		{
			if (CurrentPos.y < xm2MoveableFinalPos.y)
			{
				CurrentPos.y += xm2MovingPlatformSpeed.y;
				if (CurrentPos.y >= xm2MoveableFinalPos.y)
					CurrentPos.y = xm2MoveableFinalPos.y;
			}
			else if (CurrentPos.y > xm2MoveableFinalPos.y)
			{
				CurrentPos.y -= xm2MovingPlatformSpeed.y;
				if (CurrentPos.y <= xm2MoveableFinalPos.y)
					CurrentPos.y = xm2MoveableFinalPos.y;
			}
		}
		if (CurrentPos.x == xm2MoveableFinalPos.x && CurrentPos.y == xm2MoveableFinalPos.y) {
			bReversingToInitialPos = true;
			nFrameDelayCounter = 0;
		}
	}
	else
	{
		if (CurrentPos.x != xm2InitialMovingPos.x)
		{
			if (CurrentPos.x < xm2InitialMovingPos.x)
			{
				CurrentPos.x += xm2MovingPlatformSpeed.x;
				if (CurrentPos.x >= xm2InitialMovingPos.x)
					CurrentPos.x = xm2InitialMovingPos.x;
			}
			else if (CurrentPos.x > xm2InitialMovingPos.x)
			{
				CurrentPos.x -= xm2MovingPlatformSpeed.x;
				if (CurrentPos.x <= xm2InitialMovingPos.x)
					CurrentPos.x = xm2InitialMovingPos.x;
			}
		}
		if (CurrentPos.y != xm2InitialMovingPos.y)
		{
			if (CurrentPos.y < xm2InitialMovingPos.y)
			{
				CurrentPos.y += xm2MovingPlatformSpeed.y;
				if (CurrentPos.y >= xm2InitialMovingPos.y)
					CurrentPos.y = xm2InitialMovingPos.y;
			}
			else if (CurrentPos.y > xm2InitialMovingPos.y)
			{
				CurrentPos.y -= xm2MovingPlatformSpeed.y;
				if (CurrentPos.y <= xm2InitialMovingPos.y)
					CurrentPos.y = xm2InitialMovingPos.y;
			}
		}
		if (CurrentPos.x == xm2InitialMovingPos.x && CurrentPos.y == xm2InitialMovingPos.y) {
			bReversingToInitialPos = false;
			nFrameDelayCounter = 0;
		}
	}
	SetWorldPosition(CurrentPos.x, CurrentPos.y);
}

//*****************************************************************************
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void SpotLight::Draw()
{
	Player* pPlayer = GetPlayerPointer();
	if (!pPlayer)
		return;
	if (pPlayer->GetPlayerState() == PLAYER_TELEPORTING || IsPlayerPaused())
		return;
	if (BorderTop)
		BorderTop->DrawSprite(GetDeviceContext());
	if (BorderBottom)
		BorderBottom->DrawSprite(GetDeviceContext());
	if (BorderRight)
		BorderRight->DrawSprite(GetDeviceContext());
	if (BorderLeft)
		BorderLeft->DrawSprite(GetDeviceContext());
	GameObject::Draw();
}

//*****************************************************************************
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void SpotLight::End()
{
	SAFE_DELETE(BorderTop);
	SAFE_DELETE(BorderBottom);
	SAFE_DELETE(BorderRight);
	SAFE_DELETE(BorderLeft);
	GameObject::End();
}

//*****************************************************************************
//SpotLightBordersSetUp�֐�
//�X�|�b�g���C�g�̋��E��ݒ肷��
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void SpotLight::SpotLightBordersSetUp()
{
	BorderLeft->SetSpriteSize(8000, 324);
	BorderRight->SetSpriteSize(8000, 324);
	BorderBottom->SetSpriteSize(4800, (3000 / 2));
	BorderTop->SetSpriteSize(4800, 3000);
	BorderTop->SetLocalPosition(0, 357);
	BorderBottom->SetLocalPosition(0, (-341.6f + 15));
	BorderRight->SetLocalPosition(((270 * 2) - 35), 15);
	BorderLeft->SetLocalPosition((-(270 * 2) + 35), 15);
	XMFLOAT3 Pos;
	Pos = BorderTop->GetWorldPosition();
	while (IsInCollision(BorderTop->GetDimensionHitbox(), GetDimensionHitbox()))
	{
		Pos.y++;
		BorderTop->SetWorldPosition(Pos.x, Pos.y);
	}
	Pos.y--;
	BorderTop->SetWorldPosition(Pos.x, Pos.y);

	Pos = BorderBottom->GetWorldPosition();
	while (IsInCollision(BorderBottom->GetDimensionHitbox(), GetDimensionHitbox()))
	{
		Pos.y--;
		BorderBottom->SetWorldPosition(Pos.x, Pos.y);
	}
	Pos.y++;
	BorderBottom->SetWorldPosition(Pos.x, Pos.y);

	Pos = BorderRight->GetWorldPosition();
	while (IsInCollision(BorderRight->GetDimensionHitbox(), GetDimensionHitbox()))
	{
		Pos.x++;
		BorderRight->SetWorldPosition(Pos.x, Pos.y);
	}
	Pos.x--;
	BorderRight->SetWorldPosition(Pos.x, Pos.y);
	Pos = BorderLeft->GetWorldPosition();
	while (IsInCollision(BorderLeft->GetDimensionHitbox(), GetDimensionHitbox()))
	{
		Pos.x--;
		BorderLeft->SetWorldPosition(Pos.x, Pos.y);
	}
	Pos.x++;
	BorderLeft->SetWorldPosition(Pos.x, Pos.y);
}