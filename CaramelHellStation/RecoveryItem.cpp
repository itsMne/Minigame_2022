//*****************************************************************************
// RecoveryItem.cpp
// �񕜃A�C�e���u�W�F�N�g�̊Ǘ�
// Author : Mane
//*****************************************************************************
#include "RecoveryItem.h"
#include "TextureManager.h"
#include "Player.h"
#include "SceneInGame.h"
#include "Sound.h"

//*****************************************************************************
// �R���X�g���N�^�֐�
//*****************************************************************************
RecoveryItem::RecoveryItem(XMFLOAT2 Position) : GameObject()
{
	Init(Position);
}


RecoveryItem::~RecoveryItem()
{
	End();
}

//*****************************************************************************
//Init�֐�
//�������̊֐�
//�����Fint, HitBox, float, int
//�߁Fvoid
//*****************************************************************************
void RecoveryItem::Init(XMFLOAT2 Position)
{
	InitSprite();
	SetType(RECOVER_TYPE);
	SetUse(true);
	gpTexture = GetTexture(TEXTURE_SOBA);
	SetSpriteSize(125, 82);
	SetWorldPosition(Position.x, Position.y);
	x2GOInitialPos = Position;
	hitbox = { 0,0,125,82 };
	fMirrorAnimation = 0.25f;
}

//*****************************************************************************
//Update�֐�
//�����̃A�b�v�f�[�g�̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void RecoveryItem::Update()
{
	//�񕜂̃A�C�e��
	GameObject::Update();
	if (!bIsInUse)
		return;
	Player* pPlayer = GetPlayerPointer();
	SceneInGame* pGame = GetCurrentGame();
	Size.x += fMirrorAnimation;
	Size.y += fMirrorAnimation;
	if (Size.x > 125 + 5 || Size.x < 125 - 5)
		fMirrorAnimation *= -1;
	if (pPlayer)
	{
		if (IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), GetHitbox()) && pPlayer->GetPlayerState() != PLAYER_TELEPORTING)
		{
			pPlayer->RecoverHP(1);
			if (!pGame)
				return;
			pGame->AddToScore(30);
			bIsInUse = false;
			PlaySound(SOUND_LABEL_RECOVERED);
		}
	}
}

//*****************************************************************************
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void RecoveryItem::Draw()
{
	GameObject::Draw();
}

//*****************************************************************************
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void RecoveryItem::End()
{
	GameObject::End();
}
