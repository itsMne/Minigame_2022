//*****************************************************************************
// GameObject.cpp
// �Q�[���̈�ʓI�ȃI�u�W�F�N�g�̊Ǘ�
// Author : Mane
//*****************************************************************************
#include "GameObject.h"
#include "Camera.h"
#include "TextureManager.h"
#include "Player.h"
#include "GameInputManager.h"
#include "sound.h"
#include "SceneInGame.h"
#include "SceneMainMenu.h"
#include "RankManager.h"
#include "UIManager.h"
#include "EventHandler.h"

#include <stdio.h>

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define PRINT_HITBOX true && SHOW_HITBOXES
#define FLOWER_TIME 24

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
int nGameObjectsCount = 0;//������Q�[���I�u�W�F�N�g

//*****************************************************************************
// �R���X�g���N�^�֐�
//*****************************************************************************
GameObject::GameObject()
{
	nGameObjectsCount++;
	bIsInUse = false;
	bMovementInProgress = false;
	fXTileSize = 0;
	fYTileSize = 0;
	nType = -1;
	x2GOInitialPos = { 0,0 };
	xm2InitialMovingPos = { 0,0 };
	Position.x = 0;
	Position.y = 0;
	uv = { 0,0 };
	n_uvTimer = 0;
	nFrame_X = 1;
	nFrame_Y = 1;
	nInUseTimer = 0;
	nMaxLogoTime = 0;
	bEndFlower = false;
	bIsUIObject = false;
	x2Acceleration = { 0,0 };
	nID_Room = MAX_ROOMS;
	bDontDraw = false;
	fMirrorAnimation = 1;
	fAcceleration = 1;
	n_goID = STATIC_ID;
	bIsMoveAble = false;
	bReversingToInitialPos = false;
	xm2MovingPlatformSpeed = {0,0};
	xm2MoveableFinalPos = {0,0};
	nFramesActivatedLogo = 0;
	bRepeatableUV_Y = bRepeatableUV_X = false;
	bLogoActivated = false;
	bLogoShown = false;
	nFrameDelayCounter = 0;
	nDelayFrames = 0;
}

GameObject::~GameObject()
{
	End();	
}

//*****************************************************************************
//Init�֐�
//�������̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void GameObject::Init()
{
	InitSprite();
	bIsInUse = true;
}

//*****************************************************************************
//Init�֐�
//�������̊֐�
//�����FHitBox
//�߁Fvoid
//*****************************************************************************
void GameObject::Init(HitBox hbox)
{
	InitSprite();
	hitbox = hbox;
	bIsInUse = true;
}

//*****************************************************************************
//Init�֐�
//�������̊֐�
//�����Fint
//�߁Fvoid
//*****************************************************************************
void GameObject::Init(int n_Type)
{
	InitSprite();
	SetType(n_Type);
}

//*****************************************************************************
//Init�֐�
//�������̊֐�
//�����FID3D11ShaderResourceView*, int
//�߁Fvoid
//*****************************************************************************
void GameObject::Init(ID3D11ShaderResourceView * n_gpTexture, int n_Type)
{
	InitSprite(n_gpTexture);
	SetType(n_Type);
	bIsInUse = true;
}

//*****************************************************************************
//InitEnemyIntroductionLogo�֐�
//�G�̃��S�̏������̊֐�
//�����Fint
//�߁Fvoid
//*****************************************************************************
void GameObject::InitEnemyIntroductionLogo(int EnemyType)
{
	InitSprite();
	SetType(ENEMY_INTRODUCTION_LOGO);
	SetUse(true);
	SetWorldPosition(0, 0);
	nFrame_Y = 1;
	nFrame_X = 1;
	switch (EnemyType)
	{
	case BUNBUN_ENEMY:
		gpTexture = GetTexture(TEXTURE_BUNBUN_ENEMYLOGO);
		SetRotation({90, 0, 0});
		SetSpriteSize(300, 127);
		nMaxLogoTime = (int)(60 * 1.4f);
		break;
	default:
		break;
	}
}

//*****************************************************************************
//GetHitBoxWithoutPosition�֐�
//�q�b�g�{�b�N�X��߂��i�I�u�W�F�N�g�̋��_�Ɗ֌W�Ȃ��j
//�����Fvoid
//�߁FHitBox
//*****************************************************************************
HitBox GameObject::GetHitBoxWithoutPosition()
{
	return hitbox;
}

