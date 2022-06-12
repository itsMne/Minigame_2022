//*****************************************************************************
// Enemy.cpp
// 敵の管理
// Author : Mane
//*****************************************************************************
#include "Enemy.h"
#include <stdio.h>
#include "Player.h"
#include "Weapon.h"
#include "Camera.h"
#include "TextureManager.h"
#include "UniversalVariables.h"
#include "SceneInGame.h"
#include "sound.h"
#include "Bullet.h"
#include "Texture.h"
#include "RankManager.h"
#include "GameInputManager.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PRINT_HITBOX true && SHOW_HITBOXES
#define REGULAR_ATTACK_DISTANCE_AFTER_PUNCH 5
#define GRAVITY_FORCE						1
#define MAX_POLYGONSIZESHIELD				250
#define SHIELD_ALFA							0.25f
#define BUN_BUN_SPEED						5
#define BUN_BUN_WAITING_FRAMES				1
#define REGULAR_ENEMY_ID					-1
#define DEAD_BY_SPIKE_MOVE					-5 
#define ZOOM_OUT_SCALE						 0.73f
#define COOLDOWN_FRAMES_AFTER_DASH_ATTACK	40
#define IFRAME_JUMP_RANK					-7
#define DEBUG_ENEMY_HEALTH					false	
#define SHOW_HP_WITH_COLOR					true
//*****************************************************************************
// エナム定義
//*****************************************************************************
enum EnemyStates //敵の状態
{
	ENEMY_ONAIR = 0,			
	ATTACKING_ENEMY,			
	ATTACKED_NORMALPUNCH_A,		
	ATTACKED_NORMALPUNCH_B,		
	IDLE_ENEMY_STATE,			
	ATTACKED_BY_AIR_GROUNDDOWN,
	ATTACKED_BY_DASH,			
	ATTACKED_BYDUALHANDS,		
	ATTACKED_BYHANDGROUND,		
	ATTACKED_BY_BIG_PUNCHES,	
	ATTACKED_BY_BIG_PUNCHES_AIR,
	MAX_BUNBUN_STATE
};
//*****************************************************************************
// グローバル変数
//*****************************************************************************
ID3D11ShaderResourceView * pAuraTexture =					nullptr;
ID3D11ShaderResourceView * pAuraTextureAlt =				nullptr;
ID3D11ShaderResourceView * pMiniAuraATexture =				nullptr;
ID3D11ShaderResourceView * pMiniAuraBTexture =				nullptr;
ID3D11ShaderResourceView * pShieldTexture =					nullptr;
Enemy* BouncingEnemy =										nullptr;
bool bPauseEnemyUpdate =									false;
int nFramesOfNewScale=										0;
float fNewScale=											0;
int nWaitingScaleFrames=									0;
bool bScalingInProcess=										false;
int nEnemyCount = 0;

//*****************************************************************************
// コンストラクタ変数
//*****************************************************************************
Enemy::Enemy()
{
	nEnemyCount++;
	fEnemySpeed = 0;
	bBlockGravity = false;
	bCatchedPositionWithX = bCatchedPositionWithY = false;
	bEnemyDyingSoundEffect = false;
	fMaxSuperArmor = 0;
	fSuperArmor = 0;
	nEnemyState = 0;
	nEnemyType = 0;
	nFramesSinceAttack = 0;
	nIdleState = 0;
	nMaxHP = 0;
	p_goCurrentFloor = nullptr;
	SA_RecoveryTimer = SA_CooldownTimer();
	bSuperArmorBroken = false;
	InitSprite();
	bIsInUse = false;
	bNoScoreEnemy = false;
	nEnemyID = REGULAR_ENEMY_ID;
	fCatchedAcceleration = 1;
	nRoomID = MAX_ROOMS;
	nFrame_X = 1;
	nFrame_Y = 1;
	nLockedAnimationFrames = 0;
	uv = { 0,0 };
	Position = { 0,0,0 };
	mirrorUv = 1;
	nCounter = 0;
	nAnimeTimer = 0;
	nHitAttackState=0;
	fSizeChangeAcceleration = 0;
	bHitCollision=false;
	bBulletCollision=false;
	bBulletCatchCollision = false;
	nRecoveringFramesFromAttack = 0;
	nDirection = LEFT_DIR;
	nAnimeMirror = 1;
	fAttackedAcceleration = 1;
	fyForce = 0;
	bDetectedByPlayer = false;
	p_goAuraEffect = new GameObject();
	xm2ImpulseDirection = {0,0};
	x2InitialPosition = { 0,0 };
	nBounces = 0;
	bCanFallFromFloor = false;
	nWaitingFrames = 0;
	nHP = 0;
	nAttackPlayerWeapon = 0;
	nFramesForPlayerDetectionVerification = 0;
	pWallLimitLeft = nullptr;
	pWallLimitRight = nullptr;
	pFloorLimit = nullptr;
	x2MaxSize = { 0,0 };
	bSizeSet = false;
	bIsIntroductionEnemy = false;
	pIntroductionLogo = nullptr;
	if (!pAuraTexture)
	{
		CreateTextureFromFile("data/TEXTURE/AuraEffect.tga", &pAuraTexture);
		CreateTextureFromFile("data/TEXTURE/AuraEffectAlt.tga", &pAuraTextureAlt);
		p_goAuraEffect->Init(pAuraTexture, AURA_EFFECT_TYPE);
	}
	else {
		p_goAuraEffect->Init(pAuraTexture, AURA_EFFECT_TYPE);
	}
	p_goAuraEffect->SetSpriteSize(1, 1);
	p_goAuraEffect->SetUse(false);
	AttackPolygonHitbox = nullptr;

	AttackHitbox = { 0,0,0,0,PRINT_HITBOX,nullptr };

	bool bUseAuraA = true;
	for (int i = 0; i < MAX_MINIAURA_EFFECTS; i++)
	{
		p_goMiniAuraEffect[i] = new GameObject();
		if (bUseAuraA)
		{
			if (!pMiniAuraATexture)
			{
				CreateTextureFromFile("data/TEXTURE/MiniAuraA.tga", &pMiniAuraATexture);
				p_goMiniAuraEffect[i]->Init(pMiniAuraATexture, MINI_AURA_EFFECT_TYPE);
			}
			else
			{
				p_goMiniAuraEffect[i]->Init(pMiniAuraATexture, MINI_AURA_EFFECT_TYPE);
			}
			bUseAuraA = false;
		}
		else
		{
			if (!pMiniAuraBTexture)
			{
				CreateTextureFromFile("data/TEXTURE/MiniAuraB.tga", &pMiniAuraBTexture);
				p_goMiniAuraEffect[i]->Init(pMiniAuraBTexture, MINI_AURA_EFFECT_TYPE);
			}
			else
			{
				p_goMiniAuraEffect[i]->Init(pMiniAuraBTexture, MINI_AURA_EFFECT_TYPE);
			}
			bUseAuraA = true;
		}
		p_goMiniAuraEffect[i]->SetSpriteSize(1, 1);
		p_goMiniAuraEffect[i]->SetUse(false);
	}
	p_ShieldEffect = new GameObject();
	if (pShieldTexture == nullptr)
	{
		CreateTextureFromFile("data/TEXTURE/ShieldTexture.tga", &pShieldTexture);
		p_ShieldEffect->Init(pShieldTexture, MAX_GOTYPE);
	}
	else {
		p_ShieldEffect->Init(pShieldTexture, MAX_GOTYPE);
	}
	p_ShieldEffect->SetSpriteSize(MAX_POLYGONSIZESHIELD, MAX_POLYGONSIZESHIELD);
	p_ShieldEffect->SetAlpha(0.0f);
}

Enemy::~Enemy()
{
	nEnemyCount--;
	End();
}

