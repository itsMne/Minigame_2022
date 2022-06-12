//*****************************************************************************
// Teleporter.cpp
// �e���|�[�g�̊Ǘ�
// Author : Mane
//*****************************************************************************
#include "Teleporter.h"
#include "TextureManager.h"
#include "Camera.h"
#include "Player.h"
#include "SceneInGame.h"
#include "SceneMainMenu.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define PRINT_HITBOX false

//*****************************************************************************
// �R���X�g���N�^�ϐ�
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
//Init�֐�
//�������̊֐�
//�����Fint
//�߁Fvoid
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
//Update�֐�
//�����̃A�b�v�f�[�g�̊֐�
//�����Fvoid
//�߁Fvoid
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
					if (!bSwitchToResultScreen) {//�N���A�̎��A�X�R�A��ۑ�����
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
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Teleporter::Draw()
{
	GameObject::Draw();
}

//*****************************************************************************
//End�֐�
//�I���̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Teleporter::End()
{
	GameObject::End();
}

//*****************************************************************************
//SetTeleporterID�֐�
//�e���|�[�g�̃I�u�W�F�N�g��ID��ݒ肷��
//�����Fint
//�߁Fvoid
//*****************************************************************************
void Teleporter::SetTeleporterID(int newID)
{
	nTeleporterID = newID;
}

//*****************************************************************************
//SetTeleporterAsEndpoint�֐�
//�e���|�[�g���X�e�[�W�̃S�[���Ƃ��Ă�ݒ肷��
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Teleporter::SetTeleporterAsEndpoint()
{
	gpTexture = GetTexture(TEXTURE_ENDPOINT_TELEPORTER);
	bIsEndpoint = true;
}

//*****************************************************************************
//GetTeleporterID�֐�
//�e���|�[�g�̃I�u�W�F�N�g��ID��߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int Teleporter::GetTeleporterID()
{
	return nTeleporterID;
}

//*****************************************************************************
//IsEndPoint�֐�
//�e���|�[�g�̓S�[�����m�F����
//�����Fvoid
//�߁Fbool
//*****************************************************************************
bool Teleporter::IsEndPoint()
{
	return bIsEndpoint;
}

//*****************************************************************************
//SetTeleporterDestination�֐�
//�e���|�[�g�̃t�@�C�i�����_��ݒ肷��
//�����FXMFLOAT2
//�߁Fvoid
//*****************************************************************************
void Teleporter::SetTeleporterDestination(XMFLOAT2 dest)
{
	xm2Teleporter_NewPlayerPos = dest;
}

//*****************************************************************************
//GetTeleporterDestination�֐�
//�e���|�[�g�̃t�@�C�i�����_��߂�
//�����Fvoid 
//�߁FXMFLOAT2
//*****************************************************************************
XMFLOAT2 Teleporter::GetTeleporterDestination()
{
	return xm2Teleporter_NewPlayerPos;
}