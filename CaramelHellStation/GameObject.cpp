//*****************************************************************************
// GameObject.cpp
// ゲームの一般的なオブジェクトの管理
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
// マクロ定義
//*****************************************************************************
#define PRINT_HITBOX true && SHOW_HITBOXES
#define FLOWER_TIME 24

//*****************************************************************************
// グローバル変数
//*****************************************************************************
int nGameObjectsCount = 0;//作ったゲームオブジェクト

//*****************************************************************************
// コンストラクタ関数
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
//Init関数
//初期化の関数
//引数：void
//戻：void
//*****************************************************************************
void GameObject::Init()
{
	InitSprite();
	bIsInUse = true;
}

//*****************************************************************************
//Init関数
//初期化の関数
//引数：HitBox
//戻：void
//*****************************************************************************
void GameObject::Init(HitBox hbox)
{
	InitSprite();
	hitbox = hbox;
	bIsInUse = true;
}

//*****************************************************************************
//Init関数
//初期化の関数
//引数：int
//戻：void
//*****************************************************************************
void GameObject::Init(int n_Type)
{
	InitSprite();
	SetType(n_Type);
}

//*****************************************************************************
//Init関数
//初期化の関数
//引数：ID3D11ShaderResourceView*, int
//戻：void
//*****************************************************************************
void GameObject::Init(ID3D11ShaderResourceView * n_gpTexture, int n_Type)
{
	InitSprite(n_gpTexture);
	SetType(n_Type);
	bIsInUse = true;
}

//*****************************************************************************
//InitEnemyIntroductionLogo関数
//敵のロゴの初期化の関数
//引数：int
//戻：void
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
//GetHitBoxWithoutPosition関数
//ヒットボックスを戻す（オブジェクトの拠点と関係ない）
//引数：void
//戻：HitBox
//*****************************************************************************
HitBox GameObject::GetHitBoxWithoutPosition()
{
	return hitbox;
}

//*****************************************************************************
//SetNewHitbox関数
//ヒットボックスを設定する
//引数：HitBox
//戻：void
//*****************************************************************************
void GameObject::SetNewHitbox(HitBox newHB)
{
	hitbox = newHB;
}