//*****************************************************************************
//Init関数
//初期化の関数
//引数：int
//戻：void
//*****************************************************************************
void Enemy::Init(int nEnemyType)
{
	bBlockGravity = false;
	p_goCurrentFloor = nullptr;
	fyForce = 0;
	this->nEnemyType = nEnemyType;
	bIsInUse = true;
	bSizeSet = false;
	bEnemyDyingSoundEffect = false;
	nFramesSinceAttack = 0;
	if (!AttackPolygonHitbox && PRINT_HITBOX) {
		AttackPolygonHitbox = new Sprite(true);
		AttackPolygonHitbox->InitSprite("data/TEXTURE/hbox.tga");
		AttackPolygonHitbox->SetColor(1, 0, 0);
		AttackPolygonHitbox->SetAlpha(0.75f);
	}
	switch (nEnemyType)
	{
	case BUNBUN_ENEMY:
		InitSprite(GetTexture(TEXTURE_BUNBUN_ENEMY));
		nMaxHP = nHP = MAX_BUN_BUN_HP;
		nFrame_X = 6;
		nFrame_Y = 15;
		x2MaxSize = { BUNBUN_SIZE ,BUNBUN_SIZE };
		SetSpriteSize(0, 0);
		hitbox = { 70, -20, 200, 175, PRINT_HITBOX, nullptr };
		LeftSideHitbox = { 30, -20, 40, 80, PRINT_HITBOX, nullptr };
		RightSideHitbox = { 100, -20, 40, 80, PRINT_HITBOX, nullptr };
		FeetHitbox = { 70, -70, 25, 80, PRINT_HITBOX, nullptr };
		uv.U = IDLE_ENEMY_STATE;
		nEnemyState = IDLE_ENEMY_STATE;
		SA_RecoveryTimer = {1,1,600};
		bSuperArmorBroken = false;
		fMaxSuperArmor=fSuperArmor=20;
		nIdleState = IDLE_ENEMY_STATE;
		fEnemySpeed = 0;
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
void Enemy::Update()
{
	if (!bIsInUse) 
		return;
	if (!bSizeSet)
	{
		fSizeChangeAcceleration++;
		if (Size.x < x2MaxSize.x)
		{
			Size.x += fSizeChangeAcceleration;
			if (Size.x > x2MaxSize.x)
				Size.x = x2MaxSize.x;
		}
		if (Size.y < x2MaxSize.y)
		{
			Size.y += fSizeChangeAcceleration;
			if (Size.y > x2MaxSize.y)
				Size.y = x2MaxSize.y;
		}
		bSizeSet = Size.x == x2MaxSize.x && Size.y == x2MaxSize.y;
		if (Size.x == x2MaxSize.x || Size.y == x2MaxSize.y) {
			fSizeChangeAcceleration = 0;
		}
		return;
	}
	if (bPauseEnemyUpdate)
		return;
	p_ShieldEffect->SetLocalPosition(Position.x + 70, Position.y);
	if (fSuperArmor < 0)
		fSuperArmor = 0;
	p_ShieldEffect->SetSpriteSize((fSuperArmor / fMaxSuperArmor)* MAX_POLYGONSIZESHIELD,
		(fSuperArmor / fMaxSuperArmor)* MAX_POLYGONSIZESHIELD);
	float shieldAlpha = p_ShieldEffect->GetColor().w;
	p_goAuraEffect->SetLocalPosition(Position.x + 70, Position.y);
	p_goAuraEffect->Update();
	if (shieldAlpha>0)
	{
		p_ShieldEffect->SetAlpha(shieldAlpha - 0.0125f);
	}
	if (IsEnemyDead()) {
		for (int i = 0; i < MAX_MINIAURA_EFFECTS; i++)
		{
			if (!(p_goMiniAuraEffect[i]->IsInUse()))continue;
			p_goMiniAuraEffect[i]->Update();
			p_goMiniAuraEffect[i]->SetLocalPosition(Position.x + 50, Position.y);
		}
		return;
	}
	if (nHP <= 0 && !(p_goAuraEffect->IsInUse()))
	{
		if (!bEnemyDyingSoundEffect) {
			PlaySound(SOUND_LABEL_SE_ENEMY_ISDYING);
			bEnemyDyingSoundEffect = true;
		}
		hitbox =		  { 0,0,0,0,false,hitbox.Sprite };
		LeftSideHitbox =  { 0,0,0,0,false,LeftSideHitbox.Sprite };
		RightSideHitbox = { 0,0,0,0,false,RightSideHitbox.Sprite };
		FeetHitbox =	  { 0,0,0,0,false,FeetHitbox.Sprite };
		if (Size.x != 0 || Size.y != 0) 
		{
			SetColor(0.15f, 0.15f, 0.15f);
			fAttackedAcceleration += 0.25f;
			Size.x -= fAttackedAcceleration;
			if (Size.x < 0)
				Size.x = 0;
			Size.y -= fAttackedAcceleration;
			if (Size.y < 0)
				Size.y = 0;
			Rotation.y += 20;
		}
		if(Size.x==0 && Size.y == 0)
		{
			ActivateAuraEffect(true);
			CameraTremble(15, 15, 7);
			PlaySound(SOUND_LABEL_SE_ENEMY_DEAD);
		}
		return;
	}
	bHitCollision = false;
	Weapon** pPlayerWeapons = GetWeapons();
	SuperArmorRecoveryControl();

	for (int i = 0; i < MAX_MINIAURA_EFFECTS; i++)
	{
		if (!(p_goMiniAuraEffect[i]->IsInUse()))continue;
		p_goMiniAuraEffect[i]->Update();
		p_goMiniAuraEffect[i]->SetLocalPosition(Position.x + 50, Position.y);
	}
	if (bSuperArmorBroken)
	{
		SetColor(SA_RecoveryTimer.current/ SA_RecoveryTimer.max, 0.65f, 0.65f);
	}
	else {
		SetColor(1, 1, 1);
	}
	if (nEnemyState == CATCHED_STATE)
	{
		CatchedStateControl();
		return;
	}
	GravityControl();
	for (int i = 0; i < WEAPON_MAX; i++)
	{
		if (!pPlayerWeapons[i])continue;
		if (!pPlayerWeapons[i]->IsWeaponAttacking())continue;
		if (IsInCollision(GetHitbox(), pPlayerWeapons[i]->GetHitbox()))
		{
			bHitCollision = true;
			nHitAttackState = pPlayerWeapons[i]->GetCurrentAttackPlaying();
			nAttackPlayerWeapon = i;
			break;
		}
	}
	Bullet** pBullets = GetBullets();
	if (pBullets) {
		for (int i = 0; i < MAX_BULLETS; i++)
		{
			if (!pBullets[i])continue;
			if (!(pBullets[i]->IsInUse()))continue;

			if (IsInCollision(pBullets[i]->GetHitbox(), GetHitbox()))
			{
				bBulletCollision = true;
				pBullets[i]->SetUse(false);
				AddFreezeRankFrames(5);
				if (nEnemyType == BUNBUN_ENEMY) {
					if (!bSuperArmorBroken)
						fSuperArmor--;
					else
					{
						uv.U = ATTACKED_NORMALPUNCH_B;
						uv.V = 7;
						nEnemyState = ATTACKED_BYDUALHANDS;
						SetDamage(1);
						SceneInGame* pGame = GetCurrentGame();
						if (!bNoScoreEnemy && pGame)
							pGame->AddToScoreWithMultiplier(1);
					}
				}
				if (!pPlayerWeapons)
					pPlayerWeapons = GetWeapons();
				Player* pPlayer = GetPlayerPointer();
				if (pPlayer) {
					if (!(CheckCollisionWithWall(GetHitbox())))
						Position.x += 3 * ((pPlayer->GetDirection()));
					else
						Position.x -= 3 * ((pPlayer->GetDirection()));
					p_ShieldEffect->SetAlpha(SHIELD_ALFA);
					PlaySound(SOUND_LABEL_SE_HITBYBULLET);
					VibrateXinput(65535 / 3, 65535 / 3, 30);
					ActivateMiniAuraEffect();
				}
			}
		}
	}
	Bullet** pCatchBullets = GetCatchBullets();
	for (int i = 0; i < MAX_CATCHBULLETS; i++)
	{
		if (!pCatchBullets[i])continue;
		if (!(pCatchBullets[i]->IsInUse()))continue;

		if (IsInCollision(pCatchBullets[i]->GetHitbox(), GetHitbox()))
		{
			if (!pPlayerWeapons)
				pPlayerWeapons = GetWeapons();
			Player* pPlayer = GetPlayerPointer();
			if (!pPlayer)
				break;
			if (!bSuperArmorBroken)
				pCatchBullets[i]->SetBulletSpeed({ 0, 75 });
			else
			{
				pCatchBullets[i]->SetUse(false);
				bBulletCatchCollision = true;
				pPlayer->SetCatchedEnemy(this);
				fCatchedAcceleration = 1;
				bCatchedPositionWithX = bCatchedPositionWithY = false;
				ActivateMiniAuraEffect();
			}
		}
	}
	if (nFramesSinceAttack > 0)
		nFramesSinceAttack--;
	switch (nEnemyType)
	{
	case BUNBUN_ENEMY:
		BunBunAIControl();
		UpdateBunBun();
		break;
	default:
		break;
	}
	if (pWallLimitLeft)
	{
		HitBox hWallHitbox = pWallLimitLeft->GetHitbox();
		while (Position.x < hWallHitbox.x)
			Position.x++;
	}
	if (pWallLimitRight)
	{
		HitBox hWallHitbox = pWallLimitRight->GetHitbox();
		while (Position.x > hWallHitbox.x + hWallHitbox.w)
			Position.x--;
	}
	if (pFloorLimit)
	{
		HitBox hFloorHitbox = pFloorLimit->GetHitbox();
		while (Position.y < hFloorHitbox.h + hFloorHitbox.h)
			Position.y++;
	}
	if (pIntroductionLogo) {
		pIntroductionLogo->Update();
		if (pIntroductionLogo->WasLogoShown())
		{
			delete(pIntroductionLogo);
			pIntroductionLogo = nullptr;
		}
	}
}	

//*****************************************************************************
//ActivateMiniAuraEffect関数
//小さいオーラのエフェクトを活性化する関数
//引数：void
//戻：void
//*****************************************************************************
void Enemy::ActivateMiniAuraEffect()
{
	for (int i = 0; i < MAX_MINIAURA_EFFECTS; i++)
	{
		if (p_goMiniAuraEffect[i]->IsInUse())continue;
		p_goMiniAuraEffect[i]->SetLocalPosition(Position.x, Position.y);
		p_goMiniAuraEffect[i]->SetSpriteSize(1, 1);
		p_goMiniAuraEffect[i]->SetUse(true);
		p_goMiniAuraEffect[i]->SetAlpha(1);
		break;
	}
}

//*****************************************************************************
//CatchedStateControl関数
//敵はプレイヤーに取られた状態の管理関数
//引数：void
//戻：void
//*****************************************************************************
void Enemy::CatchedStateControl()
{
	Player* pPlayer = GetPlayerPointer();
	if (!pPlayer)
		return;
	XMFLOAT3 x2GoToPosition = pPlayer->GetHalo()->GetWorldPosition();
	x2GoToPosition.y += 70;
	x2GoToPosition.x -= 35;
	if (bCatchedPositionWithX) {
		Position.x = x2GoToPosition.x;
	}
	if (bCatchedPositionWithY)
		Position.y = x2GoToPosition.y;

	if (!bCatchedPositionWithX)
	{
		if (Position.x < x2GoToPosition.x)
		{
			Position.x+= fCatchedAcceleration;
			if(Position.x >= x2GoToPosition.x)
				bCatchedPositionWithX = true;
		}
		else if (Position.x > x2GoToPosition.x)
		{
			Position.x-= fCatchedAcceleration;
			if (Position.x <= x2GoToPosition.x)
				bCatchedPositionWithX = true;
		}
		if(bCatchedPositionWithX)
			Position.x = x2GoToPosition.x;
	}
	if (!bCatchedPositionWithY)
	{
		if (Position.y < x2GoToPosition.y)
		{
			Position.y += fCatchedAcceleration;
			if (Position.y >= x2GoToPosition.y)
				bCatchedPositionWithY = true;
		}
		else if (Position.y > x2GoToPosition.y)
		{
			Position.y -= fCatchedAcceleration;
			if (Position.y <= x2GoToPosition.y)
				bCatchedPositionWithY = true;
		}
		if (bCatchedPositionWithY)
			Position.y = x2GoToPosition.y;
	}
	if (!bCatchedPositionWithX || !bCatchedPositionWithY)
		fCatchedAcceleration+= 6.75f;

}

//*****************************************************************************
//SuperArmorRecoveryControl関数
//スーパーアーマーを直す管理の関数
//引数：void
//戻：void
//*****************************************************************************
void Enemy::SuperArmorRecoveryControl()
{
	if (!bSuperArmorBroken)
		return;
	
	if (SA_RecoveryTimer.current < SA_RecoveryTimer.max)
		SA_RecoveryTimer.current+= SA_RecoveryTimer.speed;
	else {
		SA_RecoveryTimer.current = 0;
		fSuperArmor = fMaxSuperArmor;
		bSuperArmorBroken = false;
		if (nEnemyState == CATCHED_STATE)
		{
			Player* pPlayer = GetPlayerPointer();
			if (pPlayer)
			{
				pPlayer->SetCatchedEnemy(nullptr);
			}
			nEnemyState = nIdleState;
		}
	}
}

//*****************************************************************************
//GravityControl関数
//重力の管理関数
//引数：void
//戻：void
//*****************************************************************************
void Enemy::GravityControl()
{
	if (bBulletCollision) {
		bBulletCollision = false;
		fyForce = 0;
		return;
		}
	Player* pPlayer = GetPlayerPointer();
	if (fyForce > 0 && pPlayer) {
		if (IsInCollision(pPlayer->GetPlayerHitbox(HB_HEAD), GetFeetHitbox()))
		{
			Position.x -= nDirection*9;
			return;
		}
	}
	if (nEnemyState == BOUNCING_STATE || bBlockGravity)
		return;
	if (p_goCurrentFloor != nullptr)
	{
		HitBox cFloor = p_goCurrentFloor->GetHitbox();
		cFloor.y++;
		if (!IsInCollision(cFloor, GetFeetHitbox()))
		{
			p_goCurrentFloor = nullptr;
			return;
		}
	}
	if (nEnemyType == BUNBUN_ENEMY) {
		bool bEnemyBeingAttackedCancelsGravity = nEnemyState == ATTACKED_BY_DASH || nEnemyState == ATTACKED_NORMALPUNCH_B || nEnemyState == ATTACKED_NORMALPUNCH_A;
		if (p_goCurrentFloor || bEnemyBeingAttackedCancelsGravity)
			return;
	}
	SceneInGame* pGame = GetCurrentGame();
	if (!pGame)
		return;
	go_node* floorsNode = pGame->GetFloors()->HeadNode;
	while (floorsNode != nullptr)
	{
		if (floorsNode->Object != nullptr) {
			if (IsInCollision(floorsNode->Object->GetHitbox(), GetFeetHitbox()))
			{
				p_goCurrentFloor = floorsNode->Object;
				while (IsInCollision(floorsNode->Object->GetHitbox(), GetHitbox()))
					Position.y++;
				if (uv.U == ENEMY_ONAIR && nEnemyType == BUNBUN_ENEMY) {
					uv.U = IDLE_ENEMY_STATE;
					nEnemyState = IDLE_ENEMY_STATE;
					uv.V = 0;
				}
				return;
			}
		}
		floorsNode = floorsNode->next;
	}
	fyForce += GRAVITY_FORCE;
	Position.y -= fyForce;
}

//*****************************************************************************
//UpdateBunBun関数
//ぶんぶんと言う敵の種類を管理する
//引数：void
//戻：void
//*****************************************************************************
void Enemy::UpdateBunBun()
{
	Player* pPlayer = GetPlayerPointer();
	if (!pPlayer)
		return;
	if (bHitCollision)
		AttackCollisionControl();
	RecoveryFramesControl();
	HitBox HitboxVerification;
	if (nDirection == RIGHT_DIR) {
		Rotation.y = 180;
		PositionOffset.x= hitbox.w - 170;
	}
	else {
		PositionOffset.x=100;
		Rotation.y = 0;
	}
	AttackHitbox = { 0,0,0,0,PRINT_HITBOX,AttackHitbox.Sprite };
	if (nHP > 0) {
		if (CheckCollisionWithSpikes())
		{
#ifdef USE_CONSOLE
			printf("スパイクで死んだ\n");
#endif
			CameraTremble(15, 15, 7);
			nHP = 0;
			SceneInGame* pGame = GetCurrentGame();
			if (!pGame)
				return;
			if (nFramesSinceAttack > 0) {
				AddMoveToRankMeter(DEAD_BY_SPIKE_MOVE, 25);
				if (!bNoScoreEnemy)
					pGame->AddToScoreWithMultiplier(5);
			}
		}
	}
	switch (nEnemyState)
	{
	case ATTACKING_ENEMY:
		bBlockGravity = true;
		nAnimeTimer = 2;//2
		uv.U = ATTACKING_ENEMY;
		if (uv.V > 11) {
			if (nDirection == RIGHT_DIR) 
				AttackHitbox = { Position.x + 120 + (uv.V * 8),Position.y - 30,12 * uv.V,50,PRINT_HITBOX,AttackHitbox.Sprite };
			else
				AttackHitbox = { Position.x + (uv.V * -8),Position.y - 30,12 * uv.V,50,PRINT_HITBOX,AttackHitbox.Sprite };
		}
		else {
			AttackHitbox = { 0,0,0,0,PRINT_HITBOX,AttackHitbox.Sprite };
		}
		if (++nCounter > nAnimeTimer)
		{
			uv.V += mirrorUv;
			nCounter = 0;
		}
		if (uv.V == 14 && mirrorUv > 0) {
			mirrorUv = -1;
			nCounter = -5;
		}
		else if (uv.V == 0 && mirrorUv < 0) {
			nEnemyState = IDLE_ENEMY_STATE;
			uv.U = IDLE_ENEMY_STATE;
			mirrorUv = 1;
		}
		if (IsInCollision(AttackHitbox, pPlayer->GetPlayerHitbox(HB_BODY)) && pPlayer->GetPlayerState() != PLAYER_TELEPORTING && !(pPlayer->InvinsibilityFramesOn()))
		{
			if (pPlayer->AttackPlayer(1)) {
				CameraTremble(5, 5, 15);
			}
			else {
				if (pPlayer->GetPlayerState() != PLAYER_ATTACKED_STATE) {
					AddMoveToRankMeter(IFRAME_JUMP_RANK, 20);
#ifdef USE_CONSOLE
					printf("IFRAMEのおかげで避けた\n");
#endif
				}
			}

		}
		break;
	case BOUNCING_STATE:
		if (nBounces < 0) 
		{
			uv.U = IDLE_ENEMY_STATE;
			nEnemyState = IDLE_ENEMY_STATE;
			break;
		}
		uv.U = ATTACKED_NORMALPUNCH_B;
		if (uv.V == nFrame_Y)
			uv.V = 0;
		uv.V++;
		if (xm2ImpulseDirection.x != 0) {
			if (xm2ImpulseDirection.x > 1)
				xm2ImpulseDirection.x += 1+fAttackedAcceleration;
			if (xm2ImpulseDirection.x < 1)
				xm2ImpulseDirection.x -= (1+fAttackedAcceleration);

			if (xm2ImpulseDirection.x > 35)
				xm2ImpulseDirection.x = 35;
			if (xm2ImpulseDirection.x < -50)
				xm2ImpulseDirection.x = -50;
			Position.x += xm2ImpulseDirection.x;
		}

		if (xm2ImpulseDirection.y != 0) {
			if (xm2ImpulseDirection.y > 1)
				xm2ImpulseDirection.y += 1 + fAttackedAcceleration;
			if (xm2ImpulseDirection.y < 1)
				xm2ImpulseDirection.y -= (1 + fAttackedAcceleration);

			if (xm2ImpulseDirection.y > 50)
				xm2ImpulseDirection.y = 50;
			if (xm2ImpulseDirection.y < -50)
				xm2ImpulseDirection.y = -50;
			Position.y += xm2ImpulseDirection.y;
		}
		fAttackedAcceleration+=5;
		HitboxVerification = GetHitbox();
		if (GetHitbox().x + HitboxVerification.w> GetCameraLimitRight()) {
			AddMoveToRankMeter(-200, 30);
			while (GetHitbox().x + HitboxVerification.w >= GetCameraLimitRight())
				Position.x--;
			xm2ImpulseDirection.x = -10;
			nBounces--;
			break;
		}
		if (GetHitbox().x < GetCameraLimitLeft()) {
			AddMoveToRankMeter(-200, 30);
			xm2ImpulseDirection.x = 10;
			while (GetHitbox().x + HitboxVerification.w <= GetCameraLimitLeft())
				Position.x++;
			nBounces--;
			break;
		}
		if (GetHitbox().y > GetCameraLimitUp()) {
			AddMoveToRankMeter(-200, 30);
			while (GetHitbox().y >= GetCameraLimitUp())
				Position.y--;
			xm2ImpulseDirection.y = -10;
			nBounces--;
			break;
		}
		if (GetHitbox().y < GetCameraLimitDown()) {
			AddMoveToRankMeter(-200, 30);
			while (GetHitbox().y < GetCameraLimitDown())
				Position.y++;
			xm2ImpulseDirection.y =10;
			nBounces--;
			break;
		}
		if (CheckBounceWithFloor())
		{
			AddMoveToRankMeter(-200, 30);
			xm2ImpulseDirection.y = 10;
			nBounces--;
			break;
		}
		if (CheckCollisionWithWall(GetRightSideHitbox()))
		{
			AddMoveToRankMeter(-200, 30);
			while (GetHitbox().x + HitboxVerification.w >= GetCameraLimitRight())
				Position.x--;
			xm2ImpulseDirection.x = -10;
			nBounces--;
			break;
		}
		if (CheckCollisionWithWall(GetLeftSideHitbox()))
		{
			AddMoveToRankMeter(-200, 30);
			xm2ImpulseDirection.x = 10;
			while (GetHitbox().x + HitboxVerification.w <= GetCameraLimitLeft())
				Position.x++;
			nBounces--;
			break;
		}
		break;
	case ATTACKED_BY_BIG_PUNCHES:
		nAnimeTimer = 0;

		if (nLockedAnimationFrames > 5)
		{
			fAttackedAcceleration += 8.25f;
			if (!(CheckCollisionWithWall(GetHitbox())))
				Position.x += fAttackedAcceleration * pPlayer->GetDirection();
			else
				Position.x -= fAttackedAcceleration * pPlayer->GetDirection();
		}
		if (++nCounter > nAnimeTimer && nLockedAnimationFrames == 0)
		{
			uv.V += 1;
			nCounter = 0;
			if (uv.V == nFrame_Y / 2)
				nCounter = -10;
		}
		if (uv.V > nFrame_Y - 1)
		{
			nEnemyState = IDLE_ENEMY_STATE;
			uv.U = (float)IDLE_ENEMY_STATE;
			bBlockGravity = false;
			uv.V = 0;
		}
		if (nLockedAnimationFrames > 0) {
			nLockedAnimationFrames--;
			bBlockGravity = true;
		}
		if (nLockedAnimationFrames < 0)
			nLockedAnimationFrames = 0;
		ControlAICollisionWithWall(false);
		break;
	case ATTACKED_BY_BIG_PUNCHES_AIR:
		nAnimeTimer = 0;
		
		if (nLockedAnimationFrames > 12) 
		{
			fAttackedAcceleration += 8.25f;
			Position.x += fAttackedAcceleration * pPlayer->GetDirection();
			if (CheckCollisionWithWall(GetHitbox()))
				Position.x -= fAttackedAcceleration * pPlayer->GetDirection()*2;
		}
		if (++nCounter > nAnimeTimer && nLockedAnimationFrames == 0)
		{
			uv.V += 1;
			nCounter = 0;
			if (uv.V == nFrame_Y / 2)
				nCounter = -10;
		}
		if (uv.V > nFrame_Y - 1)
		{
			nEnemyState = IDLE_ENEMY_STATE;
			uv.U = IDLE_ENEMY_STATE;
			bBlockGravity = false;
			uv.V = 0;
		}
		if (nLockedAnimationFrames > 0) {
			nLockedAnimationFrames--;
			bBlockGravity = true;
		}
		if (nLockedAnimationFrames < 0)
			nLockedAnimationFrames = 0;
		ControlAICollisionWithWall(false);
		break;
	case ATTACKED_BYDUALHANDS:
		nAnimeTimer = 1;
		if (mirrorUv != 1 && mirrorUv != -1)
			mirrorUv = 1;
		if (++nCounter > nAnimeTimer)
		{
			uv.V += mirrorUv;
			nCounter = 0;
		}
		if (uv.U == ENEMY_ONAIR)
		{
			if (uv.V > nFrame_Y-5 && mirrorUv>0)
			{
				mirrorUv = -1;
			}
			if (uv.V ==0 && mirrorUv<0)
			{
				mirrorUv = 1;
			}
			break;
		}

		if (uv.V > nFrame_Y - 1)
		{
			nEnemyState = IDLE_ENEMY_STATE;
			uv.U = IDLE_ENEMY_STATE;
			uv.V = 0;
		}
		break;
	case ATTACKED_BYHANDGROUND:
		nAnimeTimer = 0;
		if (++nCounter > nAnimeTimer)
		{
			uv.V += nAnimeMirror;
			nCounter = 0;
			if (uv.V == nFrame_Y - 1) {
				nCounter = -12;
				nAnimeMirror *= -1;
			}
		}
		fAttackedAcceleration += 0.5f;
		if (nAnimeMirror > 0 && uv.V < 3) 
		{
			if (!(CheckCollisionWithWall(GetHitbox())))
				Position.x += fAttackedAcceleration * nDirection;
			else
				Position.x -= fAttackedAcceleration * nDirection;
		}
			
		if (uv.V < 0)
		{
			nEnemyState = IDLE_ENEMY_STATE;
			uv.U = IDLE_ENEMY_STATE;
			uv.V = 0;
		}
		ControlAICollisionWithWall(false);
		break;
	case ATTACKED_BY_AIR_GROUNDDOWN:
		nAnimeTimer = 0;
		if (++nCounter > nAnimeTimer)
		{
			uv.V += nAnimeMirror;
			nCounter = 0;
			if (uv.V == nFrame_Y - 1) {
				nCounter = -12;
				nAnimeMirror *= -1;
			}
		}
		fAttackedAcceleration += 2;
		if (nAnimeMirror > 0 && uv.V < 3)
		{
			if (!(CheckCollisionWithWall(GetHitbox())))
				Position.x += fAttackedAcceleration * nDirection*-1;
			else
				Position.x -= fAttackedAcceleration * nDirection*-1;
		}
			
		if (uv.V < 0)
		{
			nEnemyState = IDLE_ENEMY_STATE;
			uv.U = IDLE_ENEMY_STATE;
			uv.V = 0;
		}
		ControlAICollisionWithWall(false);
		break;
	case IDLE_ENEMY_STATE:
		bBlockGravity = false;
		fAttackedAcceleration = 1;
		if (++nCounter == nAnimeTimer)
		{
			uv.V += mirrorUv;
			nCounter = 0;
		}
		nAnimeTimer = 2;
		if (uv.V == 14) 
			mirrorUv = -1;
		else if(uv.V == 0)
			mirrorUv = 1;
		break;
	case ATTACKED_BY_DASH:
		nAnimeTimer = 1;
		if (++nCounter > nAnimeTimer)
		{
			uv.V += 1;
			nCounter = 0;
		}
		if (uv.V > nFrame_Y - 1)
		{
			nEnemyState = IDLE_ENEMY_STATE;
			uv.U = (float)IDLE_ENEMY_STATE;
			uv.V = 0;
		}
		ControlAICollisionWithWall(false);
		while (IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), GetHitbox()))
			pPlayer->SetPlayerTranslation(1.f*nDirection, 0);
		break;
	case ATTACKED_NORMALPUNCH_A: case ATTACKED_NORMALPUNCH_B:
		nAnimeTimer = 0;
		if(!(CheckCollisionWithWall(GetHitbox())))
			Position.x += fAttackedAcceleration * pPlayer->GetDirection();
		else
		{
			pPlayer->CancelExternalMovement();
			Position.x -= fAttackedAcceleration * pPlayer->GetDirection();
		}
		if (uv.V < (nFrame_Y / 2) - 6)
			fAttackedAcceleration+=4.25f;
		if (++nCounter > nAnimeTimer)
		{
			uv.V += 1;
			nCounter = 0;
			if (uv.V == nFrame_Y / 2)
				nCounter = -10;
		}
		if (uv.V > nFrame_Y-1)
		{
			nEnemyState = IDLE_ENEMY_STATE;
			uv.U = (float)(IDLE_ENEMY_STATE);
			uv.V = 0;
		}
		ControlAICollisionWithWall(false);
		break;
	default: 
		break;
	}
	if (pIntroductionLogo)
	{
		pIntroductionLogo->SetWorldPosition(Position.x+65, Position.y+65);
		if (GetFocalPoint() == this && IsCameraOnFocus())
		{
			pIntroductionLogo->ActivateIntroductionLogo();
		}
	}
}

