//*****************************************************************************
// Weapon.cpp
// 武器を管理する
// Author : Mane
//*****************************************************************************
#include "Weapon.h"
#include "GameInputManager.h"
#include "Camera.h"
#include <stdio.h>
#include "Player.h"
#include "TextureManager.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define HANDS_SCALE				0.95f
#define GUITAR_SCALE			0.30f
#define FLIPPING_SPEED			180
#define PRINT_WEAPON_HITBOX		true
#define UNLIMITED_AIR_DASHING	true
#define BULLET_COOLDOWN			5
#define COMMON_BULLET_SPEED		20

//*****************************************************************************
// エナム定義
//*****************************************************************************
enum AttackAirType {

	AIR_ONLY = 0,
	GROUND_ONLY,
	TYPE_BOTH
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	char WeaponInput[MAX_PLAYER_INPUT];
	bool bResetInputs;
	AttackAirType eAirAttack;
}Weapons_Inputs;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
int nAltHands = -1;
Weapon* pCurrentGuitar = nullptr;
Weapons_Inputs tHandInputs[MAX_HAND_STATES] =
{									    
{ { "A"        },	false   ,	   TYPE_BOTH    }, //	HAND_PUNCH_A_STATE=0
{ { "N"        },	false   ,	   TYPE_BOTH    }, //	HAND_PUNCH_B_STATE
{ { "DA"       },	true    ,	   AIR_ONLY		},//	HAND_DOWNSTOGROUND_STATE
{ { "DBA"      },	true    ,	   TYPE_BOTH    },//	HAND_DUAL_A_STATE
{ { "AASA"     },	true    ,	   GROUND_ONLY  },//	HAND_DUAL_B_STATE
{ { "UA"       },	true    ,	   GROUND_ONLY  },//	HAND_GROUND_A_STATE
{ { "AASA"     },	true    ,	   AIR_ONLY		},//	HAND_SPIN_STATE
{ { "N"        },	false   ,	   TYPE_BOTH    },//	HAND_DOWN_IDLE_STATE
{ { "N"        },	false   ,	   TYPE_BOTH    },//	HAND_IDLE_STATE
{ { "ASA"       },	true    ,	   GROUND_ONLY  },//	HAND_UPPERCUT
{ { "DA"       },	true    ,	   GROUND_ONLY  },//	HAND_STINGER
{ { "BDFA"     },	true    ,	   GROUND_ONLY  },//	HAND_STOMP
{ { "AAAAA"	   },	true    ,	   TYPE_BOTH    },//	HAND_STRONGPUNCH_A
{ { "UFA"	   },	true    ,      TYPE_BOTH	},//	HAND_STRONGPUNCH_B
{ { "UA"       },	true    ,	   AIR_ONLY		},//	HAND_SENDDOWN
{ { "FUA"      },	true    ,      TYPE_BOTH    },//	HAND_STRONGPUNCH_A_ALT
{ { "DFA"      },	true    ,      GROUND_ONLY  },//	HAND_UPPERCUT_ALT
{ { "DFA"      },	true    ,      AIR_ONLY	    },//	HAND_AIR_DASH
};			

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
Weapon::Weapon()										
{
	nBulletCoolDown = 0;
	nState = 0;
	bShowWeapon = false;
	bEndAttack=false;
	nAnimationTimer = 0;
	nDashTimer = 0; 
	hitbox = { 0,0,0,0, PRINT_WEAPON_HITBOX, nullptr };
	bAirDashExecutedOnce = false;
	uv = { 0,0 };
	nFrame_X = 0;
	nFrame_Y = 0;
	nType = WEAPON_MAX;
	spPlayer = nullptr;
	nDirection = 1;
	nCurrentAttackPlaying = -1;
	fDashAttackSpeed = 0;
	uvMirrorDirection = 1; 
	nIdleState = 0;
	bIsAttacking = false;
	bDontShowWeaponDuringAttack = false;
	nCoolDownDashAttack = 0;
	for (int i = 0; i < MAX_BULLETS; i++) {
		pBullets[i] = nullptr;
	}
	for (int i = 0; i < MAX_CATCHBULLETS; i++)
		pCatchBullets[i] = nullptr;
}


Weapon::~Weapon()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化の関数
//引数：int
//戻：void
//*****************************************************************************
void Weapon::Init(int Type)
{
	spPlayer = nullptr;
	nType = Type;
	uvMirrorDirection = 1;
	bDontShowWeaponDuringAttack = false;
	bIsAttacking = false;
	SetAlpha(0);
	switch (nType)
	{
	case WEAPON_HANDS:
		InitSprite();
		gpTexture = GetTexture(TEXTURE_WEAPON_HANDS);
		nState = HAND_IDLE_STATE;
		nIdleState = HAND_IDLE_STATE;
		fDashAttackSpeed = 30;
		nFrame_X = 16;
		nFrame_Y = 10;
		break;
	case WEAPON_GUITAR:
		InitSprite();
		gpTexture =GetTexture(TEXTURE_WEAPON_GUITAR);
		nState = GUITAR_FRONTAL;
		nIdleState = GUITAR_FRONTAL;
		nFrame_X = 12;
		nFrame_Y = 1;
		for (int i = 0; i < MAX_BULLETS; i++)
			pBullets[i] = new Bullet(PLAYER_BULLET_TYPE);
		for (int i = 0; i < MAX_CATCHBULLETS; i++)
			pCatchBullets[i] = new Bullet(PLAYER_CATCHBULLET_TYPE);
		pCurrentGuitar = this;
		break;
	default:
		break;
	}
}