//*****************************************************************************
//SetNewHitbox�֐�
//�q�b�g�{�b�N�X��ݒ肷��
//�����FHitBox
//�߁Fvoid
//*****************************************************************************
void GameObject::SetNewHitbox(HitBox newHB)
{
	hitbox = newHB;
}

//*****************************************************************************
//Update�֐�
//�����̃A�b�v�f�[�g�̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void GameObject::Update()
{
	if (!bIsInUse)
		return;
	switch (nType)
	{
	case ENEMY_INTRODUCTION_LOGO://�G�̏Љ�S�i�`���[�g���A���j
		if (nFramesActivatedLogo < nMaxLogoTime) {
			if (bLogoActivated)
			{
				fAcceleration += 0.45f;
				if (Rotation.x > 0)
				{
					Rotation.x -= fAcceleration;
					if (Rotation.x < 0)
						Rotation.x = 0;
				}
				if (Rotation.x == 0)
				{
					fAcceleration = 0;
					nFramesActivatedLogo++;
				}
			}
		}
		else {
			fAcceleration += 0.45f;
			if (Rotation.x < 90)
			{
				Rotation.x += fAcceleration;
				if (Rotation.x > 90)
					Rotation.x = 90;
			}
			if (Rotation.x == 90)
			{
				bLogoShown = true;
			}
		}
		break;
	case SCROLLING_BACKGROUND_TYPE://���j���[�̃o�b�N�O���E���h
		SetSpriteSize(WINDOW_WIDTH, WINDOW_HEIGHT);
		uv.U += 0.0025f;
		break;
	case GO_HALO_TYPE://�v���C���[�̃n���[�I�u�W�F�N�g
		HaloUpdate();
		break;
	case ANIMATED_TITLE_TYPE://�^�C�g���̃��S�̃A�j���[�V�����Ǘ�
		AnimatedLogoUpdate();
		break;
	case NOISE_TYPE://�Q�[���I�[�o�[�̃A�j���[�V�����Ǘ�
		nFrame_Y = 3;
		nFrame_X = 1;
		uv.U = 0;
		if (++n_uvTimer > 3)
		{
			n_uvTimer = 0;
			uv.V++;
		}
		break;
	case ANIMATED_LOGO_TYPE://���S�̃A�j���[�V����
		if (uv.V < 29)
		{
			if (++n_uvTimer > 3)
			{
				n_uvTimer = 0;
				uv.V++;
			}
		}
		break;
	case FLOWER_TYPE://�v���C���[�̉Ԃ̊Ǘ�
		FlowerUpdate();
		break;
	case ROTATING_OBJECT_TYPE://��]����I�u�W�F�N�g
		Rotation.y++;
		if (Rotation.y == 360)
			Rotation.y = 0;
		break;
	case AURA_EFFECT_TYPE://�I�[���[�̊Ǘ�
		Size.x+=85; 
		Size.y+=85;
		if (Size.x > 2831)
			bIsInUse = false;
		break;
	case MINI_AURA_EFFECT_TYPE://�~�j�I�[���[�̊Ǘ�
		Size.x += 10;
		Size.y += 10;
		if(color.w>0)
			SetAlpha(color.w-0.035f);
		if (color.w < 0)
			SetAlpha(0);
		if (color.w==0)
			bIsInUse = false;
		break;
	case BACKGROUND_OBJECT_TYPE://�o�b�N�O���E���h�I�u�W�F�N�g�̊Ǘ�
		BackgroundControl();
		break;
	case BACKGROUND_EVENT_TYPE:
		Rotation.z += 0.25f;
		if (GetCurrentActiveEvent())
		{
			if (Size.x == 1500 && Size.y == 1500) {
				fAcceleration = 0;
				return;
			}
			fAcceleration+=10;
			SizeUp({fAcceleration, fAcceleration});
			if (Size.x > 1500)
				Size.x = 1500;
			if (Size.y > 1500)
				Size.y = 1500;
		}
		else
		{
			if (Size.x == 0 && Size.y == 0) {
				fAcceleration = 0;
				return;
			}
			fAcceleration+=10;
			SizeUp({ -fAcceleration, -fAcceleration });
		}
		break;
	default:
		break;
	}
	if (bIsMoveAble)
		MoveControl();
}