//*****************************************************************************
//BunBunAIControl関数
//ぶんぶんと言う敵の種類のAIを管理する
//引数：void
//戻：void
//*****************************************************************************
void Enemy::BunBunAIControl()
{
	//敵のAIのコントロール
	if (nEnemyState != nIdleState) {
		fEnemySpeed = 0;
		return;
	}
	Player* pPlayer = GetPlayerPointer();
	if (!pPlayer)
		return;
	if(!bDetectedByPlayer)
		bDetectedByPlayer = IsInCollision(GetHitbox(), pPlayer->GetPlayerHitbox(HB_ENEMY_DETECTION_ZONE));
	if (bDetectedByPlayer && !IsInCollision(GetHitbox(), pPlayer->GetPlayerHitbox(HB_ENEMY_DETECTION_ZONE))
		&& p_goCurrentFloor != pPlayer->GetFloor() && ++nFramesForPlayerDetectionVerification>60*7) {
		nFramesForPlayerDetectionVerification = 0;
		bDetectedByPlayer = false;
	}
	if (p_goCurrentFloor && p_goCurrentFloor == pPlayer->GetFloor())
	{
		if (pPlayer->GetWorldPosition().x < Position.x)
			nDirection = LEFT_DIR;
		else
			nDirection = RIGHT_DIR;
	}
	if (bDetectedByPlayer && p_goCurrentFloor)
	{
		if (IsInCollision(GetHitbox(), pPlayer->GetPlayerHitbox(HB_LEFT_LIMIT)) && nDirection == RIGHT_DIR ||
			IsInCollision(GetHitbox(), pPlayer->GetPlayerHitbox(HB_RIGHT_LIMIT)) && nDirection == LEFT_DIR)
		{
			if (nWaitingFrames < BUN_BUN_WAITING_FRAMES)
			{
				nWaitingFrames++;
			}
			else {
				nCounter = 0;
				uv.V = 0;
				nEnemyState = ATTACKING_ENEMY;
			}
		}
		else {
			nWaitingFrames = 0;
			if (fEnemySpeed < BUN_BUN_SPEED)
				fEnemySpeed += 0.075f;
			if (bBulletCollision)
				fEnemySpeed -= 0.075f;
			if (!bSuperArmorBroken)
				Position.x += fEnemySpeed * nDirection;
			else
				Position.x += fEnemySpeed * nDirection *-0.5f;
			ControlAICollisionWithWall(true);
			if (!bCanFallFromFloor)
			{
				if (!IsInCollision(GetFeetHitbox(), p_goCurrentFloor->GetHitbox()))
					Position.x -= fEnemySpeed * nDirection;
			}
		}
	}
}

