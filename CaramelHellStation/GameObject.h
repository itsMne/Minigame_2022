//*****************************************************************************
// GameObject.h
// Author : Mane
//*****************************************************************************
#pragma once
#include "Sprite.h"
#include "UniversalVariables.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define STAR_SIZE 150
#define SPIKE_SIZE 85
#define SPIKE_SIZE_TALL 125
#define TELEPORTER_SIZE 300
#define STATIC_ID -1

//*****************************************************************************
// エナム定義
//*****************************************************************************
enum GameObjectType
{
	GO_HALO_TYPE=0,
	ENEMY_INTRODUCTION_LOGO,
	SCROLLING_BACKGROUND_TYPE,
	DEBUG_AIM_TYPE,
	SPOTLIGHT_TYPE,
	ANIMATED_LOGO_TYPE,
	ANIMATED_TITLE_TYPE,
	NOISE_TYPE,
	FLOOR_TYPE,
	FLOWER_TYPE,
	PLAYER_BULLET_TYPE,
	PLAYER_CATCHBULLET_TYPE,
	ROTATING_OBJECT_TYPE,
	AURA_EFFECT_TYPE,
	MINI_AURA_EFFECT_TYPE,
	WALL_TYPE,
	SPIKE_TYPE,
	TELEPORTER_TYPE,
	PUNCHABLE_STAR_TYPE,
	BULB_SWITCH_TYPE,
	RECOVER_TYPE,
	GEM_TYPE,
	BACKGROUND_OBJECT_TYPE,
	BACKGROUND_EVENT_TYPE,
	MAX_GOTYPE
};

enum eObjMoveable
{
	STATIC_OBJECT = 0,
	MOVEABLE_OBJECT,
	MOVEABLE_BY_SWITCH_OBJECT,
};

//*****************************************************************************
// クラス
//*****************************************************************************
class GameObject :
	public Sprite
{
protected:
	int			nType;
	int			nFrame_X, nFrame_Y;
	int			n_uvTimer;
	tUV			uv;
	bool		bRepeatableUV_Y;
	bool		bRepeatableUV_X;
	bool		bIsInUse;
	bool		bMovementInProgress;
	float		fXTileSize;
	float		fYTileSize;
	float		fAcceleration;
	float		fMirrorAnimation;
	//動けるオブジェクト
	bool		bIsMoveAble;
	bool		bReversingToInitialPos;
	XMFLOAT2	xm2MovingPlatformSpeed;
	XMFLOAT2	xm2InitialMovingPos;
	XMFLOAT2	xm2MoveableFinalPos;
	XMFLOAT2	x2GOInitialPos;
	int			nDelayFrames;
	int			nFrameDelayCounter;
	XMFLOAT2	x2Acceleration;
	int			nID_Room;
	bool		bDontDraw;
private:
	
	int			n_goID;//ゲームオブジェクトID
	//花の変数
	int			nInUseTimer;
	bool		bEndFlower;
	bool		bIsUIObject;
	//敵のロゴに関する変数
	bool bLogoActivated;
	bool bLogoShown;
	int nFramesActivatedLogo;
	int nMaxLogoTime;
public:
	GameObject();
	virtual ~GameObject();
	void	Init();
	void	Init(HitBox hbox);
	void	Init(int n_Type);
	void	Init(ID3D11ShaderResourceView * n_gpTexture, int n_Type);
	void	InitEnemyIntroductionLogo(int EnemyType);
	HitBox	GetHitBoxWithoutPosition();
	void	SetNewHitbox(HitBox newHB);
	virtual void	Update();
	void	BackgroundControl();
	void	RegularUVAnimationControl(int speed);
	void	MoveControl();
	void	SetUpMoveControl(XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed);
	void	SetUpMoveControl(XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, float MovingSpeed);
	void	FlowerUpdate();
	void	AnimatedLogoUpdate();
	void	HaloUpdate();
	virtual void	Draw();
	virtual void	End();
	void	SetType(int Type);
	void	SetUse(bool b_use);
	void	SetVisibility(bool bVis);
	void	SetFlowerInUse();
	void	SetUIObject(bool isUI);
	void	SetFramesForUV(float x, float y);
	void	SetAcceleration(XMFLOAT2 newAccel);
	void	SetRoom(int newID_Room);
	void	SetRepeatableUV_X(bool bRepeatable);
	void	SetRepeatableUV_Y(bool bRepeatable);
	void	ActivateIntroductionLogo();
	void	SetDelayFrames(int);
	void	SetObjectInitialPos(XMFLOAT2 initPos);
	void	SetNewID(int nID);
	int		GetType();
	int		GetMovementDelayFrames();
	int		GetGameObjectID();
	float	GetTileSizeX();
	float	GetTileSizeY();
	bool	IsInUse();
	bool	WasLogoShown();
	bool	IsMoveable();
	XMFLOAT2 GetObjectInitialPos();
	XMFLOAT2 GetMovingInitialPos();
	XMFLOAT2 GetMovingFinalPos();
	XMFLOAT2 GetMovingSpeed();
};

int GetNumOfGameObjectsSetInMemory();