//*****************************************************************************
//BackgroundControl�֐�
//�o�b�N�O���E���h�̊Ǘ�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void GameObject::BackgroundControl()
{
	XMFLOAT3 CamPos = GetCameraPos();
	static float fPlayerPosX = CamPos.x;
	static float fPlayerPosXPrevious = CamPos.x;
	static float fPlayerPosY = CamPos.y;
	static float fPlayerPosYPrevious = CamPos.y;
	fPlayerPosX = CamPos.x;
	fPlayerPosY = CamPos.y;
	//uv.U = 0.0005f*GetCameraPos().x;
	int nDirection = 1;
	if(GetUniversalScale()<0)
		nDirection = -1;
	uv.U += (0.0005f + (fPlayerPosX - fPlayerPosXPrevious)*0.00075f)*nDirection;
	uv.V -= (0.0005f + (fPlayerPosY - fPlayerPosYPrevious)*0.00075f)*nDirection;
	fPlayerPosXPrevious = fPlayerPosX;
	fPlayerPosYPrevious = fPlayerPosY;
}

//*****************************************************************************
//RegularUVAnimationControl�֐�
//UV�̃A�j���[�V�����̊Ǘ�
//�����Fint
//�߁Fvoid
//*****************************************************************************
void GameObject::RegularUVAnimationControl(int speed)
{
	if (++n_uvTimer > speed)
	{
		n_uvTimer = 0;
		uv.U++;
		if (uv.U == nFrame_X)
		{
			uv.V++;
			uv.U = 0;
			if (uv.V == nFrame_Y)
				uv.V = 0;
		}
	}
}

//*****************************************************************************
//SetUpMoveControl�֐�
//�������Ƃ�ݒ肷��
//�����FXMFLOAT2, XMFLOAT2, XMFLOAT2
//�߁Fvoid
//*****************************************************************************
void GameObject::SetUpMoveControl(XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed)
{
	bIsMoveAble = true;
	xm2MovingPlatformSpeed = MovingSpeed;
	xm2InitialMovingPos = InitialPos;
	xm2MoveableFinalPos = FinalPos;
}

//*****************************************************************************
//SetUpMoveControl�֐�
//�������Ƃ�ݒ肷��
//�����FXMFLOAT2, XMFLOAT2, float
//�߁Fvoid
//*****************************************************************************
void GameObject::SetUpMoveControl(XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, float MovingSpeed)
{
	bIsMoveAble = true;
	xm2MovingPlatformSpeed = { MovingSpeed, MovingSpeed };
	xm2InitialMovingPos = InitialPos;
	xm2MoveableFinalPos = FinalPos;
}

//*****************************************************************************
//FlowerUpdate�֐�
//�ԃI�u�W�F�N�g�̕ύX����
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void GameObject::FlowerUpdate()
{
	nFrame_X = 2;
	nFrame_Y = 24;
	SetSpriteSize(60, 60);
	SetSpriteFrameSize(960, 540);
	if (nInUseTimer > ((nFrame_Y * 2) + FLOWER_TIME) && !bEndFlower)
	{
		bEndFlower = true;
		uv.U = 0.f;
		uv.V =(float)(nFrame_Y - 1);
	}
	if (bEndFlower)
	{
		uv.V--;
		if (uv.V == 0)
		bIsInUse = false;
		return;
	}
	n_uvTimer++;
	if (uv.U == 0 && uv.V == nFrame_Y - 1)
	{
		uv.U = 1;
		uv.V = 0;
	}
	if (uv.U == 1 && uv.V == nFrame_Y - 1)
	{
		uv.V = 0;
	}
	if (n_uvTimer > 0) {
		uv.V++;
		nInUseTimer++;
		n_uvTimer = 0;
	}
}

//*****************************************************************************
//AnimatedLogoUpdate�֐�
//���S�̃A�j���[�V�����̊Ǘ�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void GameObject::AnimatedLogoUpdate()
{
	nFrame_X = 6;
	nFrame_Y = 16;
	SetSpriteSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	SetSpriteFrameSize(960, 540);
	SetLocalPosition(0, 0);
	n_uvTimer++;
	bIsUIObject = true;
	if (n_uvTimer > 2) {
		uv.U++;
		n_uvTimer = 0;
	}
	if (uv.U == nFrame_X && uv.V == nFrame_Y - 1)
	{
		uv = { 1,9 };
	}
	else {
		if (uv.U == nFrame_X && uv.V != nFrame_Y) {
			uv.U = 0;
			uv.V++;
		}
	}

}