//*****************************************************************************
//SetNoScoreEnemy関数
//敵はスコアを上げないことを設定する
//引数：bool
//戻：void
//*****************************************************************************
void Enemy::SetNoScoreEnemy(bool bNoScored)
{
	bNoScoreEnemy = bNoScored;
}

//*****************************************************************************
//ControlAICollisionWithWall関数
//壁と当たり判定を管理する
//引数：bool
//戻：void
//*****************************************************************************
void Enemy::ControlAICollisionWithWall(bool bChangeDirection)
{
	//壁の当たり判定を確認する
	bool bCollisionWithWall = false;
	SceneInGame* pGame = GetCurrentGame();
	if (!pGame)
		return;
	Go_List* Walls = pGame->GetWalls();
	if (!Walls)
		return;
	go_node* pWalls = Walls->HeadNode;
	while (pWalls != nullptr)
	{
		if (pWalls->Object != nullptr)
		{
			if (pWalls->Object->IsInUse()) {
				if (IsInCollision(pWalls->Object->GetHitbox(), GetHitbox()) && IsInCollision(pWalls->Object->GetHitbox(), GetLeftSideHitbox()))
				{
					while (IsInCollision(pWalls->Object->GetHitbox(), GetHitbox()))
						Position.x++;
					if (bChangeDirection)
						nDirection *= -1;
					return;
				}
				else if (IsInCollision(pWalls->Object->GetHitbox(), GetHitbox()) && IsInCollision(pWalls->Object->GetHitbox(), GetRightSideHitbox()))
				{
					while (IsInCollision(pWalls->Object->GetHitbox(), GetHitbox()))
						Position.x--;
					if (bChangeDirection)
						nDirection *= -1;
					return;
				}
			}
		}
		pWalls = pWalls->next;
	}
}