//*****************************************************************************
//Update関数
//処理のアップデートの関数
//引数：void
//戻：void
//*****************************************************************************
void GameObject::Update()
{
	if (!bIsInUse)
		return;
	switch (nType)
	{
	case ENEMY_INTRODUCTION_LOGO://敵の紹介ロゴ（チュートリアル）
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
	case SCROLLING_BACKGROUND_TYPE://メニューのバックグラウンド
		SetSpriteSize(WINDOW_WIDTH, WINDOW_HEIGHT);
		uv.U += 0.0025f;
		break;
	case GO_HALO_TYPE://プレイヤーのハローオブジェクト
		HaloUpdate();
		break;
	case ANIMATED_TITLE_TYPE://タイトルのロゴのアニメーション管理
		AnimatedLogoUpdate();
		break;
	case NOISE_TYPE://ゲームオーバーのアニメーション管理
		nFrame_Y = 3;
		nFrame_X = 1;
		uv.U = 0;
		if (++n_uvTimer > 3)
		{
			n_uvTimer = 0;
			uv.V++;
		}
		break;
	case ANIMATED_LOGO_TYPE://ロゴのアニメーション
		if (uv.V < 29)
		{
			if (++n_uvTimer > 3)
			{
				n_uvTimer = 0;
				uv.V++;
			}
		}
		break;
	case FLOWER_TYPE://プレイヤーの花の管理
		FlowerUpdate();
		break;
	case ROTATING_OBJECT_TYPE://回転するオブジェクト
		Rotation.y++;
		if (Rotation.y == 360)
			Rotation.y = 0;
		break;
	case AURA_EFFECT_TYPE://オーラーの管理
		Size.x+=85; 
		Size.y+=85;
		if (Size.x > 2831)
			bIsInUse = false;
		break;
	case MINI_AURA_EFFECT_TYPE://ミニオーラーの管理
		Size.x += 10;
		Size.y += 10;
		if(color.w>0)
			SetAlpha(color.w-0.035f);
		if (color.w < 0)
			SetAlpha(0);
		if (color.w==0)
			bIsInUse = false;
		break;
	case BACKGROUND_OBJECT_TYPE://バックグラウンドオブジェクトの管理
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
//BackgroundControl関数
//バックグラウンドの管理
//引数：void
//戻：void
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
//RegularUVAnimationControl関数
//UVのアニメーションの管理
//引数：int
//戻：void
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
//SetUpMoveControl関数
//動くことを設定する
//引数：XMFLOAT2, XMFLOAT2, XMFLOAT2
//戻：void
//*****************************************************************************
void GameObject::SetUpMoveControl(XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed)
{
	bIsMoveAble = true;
	xm2MovingPlatformSpeed = MovingSpeed;
	xm2InitialMovingPos = InitialPos;
	xm2MoveableFinalPos = FinalPos;
}

//*****************************************************************************
//SetUpMoveControl関数
//動くことを設定する
//引数：XMFLOAT2, XMFLOAT2, float
//戻：void
//*****************************************************************************
void GameObject::SetUpMoveControl(XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, float MovingSpeed)
{
	bIsMoveAble = true;
	xm2MovingPlatformSpeed = { MovingSpeed, MovingSpeed };
	xm2InitialMovingPos = InitialPos;
	xm2MoveableFinalPos = FinalPos;
}

//*****************************************************************************
//FlowerUpdate関数
//花オブジェクトの変更処理
//引数：void
//戻：void
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
//AnimatedLogoUpdate関数
//ロゴのアニメーションの管理
//引数：void
//戻：void
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
//HaloUpdate関数
//ハローのアップデート関数
//引数：void
//戻：void
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
//Draw関数
//レンダリング関数
//引数：void
//戻：void
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
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void GameObject::End()
{
	UninitSprite();
	nGameObjectsCount--;
}

//*****************************************************************************
//SetType関数
//オブジェクトのタイプを設定する
//引数：int
//戻：void
//*****************************************************************************
void GameObject::SetType(int Type)
{
	nType = Type;
}

//*****************************************************************************
//SetUse関数
//オブジェクトを使うことを設定する
//引数：bool
//戻：void
//*****************************************************************************
void GameObject::SetUse(bool b_use)
{
	bIsInUse = b_use;
}

//*****************************************************************************
//SetVisibility関数
//オブジェクトが見えることを設定する
//引数：bool
//戻：void
//*****************************************************************************
void GameObject::SetVisibility(bool bVis)
{
	if (!bVis)
		bDontDraw = true;
	else
		bDontDraw = false;
}

//*****************************************************************************
//SetFlowerInUse関数
//花オブジェクトを使うことを設定する
//引数：void
//戻：void
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
//SetUIObject関数
//オブジェクトはUIとしてを設定する
//引数：bool
//戻：void
//*****************************************************************************
void GameObject::SetUIObject(bool isUI)
{
	bIsUIObject = isUI;
}

//*****************************************************************************
//SetFramesForUV関数
//UVのフレームを設定する
//引数：float, float
//戻：void
//*****************************************************************************
void GameObject::SetFramesForUV(float x, float y)
{
	nFrame_X = (int)x;
	nFrame_Y = (int)y;
}

//*****************************************************************************
//SetAcceleration関数
//加速度を設定する
//引数：XMFLOAT2
//戻：void
//*****************************************************************************
void GameObject::SetAcceleration(XMFLOAT2 newAccel)
{
	x2Acceleration = newAccel;
}

//*****************************************************************************
//SetRoom関数
//ルームIDを設定する
//引数：int
//戻：void
//*****************************************************************************
void GameObject::SetRoom(int newID_Room)
{
	nID_Room = newID_Room;//CHECK
}

//*****************************************************************************
//SetRepeatableUV_X関数
//UVの繰り返しを設定する（X座標）
//引数：bool
//戻：void
//*****************************************************************************
void GameObject::SetRepeatableUV_X(bool bRepeatable)
{
	bRepeatableUV_X = bRepeatable;
}

//*****************************************************************************
//SetRepeatableUV_Y関数
//UVの繰り返しを設定する（Y座標）
//引数：bool
//戻：void
//*****************************************************************************
void GameObject::SetRepeatableUV_Y(bool bRepeatable)
{
	bRepeatableUV_Y = bRepeatable;
}

//*****************************************************************************
//GetType関数
//オブジェクトの種類を戻す
//引数：void
//戻：int
//*****************************************************************************
int GameObject::GetType()
{
	return nType;
}

//*****************************************************************************
//GetMovementDelayFrames関数
//オブジェクトのディレイフレームを戻す
//引数：void
//戻：int
//*****************************************************************************
int GameObject::GetMovementDelayFrames()
{
	return nDelayFrames;
}

//*****************************************************************************
//GetGameObjectID関数
//オブジェクトのIDを戻す
//引数：void
//戻：int
//*****************************************************************************
int GameObject::GetGameObjectID()
{
	return n_goID;
}

//*****************************************************************************
//GetTileSizeX関数
//オブジェクトのタイルXのサイズを戻す
//引数：void
//戻：float
//*****************************************************************************
float GameObject::GetTileSizeX()
{
	return fXTileSize;
}

//*****************************************************************************
//GetTileSizeX関数
//オブジェクトのタイルYのサイズを戻す
//引数：void
//戻：float
//*****************************************************************************
float GameObject::GetTileSizeY()
{
	return fYTileSize;
}

//*****************************************************************************
//IsInUse関数
//オブジェクトを使っていることを確認する
//引数：void
//戻：int
//*****************************************************************************
bool GameObject::IsInUse()
{
	return bIsInUse;
}

//*****************************************************************************
//WasLogoShown関数
//ロゴを見せたことを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool GameObject::WasLogoShown()
{
	return bLogoShown;
}

//*****************************************************************************
//IsMoveable関数
//オブジェクトが動けることを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool GameObject::IsMoveable()
{
	return bIsMoveAble;
}

//*****************************************************************************
//ActivateIntroductionLogo関数
//ロゴを有効する
//引数：void
//戻：void
//*****************************************************************************
void GameObject::ActivateIntroductionLogo()
{
	bLogoActivated = true;
}

//*****************************************************************************
//SetDelayFrames関数
//ディレイフレームを設定する
//引数：int
//戻：void
//*****************************************************************************
void GameObject::SetDelayFrames(int newFrames)
{
	nDelayFrames = newFrames;
}

//*****************************************************************************
//SetObjectInitialPos関数
//最初の拠点を設定する
//引数：XMFLOAT2
//戻：void
//*****************************************************************************
void GameObject::SetObjectInitialPos(XMFLOAT2 initPos)
{
	x2GOInitialPos = initPos;
}

//*****************************************************************************
//SetObjectInitialPos関数
//IDを設定する
//引数：int
//戻：void
//*****************************************************************************
void GameObject::SetNewID(int nID)
{
	n_goID = nID;
}

//*****************************************************************************
//GetObjectInitialPos関数
//最初の拠点を戻す
//引数：void
//戻：XMFLOAT2
//*****************************************************************************
XMFLOAT2 GameObject::GetObjectInitialPos()
{
	return x2GOInitialPos;
}

//*****************************************************************************
//GetMovingInitialPos関数
//最初の（動ける）拠点を戻す
//引数：void
//戻：XMFLOAT2
//*****************************************************************************
XMFLOAT2 GameObject::GetMovingInitialPos()
{
	return xm2InitialMovingPos;
}

//*****************************************************************************
//GetMovingInitialPos関数
//最後の（動ける）拠点を戻す
//引数：void
//戻：XMFLOAT2
//*****************************************************************************
XMFLOAT2 GameObject::GetMovingFinalPos()
{
	return xm2MoveableFinalPos;
}

//*****************************************************************************
//GetMovingSpeed関数
//動けるオブジェクトの速さを戻す
//引数：void
//戻：XMFLOAT2
//*****************************************************************************
XMFLOAT2 GameObject::GetMovingSpeed()
{
	return xm2MovingPlatformSpeed;
}


int GetNumOfGameObjectsSetInMemory()
{
	return nGameObjectsCount;
}