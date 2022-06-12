//*****************************************************************************
// Gem.cpp
// �v���C���[�̃X�|�b�g���C�g�̊Ǘ�
// Author : Mane
//*****************************************************************************
#include "Gem.h"
#include "TextureManager.h"
#include "Player.h"
#include "SceneInGame.h"
#include "sound.h"

//*****************************************************************************
// �R���X�g���N�^�֐�
//*****************************************************************************
Gem::Gem(XMFLOAT2 Position)
{
	Init(Position);
}


Gem::~Gem()
{
	End();
}

//*****************************************************************************
//Init�֐�
//�������̊֐�
//�����FXMFLOAT2
//�߁Fvoid
//*****************************************************************************
void Gem::Init(XMFLOAT2 Position)
{
	InitSprite();
	SetType(GEM_TYPE);
	SetUse(true);
	gpTexture = GetTexture(TEXTURE_CRYSTAL);
	SetSpriteSize(50, 71);
	SetWorldPosition(Position.x, Position.y);
	x2GOInitialPos = Position;
	hitbox = { 0,0,70,91 };
	nFrame_Y = 4;
	nFrame_X = 5;
}

//*****************************************************************************
//Update�֐�
//�����̃A�b�v�f�[�g�̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Gem::Update()
{
	GameObject::Update();
	//�W�F���̃I�u�W�F�N�g
	if (!bIsInUse)
		return;
	Player* pPlayer = GetPlayerPointer();
	SceneInGame* pGame = GetCurrentGame();
	RegularUVAnimationControl(3);
	if (pPlayer)
	{
		Weapon* cWeapon = pPlayer->GetCurrentWeapon();
		if ((IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), GetHitbox()) || (IsInCollision(cWeapon->GetHitbox(), GetHitbox()) && cWeapon->IsWeaponAttacking()))
			&& pPlayer->GetPlayerState() != PLAYER_TELEPORTING)
		{
			bIsInUse = false;
			if (!pGame)
				return;
			pGame->AddToScore(10);
			pGame->AddGem(1);
			PlaySound(SOUND_GEM_PICKUP);
		}
	}
}

//*****************************************************************************
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Gem::Draw()
{
	GameObject::Draw();
}

//*****************************************************************************
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Gem::End()
{
	GameObject::End();
}