//*****************************************************************************
//CheckCollisionWithSpikes関数
//スパイクと当たり判定を管理する
//引数：void
//戻：void
//*****************************************************************************
bool Enemy::CheckCollisionWithSpikes()
{
	//スパイクの当たり判定を確認する
	SceneInGame* pGame = GetCurrentGame();
	if (!pGame)
		return false;
	Go_List* Walls = pGame->GetLevelObjects();
	if (!Walls)
		return false;
	go_node* pSpikes = Walls->HeadNode;
	HitBox enemyHB = GetHitbox();
	while (pSpikes != nullptr)
	{
		if (pSpikes->Object != nullptr)
		{
			if (IsInCollision(pSpikes->Object->GetHitbox(), enemyHB) && pSpikes->Object->GetType()==SPIKE_TYPE)
				return true;
		}
		pSpikes = pSpikes->next;
	}
	return false;
}

//*****************************************************************************
//IsNoScoreEnemy関数
//敵はスコアを上げないことを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Enemy::IsNoScoreEnemy()
{
	return bNoScoreEnemy;
}

//*****************************************************************************
//AttackCollisionControl関数
//プレイヤーの攻撃動作と当たり判定を管理する
//引数：void
//戻：void
//*****************************************************************************
void Enemy::AttackCollisionControl()
{
	static int nAlternateAttack = 0;
	Player* pPlayer = nullptr;
	pPlayer = GetPlayerPointer();
	if (!pPlayer)
		return;
	Weapon** pPlayerWeapons;
	nCounter = 0;
	bBlockGravity = false;
	bool bWallCollision;
	float newPosXForPlayer;
	if (fSuperArmor < 0)
		fSuperArmor = 0;
	if (fSuperArmor == 0 && !bSuperArmorBroken) 
	{
		ActivateAuraEffect(true);
		bSuperArmorBroken = true;
		PlaySound(SOUND_LABEL_SE_SA_BROKEN);
		VibrateXinput(65535, 65535, 30);
	}
	int nAirDifference = 1;
	if (!(pPlayer->IsPlayerOnTheGround()))
		nAirDifference = 25;
	int power=13;
	if (bSuperArmorBroken)
		power += 10;
	int nMoveToAddCalculation = 1;
	if (nHitAttackState == HAND_PUNCH_B_STATE)
		nMoveToAddCalculation += HAND_PUNCH_A_STATE * nAttackPlayerWeapon*nAirDifference;
	else
		nMoveToAddCalculation += nHitAttackState * nAttackPlayerWeapon*nAirDifference;
	AddMoveToRankMeter(nMoveToAddCalculation, power);
	int nScoredEnemy = 1;
	if (bNoScoreEnemy)
		nScoredEnemy = 0;
	nFramesSinceAttack = 120;
	SceneInGame* pGame = GetCurrentGame();
	SetZoomOffset(1.115f, 0.0095f, 20);
	if (!pGame)
		return;
	switch (nHitAttackState)
	{
	case HAND_STRONGPUNCH_B: case HAND_STRONGPUNCH_A: case HAND_STRONGPUNCH_A_ALT:
		if (nRecoveringFramesFromAttack > 0 || nEnemyState == ATTACKED_BYDUALHANDS)
			break;
		if (nHitAttackState == HAND_STRONGPUNCH_B) {
			SetDamage(10);
			pGame->AddToScoreWithMultiplier(10* nScoredEnemy);
		}
		else {
			SetDamage(20);
			pGame->AddToScoreWithMultiplier(20* nScoredEnemy);
		}
		if (bSuperArmorBroken) {
			SetDamage(2);
			pGame->AddToScoreWithMultiplier(5* nScoredEnemy);
		}
		uv.V = 0.f;
		nEnemyState = ATTACKED_BY_BIG_PUNCHES_AIR;
		uv.U = (float)(ATTACKED_NORMALPUNCH_A + nAlternateAttack);
		nAlternateAttack++;
		nRecoveringFramesFromAttack = nFrame_Y + 30;
		nCounter = 0;
		nDirection = pPlayer->GetDirection()*-1;
		if (nAlternateAttack > 1)
			nAlternateAttack = 0;
		uv.V = 5;
		nLockedAnimationFrames = 20;
		if (p_goCurrentFloor) {
			nEnemyState = ATTACKED_BY_BIG_PUNCHES;
			nLockedAnimationFrames = 14;
		}
		CameraTremble(15, 15, 7);
		PlaySound(SOUND_LABEL_SE_BIGHIT);
		VibrateXinput(65535, 65535, 25);
		SetZoomOffset(0.825f, 0.0095f, 15);
		if (!bSuperArmorBroken)
			fSuperArmor -= 5.5f;
		if (bSuperArmorBroken) {
			SetZoomOffset(0.825f, 0.0095f, 15);
			SetBouncingState({ 25 * (float)(pPlayer->GetDirection()),0 }, 4);
		}
		break;
	case HAND_GROUND_A_STATE:
		SetDamage(1);
		pGame->AddToScoreWithMultiplier(1* nScoredEnemy);
		if(bSuperArmorBroken)
			pGame->AddToScoreWithMultiplier(3* nScoredEnemy);
		uv.V = 0;
		nEnemyState = ATTACKED_BYHANDGROUND;
		uv.U = ATTACKED_BY_AIR_GROUNDDOWN;
		nRecoveringFramesFromAttack = 0;
		nDirection = pPlayer->GetDirection();
		pPlayerWeapons = GetWeapons();
		pPlayerWeapons[WEAPON_HANDS]->SetDashTime(0);
		VibrateXinput(65535, 65535, 25);
		nAnimeMirror = 1;
		if (!bSuperArmorBroken)
			fSuperArmor -= 0.5f;
		else
			SetBouncingState({ 15 * (float)(pPlayer->GetDirection()),-15 }, 4);
		break;
	case HAND_SENDDOWN:
		if (nRecoveringFramesFromAttack > 0)
			break;
		SetDamage(5);
		pGame->AddToScoreWithMultiplier(5* nScoredEnemy);
		if (bSuperArmorBroken) {
			SetDamage(2);
			pGame->AddToScoreWithMultiplier(3* nScoredEnemy);
		}
		PlaySound(SOUND_LABEL_SE_MEDIUMHIT);
		VibrateXinput(65535 / 1.5f, 65535 / 1.5f, 35);
		uv.V = 0;
		nEnemyState = ATTACKED_BYDUALHANDS;
		nAlternateAttack++;
		nRecoveringFramesFromAttack = 20;
		nCounter = 0;
		Position.x = pPlayer->GetWorldPosition().x + 60 * (int)pPlayer->GetDirection();
		CameraTremble(5, 5, 2);
		if ((int)pPlayer->GetDirection() < 0)
			Position.x -= 190;
		if (!p_goCurrentFloor) {
			uv.U = ENEMY_ONAIR;
			fyForce = 30;
		}
		else {
			uv.U = ATTACKED_BY_AIR_GROUNDDOWN;
			uv.V = 4;
		}
		nDirection = pPlayer->GetDirection()*-1;
		if (nAlternateAttack > 1)
			nAlternateAttack = 0;
		if (!bSuperArmorBroken)
			fSuperArmor -= 0.25f;
		else
			SetBouncingState({ 0,-15 }, 1);
		break;
	case HAND_UPPERCUT_ALT:
		if (nRecoveringFramesFromAttack > 0 || nEnemyState == ATTACKED_BYDUALHANDS)
			break;
		SetDamage(5);
		pGame->AddToScoreWithMultiplier(5* nScoredEnemy);
		if (bSuperArmorBroken) {
			SetDamage(2);
			pGame->AddToScoreWithMultiplier(3* nScoredEnemy);
		}
		PlaySound(SOUND_LABEL_SE_MEDIUMHIT);
		VibrateXinput(65535 / 1.5f, 65535 / 1.5f, 30);
		uv.V = 0;
		nEnemyState = ATTACKED_BYDUALHANDS;//TODO:新しい状態を作る
		uv.U = ENEMY_ONAIR;
		nAlternateAttack++;
		nRecoveringFramesFromAttack = 2;
		nCounter = 0;
		Position.x = pPlayer->GetWorldPosition().x + 60 * (int)pPlayer->GetDirection();
		CameraTremble(5, 5, 2);
		if ((int)pPlayer->GetDirection() < 0)
			Position.x -= 190;
		Position.y += 15;
		fyForce = -30;
		nDirection = pPlayer->GetDirection()*-1;
		if (nAlternateAttack > 1)
			nAlternateAttack = 0;
		if (!bSuperArmorBroken)
			fSuperArmor -= 0.25f;
		break;

	case HAND_SPIN_STATE:
		PlaySound(SOUND_LABEL_SE_ALTHIT);
		VibrateXinput(65535 / 2, 65535 / 2, 30);
		if (!bSuperArmorBroken)
			fSuperArmor -= 0.025f;
		else
			pGame->AddToScoreWithMultiplier(1* nScoredEnemy);
		SetDamage(1);
		pGame->AddToScoreWithMultiplier(1* nScoredEnemy);
		nEnemyState = ATTACKED_BYDUALHANDS;//TODO:新しい状態を作る
		if(uv.U!= ATTACKED_NORMALPUNCH_A || (uv.V>9 && (uv.U == ATTACKED_NORMALPUNCH_A || uv.U == ATTACKED_NORMALPUNCH_B)))
			uv.V = 4.f;
		uv.U = (float)(ATTACKED_NORMALPUNCH_A + nAlternateAttack);
		nAlternateAttack++;
		nRecoveringFramesFromAttack = 5;
		nCounter = 0;
		Position.x = pPlayer->GetWorldPosition().x + 20 * (int)pPlayer->GetDirection();
		if(pPlayer->GetDirection()<0)
			Position.x -= 220;
		CameraTremble(5, 5, 2);
		Position.y = pPlayer->GetWorldPosition().y;
		fyForce = -5;
		nDirection = pPlayer->GetDirection()*-1;
		if (nAlternateAttack > 1)
			nAlternateAttack = 0;
		break;

	case HAND_UPPERCUT:
		if (nRecoveringFramesFromAttack > 0 || nEnemyState == ATTACKED_BYDUALHANDS)
			break;
		PlaySound(SOUND_LABEL_SE_MEDIUMHIT);
		VibrateXinput(65535 / 1.5f, 65535 / 1.5f, 35);
		SetDamage(5);
		pGame->AddToScoreWithMultiplier(5* nScoredEnemy);
		if (bSuperArmorBroken) {
			SetDamage(2);
			pGame->AddToScoreWithMultiplier(2* nScoredEnemy);
		}
		uv.V = 0;
		nEnemyState = ATTACKED_BYDUALHANDS;//TODO:新しい状態を作る
		uv.U = ENEMY_ONAIR;
		nAlternateAttack++;
		nRecoveringFramesFromAttack = nFrame_Y + 30;
		nCounter = 0;
		Position.x = pPlayer->GetWorldPosition().x + 60 * (int)pPlayer->GetDirection();
		CameraTremble(5, 5, 2);
		if ((int)pPlayer->GetDirection() < 0)
			Position.x -= 190;
		Position.y += 15;
		fyForce = -30;
		nDirection = pPlayer->GetDirection()*-1;
		if (nAlternateAttack > 1)
			nAlternateAttack = 0;
		if (!bSuperArmorBroken)
			fSuperArmor -= 0.25f;
		else
			SetBouncingState({ 0,15 }, 2);
		break;
	case HAND_DUAL_A_STATE: case HAND_DUAL_B_STATE:
		if (nRecoveringFramesFromAttack > 0 || nEnemyState == ATTACKED_BYDUALHANDS)
			break;	
		if (!bSuperArmorBroken)
			fSuperArmor -= 5.5f;
		uv.V = 0;
		SetDamage(10);
		VibrateXinput(65535/2, 65535/2, 30);
		pGame->AddToScoreWithMultiplier(10* nScoredEnemy);
		if (bSuperArmorBroken) {
			SetDamage(3);
			pGame->AddToScoreWithMultiplier(3* nScoredEnemy);
		}
		nEnemyState = ATTACKED_BYDUALHANDS;
		uv.U = (float)(ATTACKED_NORMALPUNCH_A + nAlternateAttack);
		nAlternateAttack++;
		nRecoveringFramesFromAttack = nFrame_Y+30;
		nCounter = 0;
		nDirection = pPlayer->GetDirection()*-1;
		if (nAlternateAttack > 1)
			nAlternateAttack = 0;
		break;

	case HAND_DOWNSTOGROUND_STATE:
		SetDamage(2);
		pGame->AddToScoreWithMultiplier(2* nScoredEnemy);
		if (bSuperArmorBroken) {
			SetDamage(2);
			pGame->AddToScoreWithMultiplier(2* nScoredEnemy);
		}
		uv.V = 0;
		nEnemyState = ATTACKED_BY_AIR_GROUNDDOWN;
		uv.U = ATTACKED_BY_AIR_GROUNDDOWN;
		nRecoveringFramesFromAttack = 0;
		nDirection = pPlayer->GetDirection() *-1;
		pPlayerWeapons = GetWeapons();
		pPlayerWeapons[WEAPON_HANDS]->SetDashTime(0);
		nAnimeMirror = 1;
		VibrateXinput(65535, 65535, 25);
		if(!p_goCurrentFloor)
			fyForce = 30;
		if (!bSuperArmorBroken)
			fSuperArmor -= 0.5f;
		else
			SetBouncingState({ 15 * (float)(pPlayer->GetDirection()),15 }, 2);
		break;
	case DOWN_DASH_ATTACK: case HAND_AIR_DASH:
		SetDamage(8);
		pGame->AddToScoreWithMultiplier(8* nScoredEnemy);
		if (bSuperArmorBroken) {
			SetDamage(2);
			pGame->AddToScoreWithMultiplier(2* nScoredEnemy);
		}
		//COOLDOWN
		PlaySound(SOUND_LABEL_SE_MEDIUMHIT);
		VibrateXinput(65535 / 1.5f, 65535 / 1.5f, 35);
		uv.V = 0.f;
		nEnemyState = ATTACKED_BY_DASH;
		uv.U = (float)(ATTACKED_NORMALPUNCH_A + nAlternateAttack);
		nAlternateAttack++;
		nRecoveringFramesFromAttack = 7;

		newPosXForPlayer = Position.x;
		bWallCollision = CheckCollisionWithWall(GetHitbox());
		if (bWallCollision)
		{
			if (pPlayer->GetDirection() != RIGHT_DIR)
				Position.x += 260;
		}
		if (pPlayer->IsPlayerOnTheGround()) {
			if(!bWallCollision)
				pPlayer->SetWorldPosition(newPosXForPlayer, pPlayer->GetWorldPosition().y);
		}
		else {
			if(!bWallCollision)
				pPlayer->SetWorldPosition(newPosXForPlayer, Position.y);
			pPlayer->SetYForce(-15);
		}
		nDirection = pPlayer->GetDirection()*-1;
		if(!bWallCollision)
			Position.x += 50 * (int)pPlayer->GetDirection();
		if ((int)pPlayer->GetDirection() < 0)
			Position.x -= 160;
		if (nAlternateAttack > 1)
			nAlternateAttack = 0;
		pPlayerWeapons = GetWeapons();
		pPlayerWeapons[WEAPON_HANDS]->SetDashTime(0);
		if (nHitAttackState == DOWN_DASH_ATTACK)
			pPlayerWeapons[WEAPON_HANDS]->SetCoolDownDashFrames(COOLDOWN_FRAMES_AFTER_DASH_ATTACK);
		if (!bSuperArmorBroken)
			fSuperArmor -= 0.5f;
		else {
			if(!bWallCollision)
				SetBouncingState({ 15 * (float)(pPlayer->GetDirection()),2 }, 2);
			else
				SetBouncingState({ 15 * (float)(pPlayer->GetDirection())*-1,2 }, 2);
		}
		CameraTremble(1, 1, 15);
		break;
	default:
		SetDamage(1);
		pGame->AddToScoreWithMultiplier(1* nScoredEnemy);
		if(bSuperArmorBroken)
			pGame->AddToScoreWithMultiplier(1* nScoredEnemy);
		if (nRecoveringFramesFromAttack > 0)
			break;
		uv.V = 0;
		PlaySound(SOUND_LABEL_SE_HIT);
		VibrateXinput(65535.f / 3.f, 65535.f / 3.f, 25);
		fAttackedAcceleration = 1;
		nEnemyState = ATTACKED_NORMALPUNCH_A + nAlternateAttack;
		uv.U = (float)(ATTACKED_NORMALPUNCH_A + nAlternateAttack);
		nAlternateAttack++;
		nRecoveringFramesFromAttack = nFrame_Y - 4;
		if (!p_goCurrentFloor)
			fyForce = -10;
		Position.x = pPlayer->GetWorldPosition().x + 60 * (int)pPlayer->GetDirection();
		CameraTremble(5, 5, 2);
		if ((int)pPlayer->GetDirection() < 0)
			Position.x -= 170;
		nDirection = pPlayer->GetDirection()*-1;
		pPlayer->SetExternalMovement(nFrame_Y, REGULAR_ATTACK_DISTANCE_AFTER_PUNCH);
		if (nAlternateAttack > 1)
			nAlternateAttack = 0;
		if (!bSuperArmorBroken)
			fSuperArmor -= 0.125f;
		break;
	}
	p_ShieldEffect->SetAlpha(SHIELD_ALFA);
	if(nHP<=0)
		CameraTremble(5, 5, 2);
}