//*****************************************************************************
//HaloUpdate�֐�
//�n���[�̃A�b�v�f�[�g�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void GameObject::HaloUpdate()
{
	nFrame_X = 5;
	nFrame_Y = 3;
	SetSpriteSize(150, 150);
	SetSpriteFrameSize(615, 615);
	n_uvTimer++;
	if (n_uvTimer > 2) {
		uv.U++;
		n_uvTimer = 0;
	}
	if (uv.U == 4 && uv.V == 5)
	{
		uv = { 0,0 };
	}
	if (uv.U == nFrame_X) {
		uv.U = 0;
		uv.V+=1.f;
	}
	else if (uv.U == -1)
	{
		uv.U = (float)(nFrame_X - 1);
		uv.V-=1.f;
	}
	if (uv.V == nFrame_Y)
		uv.V = 0;
}

//*****************************************************************************
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void GameObject::Draw()
{
	if (!bIsInUse || bDontDraw)
		return;
	SetLocalPosition(Position.x, Position.y);
	SetSpriteFrameSize(1.0f / nFrame_X, 1.0f / nFrame_Y);
	if (bRepeatableUV_X) {
		SetSpriteFrameSize(Size.x/fXTileSize, 1.0f / nFrame_Y);
		SetSpriteUV(uv.U, uv.V / (float)nFrame_Y);
	}
	else if (bRepeatableUV_Y) {
		SetSpriteFrameSize(1.0f / nFrame_X, Size.y / fYTileSize);
		SetSpriteUV(uv.U / (float)nFrame_X, uv.V);
	}
	else {
		SetSpriteUV(uv.U / (float)nFrame_X, uv.V / (float)nFrame_Y);
	}
	if(!bIsUIObject)
		DrawSprite(GetDeviceContext());
	else
		DrawSpriteUI(GetDeviceContext());
}

//*****************************************************************************
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void GameObject::End()
{
	UninitSprite();
	nGameObjectsCount--;
}

//*****************************************************************************
//SetType�֐�
//�I�u�W�F�N�g�̃^�C�v��ݒ肷��
//�����Fint
//�߁Fvoid
//*****************************************************************************
void GameObject::SetType(int Type)
{
	nType = Type;
}

//*****************************************************************************
//SetUse�֐�
//�I�u�W�F�N�g���g�����Ƃ�ݒ肷��
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void GameObject::SetUse(bool b_use)
{
	bIsInUse = b_use;
}

//*****************************************************************************
//SetVisibility�֐�
//�I�u�W�F�N�g�������邱�Ƃ�ݒ肷��
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void GameObject::SetVisibility(bool bVis)
{
	if (!bVis)
		bDontDraw = true;
	else
		bDontDraw = false;
}

//*****************************************************************************
//SetFlowerInUse�֐�
//�ԃI�u�W�F�N�g���g�����Ƃ�ݒ肷��
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void GameObject::SetFlowerInUse()
{
	bIsInUse = true;
	nInUseTimer = 0;
	uv.U = 0;
	uv.V = 0;
	bEndFlower = false;
}

//*****************************************************************************
//SetUIObject�֐�
//�I�u�W�F�N�g��UI�Ƃ��Ă�ݒ肷��
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void GameObject::SetUIObject(bool isUI)
{
	bIsUIObject = isUI;
}

//*****************************************************************************
//SetFramesForUV�֐�
//UV�̃t���[����ݒ肷��
//�����Ffloat, float
//�߁Fvoid
//*****************************************************************************
void GameObject::SetFramesForUV(float x, float y)
{
	nFrame_X = (int)x;
	nFrame_Y = (int)y;
}

//*****************************************************************************
//SetAcceleration�֐�
//�����x��ݒ肷��
//�����FXMFLOAT2
//�߁Fvoid
//*****************************************************************************
void GameObject::SetAcceleration(XMFLOAT2 newAccel)
{
	x2Acceleration = newAccel;
}

//*****************************************************************************
//SetRoom�֐�
//���[��ID��ݒ肷��
//�����Fint
//�߁Fvoid
//*****************************************************************************
void GameObject::SetRoom(int newID_Room)
{
	nID_Room = newID_Room;//CHECK
}

//*****************************************************************************
//SetRepeatableUV_X�֐�
//UV�̌J��Ԃ���ݒ肷��iX���W�j
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void GameObject::SetRepeatableUV_X(bool bRepeatable)
{
	bRepeatableUV_X = bRepeatable;
}

//*****************************************************************************
//SetRepeatableUV_Y�֐�
//UV�̌J��Ԃ���ݒ肷��iY���W�j
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void GameObject::SetRepeatableUV_Y(bool bRepeatable)
{
	bRepeatableUV_Y = bRepeatable;
}