//*****************************************************************************
//Update関数
//処理のアップデートの関数
//引数：void
//戻：void
//*****************************************************************************
void Weapon::Update()
{
	if (spPlayer == nullptr) {
		spPlayer = GetPlayerPointer();
		return;
	}
	Player *pPlayer = (Player*)spPlayer;
	if (nCoolDownDashAttack > 0)
		nCoolDownDashAttack--;
	float alpha = color.w;
	if (UNLIMITED_AIR_DASHING)
		bAirDashExecutedOnce = false;
	if (bShowWeapon)
	{
		if (alpha < 1)
			alpha += 0.075f;
		if (alpha > 1)
			alpha = 1;
	}
	else {
		if (alpha > 0)
			alpha -= 0.075f;
		if (alpha < 0)
			alpha = 0;
	}
	SetAlpha(alpha);
	switch (nType)
	{
	case WEAPON_HANDS:
		HandsUpdateControl();
		break;
	case WEAPON_GUITAR:
		UpdateGuitarControl();
		break;
	default:
		break;
	}
	ControlDirection();
}

//*****************************************************************************
//UpdateGuitarControl関数
//ギター武器の処理アップデートの関数
//引数：void
//戻：void
//*****************************************************************************
void Weapon::UpdateGuitarControl()
{
	Player* pPlayer = (Player*)spPlayer;
	if (!pPlayer)
		return;
	bool bSetUpBullet = false;
	float alpha = color.w;
	
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (pBullets[i] && !pBullets[i]->IsInUse() && GetInput(INPUT_SHOOT) && !GetInput(INPUT_CATCHSHOOT))
		{
			if (bSetUpBullet || nBulletCoolDown<BULLET_COOLDOWN)continue;
			if (alpha != 1) continue;
			bSetUpBullet = true;
			pBullets[i]->SetUse(true);
			pBullets[i]->SetType(PLAYER_BULLET_TYPE);
			float fDirectionPlayer = (float)pPlayer->GetDirection();
			switch (nState)
			{
			case GUITAR_FRONTAL:
				pBullets[i]->SetLocalPosition(Position.x + 110* fDirectionPlayer, Position.y);
				pBullets[i]->SetBulletSpeed({ COMMON_BULLET_SPEED * fDirectionPlayer, 0 });
				break;
			case GUITAR_DIAGONAL_UP:
				pBullets[i]->SetLocalPosition(Position.x + 90 * fDirectionPlayer, Position.y+90);
				pBullets[i]->SetBulletSpeed({ COMMON_BULLET_SPEED * fDirectionPlayer, COMMON_BULLET_SPEED });
				break;
			case GUITAR_UP:
				pBullets[i]->SetLocalPosition(Position.x, Position.y +125);
				pBullets[i]->SetBulletSpeed({ 0, COMMON_BULLET_SPEED });
				break;
			case GUITAR_DIAGONAL_DOWN:
				pBullets[i]->SetLocalPosition(Position.x + 90 * fDirectionPlayer, Position.y - 90);
				pBullets[i]->SetBulletSpeed({ COMMON_BULLET_SPEED * fDirectionPlayer, -COMMON_BULLET_SPEED });
				break;
			case GUITAR_DOWN:
				pBullets[i]->SetLocalPosition(Position.x, Position.y - 125);
				pBullets[i]->SetBulletSpeed({ 0, -COMMON_BULLET_SPEED });
				break;
			default:
				break;
			}
		}
		if (pBullets[i])
			pBullets[i]->Update();
	}

	for (int i = 0; i < MAX_CATCHBULLETS; i++)
	{
		if ((pPlayer->GetCatchedEnemy()))break;
		if (pCatchBullets[i] && !pCatchBullets[i]->IsInUse() && GetInput(INPUT_CATCHSHOOT))
		{
			if (bSetUpBullet)continue;
			if (alpha != 1) continue;
			bSetUpBullet = true;
			pCatchBullets[i]->SetUse(true);
			float fDirectionPlayer = (float)pPlayer->GetDirection();
			switch (nState)
			{
			case GUITAR_FRONTAL:
				pCatchBullets[i]->SetLocalPosition(Position.x + 110 * fDirectionPlayer, Position.y);
				pCatchBullets[i]->SetBulletSpeed({ COMMON_BULLET_SPEED*2 * fDirectionPlayer, 0 });
				break;
			case GUITAR_DIAGONAL_UP:
				pCatchBullets[i]->SetLocalPosition(Position.x + 90 * fDirectionPlayer, Position.y + 90);
				pCatchBullets[i]->SetBulletSpeed({ COMMON_BULLET_SPEED*2 * fDirectionPlayer, COMMON_BULLET_SPEED });
				break;
			case GUITAR_UP:
				pCatchBullets[i]->SetLocalPosition(Position.x, Position.y + 125);
				pCatchBullets[i]->SetBulletSpeed({ 0, COMMON_BULLET_SPEED*2 });
				break;
			case GUITAR_DIAGONAL_DOWN:
				pCatchBullets[i]->SetLocalPosition(Position.x + 90 * fDirectionPlayer, Position.y - 90);
				pCatchBullets[i]->SetBulletSpeed({ COMMON_BULLET_SPEED*2 * fDirectionPlayer, -COMMON_BULLET_SPEED });
				break;
			case GUITAR_DOWN:
				pCatchBullets[i]->SetLocalPosition(Position.x, Position.y - 125);
				pCatchBullets[i]->SetBulletSpeed({ 0, -COMMON_BULLET_SPEED });
				break;
			default:
				break;
			}
		}
		pCatchBullets[i]->Update();
	}

	if (++nBulletCoolDown > BULLET_COOLDOWN)
	{
		nBulletCoolDown = 0;
	}
	nFrame_X = 12;
	nFrame_Y = 1;
	SetSpriteSize(960 * GUITAR_SCALE, 292 * GUITAR_SCALE);
	SetSpriteFrameSize(615, 615);
	int nPlayerCurrentState = pPlayer->GetPlayerState();
	nDirection = pPlayer->GetDirection();
	Position = pPlayer->GetWorldPosition();
	Position.y -= 30;
	if (++nAnimationTimer > 2) {
		nAnimationTimer = 0;
		uv.U += uvMirrorDirection;
		if (uv.U == nFrame_X - 1 || uv.U == 0)
		{
			uvMirrorDirection *= -1;
		}
	}
	switch (nState)
	{
	case GUITAR_FRONTAL:
		if (Rotation.z>0)
			Rotation.z -= 20;
		if (Rotation.z < 0)
			Rotation.z = 0;
		break;
	case GUITAR_DIAGONAL_UP:
		if (Rotation.z<45)
			Rotation.z += 20;
		if (Rotation.z > 45)
			Rotation.z = 45;
		break;
	case GUITAR_UP:
		if (Rotation.z<90)
			Rotation.z += 20;
		if (Rotation.z > 90)
			Rotation.z = 90;
		break;
	case GUITAR_DIAGONAL_DOWN:
		if (Rotation.z>-45)
			Rotation.z -= 20;
		if (Rotation.z < -45)
			Rotation.z = -45;
		break;
	case GUITAR_DOWN:
		if (Rotation.z>-90)
			Rotation.z -= 20;
		if (Rotation.z < -90)
			Rotation.z = -90;
		break;
	default:
		break;
	}
}