//*****************************************************************************
//ActivateAuraEffect関数
//オーラのエフェクトを活性化する関数
//引数：bool
//戻：void
//*****************************************************************************
void Enemy::ActivateAuraEffect(bool bNormalAura)
{
	if (bNormalAura)
		p_goAuraEffect->gpTexture = pAuraTexture;
	else
		p_goAuraEffect->gpTexture = pAuraTextureAlt;
	p_goAuraEffect->SetSpriteSize(1, 1);
	p_goAuraEffect->SetUse(true);
}

//*****************************************************************************
//RecoveryFramesControl関数
//回復フレームを管理する
//引数：void
//戻：void
//*****************************************************************************
void Enemy::RecoveryFramesControl()
{
	nRecoveringFramesFromAttack--;
	if (nRecoveringFramesFromAttack < 0)
		nRecoveringFramesFromAttack = 0;
}

//*****************************************************************************
//SetDamage関数
//ダメージを設定する
//引数：int
//戻：void
//*****************************************************************************
void Enemy::SetDamage(int damage)
{
#if DEBUG_ENEMY_HEALTH
	printf("HP: %d rank: %d\n", nHP, GetRank());
#endif
	if (IsNoStyleNoGameOn() && GetRank() < 5)
		return;
	nHP -= damage;
}

//*****************************************************************************
//End関数
//終了の関数
//引数：void
//戻：void
//*****************************************************************************
void Enemy::End()
{
	UninitSprite();
	SAFE_DELETE(p_goAuraEffect);
	for (int i = 0; i < MAX_MINIAURA_EFFECTS; i++)
		SAFE_DELETE(p_goMiniAuraEffect[i]);
	SAFE_DELETE(p_ShieldEffect);
	SAFE_DELETE(pIntroductionLogo);
}

//*****************************************************************************
//CheckBounceWithFloor関数
//弾みの状態の間に床と当たり判定を確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Enemy::CheckBounceWithFloor()
{
	SceneInGame* pGame = GetCurrentGame();
	if (!pGame)
		return false;
	go_node* floorsNode = pGame->GetFloors()->HeadNode;
	while (floorsNode != nullptr)
	{
		if (floorsNode->Object != nullptr) {

			if (IsInCollision(floorsNode->Object->GetHitbox(), GetHitbox()))
			{
				while (IsInCollision(floorsNode->Object->GetHitbox(), GetHitbox()))
					Position.y++;
				return true;
			}
		}
		floorsNode = floorsNode->next;
	}
	return false;
}

