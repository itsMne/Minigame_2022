//*****************************************************************************
// Wall.cpp
// �ǂƕǂ̓����蔻����Ǘ�����
// Author : Mane
//*****************************************************************************
#include "Wall.h"
#include "Player.h"
#include "TextureManager.h"

//*****************************************************************************
// �R���X�g���N�^�ϐ�
//*****************************************************************************
Wall::Wall(int TextureID, int n_Type, HitBox hbox, int WallTileSize, int nActivation) : GameObject()
{
	nWallID = STATIC_ID;
	nWallScaleActivation = nActivation;
	nWallTexture = TextureID;
	Init(TextureID, n_Type, hbox, WallTileSize);
	switch (nWallScaleActivation)
	{
	case WALL_SCALE_POSITIVE:
		SetWallDeactivateableWhenScaleIsNegative(true);
		break;
	case WALL_SCALE_NEGATIVE:
		SetWallDeactivateableWhenScaleIsPositive(true);
		break;
	default:
		break;
	}
}


Wall::~Wall()
{
	End();
}

//*****************************************************************************
//Init�֐�
//�������֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Wall::Init(int TextureID, int n_Type, HitBox hbox, int WallTileSize)
{
	GameObject::Init(GetTexture(TextureID), n_Type);
	hitbox = hbox;
	hitbox.print = SHOW_HITBOXES;
	fYTileSize = WallTileSize;
	bIsInUse = true;
	bRepeatableUV_Y = true;
	Parent = nullptr;
	bDeactivateWhenScaleIsNegative = false;
	bDeactivateWhenScaleIsPositive = false;
}

//*****************************************************************************
//Update�֐�
//�ύX�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Wall::Update()
{
	if (!bIsInUse)
		return;
	GameObject::Update();
	uv.U = 1;
	Player* pPlayer = GetPlayerPointer();
	if (bDeactivateWhenScaleIsNegative)
	{
		if (GetUniversalScale() < 0)
		{
			hitbox = { 0,0,0,0 };
			SetAlpha(0.35f);
		}
		else if (GetUniversalScale() > 0)
		{
			hitbox = hbOriginalHitbox;
			SetAlpha(1);
		}
	}
	if (bDeactivateWhenScaleIsPositive)
	{
		if (GetUniversalScale() > 0)
		{
			hitbox = { 0,0,0,0 };
			SetAlpha(0.35f);
		}
		else if (GetUniversalScale() < 0)
		{
			hitbox = hbOriginalHitbox;
			SetAlpha(1);
		}
	}
	if (!pPlayer)
		return;
	if (pPlayer->GetPlayerState() == PLAYER_TELEPORTING)
		return;
	int nPlayerDirection = pPlayer->GetDirection();
	while (IsInCollision(pPlayer->GetPlayerHitbox(HB_LEFT_LIMIT), GetHitbox()))
	{
		XMFLOAT3 x3PlayerPos = pPlayer->GetWorldPosition();
		x3PlayerPos.x++;
		pPlayer->SetWorldPosition(x3PlayerPos);
	}
	while (IsInCollision(pPlayer->GetPlayerHitbox(HB_RIGHT_LIMIT), GetHitbox()))
	{
		XMFLOAT3 x3PlayerPos = pPlayer->GetWorldPosition();
		x3PlayerPos.x--;
		pPlayer->SetWorldPosition(x3PlayerPos);
	}
}

//*****************************************************************************
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Wall::Draw()
{
	GameObject::Draw();
}

//*****************************************************************************
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Wall::End()
{
	GameObject::End();
}

//*****************************************************************************
//SetWallID�֐�
//�ǂ�ID��ݒ肷��
//�����Fint
//�߁Fvoid
//*****************************************************************************
void Wall::SetWallID(int newID)
{
	nWallID = newID;
}

//*****************************************************************************
//SetWallDeactivateableWhenScaleIsNegative�֐�
//�ǂ̓X�P�[�������ʂŗ����Ă��邱�Ƃ�ݒ肷��
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void Wall::SetWallDeactivateableWhenScaleIsNegative(bool deact)
{
	bDeactivateWhenScaleIsNegative = deact;
	bDeactivateWhenScaleIsPositive = !deact;
	hbOriginalHitbox = hitbox;
}

//*****************************************************************************
//SetWallDeactivateableWhenScaleIsPositive�֐�
//�ǂ̓X�P�[�����t���ŗ����Ă��邱�Ƃ�ݒ肷��
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void Wall::SetWallDeactivateableWhenScaleIsPositive(bool act)
{
	bDeactivateWhenScaleIsPositive = act;
	bDeactivateWhenScaleIsNegative = !act;
	hbOriginalHitbox = hitbox;
}

//*****************************************************************************
//GetWallID�֐�
//ID��߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int Wall::GetWallID()
{
	return nWallID;
}

//*****************************************************************************
//GetWallTexture�֐�
//�e�N�X�`����߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int Wall::GetWallTexture()
{
	return nWallTexture;
}

//*****************************************************************************
//GetScaleActivation�֐�
//�ǂ̎�ނ�߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int Wall::GetTypeScaleActivation()
{
	return nWallScaleActivation;
}
