//*****************************************************************************
// Spike.cpp
// �X�p�C�N�u�W�F�N�g�̊Ǘ�
// Author : Mane
//*****************************************************************************
#include "Spike.h"
#include "TextureManager.h"
#include "Player.h"
#include "sound.h"

//*****************************************************************************
// �}�N��
//*****************************************************************************
#define PRINT_HITBOX false

//*****************************************************************************
// �R���X�g���N�^�֐�
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
//Init�֐�
//�������̊֐�
//�����FXMFLOAT2, int
//�߁Fvoid
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
//GetSpikeNumber�֐�
//�X�p�C�N�̐���߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int Spike::GetSpikeNumber()
{
	return nSpikeNumber;
}

//*****************************************************************************
//Update�֐�
//�����̃A�b�v�f�[�g�̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Spike::Update()
{
	GameObject::Update();
	//�X�p�C�N�̃I�u�W�F�N�g�̊Ǘ�
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
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Spike::Draw()
{
	GameObject::Draw();
}

//*****************************************************************************
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Spike::End()
{
	GameObject::End();
}