//*****************************************************************************
//SetRoom関数
//レベル部屋を設定する
//引数：int
//戻：void
//*****************************************************************************
void Enemy::SetRoom(int newID_Room)
{
	nRoomID = newID_Room;
}

//*****************************************************************************
//GetFeetHitbox関数
//足のヒットボックスを戻す
//引数：void
//戻：HitBox
//*****************************************************************************
HitBox Enemy::GetFeetHitbox()
{
	GetHitbox();
	return { FeetHitbox.x + Position.x, FeetHitbox.y + Position.y, FeetHitbox.w, FeetHitbox.h, FeetHitbox.print, FeetHitbox.Sprite };
}

//*****************************************************************************
//GetRightSideHitbox関数
//右境界のヒットボックスを戻す
//引数：void
//戻：HitBox
//*****************************************************************************
HitBox Enemy::GetRightSideHitbox()
{
	return { RightSideHitbox.x + Position.x, RightSideHitbox.y + Position.y, RightSideHitbox.w, RightSideHitbox.h, RightSideHitbox.print, RightSideHitbox.Sprite };
}

//*****************************************************************************
//GetRightSideHitbox関数
//左境界のヒットボックスを戻す
//引数：void
//戻：HitBox
//*****************************************************************************
HitBox Enemy::GetLeftSideHitbox()
{
	return { LeftSideHitbox.x + Position.x, LeftSideHitbox.y + Position.y, LeftSideHitbox.w, LeftSideHitbox.h, LeftSideHitbox.print, LeftSideHitbox.Sprite };
}

//*****************************************************************************
//GetState関数
//敵の状態を戻す
//引数：void
//戻：int
//*****************************************************************************
int Enemy::GetState()
{
	return nEnemyState;
}

GameObject * Enemy::GetCurrentFloor()
{
	return p_goCurrentFloor;
}

GameObject * Enemy::GetIntroLogoGameObject()
{
	return pIntroductionLogo;
}

//*****************************************************************************
//SetBouncingState関数
//弾みの状態を設定する
//引数：XMFLOAT2, int
//戻：void
//*****************************************************************************
void Enemy::SetBouncingState(XMFLOAT2 ImpulseDirection, int Bounces)
{
	SceneInGame* pGame = GetCurrentGame();
	if (!pGame)
		return;
	if (p_goCurrentFloor)
	{
		while (IsInCollision(p_goCurrentFloor->GetHitbox(), GetHitbox()))
			Position.y++;
		p_goCurrentFloor = nullptr;
	}
	nEnemyState = BOUNCING_STATE;
	xm2ImpulseDirection = ImpulseDirection;
	nBounces = Bounces;
	fNewScale = ZOOM_OUT_SCALE * pGame->GetLevelScale();
	nWaitingScaleFrames = 60;
	nFramesSinceAttack = 120;
	bScalingInProcess = true;
	BouncingEnemy = this;
}

//*****************************************************************************
//SetEnemyAsCatched関数
//プレイヤーに取られた状態を設定する
//引数：void
//戻：void
//*****************************************************************************
void Enemy::SetEnemyAsCatched()
{
	nEnemyState = CATCHED_STATE;
}

//*****************************************************************************
//SetCanFallFromFloor関数
//床から落ちることが出来る可能性を設定する
//引数：bool
//戻：void
//*****************************************************************************
void Enemy::SetCanFallFromFloor(bool bCanfall)
{
	bCanFallFromFloor = bCanfall;
}

//*****************************************************************************
//SetEnemyID関数
//敵のIDを設定する
//引数：int
//戻：void
//*****************************************************************************
void Enemy::SetEnemyID(int newID)
{
	nEnemyID = newID;
}

//*****************************************************************************
//Draw関数
//レンダリングの関数
//引数：void
//戻：void
//*****************************************************************************
void Enemy::Draw()
{
	if (IsEnemyDead() || !bIsInUse)
		return;
	SetLocalPosition(Position.x, Position.y);
	SetSpriteFrameSize(1.0f / nFrame_X, 1.0f / nFrame_Y);
	SetSpriteUV(uv.U / (float)nFrame_X, uv.V / (float)nFrame_Y);
	bool bShowHPWithColor = SHOW_HP_WITH_COLOR;
if(bShowHPWithColor || IsNoStyleNoGameOn())
	if(!bSuperArmorBroken)
		SetColorB((float)nHP / (float)nMaxHP);
	DrawSprite(GetDeviceContext());
	if(p_goAuraEffect)
		p_goAuraEffect->Draw();
	for (int i = 0; i < MAX_MINIAURA_EFFECTS; i++)
	{
		if (!p_goMiniAuraEffect[i])continue;
		if (!(p_goMiniAuraEffect[i]->IsInUse()))continue;
		p_goMiniAuraEffect[i]->Draw();
	}
	if(p_ShieldEffect)
		p_ShieldEffect->Draw();
	if (pIntroductionLogo)
		pIntroductionLogo->Draw();
	if (PRINT_HITBOX)
	{
		if (AttackPolygonHitbox && AttackPolygonHitbox->IsSpriteInited() && AttackHitbox.w>0 && AttackHitbox.h >0) {
			AttackPolygonHitbox->SetLocalPosition(AttackHitbox.x, AttackHitbox.y);
			AttackPolygonHitbox->SetSpriteSize(AttackHitbox.w, AttackHitbox.h);
			AttackPolygonHitbox->DrawSpriteHB(GetDeviceContext());
		}
	}
}

//*****************************************************************************
//IsEnemyDead関数
//敵は死んでいることを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Enemy::IsEnemyDead()
{
	if (p_goAuraEffect)
		return nHP <= 0 && Size.x <= 0 && Size.y <= 0 && !(p_goAuraEffect->IsInUse());
	else
		return nHP <= 0 && Size.x <= 0 && Size.y <= 0;
}

//*****************************************************************************
//IsEnemyInUse関数
//敵は使えることを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Enemy::IsEnemyInUse()
{
	return bIsInUse;
}

bool Enemy::IsSuperArmorBroken()
{
	return bSuperArmorBroken;
}

//*****************************************************************************
//SetUse関数
//敵は使えることを設定する
//引数：bool
//戻：void
//*****************************************************************************
void Enemy::SetUse(bool bUse)
{
	bIsInUse = bUse;
}

//*****************************************************************************
//GetEnemyID関数
//敵のIDを戻す
//引数：void
//戻：int
//*****************************************************************************
int Enemy::GetEnemyID()
{
	return nEnemyID;
}

//*****************************************************************************
//GetEnemyHP関数
//敵の活力を戻す
//引数：void
//戻：int
//*****************************************************************************
int Enemy::GetEnemyHP()
{
	return nHP;
}

//*****************************************************************************
//SetWallLimits関数
//壁が境界としてを設定する
//引数：GameObject*, GameObject*
//戻：void
//*****************************************************************************
void Enemy::SetWallLimits(GameObject* pWallR, GameObject* pWallL)
{
	pWallLimitLeft = pWallL;
	pWallLimitRight = pWallR;
}

//*****************************************************************************
//SetFloorLimit関数
//床が下の境界としてを設定する
//引数：GameObject*
//戻：void
//*****************************************************************************
void Enemy::SetFloorLimit(GameObject* FloorLimit)
{
	pFloorLimit = FloorLimit;
}

void Enemy::SetAsIntroductionEnemy()
{
	if (bIsIntroductionEnemy)
		return;
	bIsIntroductionEnemy = true;
	pIntroductionLogo = new GameObject();
	pIntroductionLogo->InitEnemyIntroductionLogo(nEnemyType);
	pIntroductionLogo->SetWorldPosition(Position);
}

bool Enemy::CheckCollisionWithWall(HitBox HB_HitBoxToCheck)
{
	SceneInGame* pGame = GetCurrentGame();
	if (!pGame)
		return false;
	Go_List* Walls = pGame->GetWalls();
	go_node* pWalls = Walls->HeadNode;
	while (pWalls != nullptr)
	{
		if (pWalls->Object != nullptr)
		{
			if (IsInCollision(pWalls->Object->GetHitbox(), HB_HitBoxToCheck) && pWalls->Object->IsInUse())
				return true;
		}
		pWalls = pWalls->next;
	}
	return false;
}

void Enemy::SetInitialPosition(XMFLOAT2 InitPos)
{
	x2InitialPosition = InitPos;
}

int Enemy::GetEnemyType()
{
	return nEnemyType;
}

XMFLOAT2 Enemy::GetInitialPos()
{
	return x2InitialPosition;
}

//****************
//敵リスト
//****************

//*****************************************************************************
// コンストラクタ変数
//*****************************************************************************
Enemy_List::Enemy_List()
{
	HeadNode = nullptr;
	bPauseEnemyUpdate = false;
	BouncingEnemy = nullptr;
}

Enemy_List::~Enemy_List()
{
	End();
}

//*****************************************************************************
//AddEnemy関数
//リストに敵を入れる
//引数：Enemy
//戻：void
//*****************************************************************************
void Enemy_List::AddEnemy(Enemy newObject)
{
	enemy_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList != nullptr)
			pPositionList = pPositionList->next;
		pPositionList = new enemy_node();
		pPositionList->Object = new Enemy();
		*(pPositionList->Object) = newObject;
		pPositionList->next = nullptr;
	}
	else {
		HeadNode = new enemy_node();
		HeadNode->Object = new Enemy();
		if(pPositionList)
			(pPositionList->Object) = &newObject;
		HeadNode->next = nullptr;
	}
}

//*****************************************************************************
//AddEnemy関数
//リストに敵を入れる
//引数：int, XMFLOAT2, int
//戻：Enemy*
//*****************************************************************************
Enemy* Enemy_List::AddEnemy(int nEnemyType, XMFLOAT2 x2Position)
{
	return AddEnemy(nEnemyType, x2Position, REGULAR_ENEMY_ID);
}