//*****************************************************************************
//HandsUpdateControl関数
//手武器の処理アップデートの関数
//引数：void
//戻：void
//*****************************************************************************
void Weapon::HandsUpdateControl()
{
	nFrame_X = 16;
	nFrame_Y = 10;
	SetSpriteSize(960 * HANDS_SCALE, 540 * HANDS_SCALE);
	SetSpriteFrameSize(615, 615);
	Player* pPlayer = (Player*)spPlayer;
	int nPlayerCurrentState = pPlayer->GetPlayerState();
	nDirection = pPlayer->GetDirection();
	bIsAttacking = nState != HAND_IDLE_STATE && nState != HAND_DOWN_IDLE_STATE;
	if (pPlayer->IsPlayerOnTheGround())
		bAirDashExecutedOnce = false;
	switch (nState)
	{
	case HAND_IDLE_STATE:
		nDashTimer = 0;
		bDontShowWeaponDuringAttack = false;
		hitbox = { 0,0,0,0, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		uv.U = (float)nState;
		if (nAnimationTimer < 1)
			nAnimationTimer++;
		else {
			uv.V++;
			nAnimationTimer = 0;
		}
		Position.x = pPlayer->GetWorldPosition().x + (110 * (nDirection));
		Position.y = pPlayer->GetWorldPosition().y + 2;
		nCurrentAttackPlaying = -1;
		if (nPlayerCurrentState == PLAYER_DOWN_STATE)
			nState = HAND_DOWN_IDLE_STATE;
		break;

	case HAND_DOWN_IDLE_STATE:
		bDontShowWeaponDuringAttack = false;
		hitbox = { 0,0,0,0, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		uv.U = (float)nState;
		if (nAnimationTimer < 1)
			nAnimationTimer++;
		else {
			uv.V++;
			nAnimationTimer = 0;
		}
		Position.x = pPlayer->GetWorldPosition().x + (140 * (nDirection));
		Position.y = pPlayer->GetWorldPosition().y + 2;
		if (nPlayerCurrentState == PLAYER_IDLE_STATE)
			nState = HAND_IDLE_STATE;
		break;

	case HAND_PUNCH_A_STATE:
		bDontShowWeaponDuringAttack = false;
		hitbox = { uv.V*22*(pPlayer->GetDirection()),0,100,100, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		if(uvMirrorDirection==-1)
			hitbox = { 0,0,0,0, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		uv.U = (float)nState;
		uv.V+=uvMirrorDirection;
		if (uv.V == 9)
			uvMirrorDirection = -1;
		pPlayer->SetPlayerUVnum(PLAYER_ATTACK_A_UV);
		Position.x = pPlayer->GetWorldPosition().x + (110 * (nDirection));
		Position.y = pPlayer->GetWorldPosition().y + 2;
		if (uv.V < 0)
		{
			nState = HAND_IDLE_STATE;
			uv.V = 0;
			pPlayer->SetPlayerState(PLAYER_IDLE_STATE);
		}
		if (uv.V == 8 && uvMirrorDirection<0)
			pPlayer->SetLockInputs(false);
		nAltHands = HAND_PUNCH_A_STATE;
		break;

	case HAND_PUNCH_B_STATE:
		bDontShowWeaponDuringAttack = false;
		hitbox = { uv.V * 18 * (pPlayer->GetDirection()),50,100,100, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		if (uvMirrorDirection == -1)
			hitbox = { 0,0,0,0, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		uv.U = (float)nState;
		uv.V += uvMirrorDirection;
		if (uv.V == 9)
			uvMirrorDirection = -1;
		pPlayer->SetPlayerUVnum(PLAYER_ATTACK_B_UV);
		Position.x = pPlayer->GetWorldPosition().x + (110 * (nDirection));
		Position.y = pPlayer->GetWorldPosition().y + 2;
		if (uv.V < 0)
		{
			nState = HAND_IDLE_STATE;
			uv.V = 0;
			pPlayer->SetPlayerState(PLAYER_IDLE_STATE);
		}
		if (uv.V == 8 && uvMirrorDirection<0)
			pPlayer->SetLockInputs(false);
		nAltHands = HAND_PUNCH_B_STATE;
		break;

	case HAND_GROUND_A_STATE:
		bDontShowWeaponDuringAttack = false;
		if (uv.V > 3 && uvMirrorDirection>0) 
			hitbox = { (float)(-130) * (pPlayer->GetDirection()),(uv.V * (-22)) + 80,325,100, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		else
			hitbox = { 0,0,0,0, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		uv.U = (float)nState;
		uv.V += uvMirrorDirection;
		if (uv.V == 9) {
			PlaySound(SOUND_LABEL_SE_ALTHIT);
			VibrateXinput(65535, 65535, 10);
			CameraTremble(12, 12, 5);
			uvMirrorDirection = -1;
		}
		Position.x = pPlayer->GetWorldPosition().x + (130 * (nDirection));
		Position.y = pPlayer->GetWorldPosition().y + 2;
		if (uv.V < 0)
		{
			nState = HAND_DOWN_IDLE_STATE;
			uv.V = 0;
			pPlayer->SetPlayerState(PLAYER_IDLE_STATE);
		}
		if (uv.V == 8 && uvMirrorDirection<0)
			pPlayer->SetLockInputs(false);
		pPlayer->SetPlayerUVnum(PLAYER_ATTACK_STRONG_UV);
		break;

	case HAND_SPIN_STATE:
		bDontShowWeaponDuringAttack = false;
		uv.U = (float)nState;
		hitbox = { (float)(-140) * (pPlayer->GetDirection()),-50,280,70, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		if (uv.V == 0 && uvMirrorDirection > 0) {
			if (!(pPlayer->GetJumpAttackLimited()))
				pPlayer->Jump(40);
			pPlayer->SetJumpAttack(true);
		}
		if (pPlayer->GetYForce() > 0 )
		{
			pPlayer->SetJumpAttack(false);
		}
		if (nAnimationTimer < 1)
			nAnimationTimer++;
		else {
			uv.V += uvMirrorDirection;
			nAnimationTimer = 0;
		}
		if (uv.V == 9)
			uvMirrorDirection = -1;
		Position.x = pPlayer->GetWorldPosition().x + (135 * (nDirection));
		Position.y = pPlayer->GetWorldPosition().y +100;
		if (uv.V < 0)
		{
			nState = HAND_DOWN_IDLE_STATE;
			uv.V = 0;
			pPlayer->SetPlayerState(PLAYER_IDLE_STATE);
			pPlayer->SetLockInputs(false);
			pPlayer->SetYForce(-10);
		}
		pPlayer->SetPlayerUVnum(PLAYER_FALLING_UV);
		break;

	case HAND_STRONGPUNCH_A:
		bDontShowWeaponDuringAttack = false;
		if (uv.V > 5 && uvMirrorDirection>0 || uv.V > 6 && uvMirrorDirection<0)
			hitbox = { (float)(pPlayer->GetDirection())*((uv.V * 10) + (uv.V * 25)),0,65 * uv.V,45 * uv.V, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		else
			hitbox = { 0,0,0,0, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		uv.U = (float)nState;
		if (nAnimationTimer < 2)
			nAnimationTimer++;
		else {
			uv.V += uvMirrorDirection;
			nAnimationTimer = 0;
		}
		if (uv.V == 9)
			uvMirrorDirection = -1;
		Position.x = pPlayer->GetWorldPosition().x + (90 * (nDirection));
		Position.y = pPlayer->GetWorldPosition().y - 10;
		if (uv.V < 0)
		{
			nState = HAND_IDLE_STATE;
			uv.V = 0;
			pPlayer->SetPlayerState(PLAYER_IDLE_STATE);
		}
		if (uv.V == 8 && uvMirrorDirection<0)
			pPlayer->SetLockInputs(false);
		pPlayer->SetPlayerUVnum(PLAYER_ATTACK_STRONG_UV);
		break;

	case HAND_STRONGPUNCH_B:
		bDontShowWeaponDuringAttack = false;
		if (uv.V > 5 && uvMirrorDirection>0 || uv.V > 6 && uvMirrorDirection<0)
			hitbox = { (float)(pPlayer->GetDirection())*((uv.V * 10) + (uv.V * 15)),0,45 * uv.V,45 * uv.V, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		else
			hitbox = { 0,0,0,0, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		uv.U = (float)nState;
		if (nAnimationTimer < 1)
			nAnimationTimer++;
		else {
			uv.V += uvMirrorDirection;
			nAnimationTimer = 0;
			if (uv.V == 9)
				nAnimationTimer = -8;
		}
		if (uv.V == 9)
			uvMirrorDirection = -1;
		Position.x = pPlayer->GetWorldPosition().x + (195 * (nDirection));
		Position.y = pPlayer->GetWorldPosition().y +20;
		if (uv.V < 0)
		{
			nState = HAND_IDLE_STATE;
			uv.V = 0;
			pPlayer->SetPlayerState(PLAYER_IDLE_STATE);
		}
		if (uv.V == 8 && uvMirrorDirection<0)
			pPlayer->SetLockInputs(false);
		pPlayer->SetPlayerUVnum(PLAYER_ATTACK_STRONG_UV);
		break;

	case HAND_STRONGPUNCH_A_ALT:
		bDontShowWeaponDuringAttack = false;
		nState = HAND_STRONGPUNCH_A;
		HandsUpdateControl();
		return;

	case HAND_DUAL_A_STATE:
		bDontShowWeaponDuringAttack = false;
		if (uv.V > 6 && uvMirrorDirection>0)
			hitbox = { (float)60 *(pPlayer->GetDirection()),0,300,180, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		else
			hitbox = { 0,0,0,0, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		uv.U = (float)nState;
		if (nAnimationTimer < 1)
			nAnimationTimer++;
		else {
			uv.V += uvMirrorDirection;
			nAnimationTimer = 0;
			if (uv.V == 9)
				nAnimationTimer = -8;
		}
		if (uv.V == 8 && uvMirrorDirection > 0) {
			PlaySound(SOUND_LABEL_SE_MEDIUMHIT);
			VibrateXinput(65535, 65535, 10);
			CameraTremble(14, 14, 7);
		}
		if (uv.V == 9)
			uvMirrorDirection = -1;

		Position.x = pPlayer->GetWorldPosition().x + (195 * (nDirection));
		Position.y = pPlayer->GetWorldPosition().y + 20;
		if (uv.V < 0)
		{
			nState = HAND_IDLE_STATE;
			uv.V = 0;
			pPlayer->SetPlayerState(PLAYER_IDLE_STATE);
		}
		if (uv.V == 1 && uvMirrorDirection<0)
			pPlayer->SetLockInputs(false);
		pPlayer->SetPlayerUVnum(PLAYER_ATTACK_STRONG_UV);
		break;

	case HAND_DUAL_B_STATE:
		bDontShowWeaponDuringAttack = false;
		if (uv.V > 6 && uvMirrorDirection>0)
			hitbox = { (float)60 * (pPlayer->GetDirection()),0,300,180, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		else
			hitbox = { 0,0,0,0, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		uv.U = (float)nState;
		if (nAnimationTimer < 1)
			nAnimationTimer++;
		else {
			uv.V += uvMirrorDirection;
			nAnimationTimer = 0;
			if (uv.V == 9)
				nAnimationTimer = -8;
		}
		if (uv.V == 9)
			uvMirrorDirection = -1;
		if (uv.V == 8 && uvMirrorDirection > 0) 
		{
			PlaySound(SOUND_LABEL_SE_MEDIUMHIT);
			VibrateXinput(65535, 65535, 10);
			CameraTremble(12, 12, 5);
		}
		Position.x = pPlayer->GetWorldPosition().x + (195 * (nDirection));
		Position.y = pPlayer->GetWorldPosition().y + 20;
		if (uv.V < 0)
		{
			nState = HAND_IDLE_STATE;
			uv.V = 0;
			pPlayer->SetPlayerState(PLAYER_IDLE_STATE);
		}
		if (uv.V == 1 && uvMirrorDirection<0)
			pPlayer->SetLockInputs(false);
		pPlayer->SetPlayerUVnum(PLAYER_ATTACK_STRONG_UV);
		break;

	case HAND_DOWNSTOGROUND_STATE:
		bDontShowWeaponDuringAttack = false;
		if (pPlayer->GetYForce() < 0)
			pPlayer->SetYForce(0);
		if (uv.V == 8) 
		{
			if(nDirection == RIGHT_DIR)
				hitbox = { -140,40,430,200, PRINT_WEAPON_HITBOX, hitbox.Sprite };
			else
				hitbox = { 140,40,430,200, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		}
		else
			hitbox = { 0,0,0,0, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		uv.U = (float)nState;
		if (bEndAttack)
		{
			if (uv.V == 5)
				pPlayer->SetLockInputs(false);
			if (nAnimationTimer < 2)
				nAnimationTimer++;
			else {
				uv.V--;
				nAnimationTimer = 0;
			}
			if (uv.V < 0) {
				nState = HAND_IDLE_STATE;
				uv.V = 0;
				pPlayer->SetLockInputs(false);
				pPlayer->SetPlayerState(PLAYER_IDLE_STATE);
				bEndAttack = false;
			}
			break;
		}
		uv.V++;
		if (uv.V == 8)
			pPlayer->SetFallAttack(true);
		if (uv.V == 10)
			uv.V = 8;
		Position.x = pPlayer->GetWorldPosition().x + (155 * (nDirection));
		Position.y = pPlayer->GetWorldPosition().y -190;

		
		if (pPlayer->IsPlayerOnTheGround()) {
			PlaySound(SOUND_LABEL_SE_MEDIUMHIT);
			VibrateXinput(65535, 65535, 10);
			bEndAttack = true;
			nAnimationTimer = -8;
		}
		pPlayer->SetPlayerUVnum(PLAYER_FALLING_UV);
		break;

	case HAND_UPPERCUT:
		bDontShowWeaponDuringAttack = false;
		if ((uv.V >= 3 && uvMirrorDirection && nCurrentAttackPlaying== HAND_UPPERCUT) ||
			(uv.V >= 1 && uvMirrorDirection && nCurrentAttackPlaying == HAND_UPPERCUT_ALT))
		{
			if (nDirection == RIGHT_DIR)
				hitbox = { -60,90,100,230, PRINT_WEAPON_HITBOX, hitbox.Sprite };
			else
				hitbox = { 60,90,100,230, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		}
		else
			hitbox = { 0,0,0,0, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		uv.U = (float)nState;
		if (nAnimationTimer < 1)
			nAnimationTimer++;
		else {
			uv.V += uvMirrorDirection;
			nAnimationTimer = 0;
			if (uv.V == 9)
				nAnimationTimer = -8;
		}
		if (uv.V == 9)
			uvMirrorDirection = -1;
		Position.x = pPlayer->GetWorldPosition().x + (195 * (nDirection));
		Position.y = pPlayer->GetWorldPosition().y + 20;
		if (uv.V < 0)
		{
			nState = HAND_IDLE_STATE;
			uv.V = 0;
			pPlayer->SetPlayerState(PLAYER_IDLE_STATE);
		}
		if (uv.V == 8 && uvMirrorDirection < 0) {
			pPlayer->SetLockInputs(false);
			
		}
		pPlayer->SetPlayerUVnum(PLAYER_LOCKON_UV);
		break;

	case HAND_UPPERCUT_ALT:
		bDontShowWeaponDuringAttack = false;
		pPlayer->Jump(40);
		nState = HAND_UPPERCUT;
		HandsUpdateControl();
		return;

	case HAND_STOMP:
		bDontShowWeaponDuringAttack = false;
		uv.U = (float)nState;
		hitbox = { 0,40,390,200, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		if (uv.V == 0)
		{
			Position.x = pPlayer->GetWorldPosition().x + (130 * (nDirection));
			Position.y = pPlayer->GetWorldPosition().y + 2;
		}
		if (nAnimationTimer < 1)
			nAnimationTimer++;
		else {
			uv.V += uvMirrorDirection;
			nAnimationTimer = 0;
		}
		if (uv.V == 9)
			uvMirrorDirection = -1;
		Position.x+=(20 *(pPlayer->GetDirection()));
		
		if (uv.V < 0)
		{
			nState = HAND_DOWN_IDLE_STATE;
			uv.V = 0;
			pPlayer->SetPlayerState(PLAYER_IDLE_STATE);
		}
		if (uv.V == 8 && uvMirrorDirection<0)
			pPlayer->SetLockInputs(false);
		pPlayer->SetPlayerUVnum(PLAYER_ATTACK_STRONG_UV);
		break;

	case HAND_SENDDOWN:
		bDontShowWeaponDuringAttack = false;
		if (uv.V >= 3 && uvMirrorDirection>0)
			hitbox = { 0,25,100,230, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		else
			hitbox = { 0,0,0,0, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		uv.U = (float)nState;
		if (nAnimationTimer < 1)
			nAnimationTimer++;
		else {
			uv.V += uvMirrorDirection;
			nAnimationTimer = 0;
			if (uv.V == 9)
				nAnimationTimer = -4;
		}
		if (uv.V == 9)
			uvMirrorDirection = -1;
		Position.x = pPlayer->GetWorldPosition().x + (195 * (nDirection));
		Position.y = pPlayer->GetWorldPosition().y + 20;
		if (uv.V < 0)
		{
			nState = HAND_IDLE_STATE;
			uv.V = 0;
			pPlayer->SetPlayerState(PLAYER_IDLE_STATE);
		}
		if (uv.V == 7) {
			pPlayer->SetLockInputs(false);

		}
		pPlayer->SetPlayerUVnum(PLAYER_LOCKON_UV);
		break;
	case DOWN_DASH_ATTACK:
		
		if (nDashTimer <= 0 && uv.V == 0)
		{
			bDontShowWeaponDuringAttack = true;
			nDashTimer = 25;
			pPlayer->SetPlayerUVnum(PLAYER_DASHGUITAR_UV);
			hitbox = { (float)10 * nDirection,-50,150,150, PRINT_WEAPON_HITBOX, hitbox.Sprite };
			Position.x = pPlayer->GetWorldPosition().x + (110 * (nDirection));
			Position.y = pPlayer->GetWorldPosition().y + 2;
		}
		uv.U = (float)nState;
		uv.V++;
		if (uv.V == 10)
			uv.V = 1;
		Position.x = pPlayer->GetWorldPosition().x + (80 * (nDirection));
		pPlayer->Translate(fDashAttackSpeed*(pPlayer->GetDirection()), 0);
		if (pPlayer->CheckCollisionWithWall(GetHitbox()))
		{
			nDashTimer = 0;
			pPlayer->Translate(fDashAttackSpeed*(pPlayer->GetDirection())*-1, 0);
			CameraTremble(15, 15, 5);
			VibrateXinput(65535, 65535, 10);
			PlaySound(SOUND_LABEL_SE_COLLISION_WALL);
		}
		if (--nDashTimer <= 0)
		{
			nState = HAND_IDLE_STATE;
			uv.V = 0;
			pPlayer->SetLockInputs(false);
			pPlayer->SetPlayerState(PLAYER_IDLE_STATE);
			bDontShowWeaponDuringAttack = false;
		}
		break;

	case HAND_AIR_DASH:
		hitbox = { (float)50 * nDirection,-25,250,150, PRINT_WEAPON_HITBOX, hitbox.Sprite };
		Position.x = pPlayer->GetWorldPosition().x + (110 * (nDirection));
		Position.y = pPlayer->GetWorldPosition().y + 2;
		nState = DOWN_DASH_ATTACK;
		pPlayer->SetPlayerUVnum(PLAYER_DASH_UV);
		nDashTimer = 18;
		HandsUpdateControl();
		bAirDashExecutedOnce = true;
		return;
	default:
		break;
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Weapon::Draw()
{
	if (nType == WEAPON_GUITAR)
	{
		for (int i = 0; i < MAX_BULLETS; i++)
		{
			if (!pBullets[i])continue;
			if (!pBullets[i]->IsInUse())continue;
			pBullets[i]->Draw();
		}
		for (int i = 0; i < MAX_CATCHBULLETS; i++)
		{
			if (!pCatchBullets[i]) continue;
			if (!pCatchBullets[i]->IsInUse())continue;
			pCatchBullets[i]->Draw();
		}
	}
	if (color.w == 0 || (bDontShowWeaponDuringAttack && nType != WEAPON_GUITAR))
		return;
	SetLocalPosition(Position.x, Position.y);
	SetSpriteFrameSize(1.0f / nFrame_X, 1.0f / nFrame_Y);
	SetSpriteUV(uv.U / (float)nFrame_X, uv.V / (float)nFrame_Y);
	DrawSprite(GetDeviceContext());

}

//*****************************************************************************
//ControlDirection関数
//武器の向こうをコントロールする
//引数：void
//戻：void
//*****************************************************************************
void Weapon::ControlDirection()
{
	if ((nDirection == RIGHT_DIR && Rotation.y > 0)) {
		Rotation.y -= FLIPPING_SPEED;
		if (Rotation.y < 0)
			Rotation.y = 0;
	}
	if ((nDirection == LEFT_DIR && Rotation.y < 180))
	{
		Rotation.y += FLIPPING_SPEED;
		if (Rotation.y >180)
			Rotation.y = 180;
	}
}

//*****************************************************************************
//ShowWeapon関数
//武器を表す管理する
//引数：bool
//戻：void
//*****************************************************************************
void Weapon::ShowWeapon(bool showweapon)
{
	bShowWeapon = showweapon;
}

//*****************************************************************************
//ExecuteAttack関数
//武器の攻撃動作を行う
//引数：char[256]
//戻：void
//*****************************************************************************
void Weapon::ExecuteAttack(char szInputs[256])
{
	ExecuteAttack(szInputs, MAX_PLAYER_INPUT + 1, false);
}

//*****************************************************************************
//ExecuteAttack関数
//武器の攻撃動作を行う
//引数：char[256], int, bool
//戻：void
//*****************************************************************************
void Weapon::ExecuteAttack(char szInputs[256], int RecursionNum, bool bVerifyLastChars)
{
	Player* pPlayer = (Player*)spPlayer;
	if (!pPlayer)
		return;
	int recursion = RecursionNum;
	RecursionNum--;
	bool verifiedchars = bVerifyLastChars;
	if (szInputs[0] == '*' || RecursionNum<-3)
		return;
	if (szInputs[0] == 'A' && szInputs[1] == '*' && GetInput(MOVE_DOWN) && !verifiedchars)
	{
		szInputs[0] = 'D';
		szInputs[1] = 'A';
		verifiedchars = true;
	}
	if (szInputs[0] == 'A' && szInputs[1] == '*' && GetInput(MOVE_UP) && !verifiedchars)
	{
		szInputs[0] = 'U';
		szInputs[1] = 'A';
		verifiedchars = true;
	}
	if (szInputs[0] == 'A' && szInputs[1] == '*' && pPlayer->IsPlayerInputForward(false) && !verifiedchars)
	{
		szInputs[0] = 'F';
		szInputs[1] = 'A';
		verifiedchars = true;
	}
	if (szInputs[0] == 'A' && szInputs[1] == '*' && pPlayer->IsPlayerInputBackward(false) && !verifiedchars)
	{
		szInputs[0] = 'B';
		szInputs[1] = 'A';
		verifiedchars = true;
	}
	char PlayerInputForComparison[MAX_PLAYER_INPUT + 1]="";
	
	for (int i = 0; i < MAX_PLAYER_INPUT + 1; i++)
	{
		if (szInputs[i] == '*' || i== MAX_PLAYER_INPUT-1)
		{
			PlayerInputForComparison[i] = '\0';
			break;
		}
		PlayerInputForComparison[i] = szInputs[i];
	}
	for (int i = 0; i < MAX_HAND_STATES; i++)
	{
		if ((pPlayer->IsPlayerOnTheGround() && tHandInputs[i].eAirAttack == AIR_ONLY)
			|| (!(pPlayer->IsPlayerOnTheGround()) && tHandInputs[i].eAirAttack == GROUND_ONLY))
			continue;
		if (nType == WEAPON_HANDS && i == HAND_AIR_DASH && bAirDashExecutedOnce)
			continue;
		if (nType == WEAPON_HANDS && i == DOWN_DASH_ATTACK && nCoolDownDashAttack>0)
			continue;		
		if (strcmp(tHandInputs[i].WeaponInput, PlayerInputForComparison)==0)
		{
			if (nType == WEAPON_HANDS && i == HAND_AIR_DASH)
				PlaySound(SOUND_AIR_DASH);
			else if (nType == WEAPON_HANDS && i == DOWN_DASH_ATTACK)
				PlaySound(SOUND_DOWNDASH);			
			else if (nType == WEAPON_HANDS && (i == HAND_STRONGPUNCH_A || i == HAND_STRONGPUNCH_B|| i == HAND_STRONGPUNCH_A_ALT))
				PlaySound(SOUND_BIG_PUNCH);
			else if (nType == WEAPON_HANDS)
				PlaySound(SOUND_REGULAR_PUNCH);
			pPlayer->SetPlayerState(PLAYER_ATTACKING_STATE);
			if (tHandInputs[i].bResetInputs)
				pPlayer->ResetInputs();
			nState = i;
			if (nType == WEAPON_HANDS && i == HAND_PUNCH_A_STATE && nAltHands == HAND_PUNCH_A_STATE)
				nState = HAND_PUNCH_B_STATE;
			uv.V = 0;
			nCurrentAttackPlaying = nState;
			uvMirrorDirection = 1;
			pPlayer->SetLockInputs(true);
			return;
		}
	}
	char szConInputs[MAX_PLAYER_INPUT+1] = "*******";
	for (int i = 1; i < MAX_PLAYER_INPUT+1; i++)
	{
		if (szInputs[i] == '*')
			break;
		szConInputs[i - 1] = szInputs[i];
	}
	ExecuteAttack(szConInputs, RecursionNum, verifiedchars);
}

//*****************************************************************************
//ResetToIdle関数
//武器リセットする
//引数：void
//戻：void
//*****************************************************************************
void Weapon::ResetToIdle()
{
	nState = nIdleState;
	uv = { 0,0 };
	nAnimationTimer = 0;
	Player* pPlayer = (Player*)spPlayer;
	nCurrentAttackPlaying = -1;
	pPlayer->SetLockInputs(false);
	pPlayer->SetJumpAttack(false);
}

//*****************************************************************************
//SetPlayerPointer関数
//プレイヤーの先頭アドレスを設定する
//引数：Sprite*
//戻：void
//*****************************************************************************
void Weapon::SetPlayerPointer(Sprite* sp_Player)
{
	spPlayer = sp_Player;
}

//*****************************************************************************
//SetState関数
//状態を設定する
//引数：int
//戻：void
//*****************************************************************************
void Weapon::SetState(int NewState)
{
	nState = NewState;
}

//*****************************************************************************
//GetCurrentAttackPlaying関数
//行っている攻撃動作を戻す
//引数：void
//戻：int
//*****************************************************************************
int Weapon::GetCurrentAttackPlaying()
{
	return nCurrentAttackPlaying;
}

//*****************************************************************************
//IsWeaponShowing関数
//武器は表しているかを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Weapon::IsWeaponShowing()
{
	return bShowWeapon;
}

//*****************************************************************************
//SetDashTime関数
//ダッシュタイマーを設定する
//引数：int
//戻：void
//*****************************************************************************
void Weapon::SetDashTime(int newTime)
{
	nDashTimer = newTime;
}

//*****************************************************************************
//IsWeaponAttacking関数
//攻撃動作を行っているを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Weapon::IsWeaponAttacking()
{
	if (hitbox.h == 0 && hitbox.w == 0 && hitbox.x == 0 && hitbox.y == 0)
		return false;
	return bIsAttacking;
}

//*****************************************************************************
//ResetWeaponState関数
//状態をリセットする
//引数：void
//戻：void
//*****************************************************************************
void Weapon::ResetWeaponState()
{
	nState = nIdleState;
	nCurrentAttackPlaying = -1;
}

//*****************************************************************************
//ResetAirDash関数
//エアーダッシュをリセットする
//引数：void
//戻：void
//*****************************************************************************
void Weapon::ResetAirDash()
{
	bAirDashExecutedOnce = false;
}

//*****************************************************************************
//SetCoolDownDashFrames関数
//クールダウンフレームを設定する
//引数：int
//戻：void
//*****************************************************************************
void Weapon::SetCoolDownDashFrames(int newDashFrames)
{
	nCoolDownDashAttack = newDashFrames;
}

//*****************************************************************************
//GetOwner関数
//プレイヤーの先頭アドレスを戻す
//引数：void
//戻：Sprite*
//*****************************************************************************
Sprite* Weapon::GetOwner()
{
	return spPlayer;
}

Bullet ** Weapon::GetBullets()
{
	return pBullets;
}

Bullet ** Weapon::GetCatchBullets()
{
	return pCatchBullets;
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Weapon::End()
{
#ifdef USE_CONSOLE
	printf("ENDING WEAPON TYPE %d\n", nType);
#endif
	if (nType == WEAPON_GUITAR) {
		for (int i = 0; i < MAX_CATCHBULLETS; i++)
			SAFE_DELETE(pCatchBullets[i]);
		for (int i = 0; i < MAX_BULLETS; i++)
			SAFE_DELETE(pBullets[i]);
		pCurrentGuitar = nullptr;
	}
	UninitSprite();
}

//*****************************************************************************
//GetBullets関数
//弾の先頭アドレスを戻す
//引数：void
//戻：Bullet*
//*****************************************************************************
Bullet** GetBullets()
{
	if (pCurrentGuitar)
		return pCurrentGuitar->GetBullets();
	return nullptr;
}

//*****************************************************************************
//GetCatchBullets関数
//キャッチオンの弾の先頭アドレスを戻す
//引数：void
//戻：Bullet*
//*****************************************************************************
Bullet** GetCatchBullets()
{
	if (pCurrentGuitar)
		return pCurrentGuitar->GetCatchBullets();
	return nullptr;
}