//*****************************************************************************
//GetType�֐�
//�I�u�W�F�N�g�̎�ނ�߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int GameObject::GetType()
{
	return nType;
}

//*****************************************************************************
//GetMovementDelayFrames�֐�
//�I�u�W�F�N�g�̃f�B���C�t���[����߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int GameObject::GetMovementDelayFrames()
{
	return nDelayFrames;
}

//*****************************************************************************
//GetGameObjectID�֐�
//�I�u�W�F�N�g��ID��߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int GameObject::GetGameObjectID()
{
	return n_goID;
}

//*****************************************************************************
//GetTileSizeX�֐�
//�I�u�W�F�N�g�̃^�C��X�̃T�C�Y��߂�
//�����Fvoid
//�߁Ffloat
//*****************************************************************************
float GameObject::GetTileSizeX()
{
	return fXTileSize;
}

//*****************************************************************************
//GetTileSizeX�֐�
//�I�u�W�F�N�g�̃^�C��Y�̃T�C�Y��߂�
//�����Fvoid
//�߁Ffloat
//*****************************************************************************
float GameObject::GetTileSizeY()
{
	return fYTileSize;
}

//*****************************************************************************
//IsInUse�֐�
//�I�u�W�F�N�g���g���Ă��邱�Ƃ��m�F����
//�����Fvoid
//�߁Fint
//*****************************************************************************
bool GameObject::IsInUse()
{
	return bIsInUse;
}

//*****************************************************************************
//WasLogoShown�֐�
//���S�����������Ƃ��m�F����
//�����Fvoid
//�߁Fbool
//*****************************************************************************
bool GameObject::WasLogoShown()
{
	return bLogoShown;
}

//*****************************************************************************
//IsMoveable�֐�
//�I�u�W�F�N�g�������邱�Ƃ��m�F����
//�����Fvoid
//�߁Fbool
//*****************************************************************************
bool GameObject::IsMoveable()
{
	return bIsMoveAble;
}

//*****************************************************************************
//ActivateIntroductionLogo�֐�
//���S��L������
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void GameObject::ActivateIntroductionLogo()
{
	bLogoActivated = true;
}

//*****************************************************************************
//SetDelayFrames�֐�
//�f�B���C�t���[����ݒ肷��
//�����Fint
//�߁Fvoid
//*****************************************************************************
void GameObject::SetDelayFrames(int newFrames)
{
	nDelayFrames = newFrames;
}

//*****************************************************************************
//SetObjectInitialPos�֐�
//�ŏ��̋��_��ݒ肷��
//�����FXMFLOAT2
//�߁Fvoid
//*****************************************************************************
void GameObject::SetObjectInitialPos(XMFLOAT2 initPos)
{
	x2GOInitialPos = initPos;
}

//*****************************************************************************
//SetObjectInitialPos�֐�
//ID��ݒ肷��
//�����Fint
//�߁Fvoid
//*****************************************************************************
void GameObject::SetNewID(int nID)
{
	n_goID = nID;
}

//*****************************************************************************
//GetObjectInitialPos�֐�
//�ŏ��̋��_��߂�
//�����Fvoid
//�߁FXMFLOAT2
//*****************************************************************************
XMFLOAT2 GameObject::GetObjectInitialPos()
{
	return x2GOInitialPos;
}

//*****************************************************************************
//GetMovingInitialPos�֐�
//�ŏ��́i������j���_��߂�
//�����Fvoid
//�߁FXMFLOAT2
//*****************************************************************************
XMFLOAT2 GameObject::GetMovingInitialPos()
{
	return xm2InitialMovingPos;
}

//*****************************************************************************
//GetMovingInitialPos�֐�
//�Ō�́i������j���_��߂�
//�����Fvoid
//�߁FXMFLOAT2
//*****************************************************************************
XMFLOAT2 GameObject::GetMovingFinalPos()
{
	return xm2MoveableFinalPos;
}

//*****************************************************************************
//GetMovingSpeed�֐�
//������I�u�W�F�N�g�̑�����߂�
//�����Fvoid
//�߁FXMFLOAT2
//*****************************************************************************
XMFLOAT2 GameObject::GetMovingSpeed()
{
	return xm2MovingPlatformSpeed;
}


int GetNumOfGameObjectsSetInMemory()
{
	return nGameObjectsCount;
}