//*****************************************************************************
//AddEnemy関数
//リストに敵を入れる
//引数：int, XMFLOAT2, int, int
//戻：Enemy*
//*****************************************************************************
Enemy* Enemy_List::AddEnemy(int nEnemyType, XMFLOAT2 x2Position, int nEnemy_ID)
{
	enemy_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		enemy_node* pWorkList = new enemy_node();
		pWorkList->Object = new Enemy();
		pWorkList->Object->Init(nEnemyType);
		pWorkList->Object->SetLocalPosition(x2Position.x, x2Position.y);
		pWorkList->Object->SetEnemyID(nEnemy_ID);
		pWorkList->Object->SetInitialPosition(x2Position);
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		return pWorkList->Object;
	}
	else {
		HeadNode = new enemy_node();
		HeadNode->Object = new Enemy();
		HeadNode->Object->Init(nEnemyType);
		HeadNode->Object->SetEnemyID(nEnemy_ID);
		HeadNode->Object->SetLocalPosition(x2Position.x, x2Position.y);
		HeadNode->Object->SetInitialPosition(x2Position);
		HeadNode->next = nullptr;
		return HeadNode->Object;
	}
	return nullptr;
}

//*****************************************************************************
//GetEnemy関数
//リストに敵を入れる
//引数：int, XMFLOAT2, int, int
//戻：void
//*****************************************************************************
Enemy* Enemy_List::GetEnemy(int nIdToGet)
{
	if (HeadNode == nullptr)
		return nullptr;
	enemy_node* pPositionList = HeadNode;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr) {
			if (nIdToGet == pPositionList->Object->GetEnemyID())
				return  pPositionList->Object;
		}
		pPositionList = pPositionList->next;
	}
	return nullptr;
}

void Enemy_List::Save(const char * szFilename)
{
	FILE *pFile;
	char szFinalfilename[256] = "data/levels/";
	strcat_s(szFinalfilename, szFilename);
	strcat_s(szFinalfilename, ".bin");
	if (strcmp(szFilename, "") == 0)
	{
		strcpy_s(szFinalfilename, "Default.bin");
	}
	fopen_s(&pFile, szFinalfilename, "wb");
	if (HeadNode == nullptr)
		return;
	enemy_node* pPositionList = HeadNode;
	while (true) {

		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr)
		{
			EnemyContainer enemyContainer;
			enemyContainer.nEnemyType = pPositionList->Object->GetEnemyType();
			enemyContainer.x2Position = pPositionList->Object->GetInitialPos();
			enemyContainer.nEnemy_ID = pPositionList->Object->GetEnemyID();
			if(pFile)
				fwrite(&enemyContainer, sizeof(EnemyContainer), 1, pFile);
		}
		pPositionList = pPositionList->next;
	}
	printf("SAVED OK: %s\n", szFinalfilename);
	if(pFile)
		fclose(pFile);
}

void Enemy_List::Load(const char * szFilename)
{
	FILE *pFile;
	char szFinalfilename[256] = "data/Levels/";
	strcat_s(szFinalfilename, szFilename);
	strcat_s(szFinalfilename, ".bin");
	fopen_s(&pFile, szFinalfilename, "rb");
	if (pFile == NULL)
	{
#ifdef USE_CONSOLE
		printf_s("%sのファイルはない…\n", szFinalfilename);
#endif
		return;
	}
	EnemyContainer* go_container = new EnemyContainer();
	while ((fread(go_container, sizeof(EnemyContainer), 1, pFile)))
		AddEnemy(go_container->nEnemyType, go_container->x2Position, go_container->nEnemy_ID);
#ifdef USE_CONSOLE
	printf("LOADED: %s\n", szFinalfilename);
#endif
	delete(go_container);
	fclose(pFile);
}

void Enemy_List::DeleteLastPosObject()
{
	if (HeadNode == nullptr)
		return;
	enemy_node* pPositionList = HeadNode->next;
	enemy_node* PreviousPos = HeadNode;
	if (pPositionList == nullptr)
	{
		if (PreviousPos->Object) {
			delete(PreviousPos->Object);
			PreviousPos->Object = nullptr;
			delete(PreviousPos);
			HeadNode = PreviousPos = nullptr;
			return;
		}
	}
	while (true) {
		if (pPositionList && pPositionList->next == nullptr)
			break;
		if(pPositionList)
			pPositionList = pPositionList->next;
		if(PreviousPos)
			PreviousPos = PreviousPos->next;
	}
	if (pPositionList->Object) {
		delete(pPositionList->Object);
		pPositionList->Object = nullptr;
		delete(pPositionList);
		pPositionList = nullptr;
		PreviousPos->next = nullptr;
	}
	return;
}

//*****************************************************************************
//Update関数
//リストの処理をアップデートする関数
//引数：void
//戻：void
//*****************************************************************************
void Enemy_List::Update()
{
	DynamicZoomControl();
	if (HeadNode == nullptr)
		return;
	enemy_node* pPositionList = HeadNode;
	enemy_node* PreviousPos = nullptr;
	int count = 0;
	while (true) {
		if (pPositionList == nullptr)
			break;
		count++;
		if (pPositionList->Object != nullptr) {
			pPositionList->Object->Update();
			if (pPositionList->Object->IsEnemyDead() && pPositionList->Object->GetEnemyID()==REGULAR_ENEMY_ID)
				DeleteObject(pPositionList->Object);
		}
		if (pPositionList) {
			pPositionList = pPositionList->next;
			if (!PreviousPos)
				PreviousPos = HeadNode;
			else
				PreviousPos = PreviousPos->next;
		}
	}
}

//*****************************************************************************
//Draw関数
//リストのオブジェクトのレンダリング
//引数：void
//戻：void
//*****************************************************************************
void Enemy_List::Draw()
{
	if (HeadNode == nullptr)
		return;
	enemy_node* pPositionList = HeadNode;
	while (true) {

		if (pPositionList == nullptr)
			break;

		if (pPositionList->Object)
			pPositionList->Object->Draw();
		pPositionList = pPositionList->next;
	}
}

void Enemy_List::DeleteObject(Enemy * pSearch)
{
	if (HeadNode == nullptr)
		return;
	if (HeadNode->Object == pSearch)
	{
		enemy_node* pPositionList = HeadNode;
		HeadNode = HeadNode->next;
		delete(pPositionList->Object);
		pPositionList->Object = nullptr;
		delete(pPositionList);
		pPositionList = nullptr;
		return;
	}
	if (HeadNode->next->Object == pSearch)
	{
		enemy_node* pPositionList = HeadNode->next;
		HeadNode->next = HeadNode->next->next;
		delete(pPositionList->Object);
		pPositionList->Object = nullptr;
		delete(pPositionList);
		pPositionList = nullptr;
		return;
	}
	enemy_node* pPositionList = HeadNode->next;
	enemy_node* PreviousPos = HeadNode;
	if (pPositionList == nullptr)
		return;
	while (true) {
		if (pPositionList->next == nullptr)
			return;
		if (pPositionList->Object == pSearch)
			break;
		pPositionList = pPositionList->next;
		PreviousPos = PreviousPos->next;
	}
	if (pPositionList->Object) {
		PreviousPos->next = pPositionList->next;
		delete(pPositionList->Object);
		pPositionList->Object = nullptr;
		delete(pPositionList);
		pPositionList = nullptr;
	}
	return;
}

void Enemy_List::ResetList()
{
	if (HeadNode == nullptr)
		return;
	enemy_node* pPositionList = HeadNode;
	enemy_node* pWork = nullptr;
	int Count = 0;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr) {
			Count++;
			SAFE_DELETE(pPositionList->Object);
		}
		pWork = pPositionList;
		pPositionList = pPositionList->next;
		if (pWork)
			SAFE_DELETE(pWork);
	}
	printf("GameObjects Deleted: %d\n", Count);
	HeadNode = nullptr;
}

//*****************************************************************************
//End関数
//終了の関数
//引数：void
//戻：void
//*****************************************************************************
void Enemy_List::End()
{
	if (HeadNode == nullptr)
		return;
	enemy_node* pPositionList = HeadNode;
	enemy_node* pWork = nullptr;
	int count = 0;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr) {
			count++;
			delete(pPositionList->Object);
			pPositionList->Object = nullptr;
			pWork = pPositionList->next;
			delete(pPositionList);
			pPositionList = pWork;
		}
		else {
			pPositionList = pPositionList->next;
		}
	/*	pWork = pPositionList;
		if (pWork) {
			pPositionList = pPositionList->next;
			delete(pWork);
			pWork = nullptr;
		}*/
	}
	Player* pPlayer = GetPlayerPointer();
	if (pPlayer)
		pPlayer->ResetPlayerEnemyList();
	SAFE_RELEASE(pAuraTexture);
	SAFE_RELEASE(pAuraTextureAlt);
	SAFE_RELEASE(pMiniAuraATexture);
	SAFE_RELEASE(pMiniAuraBTexture);
	SAFE_RELEASE(pShieldTexture);
#ifdef USE_CONSOLE
	printf("DELETED ENEMIES: %d\n", count);
#endif
}

//*****************************************************************************
//PauseEnemyUpdate関数
//敵の処理を止める
//引数：bool
//戻：void
//*****************************************************************************
void PauseEnemyUpdate(bool bPause)
{
	bPauseEnemyUpdate = bPause;
}

int GetEnemyCountInMemory()
{
	return nEnemyCount;
}

//*****************************************************************************
//DynamicZoomControl関数
//敵が攻撃されている間に、カメラのズームを管理する
//引数：void
//戻：void
//*****************************************************************************
void DynamicZoomControl()
{
	SceneInGame* pGame = GetCurrentGame();
	if (!pGame)
		return;
	if (!bScalingInProcess || NO_DYNAMIC_ZOOM)
		return;
	float fCurrentScale = GetUniversalScale();
	float fCurrentStageScale = pGame->GetLevelScale();
	if (nWaitingScaleFrames > 0 && BouncingEnemy) 
	{
		if (fCurrentScale != fNewScale)
		{
			fCurrentScale -= 0.025f;
			if (fCurrentScale < fNewScale)
				fCurrentScale = fNewScale;
			SetUniversalScale(fCurrentScale);
		}
		else {
			if(BouncingEnemy->GetState()!=BOUNCING_STATE || BouncingEnemy->GetEnemyHP()<=0)
				nWaitingScaleFrames--;
		}
	} else {
		if (fCurrentScale < fCurrentStageScale)
		{
			fCurrentScale += 0.0125f;
			if (fCurrentScale >= fCurrentStageScale) {
				fCurrentScale = fCurrentStageScale;
				bScalingInProcess = false;
			}
			SetUniversalScale(fCurrentScale);
		}
	}
}
