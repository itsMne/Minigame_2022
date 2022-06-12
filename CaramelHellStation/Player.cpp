//*****************************************************************************
// Player.cpp
// プレイヤーコントロールと主人公のステートマシンの管理
// Author : Mane
//*****************************************************************************
#include "Player.h"
#include "GameInputManager.h"
#include "string.h"
#include "Camera.h"
#include <stdio.h>
#include "SceneInGame.h"
#include "sound.h"
#include "UniversalVariables.h"
#include "RankManager.h"
#include "EventHandler.h"
#include "TextureManager.h"
#include "DebugAim.h"
#include "Enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define INITIAL_POSITION		   {0.0f, 0.0f,0.0f}
#define FRAME_X					   4
#define FRAME_Y					   10
#define POLYGON_SIZE_X			   1595 / 6
#define POLYGON_SIZE_Y			   1595 / 6
#define P_SPEED					   7
#define FLIPPING_SPEED			   15//180 インスタント　//15　ペーパー感じ
#define UV_IDLE_A				   {0,0} 
#define UV_IDLE_B				   {0,1} 
#define UV_MOVING_A				   {0,2} 
#define UV_MOVING_B				   {0,3} 
#define UV_LOCKON_A				   {2,3} 
#define UV_LOCKON_B				   {2,4} 
#define UV_DOWN_B				   {1,2} 
#define UV_JUMPING				   {2,2} 
#define UV_FALLING				   {0,2} 
#define UV_ATTACKA				   {1,0} 
#define UV_ATTACKB				   {2,0} 
#define UV_ATTACK_STRONG		   {1,3} 
#define UV_ATTACKED				   {1,4} 
#define UV_DASH					   {1,1} 
#define UV_GUITARDASH			   {0.5f,7} 
#define UV_AIRTAUNT				   {1,5} 
#define UV_TAUNT_A				   {2,5} 
#define UV_TAUNT_B				   {2,6} 
#define PRINT_PLAYER_HITBOX		   true
#define PRINT_ENEMY_DETECTION_ZONE false
#define GRAVITY_FORCE			   2
#define MAX_YFORCE				   90
#define JUMP_FORCE				   36.0f
#define JUMP_RESIST_TIMER		   10
#define INPUT_RESET_MAX			   21
#define FLOWER_SPAWN			   20
#define INITIAL_HP				   5
#define IFRAMES_AFTER_JUMP		   35
#define OFFSET_FRAMES			   15
#define TAUNT_FRAMES			   45
#define DROPPED_ENEMY_MOVE_ID	   -100
#define TAUNTED_ENEMY_MOVE_ID	   -101
#define AIR_TAUNTED_ENEMY_MOVE_ID	   -102

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Player* thisPlayer;
Weapon* weapons[WEAPON_MAX] = { nullptr };
float		fTeleportFrameCounter;
bool	bPausePlayerUpdate;
bool	bAlternateFlowerSound;
SceneInGame* pGame=nullptr;

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
Player::Player()
{
	bStateAfterTeleportIsDamage = false;
	nDirection = 1;
	nExternalMoveDistance = 0;
	nExternalMoveFrames = 0;
	nInvinsibilityFramesAfterDamage = 0;
	thisPlayer = this;
	bJumpAttackLimit = false;
	bAddedEmptySpace = false;
	bLockInputs = false;
	fTeleportAcceleration = 0;
	nAttackedStateTimer = 0;
	nColorSwapFrame = 0;
	nCurrentUV = 0;
	fTeleportFrameCounter = 0;
	nGravityCancelFrames = 0;
	nHP = nMaxHP = INITIAL_HP;
	if (IsHellsTouchMode())
		nHP = nMaxHP = 1;
	Position = { 0,0,0 };
	uvAnim = 0;
	uv = UV_IDLE_A;
	nCurrentState = PLAYER_IDLE_STATE;
	bLockOn = false;
	//円光を設定する
	p_Halo = new GameObject();
	p_Halo->Init(GetTexture(TEXTURE_HALO), GO_HALO_TYPE);
	bAlternateFlowerSound = false;
	nCurrentWeapon = WEAPON_HANDS;
	Floors = nullptr;
	Walls = nullptr;
	Enemies = nullptr;
	goCurrentFloor = nullptr;
	bIsOnTheFloor = false;
	f_yForce = 0;
	nFrame_X = FRAME_X;
	nFrame_Y = FRAME_Y;
	nAirResist = 0;
	strcpy_s(szInputs, "********");
	nResetInputTimer = INPUT_RESET_MAX;
	nFlowerSpawnTimer = 0;
	bFallAttack = false;
	bJumpAttack = false;
	SetPlayerPointer(this);
	enCatchedEnemy = nullptr;
	p_goAuraEffect = nullptr;
	p_DebugAim = nullptr;
	Parent = nullptr;
	p_Spotlight = nullptr;
	bTeleportingToPos = true;
	nOffsetInputFrames = 0;
	nUVGroundTauntChange = 0;
	nTauntFrames = 0;
	bDrawWeapon = true;
	bCanAirTaunt = true;
	xm2NewTeleportPos = { 0,0 };
	xm2LastPosBeforeJump = { 0,0 };
	nInvinsibilityFramesAfterJumping = 0;
	nLockInputWhileTauntCounter = 0;
	for (int i = 0; i < WEAPON_MAX; i++)
		weapons[i] = nullptr;
}

Player::~Player()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void Player::Init()
{
	bPausePlayerUpdate = false;
	nInvinsibilityFramesAfterDamage = 0;
	bLockInputs = false;
	InitSprite();
	gpTexture = GetTexture(TEXTURE_PLAYER);
	Position = INITIAL_POSITION; // {0,0,0};
	SetSpriteSize(POLYGON_SIZE_X, POLYGON_SIZE_Y);
	nDirection = RIGHT_DIR;
	fTeleportAcceleration = 0;

	//武器の設定
	for (int i = 0; i < WEAPON_MAX; i++)
	{
		weapons[i] = new Weapon();
		weapons[i]->Init(i);
		weapons[i]->SetPlayerPointer(this);
	}
	//ヒットボックスの設定
	hitBoxes[HB_BODY] = { 0,-10,40,225,  PRINT_PLAYER_HITBOX, nullptr };
	hitBoxes[HB_RIGHT_LIMIT] = { 30,-10,10,215,  PRINT_PLAYER_HITBOX, nullptr };
	hitBoxes[HB_LEFT_LIMIT] = { -30,-10,10,215,  PRINT_PLAYER_HITBOX, nullptr };
	hitBoxes[HB_FEET] = { 0,-110,40,40,  PRINT_PLAYER_HITBOX, nullptr };
	hitBoxes[HB_HEAD] = { 0,85,60,40, PRINT_PLAYER_HITBOX, nullptr };
	hitBoxes[HB_ENEMY_DETECTION_ZONE] = { 0,50,1300,400, PRINT_ENEMY_DETECTION_ZONE, nullptr };
	if (PRINT_PLAYER_HITBOX && SHOW_HITBOXES)
	{
		for (int i = 0; i < HB_MAX; i++)
		{
			Sprite* hitBoxPrint;
			for (int i = 0; i < HB_MAX; i++)
			{
				hitBoxPrint = (Sprite*)hitBoxes[i].Sprite;
				if (!hitBoxes[i].print)continue;

				if (hitBoxes[i].Sprite == nullptr) {
					hitBoxes[i].Sprite = new Sprite(true);
					hitBoxPrint = (Sprite*)hitBoxes[i].Sprite;
				}
				
				if (!(hitBoxPrint->IsSpriteInited()))
				{
					hitBoxPrint->InitSprite();
					hitBoxPrint->gpTexture = GetTexture(TEXTURE_HITBOX);
				}
				hitBoxPrint->SetAlpha(0.5f);
			}
		}
	}
	//花の設定
	for (int i = 0; i < MAX_FLOWERS; i++)
	{
		go_Flowers[i].Init(GetTexture(TEXTURE_FLOWERS), FLOWER_TYPE);
		go_Flowers[i].SetUse(false);
	}
	bIsOnTheFloor = false;
	nExternalMoveDistance = 0;
	nExternalMoveFrames = 0;

	p_goAuraEffect = new GameObject();
	p_goAuraEffect->Init(GetTexture(TEXTURE_AURA_EFFECT_PLAYER), AURA_EFFECT_TYPE);
	p_goAuraEffect->SetSpriteSize(1, 1);
	p_goAuraEffect->SetUse(false);
	if (DEBUG_MODE)
	{
		p_DebugAim = new DebugAim();
		//p_DebugAim->InitDebugAim();
	}
	if (!p_Spotlight)
	{
		p_Spotlight = new SpotLight();
		p_Spotlight->SetParent(this);
		p_Spotlight->SetLocalPosition(0, 15);
	}
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void Player::Update()
{
	if (!pGame) {//ゲームがなければプレイヤーが使えない
		pGame = GetCurrentGame();
		if (!pGame)
			return;
	}
	if (DEBUG_MODE && p_DebugAim)//デバッグで拠点を見える為に、このオブジェクトを管理する
	{
		if (p_DebugAim->IsInUse()) 
		{
			p_DebugAim->Update();
			if (GetInput(DEBUG_POINTER_ACTIVATE))
			{
				SetFocalPoint(this);
				PauseEnemyUpdate(false);
				PausePlayerUpdate(false);
				p_DebugAim->SetUse(false);
			}
			return;
		}
		else {
			XMFLOAT3 PlayerPos = GetWorldPosition();
			p_DebugAim->SetLocalPosition(PlayerPos.x, PlayerPos.y);
			if (GetInput(DEBUG_POINTER_ACTIVATE)) 
			{
				SetFocalPoint(p_DebugAim);
				PauseEnemyUpdate(true);
				PausePlayerUpdate(true);
				p_DebugAim->SetUse(true);
			}
		}
	}
	if (bPausePlayerUpdate)//プレイヤーはポースなら使えない
		return;
	for (int i = 0; i < WEAPON_MAX; i++)//全部の武器を確認する
		if (!weapons[i])return;
	if (nHP <= 0)//HPは0ならプレイヤーが死んでるコントロール関数しか使わない
	{
		DeadControl();
		return;
	}
	if (nCurrentState == PLAYER_TELEPORTING)//プレイヤーはテレポートを使っている場合で、テレポートのコントロールしか使わない
	{
		TeleportControl();
		if (nCurrentState == PLAYER_TELEPORTING)
			return;
	}
	float fLockOnSpeed = 1;
	if(nOffsetInputFrames>0 && !GetInput(HOLD_ATTACK))//あざけている時に、プレイヤーは攻撃ボタンずっと押したら、攻撃動作のコマンド続けられる
		nOffsetInputFrames--;
	bLockOn = GetInput(INPUT_LOCKON);
	weapons[nCurrentWeapon]->ShowWeapon(bLockOn && nCurrentState != PLAYER_SHOOTING_STATE);
	GeneralInputsControl();
	GravityControl();
	ExternalMoveControl();
	if (enCatchedEnemy)//取った敵の管理
	{
		enCatchedEnemy->SetEnemyAsCatched();
	}
	InvinsibilityFramesControl();//ダメージをもらう後のフレームを管理する
	if (GetInput(INPUT_TAUNT) && nCurrentState != PLAYER_ATTACKED_STATE && nCurrentState != PLAYER_TAUNTING_STATE)//あざける管理
	{
		if (!bIsOnTheFloor && bCanAirTaunt) {
			nCurrentState = PLAYER_TAUNTING_STATE;
			if (!GetInput(HOLD_ATTACK))
				ResetInputs();
			else {
				nOffsetInputFrames = OFFSET_FRAMES;
#if DEBUG_OFFSET
				printf("TAUNT OFFSET\n");
#endif
			}
			f_yForce = 0;
			Jump(JUMP_FORCE*0.55f);
			bCanAirTaunt = false;
			nLockInputWhileTauntCounter = 10;
			weapons[nCurrentWeapon]->ResetToIdle();
			if (CheckCollisionWithEnemy(HB_ENEMY_DETECTION_ZONE))
				AddMoveToRankMeter(AIR_TAUNTED_ENEMY_MOVE_ID, 60);
		}
		else if(bIsOnTheFloor)
		{
			weapons[nCurrentWeapon]->ResetToIdle();
			nCurrentState = PLAYER_TAUNTING_STATE;
			nTauntFrames = TAUNT_FRAMES;
			nLockInputWhileTauntCounter = TAUNT_FRAMES;
			if (!GetInput(HOLD_ATTACK))
				ResetInputs();
			else {
				nOffsetInputFrames = OFFSET_FRAMES;
#if DEBUG_OFFSET
				printf("TAUNT OFFSET\n");
#endif
			}
		}

	}

	//ステートマシン管理
	switch (nCurrentState)
	{
	case PLAYER_IDLE_STATE://アイドル状態
		SetColor(1.0f, 1.0f, 1.0f);
		if(!(GetInput(MOVE_DOWN) && bIsOnTheFloor))
			nCurrentUV = PLAYER_IDLE_UV;
		if (!bIsOnTheFloor)
		{
			if(f_yForce>0)
				nCurrentUV = PLAYER_FALLING_UV;
			else
				nCurrentUV = PLAYER_JUMPING_UV;
		}
		if (GetInput(MOVE_LEFT) || GetInput(MOVE_RIGHT))
			nCurrentState = PLAYER_MOVING_STATE;
		if (GetInput(MOVE_DOWN) && bIsOnTheFloor && nCurrentState!= PLAYER_SHOOTING_STATE)
			nCurrentState = PLAYER_DOWN_STATE;
		if (goCurrentFloor)
		{
			if(goCurrentFloor->IsSafeFloor())
				SetLastPosBeforeJump();
		}
		break;
	case PLAYER_MOVING_STATE://動いてる状態
		MoveStateControl(&fLockOnSpeed);
		break;
	case PLAYER_DOWN_STATE://下の方向ボタンを押してる状態
		nCurrentUV = PLAYER_DOWN_UV;
		if (!GetInput(MOVE_DOWN))
			nCurrentState = PLAYER_IDLE_STATE;
		break;
	case PLAYER_ATTACKING_STATE://攻撃動作を使ってる状態
		if (bFallAttack == true)
			f_yForce+=4;
		weapons[nCurrentWeapon]->ShowWeapon(true);
		break;
	case PLAYER_SHOOTING_STATE://シューティングの状態
		ShootingControl(fLockOnSpeed);
		if (GetInput(MOVE_UP))
		{
				if (!GetInput(MOVE_RIGHT) && !GetInput(MOVE_LEFT))
					weapons[WEAPON_GUITAR]->SetState(GUITAR_UP);
				else
					weapons[WEAPON_GUITAR]->SetState(GUITAR_DIAGONAL_UP);
		}else if (GetInput(MOVE_DOWN) && !bIsOnTheFloor)
		{
				if (!GetInput(MOVE_RIGHT) && !GetInput(MOVE_LEFT))
					weapons[WEAPON_GUITAR]->SetState(GUITAR_DOWN);
				else					 
					weapons[WEAPON_GUITAR]->SetState(GUITAR_DIAGONAL_DOWN);
		}
		else if (GetInput(MOVE_DOWN) && bIsOnTheFloor)
		{
			nCurrentState = PLAYER_DOWN_STATE;
		}else{
			weapons[WEAPON_GUITAR]->SetState(GUITAR_FRONTAL);
		}
		break;
	case PLAYER_TAUNTING_STATE://あざける状態
		if (goCurrentFloor && nCurrentUV == PLAYER_AIR_TAUNT_UV) {
			nCurrentState = PLAYER_IDLE_STATE;
			bCanAirTaunt = true;
			bLockInputs = false;
			break;
		}
		if (!bIsOnTheFloor) {
			nCurrentUV = PLAYER_AIR_TAUNT_UV;
			if((GetInput(MOVE_RIGHT) && nDirection == RIGHT_DIR) || (GetInput(MOVE_LEFT) && nDirection == LEFT_DIR))
				Position.x += 7 * nDirection;
			else if ((GetInput(MOVE_RIGHT) && nDirection == LEFT_DIR) || (GetInput(MOVE_LEFT) && nDirection == RIGHT_DIR))
				Position.x -= 7 * nDirection;
			Position.x -= 10 * nDirection;
		}
		else {
			nCurrentUV = PLAYER_TAUNT_UV;
			bLockInputs = true;
			if (nTauntFrames == TAUNT_FRAMES - 15) {
				if (CheckCollisionWithEnemy(HB_ENEMY_DETECTION_ZONE))
					AddMoveToRankMeter(TAUNTED_ENEMY_MOVE_ID, 60);
			}
			if(nTauntFrames < TAUNT_FRAMES/2)
				bLockInputs = false;
			if (--nTauntFrames <= 0)
			{
				nCurrentState = PLAYER_IDLE_STATE;
				bCanAirTaunt = true;
				bLockInputs = false;
			}
			break;
		}
		if (--nLockInputWhileTauntCounter > 0)
			bLockInputs = true;
		else
			bLockInputs = false;
		if (GetInput(HOLD_ATTACK))
		{
			nOffsetInputFrames = OFFSET_FRAMES;
#if DEBUG_OFFSET
			printf("TAUNT OFFSET\n");
#endif
		}
		break;
	case PLAYER_ATTACKED_STATE://ダメージをもらった状態
		nCurrentUV = PLAYER_ATTACKED_UV;
		if (++nAttackedStateTimer > 40)
		{
			nInvinsibilityFramesAfterDamage = 60;
			SetColor(1, 1, 1);
			Rotation.y = 0;
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
			nCurrentState = PLAYER_IDLE_STATE;
		}
		if (nColorSwapFrame++ > 5)
		{
			nColorSwapFrame = 0;
			if (color.x == 1)
				SetColor(0.85f, 0.45f, 0.45f);
			else
				SetColor(1, 1, 1);
		}
		Rotation.y+=10;
		break;
	default:
		break;
	}
	if(nCurrentUV == PLAYER_DASHGUITAR_UV)
		FlowerSpawnControl();
	weapons[WEAPON_GUITAR]->ShowWeapon(nCurrentState == PLAYER_SHOOTING_STATE && !weapons[nCurrentWeapon]->IsWeaponShowing());
	ControlUVAnimation();
	HaloControl();
	WeaponsControl();
	for (int i = 0; i < MAX_FLOWERS; i++)
	{
		if (!((go_Flowers + i)->IsInUse()))
			continue;
		(go_Flowers + i)->Update();
	}
	AuraControl();
	if (p_Spotlight)
		p_Spotlight->Update();
	if (GetInput(INPUT_PAUSE))
		pGame->PauseGame();
}

//*****************************************************************************
//InvinsibilityFramesControl関数
//ダメージがもらえないフレームを管理する
//引数：void
//戻：void
//*****************************************************************************
void Player::InvinsibilityFramesControl()
{
	if (nInvinsibilityFramesAfterDamage > 0)
		nInvinsibilityFramesAfterDamage--;
	if (nInvinsibilityFramesAfterJumping > 0)
		nInvinsibilityFramesAfterJumping--;
#if DEBUG_IFRAMES_JUMP
	printf("IFRAMES JUMP: %d\n", nInvinsibilityFramesAfterJumping);
#endif
}

//*****************************************************************************
//SetLastPosBeforeJump関数
//スパイクと当たり判定の後で最後の安全な場所に戻るなので
//この関数でその場所を決めます
//引数：void
//戻：void
//*****************************************************************************
void Player::SetLastPosBeforeJump()
{
	XMFLOAT3 CurrentPlayerPos = GetWorldPosition();
	xm2LastPosBeforeJump.x = CurrentPlayerPos.x;
	xm2LastPosBeforeJump.y = CurrentPlayerPos.y;
}

//*****************************************************************************
//AuraControl関数
//オーラのエフェクトを管理する
//引数：void
//戻：void
//*****************************************************************************
void Player::AuraControl()
{
	if (p_goAuraEffect) {
		XMFLOAT3 CurrentPlayerPos = GetWorldPosition();
		p_goAuraEffect->SetWorldPosition(CurrentPlayerPos.x, CurrentPlayerPos.y);
		p_goAuraEffect->Update();
	}
}

//*****************************************************************************
//TeleportControl関数
//プレイヤーとテレポートの管理
//引数：void
//戻：void
//*****************************************************************************
void Player::TeleportControl()
{
	AuraControl();
	//サイズコントロール
	XMFLOAT3 CurrentPlayerPos = GetWorldPosition();
	if(bStateAfterTeleportIsDamage)
		nCurrentUV = PLAYER_ATTACKED_UV;
	else
		nCurrentUV = PLAYER_LOCKON_UV;
	fTeleportAcceleration+=1.75f;
	if (bTeleportingToPos)
	{
		if (Size.x > 0)
		{
			Size.x -= fTeleportAcceleration;
			if (Size.x < 0)
				Size.x = 0;
			if (Size.y == 0 && Size.x == 0) {
				fTeleportAcceleration = 0;
				ActivateAuraEffect();
			}
		}
		if (Size.y > 0)
		{
			Size.y -= fTeleportAcceleration;
			if (Size.y < 0)
				Size.y = 0;
			if (Size.y == 0 && Size.x == 0) {
				fTeleportAcceleration = 0;
				ActivateAuraEffect();
			}
		}

		if (Size.y == 0 && Size.x == 0)
		{
			if (CurrentPlayerPos.x < xm2NewTeleportPos.x)
			{
				CurrentPlayerPos.x += fTeleportAcceleration;
				if (CurrentPlayerPos.x > xm2NewTeleportPos.x)
					CurrentPlayerPos.x = xm2NewTeleportPos.x;
			}else if (CurrentPlayerPos.x > xm2NewTeleportPos.x)
			{
				CurrentPlayerPos.x -= fTeleportAcceleration;
				if (CurrentPlayerPos.x < xm2NewTeleportPos.x)
					CurrentPlayerPos.x = xm2NewTeleportPos.x;
			}

			if (CurrentPlayerPos.y < xm2NewTeleportPos.y)
			{
				CurrentPlayerPos.y += fTeleportAcceleration;
				if (CurrentPlayerPos.y > xm2NewTeleportPos.y)
					CurrentPlayerPos.y = xm2NewTeleportPos.y;
			}else if (CurrentPlayerPos.y > xm2NewTeleportPos.y)
			{
				CurrentPlayerPos.y -= fTeleportAcceleration;
				if (CurrentPlayerPos.y < xm2NewTeleportPos.y)
					CurrentPlayerPos.y = xm2NewTeleportPos.y;
			}
		}
		if (CurrentPlayerPos.y == xm2NewTeleportPos.y && CurrentPlayerPos.x == xm2NewTeleportPos.x) {
			fTeleportAcceleration = 0;
			bTeleportingToPos = false;
		}
		SetWorldPosition(CurrentPlayerPos.x, CurrentPlayerPos.y);
	}
	else 
	{
		fTeleportFrameCounter+=1.75f;
		if (Size.x < POLYGON_SIZE_X)
		{
			Size.x += fTeleportAcceleration;
			if (Size.x > POLYGON_SIZE_X)
				Size.x = POLYGON_SIZE_X;
		}
		if (Size.y < POLYGON_SIZE_Y)
		{
			Size.y += fTeleportAcceleration;
			if (Size.y > POLYGON_SIZE_Y)
				Size.y = POLYGON_SIZE_Y;
		}
		if (Size.y == POLYGON_SIZE_Y && Size.x == POLYGON_SIZE_X)
		{
			fTeleportAcceleration = 0;
			if (bStateAfterTeleportIsDamage) {
				bTeleportingToPos = true;
				AttackPlayer(0, true);
			}
			else
			{
				nCurrentState = PLAYER_IDLE_STATE;
				bTeleportingToPos = true;
			}
			ActivateAuraEffect();
			PlaySound(SOUND_LABEL_SE_TELEPORTOUT);
			weapons[nCurrentWeapon]->ResetWeaponState();
			SetLockInputs(false);
		}
		if (fTeleportFrameCounter > 30) {
			nCurrentState = PLAYER_IDLE_STATE;
			bTeleportingToPos = true;
		}
		f_yForce = 0;
	}
	ControlUVAnimation();
	HaloControl();
}

//*****************************************************************************
//DeadControl関数
//死んでいる状態の管理
//引数：void
//戻：void
//*****************************************************************************
void Player::DeadControl()
{
	nCurrentUV = PLAYER_ATTACKED_UV;
	ResetUniversalScale();
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
	if (nColorSwapFrame++ > 5)
	{
		nColorSwapFrame = 0;
		if (color.x == 1)
			SetColor(0.85f, 0.45f, 0.45f);
		else
			SetColor(1, 1, 1);
	}
	fTeleportAcceleration+=0.075f;
	if (Size.y > 0) {
		Size.y-= fTeleportAcceleration;
		if (Size.y < 0)
			Size.y = 0;
	}
	if (Size.x > 0) {
		Size.x-= fTeleportAcceleration;
		if (Size.x < 0)
			Size.x = 0;
	}
	ControlUVAnimation();
	HaloControl();
}

//*****************************************************************************
//MoveStateControl関数
//動いている状態の管理
//引数：float
//戻：void
//*****************************************************************************
void Player::MoveStateControl(float *fLockOnSpeed)
{
	float fSpeedWhenColWithEnemy = 1;
	Enemy* pEnemyCol = nullptr;
	if (nDirection == RIGHT_DIR)
		pEnemyCol = CheckCollisionWithEnemy(HB_RIGHT_LIMIT);
	else
		pEnemyCol = CheckCollisionWithEnemy(HB_LEFT_LIMIT);
	if (pEnemyCol) 
	{
		fSpeedWhenColWithEnemy = 0.25f;
		if (CheckCollisionWithWall(pEnemyCol->GetHitbox()))
			fSpeedWhenColWithEnemy = 0;
	}
	nCurrentUV = PLAYER_MOVING_UV;
	if (!bIsOnTheFloor)
	{
		if (f_yForce>0)
			nCurrentUV = PLAYER_FALLING_UV;
		else
			nCurrentUV = PLAYER_JUMPING_UV;
	}
	if (GetInput(MOVE_RIGHT)) {
		if (nDirection == LEFT_DIR && !bLockOn)
			nDirection = RIGHT_DIR;
		else if (nDirection == LEFT_DIR && bLockOn) {
			*fLockOnSpeed = -0.25f;
			nCurrentUV = PLAYER_LOCKON_UV;
		}
		if (GetInput(MOVE_DOWN) && nCurrentState != PLAYER_SHOOTING_STATE)
			nCurrentState = PLAYER_DOWN_STATE;
	}
	if (GetInput(MOVE_LEFT)) {
		if (nDirection == RIGHT_DIR && !bLockOn)
			nDirection = LEFT_DIR;
		else if (nDirection == RIGHT_DIR && bLockOn) {
			*fLockOnSpeed = -0.25f;
			nCurrentUV = PLAYER_LOCKON_UV;
		}

	}
	int CollisionWithWall = 1;
	if (!bLockOn) {
		if (nDirection == RIGHT_DIR) {
			if (CheckCollisionWithWall(GetPlayerHitbox(HB_RIGHT_LIMIT)))
			{
				CollisionWithWall = 0;
			}
		}
		else {
			if (CheckCollisionWithWall(GetPlayerHitbox(HB_LEFT_LIMIT)))
			{
				CollisionWithWall = 0;
			}
		}
	}
	else {
		if (CheckCollisionWithWall(GetPlayerHitbox(HB_BODY)))
		{
			CollisionWithWall = 0;
		}
	}
	Position.x += P_SPEED * (*fLockOnSpeed) * nDirection * fSpeedWhenColWithEnemy * CollisionWithWall;
	if (pEnemyCol)
	{
		pEnemyCol->SetLocalPosition(pEnemyCol->GetLocalPosition().x + P_SPEED * (*fLockOnSpeed) * nDirection * fSpeedWhenColWithEnemy, pEnemyCol->GetLocalPosition().y);
	}
	if (!(GetInput(MOVE_LEFT) || GetInput(MOVE_RIGHT)))
		nCurrentState = PLAYER_IDLE_STATE;
	FlowerSpawnControl();
}

//*****************************************************************************
//FlowerSpawnControl関数
//花が出るの管理
//引数：void
//戻：void
//*****************************************************************************
void Player::FlowerSpawnControl()
{
	int nSpawnTime = FLOWER_SPAWN;
	if (nCurrentUV == PLAYER_DASHGUITAR_UV)
		nSpawnTime /= 4;
	if (++nFlowerSpawnTimer > nSpawnTime && (bIsOnTheFloor || nCurrentUV == PLAYER_DASHGUITAR_UV))
	{
		for (int i = 0; i < MAX_FLOWERS; i++)
		{
			if ((go_Flowers + i)->IsInUse())continue;
			XMFLOAT3 CurrentPlayerPos = GetWorldPosition();
			(go_Flowers + i)->SetLocalPosition(CurrentPlayerPos.x, CurrentPlayerPos.y - 97);
			if (nCurrentUV == PLAYER_DASHGUITAR_UV)
				(go_Flowers + i)->gpTexture = GetTexture(TEXTURE_FLOWERS_SPEED);
			else
				(go_Flowers + i)->gpTexture = GetTexture(TEXTURE_FLOWERS);
			(go_Flowers + i)->SetFlowerInUse();
			if (USE_FLOWER_SOUND && nCurrentUV != PLAYER_DASHGUITAR_UV) {
				if (bAlternateFlowerSound)
					PlaySound(SOUND_LABEL_SE_FLOWER_A);
				else
					PlaySound(SOUND_LABEL_SE_FLOWER_B);
				bAlternateFlowerSound = !bAlternateFlowerSound;
			}
			break;
		}
		nFlowerSpawnTimer = 0;
	}
}

//*****************************************************************************
//AttackPlayer関数
//ダメージをもらう管理
//引数：int
//戻：bool
//*****************************************************************************
bool Player::AttackPlayer(int nDamage)
{
	return AttackPlayer(nDamage, false);
}

//*****************************************************************************
//AttackPlayer関数
//ダメージをもらう管理
//引数：int, bool
//戻：bool
//*****************************************************************************
bool Player::AttackPlayer(int nDamage, bool IsAfterTeleport)
{
	if (!IsAfterTeleport){
		if (nCurrentState == PLAYER_TELEPORTING || nCurrentState == PLAYER_ATTACKED_STATE || nInvinsibilityFramesAfterDamage>0 || nInvinsibilityFramesAfterJumping>0)
			return false;
	}
	nAttackedStateTimer = 0;
	nColorSwapFrame = 0;
	nHP -= nDamage;
	SetSpriteSize(POLYGON_SIZE_X, POLYGON_SIZE_Y);
	nCurrentState = PLAYER_ATTACKED_STATE;
	if (pGame->GetCurrentLevel() == TUTORIAL_ROOM && nHP == 0)
		nHP = 1;
	if (nHP>0)
		PlaySound(SOUND_LABEL_SE_PLAYER_DAMAGED);
	else
		PlaySound(SOUND_LABEL_SE_PLAYER_DEAD);
	ResetRanks();
	RankedEvent* p_reActive = GetCurrentActiveEvent();
	if (p_reActive)
		p_reActive->AddDamage(nDamage);
	return true;
}

//*****************************************************************************
//GeneralInputsControl関数
//インプットの管理
//引数：void
//戻：void
//*****************************************************************************
void Player::GeneralInputsControl()
{
	if (nCurrentState == PLAYER_ATTACKED_STATE)
		return;
	if (nResetInputTimer < INPUT_RESET_MAX) {
		nResetInputTimer++;
	}
	else {
		if (!bAddedEmptySpace) {
			AddInput('S');
			bAddedEmptySpace = true;
			nResetInputTimer = INPUT_RESET_MAX / 3;
		}
		else {
			if (nOffsetInputFrames <= 0)
				ResetInputs();
			else {
				nOffsetInputFrames--;
				nResetInputTimer = INPUT_RESET_MAX / 3;
			}
		}
	}
	if (GetInput(INPUT_ATTACK) && !bLockInputs) {
		AddInput('A');
		nResetInputTimer = 0;
		bAddedEmptySpace = false;
		weapons[nCurrentWeapon]->ExecuteAttack(szInputs);
	}

	if (IsPlayerInputForward(true)) {
		AddInput('F');
		nResetInputTimer = INPUT_RESET_MAX / 5;
		bAddedEmptySpace = false;
	}
	if (IsPlayerInputBackward(true)) {
		AddInput('B');
		nResetInputTimer = INPUT_RESET_MAX / 5;
		bAddedEmptySpace = false;
	}
	if (GetInput(INPUT_DOWN)) {
		AddInput('D');
		nResetInputTimer = INPUT_RESET_MAX / 5;
		bAddedEmptySpace = false;
	}
	if (GetInput(INPUT_UP)) {
		AddInput('U');
		nResetInputTimer = INPUT_RESET_MAX / 5;
		bAddedEmptySpace = false;
	}
	if ((bIsOnTheFloor && GetInput(MOVE_JUMP_TRIGGER)) || (!bIsOnTheFloor && GetInput(MOVE_JUMP_TRIGGER) && enCatchedEnemy) || (GetInput(MOVE_JUMP_TRIGGER) && weapons[nCurrentWeapon]->GetCurrentAttackPlaying() == DOWN_DASH_ATTACK)) {
		if (!GetInput(MOVE_DOWN)) {
			if (bIsOnTheFloor && weapons[nCurrentWeapon]->GetCurrentAttackPlaying() != DOWN_DASH_ATTACK) {
				Jump(JUMP_FORCE);
				PlaySound(SOUND_LABEL_SE_JUMP);
				nInvinsibilityFramesAfterJumping = IFRAMES_AFTER_JUMP;
				bAddedEmptySpace = false;
				nResetInputTimer = 0;
				if (goCurrentFloor) {
					if(goCurrentFloor->IsSafeFloor())
					SetLastPosBeforeJump();
				}
				strcpy_s(szInputs, "J*******");
				nCurrentState = PLAYER_IDLE_STATE;
				weapons[nCurrentWeapon]->ResetToIdle();
			}else if (!bIsOnTheFloor && enCatchedEnemy && weapons[nCurrentWeapon]->GetCurrentAttackPlaying() != DOWN_DASH_ATTACK)
			{
				PlaySound(SOUND_LABEL_SE_BIGHIT);
				f_yForce = 0;
				Jump(JUMP_FORCE+(JUMP_FORCE*0.35f));
				AddMoveToRankMeter(DROPPED_ENEMY_MOVE_ID, 30);
				bAddedEmptySpace = false;
				nResetInputTimer = 0;
				strcpy_s(szInputs, "J*******");
				nCurrentState = PLAYER_IDLE_STATE;
				weapons[nCurrentWeapon]->ResetToIdle();
				weapons[nCurrentWeapon]->ResetAirDash();
				enCatchedEnemy->SetBouncingState({ 0, -10 }, 2);
				enCatchedEnemy->ActivateAuraEffect(false);
				enCatchedEnemy->SetDamage(10);
				VibrateXinput(65535, 65535, 25);
				nInvinsibilityFramesAfterJumping = IFRAMES_AFTER_JUMP;
				if (!(enCatchedEnemy->IsNoScoreEnemy()))
					pGame->AddToScoreWithMultiplier(15);
				enCatchedEnemy = nullptr;
			}
			else if (weapons[nCurrentWeapon]->GetCurrentAttackPlaying() == DOWN_DASH_ATTACK)
			{
				Jump(JUMP_FORCE);
				nInvinsibilityFramesAfterJumping = IFRAMES_AFTER_JUMP;
				PlaySound(SOUND_LABEL_SE_JUMP);
				bAddedEmptySpace = false;
				nResetInputTimer = 0;
				strcpy_s(szInputs, "J*******");
				nCurrentState = PLAYER_IDLE_STATE;
				weapons[nCurrentWeapon]->ResetToIdle();
			}
		} else {
			nResetInputTimer = INPUT_RESET_MAX / 5;
			AddInput('D');
			AddInput('J');
		}
	}
	if ((GetInput(INPUT_SHOOT) || GetInput(INPUT_CATCHSHOOT)) && nCurrentState != PLAYER_ATTACKING_STATE && nCurrentState != PLAYER_DOWN_STATE) {
		nCurrentState = PLAYER_SHOOTING_STATE;
	}
}

//*****************************************************************************
//ExternalMoveControl関数
//他のオブジェクトからプレイヤーに動かせる管理
//引数：void
//戻：void
//*****************************************************************************
void Player::ExternalMoveControl()
{
	if (nExternalMoveFrames > 0) {
		nExternalMoveFrames--;
		Position.x += nDirection * nExternalMoveDistance;
	}
}

//*****************************************************************************
//ShootingControl関数
//シューティング状態の管理
//引数：float
//戻：void
//*****************************************************************************
void Player::ShootingControl(float &fLockOnSpeed)
{
	if (!(GetInput(INPUT_SHOOT) || GetInput(INPUT_CATCHSHOOT)))
	{
		nCurrentState = PLAYER_IDLE_STATE;
		return;
	}
	nCurrentUV = PLAYER_LOCKON_UV;
	float fMove = 0;
	if (GetInput(MOVE_RIGHT)) {
		fMove = 1;
		if (nDirection == LEFT_DIR && !bLockOn)
			nDirection = RIGHT_DIR;
		else if (nDirection == LEFT_DIR && bLockOn) {
			fLockOnSpeed = -0.25f;
			nCurrentUV = PLAYER_LOCKON_UV;
		}
		if (GetInput(MOVE_DOWN) && bIsOnTheFloor)
			nCurrentState = PLAYER_DOWN_STATE;
	}
	if (GetInput(MOVE_LEFT)) {
		fMove = 1;
		if (nDirection == RIGHT_DIR && !bLockOn)
			nDirection = LEFT_DIR;
		else if (nDirection == RIGHT_DIR && bLockOn) {
			fLockOnSpeed = -0.25f;
			nCurrentUV = PLAYER_LOCKON_UV;
		}

	}
	float fSpeedWhenColWithEnemy = 1;
	Enemy* pEnemyCol = nullptr;
	if (nDirection == RIGHT_DIR)
		pEnemyCol = CheckCollisionWithEnemy(HB_RIGHT_LIMIT);
	else
		pEnemyCol = CheckCollisionWithEnemy(HB_LEFT_LIMIT);
	if (pEnemyCol)
		fSpeedWhenColWithEnemy = 0;

	int CollisionWithWall = 1;
	if (!bLockOn) {
		if (nDirection == RIGHT_DIR) {
			if (CheckCollisionWithWall(GetPlayerHitbox(HB_RIGHT_LIMIT)))
			{
				CollisionWithWall = 0;
			}
		}
		else {
			if (CheckCollisionWithWall(GetPlayerHitbox(HB_LEFT_LIMIT)))
			{
				CollisionWithWall = 0;
			}
		}
	}
	else {
		if (CheckCollisionWithWall(GetPlayerHitbox(HB_BODY)))
		{
			CollisionWithWall = 0;
		}
	}

	Position.x += P_SPEED * fLockOnSpeed * nDirection * fMove * fSpeedWhenColWithEnemy * CollisionWithWall;
}

//*****************************************************************************
//ResetInputs関数
//インプットのリセットする
//引数：void
//戻：void
//*****************************************************************************
void Player::ResetInputs()
{
	bAddedEmptySpace = false;
	nResetInputTimer = 0;
	for (int i = 0; i < MAX_PLAYER_INPUT; i++)
	{
		szInputs[i] = '*';
	}
}

//*****************************************************************************
//AddInput関数
//インプットの文字を追加する
//引数：char
//戻：void
//*****************************************************************************
void Player::AddInput(char A)
{
	if (A == 'S' && szInputs[0] == '*')
		return;
	for (int i = 0; i < MAX_PLAYER_INPUT; i++)
	{
		if (szInputs[i] == '*') {
			szInputs[i] = A;
			return;
		}
	}
	for (int i = 0; i < MAX_PLAYER_INPUT; i++)
	{
		if (i != MAX_PLAYER_INPUT - 1)
		{
			szInputs[i] = szInputs[i + 1];
		}
		else {
			szInputs[i] = A;
		}
	}
}

//*****************************************************************************
//WeaponsControl関数
//武器の管理
//引数：void
//戻：void
//*****************************************************************************
void Player::WeaponsControl()
{
	for (int i = 0; i < WEAPON_MAX; i++)
	{
		weapons[i]->Update();
	}
}

//*****************************************************************************
//HaloControl関数
//ハローの管理
//引数：void
//戻：void
//*****************************************************************************
void Player::HaloControl()
{
	static int nHaloAcceleration=0;
	if (p_Halo != nullptr) {
		p_Halo->Update();
		if (Size.x == 0 && Size.y == 0 && nHP <= 0)
		{
			p_Halo->SetLocalPosition(p_Halo->GetLocalPosition().x, p_Halo->GetLocalPosition().y - nHaloAcceleration);
			nHaloAcceleration++;
		}
		else {
			nHaloAcceleration = 0;
		}
	}

}

//*****************************************************************************
//Jump関数
//ジャンプする
//引数：int
//戻：void
//*****************************************************************************
void Player::Jump(float jumpForce)
{
	bIsOnTheFloor = false;
	bCanAirTaunt = true;
	f_yForce = -jumpForce;
}

//*****************************************************************************
//GravityControl関数
//重力の管理
//引数：void
//戻：void
//*****************************************************************************
void Player::GravityControl()
{
	if (!Floors)
		Floors = pGame->GetFloors();
	if (goCurrentFloor)
	{
		HitBox cFloor = goCurrentFloor->GetHitbox();
		cFloor.y++;
		if (!IsInCollision(cFloor, GetPlayerHitbox(HB_FEET)))
		{
			goCurrentFloor = nullptr;
			bIsOnTheFloor = false;
			return;
		}
		else {
			if (nCurrentUV != PLAYER_DASHGUITAR_UV) {
				while (IsInCollision(cFloor, GetPlayerHitbox(HB_FEET)))
					Position.y++;
				Position.y--;
			}
		}
	}
	if (nGravityCancelFrames > 0)
	{
		nGravityCancelFrames--;
		f_yForce = 0.1f;
		if (GetInput(MOVE_JUMP)) {
			nGravityCancelFrames = 0;
			Jump(JUMP_FORCE);
		}
		return;
	}
	if ((!bIsOnTheFloor && nCurrentState != PLAYER_ATTACKING_STATE) || bFallAttack || bJumpAttack || f_yForce<0)
	{
		if (CheckCollisionWithEnemy(HB_FEET))
		{
			return;
		}
		float fShootingAirResist=1;
		f_yForce += GRAVITY_FORCE;
		if (nCurrentState == PLAYER_SHOOTING_STATE && f_yForce>0)
			fShootingAirResist = 0.0425f;
		if (nAirResist < JUMP_RESIST_TIMER && f_yForce>0 && GetInput(MOVE_JUMP)) 
		{
			nAirResist++;
			return;
		} 
		if (!GetInput(MOVE_JUMP))
			nAirResist = JUMP_RESIST_TIMER;
		if (f_yForce > MAX_YFORCE)
			f_yForce = MAX_YFORCE;
		Position.y -= f_yForce * fShootingAirResist;

		if (!Floors  || f_yForce <= 0)
			return;
		Floor* FindFloor = (Floor*)Floors->FindWithCollision(GetPlayerHitbox(HB_FEET));
		if (FindFloor)
		{
			goCurrentFloor = FindFloor;
			bIsOnTheFloor = true;
			while (IsInCollision(FindFloor->GetHitbox(), GetPlayerHitbox(HB_FEET)))
				Position.y++;
			Position.y--;
			nAirResist = 0;
			f_yForce = 0;
			if (bFallAttack)
				CameraTremble(12, 12, 5);
			bFallAttack = false;
			bJumpAttackLimit = false;
		}
	}
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Player::End()
{
#if DEBUG_MODE
	SAFE_DELETE(p_DebugAim);
#endif
	SAFE_DELETE(p_goAuraEffect);
	for (int i = 0; i < MAX_FLOWERS; i++)
	{
		go_Flowers[i].End();
		go_Flowers[i].UninitSprite();
	}
	for (int i = 0; i < WEAPON_MAX; i++)
		SAFE_DELETE(weapons[i]);
	SAFE_DELETE(p_Halo);
	SAFE_DELETE(p_Spotlight);
	SetPlayerPointer(nullptr);
	pGame = nullptr;
#ifdef USE_CONSOLE
	printf("PLAYER ENDED\n");
#endif
	UninitSprite();
}

//*****************************************************************************
//ControlUVAnimation関数
//UVのアニメーションを管理する
//引数：void
//戻：void
//*****************************************************************************
void Player::ControlUVAnimation()
{
	if (Size.x == 0 && Size.y == 0 && nHP <= 0)
	{
		return;
	}
	XMFLOAT3 PlayerPos = GetWorldPosition();
	p_Halo->SetLocalPosition(PlayerPos.x, PlayerPos.y + 140);
	if (nCurrentUV == PLAYER_DASHGUITAR_UV) {
		hitBoxes[HB_BODY] = { 0,-40,253.000f,58.000f,  hitBoxes[HB_BODY].print, hitBoxes[HB_BODY].Sprite };
		hitBoxes[HB_FEET] = { 0,-50,40,40,  hitBoxes[HB_FEET].print, hitBoxes[HB_FEET].Sprite };
	}
	else if (nCurrentUV == PLAYER_DOWN_UV) {
		hitBoxes[HB_BODY] = { 0,-40,60,125, hitBoxes[HB_BODY].print, hitBoxes[HB_BODY].Sprite };
		hitBoxes[HB_FEET] = { 0,-90,40,40,  hitBoxes[HB_FEET].print, hitBoxes[HB_FEET].Sprite };
	}
	else {
		hitBoxes[HB_BODY] = { 0,-10,40,225, hitBoxes[HB_BODY].print, hitBoxes[HB_BODY].Sprite };
		hitBoxes[HB_FEET] = { 0,-90,40,40,  hitBoxes[HB_FEET].print, hitBoxes[HB_FEET].Sprite };
	}
	switch (nCurrentUV)
	{
	case PLAYER_IDLE_UV:
		SetSpriteSize(POLYGON_SIZE_X, POLYGON_SIZE_Y);
		nFrame_X = FRAME_X;
		nFrame_Y = FRAME_Y;
		uvAnim++;
		if (uvAnim < 20)
			uv = UV_IDLE_A;
		else
			uv = UV_IDLE_B;
		if (uvAnim > 40)
			uvAnim = 0;
		break;
	case PLAYER_MOVING_UV:
		p_Halo->SetLocalPosition(PlayerPos.x+(35 * nDirection), PlayerPos.y + 125);
		SetSpriteSize(POLYGON_SIZE_X, POLYGON_SIZE_Y);
		nFrame_X = FRAME_X;
		nFrame_Y = FRAME_Y;
		uvAnim++;
		if (uvAnim < 20)
			uv = UV_MOVING_A;
		else
			uv = UV_MOVING_B;
		if (uvAnim > 40)
			uvAnim = 0;
		break;
	case PLAYER_LOCKON_UV:
		p_Halo->SetLocalPosition(PlayerPos.x-(12*nDirection), PlayerPos.y + 140);
		if (nCurrentState != PLAYER_TELEPORTING)
			SetSpriteSize(POLYGON_SIZE_X, POLYGON_SIZE_Y);
		nFrame_X = FRAME_X;
		nFrame_Y = FRAME_Y;
		uvAnim++;
		if (uvAnim < 20)
			uv = UV_LOCKON_A;
		else
			uv = UV_LOCKON_B;
		if (uvAnim > 40)
			uvAnim = 0;
		break;
	case PLAYER_DOWN_UV:
		p_Halo->SetLocalPosition(PlayerPos.x+30, PlayerPos.y+18);
		SetSpriteSize(POLYGON_SIZE_X, POLYGON_SIZE_Y);
		nFrame_X = FRAME_X;
		nFrame_Y = FRAME_Y;
		uv = UV_DOWN_B;
		break;
	case PLAYER_JUMPING_UV:
		SetSpriteSize(POLYGON_SIZE_X, POLYGON_SIZE_Y);
		nFrame_X = FRAME_X;
		nFrame_Y = FRAME_Y;
		uv = UV_JUMPING;
		break;
	case PLAYER_FALLING_UV:
		p_Halo->SetLocalPosition(PlayerPos.x, PlayerPos.y+80);
		SetSpriteSize(POLYGON_SIZE_X, POLYGON_SIZE_Y * 2);
		nFrame_X = FRAME_X;
		nFrame_Y = FRAME_Y/2;
		uv = UV_FALLING;
		break;
	case PLAYER_ATTACK_A_UV:
		p_Halo->SetLocalPosition(PlayerPos.x + (9 * nDirection), PlayerPos.y + 140);
		SetSpriteSize(POLYGON_SIZE_X, POLYGON_SIZE_Y);
		nFrame_X = FRAME_X;
		nFrame_Y = FRAME_Y;
		uv = UV_ATTACKA;
		break;
	case PLAYER_ATTACK_B_UV:
		p_Halo->SetLocalPosition(PlayerPos.x+(7 * nDirection), PlayerPos.y + 140);
		SetSpriteSize(POLYGON_SIZE_X, POLYGON_SIZE_Y);
		nFrame_X = FRAME_X;
		nFrame_Y = FRAME_Y;
		uv = UV_ATTACKB;
		break;
	case PLAYER_ATTACK_STRONG_UV:
		p_Halo->SetLocalPosition(PlayerPos.x+(25 * nDirection), PlayerPos.y + 140);
		SetSpriteSize(POLYGON_SIZE_X, POLYGON_SIZE_Y);
		nFrame_X = FRAME_X;
		nFrame_Y = FRAME_Y;
		uv = UV_ATTACK_STRONG;
		break;
	case PLAYER_DASH_UV:
		p_Halo->SetLocalPosition(PlayerPos.x + (90 * nDirection), PlayerPos.y + 100);
		SetSpriteSize(POLYGON_SIZE_X, POLYGON_SIZE_Y);
		nFrame_X = FRAME_X;
		nFrame_Y = FRAME_Y;
		uv = UV_DASH;
		break;
	case PLAYER_DASHGUITAR_UV:
		p_Halo->SetLocalPosition(PlayerPos.x - (40 * nDirection), PlayerPos.y + 30);
		SetSpriteSize(POLYGON_SIZE_X*2, POLYGON_SIZE_Y);
		nFrame_X = FRAME_X / 2;
		nFrame_Y = FRAME_Y;
		uv = UV_GUITARDASH;
		break;
	case PLAYER_ATTACKED_UV:
		
		if (nHP > 0 && nCurrentState != PLAYER_TELEPORTING) {
			p_Halo->SetLocalPosition(PlayerPos.x + (9 * nDirection), PlayerPos.y + 140);
			SetSpriteSize(POLYGON_SIZE_X, POLYGON_SIZE_Y);
		}
		nFrame_X = FRAME_X;
		nFrame_Y = FRAME_Y;
		uv = UV_ATTACKED;
		break;
	case PLAYER_AIR_TAUNT_UV:
		p_Halo->SetLocalPosition(PlayerPos.x + (-55 * nDirection), PlayerPos.y + 130);
		SetSpriteSize(POLYGON_SIZE_X, POLYGON_SIZE_Y);
		nFrame_X = FRAME_X;
		nFrame_Y = FRAME_Y;
		uv = UV_AIRTAUNT;
		break;
	case PLAYER_TAUNT_UV:
		p_Halo->SetLocalPosition(PlayerPos.x + (35 * nDirection), PlayerPos.y + 100);
		SetSpriteSize(POLYGON_SIZE_X, POLYGON_SIZE_Y);
		nFrame_X = FRAME_X;
		nFrame_Y = FRAME_Y;
		uv = UV_TAUNT_A;
		nUVGroundTauntChange++;
		if (nUVGroundTauntChange > 15)
			uv = UV_TAUNT_B;
		if (nUVGroundTauntChange > 30)
			nUVGroundTauntChange = 0;
		break;
	default:
		break;
	}
	ControlDirection();
}

//*****************************************************************************
//ControlDirection関数
//方向を管理する
//引数：void
//戻：void
//*****************************************************************************
void Player::ControlDirection()
{
	if (nCurrentState == PLAYER_ATTACKED_STATE)
		return;
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
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Player::Draw()
{
	if (nInvinsibilityFramesAfterDamage > 0)
	{
		if (nInvinsibilityFramesAfterDamage % 2 == 0) {
			if (p_Spotlight)
				p_Spotlight->Draw();
			return;
		}
	}
	for (int i = 0; i < MAX_FLOWERS; i++)
	{
		if (!((go_Flowers + i)->IsInUse()))
			continue;
		(go_Flowers + i)->Draw();
	}
	SetLocalPosition(Position.x, Position.y);
	SetSpriteFrameSize(1.0f / nFrame_X, 1.0f / nFrame_Y);
	SetSpriteUV(uv.U / (float)nFrame_X, uv.V / (float)nFrame_Y);
	DrawSprite(GetDeviceContext());

	if (p_Halo != nullptr)
		p_Halo->Draw();
	if (nHP <= 0)
		return;
	if (bDrawWeapon) {
		for (int i = 0; i < WEAPON_MAX; i++)
		{
			weapons[i]->Draw();
		}
	}
	Sprite* hitBoxPrint;
	for (int i = 0; i < HB_MAX; i++)
	{
		hitBoxPrint = (Sprite*)hitBoxes[i].Sprite;
		if (hitBoxPrint == nullptr )continue;
		HitBox tHBtoPrint = GetPlayerHitbox(i);
		hitBoxPrint->SetLocalPosition(tHBtoPrint.x, tHBtoPrint.y);
		hitBoxPrint->SetSpriteSize(tHBtoPrint.w, tHBtoPrint.h);
		hitBoxPrint->DrawSpriteHB(GetDeviceContext());
	}
	if (p_goAuraEffect)
		p_goAuraEffect->Draw();
	if (p_Spotlight)
		p_Spotlight->Draw();
	if (DEBUG_MODE && p_DebugAim)
		p_DebugAim->Draw();
}

//*****************************************************************************
//SetHalo関数
//ハローのオブジェクトのポインターを設定する
//引数：GameObject*
//戻：void
//*****************************************************************************
void Player::SetHalo(GameObject * goHalo)
{
	p_Halo = goHalo;
}

//*****************************************************************************
//GetDirection関数
//プレイヤーの方向を戻す
//引数：void
//戻：int
//*****************************************************************************
int Player::GetDirection()
{
	return nDirection;
}

//*****************************************************************************
//GetPlayerState関数
//プレイヤーの状態を戻す
//引数：void
//戻：int
//*****************************************************************************
int Player::GetPlayerState()
{
	return nCurrentState;
}

//*****************************************************************************
//GetYForce関数
//プレイヤーの重力フォースを戻す
//引数：void
//戻：float
//*****************************************************************************
float Player::GetYForce()
{
	return f_yForce;
}

//*****************************************************************************
//SetYForce関数
//プレイヤーの重力フォースを設定
//引数：float
//戻：void
//*****************************************************************************
void Player::SetYForce(float newYForce)
{
	f_yForce = newYForce;
}

//*****************************************************************************
//SetPlayerState関数
//プレイヤーの状態を設定
//引数：int
//戻：void
//*****************************************************************************
void Player::SetPlayerState(int newState)
{
	nCurrentState = newState;
}

//*****************************************************************************
//GetPlayerHitbox関数
//プレイヤーのヒットボックスを戻す
//引数：int
//戻：HitBox
//*****************************************************************************
HitBox Player::GetPlayerHitbox(int hitBoxToget)
{
	XMFLOAT3 CurrentPos = GetWorldPosition();
	if (hitBoxToget == HB_RIGHT_LIMIT || hitBoxToget == HB_LEFT_LIMIT)
	{
		if (nCurrentUV == PLAYER_DASHGUITAR_UV)
		{
			return { CurrentPos.x + hitBoxes[hitBoxToget].x, CurrentPos.y + hitBoxes[hitBoxToget].y-45, hitBoxes[hitBoxToget].w, hitBoxes[hitBoxToget].h/2.25f, hitBoxes[hitBoxToget].print, hitBoxes[hitBoxToget].Sprite };
		}
	}
	return { CurrentPos.x + hitBoxes[hitBoxToget].x, CurrentPos.y + hitBoxes[hitBoxToget].y, hitBoxes[hitBoxToget].w, hitBoxes[hitBoxToget].h, hitBoxes[hitBoxToget].print, hitBoxes[hitBoxToget].Sprite };
}

//*****************************************************************************
//SetLockInputs関数
//プレイヤーのインプットをロックする
//引数：bool
//戻：void
//*****************************************************************************
void Player::SetLockInputs(bool inputsLock)
{
	bLockInputs = inputsLock;
}

//*****************************************************************************
//IsPlayerInputForward関数
//プレイヤーは現在の向いている方向に動いてることを確認する
//引数：bool
//戻：bool
//*****************************************************************************
bool Player::IsPlayerInputForward(bool isTrigger)
{
	if(!isTrigger)
		return  ((((GetInput(MOVE_RIGHT) && nDirection == RIGHT_DIR)) || ((GetInput(MOVE_LEFT) && nDirection == LEFT_DIR))));
	return  ((((GetInput(INPUT_RIGHT) && nDirection == RIGHT_DIR)) || ((GetInput(INPUT_LEFT) && nDirection == LEFT_DIR))));
}

//*****************************************************************************
//IsPlayerInputBackward関数
//プレイヤーは現在の逆に向いている方向に動いてることを確認する
//引数：bool
//戻：bool
//*****************************************************************************
bool Player::IsPlayerInputBackward(bool isTrigger)
{
	if (!isTrigger)
		return  ((((GetInput(MOVE_RIGHT) && nDirection == LEFT_DIR)) || ((GetInput(MOVE_LEFT) && nDirection == RIGHT_DIR))) && GetInput(INPUT_LOCKON));
	return  ((((GetInput(INPUT_RIGHT) && nDirection == LEFT_DIR)) || ((GetInput(INPUT_LEFT) && nDirection == RIGHT_DIR)))	&& GetInput(INPUT_LOCKON));
}

//*****************************************************************************
//IsPlayerOnTheGround関数
//プレイヤーは床にいることを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Player::IsPlayerOnTheGround()
{
	return bIsOnTheFloor;
}

//*****************************************************************************
//IsPlayerDead関数
//プレイヤーは死んだことを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Player::IsPlayerDead()
{
	if (!p_Halo)
		return false;
	return nHP<=0 && Size.x == 0 && Size.y == 0 && p_Halo->GetLocalPosition().y<GetCameraLimitDown();
}

//*****************************************************************************
//IsPlayerLockingOn関数
//プレイヤーはロックオンを使ってることを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Player::IsPlayerLockingOn()
{
	return bLockOn;
}

//*****************************************************************************
//SetFallAttack関数
//プレイヤーは落ちているダメージを設定する
//引数：bool
//戻：void
//*****************************************************************************
void Player::SetFallAttack(bool fAtt)
{
	bFallAttack = fAtt;
}

//*****************************************************************************
//SetJumpAttack関数
//ジャンプの後の攻撃動作を設定する
//引数：bool
//戻：void
//*****************************************************************************
void Player::SetJumpAttack(bool fAtt)
{
	bJumpAttack = fAtt;
	if(fAtt)
		bJumpAttackLimit = fAtt;
}

//*****************************************************************************
//SetJumpAttackLimited関数
//ジャンプの後の攻撃動作の限界を設定する
//引数：bool
//戻：void
//*****************************************************************************
void Player::SetJumpAttackLimited(bool setBool)
{
	bJumpAttackLimit = setBool;
}

//*****************************************************************************
//GetJumpAttackLimited関数
//ジャンプの後の攻撃動作の限界を戻す
//引数：void
//戻：bool
//*****************************************************************************
bool Player::GetJumpAttackLimited()
{
	return bJumpAttackLimit;
}

//*****************************************************************************
//SetExternalMovement関数
//他のオブジェクトからプレイヤーの動作を設定する
//引数：int, int
//戻：void
//*****************************************************************************
void Player::SetExternalMovement(int NumOfFrames, int nDistance)
{
	if (nExternalMoveFrames > 0)
		return;
	nExternalMoveFrames = NumOfFrames;
	nExternalMoveDistance = nDistance;
}

//*****************************************************************************
//SetTeleportingDamageAfterJumping関数
//プレイヤーのテレポートを設定して、ダメージをもらう管理
//引数：int
//戻：void
//*****************************************************************************
void Player::SetTeleportingDamageAfterJumping(int Damage)
{
	nCurrentState = PLAYER_TELEPORTING;
	xm2NewTeleportPos = xm2LastPosBeforeJump;
	bStateAfterTeleportIsDamage = true;
	Size.x = POLYGON_SIZE_X;
	Size.y = POLYGON_SIZE_Y;
	fTeleportAcceleration = 0;
	nInvinsibilityFramesAfterDamage = 0;
	fTeleportFrameCounter = 0;
	nHP -= Damage;
	if (pGame->GetCurrentLevel() == TUTORIAL_ROOM && nHP == 0)
		nHP = 1;
	RankedEvent* p_reActive = GetCurrentActiveEvent();
	if (p_reActive)
		p_reActive->AddDamage(Damage);
}

//*****************************************************************************
//SetPlayerTeleporting関数
//プレイヤーのテレポートを設定する
//引数：XMFLOAT2
//戻：void
//*****************************************************************************
void Player::SetPlayerTeleporting(XMFLOAT2 newPosition)
{
	nCurrentState = PLAYER_TELEPORTING;
	xm2NewTeleportPos = newPosition;
	bStateAfterTeleportIsDamage = false;
	Size.x = POLYGON_SIZE_X;
	Size.y = POLYGON_SIZE_Y;
	fTeleportAcceleration = 0;
	fTeleportFrameCounter = 0;
	PlaySound(SOUND_LABEL_SE_TELEPORTIN);
}

//*****************************************************************************
//CancelExternalMovement関数
//他のオブジェクトの設定された動作をキャンセルする
//引数：void
//戻：void
//*****************************************************************************
void Player::CancelExternalMovement()
{
	nExternalMoveFrames = 0;
}

//*****************************************************************************
//RecoverHP関数
//HPが増える関数
//引数：int
//戻：void
//*****************************************************************************
void Player::RecoverHP(int hpToRecover)
{
	nHP += hpToRecover;
	if (nHP > nMaxHP)
		nHP = nMaxHP;
}

//*****************************************************************************
//SetGravityCancelFrames関数
//重力をキャンセルフレーム
//引数：int
//戻：void
//*****************************************************************************
void Player::SetGravityCancelFrames(int newFrames)
{
	nGravityCancelFrames = newFrames;
}

//*****************************************************************************
//GetHP関数
//現在のHPを戻す
//引数：void
//戻：int
//*****************************************************************************
int Player::GetHP()
{
	return nHP;
}

//*****************************************************************************
//GetHP関数
//マックスのHPを戻す
//引数：void
//戻：int
//*****************************************************************************
int Player::GetMaxHP()
{
	return nMaxHP;
}

//*****************************************************************************
//CheckCollisionWithWall関数
//壁と当たり判定を確認する
//引数：HitBox
//戻：bool
//*****************************************************************************
bool Player::CheckCollisionWithWall(HitBox HB_HitBoxToCheck)
{
	
	if (!Walls) {
		Walls = pGame->GetWalls();
	}
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

//*****************************************************************************
//CheckCollisionWithEnemy関数
//敵と当たり判定を確認する
//引数：int
//戻：Enemy*
//*****************************************************************************
Enemy * Player::CheckCollisionWithEnemy(int HitBoxToCheck)
{
	if (Enemies == nullptr) {
		Enemies = pGame->GetEnemies();
		return nullptr;
	}
	enemy_node* pEnemyNode = Enemies->HeadNode;
	while (pEnemyNode != nullptr)
	{
		if (pEnemyNode->Object != nullptr) {
			if (enCatchedEnemy == pEnemyNode->Object)
				break;
			if (pEnemyNode->Object->GetState() == BOUNCING_STATE || !(pEnemyNode->Object->IsEnemyInUse()))
			{
				pEnemyNode = pEnemyNode->next;
				continue;
			}
			if (HitBoxToCheck == HB_ENEMY_DETECTION_ZONE)
			{
				if (IsInCollision(pEnemyNode->Object->GetHitbox(), GetPlayerHitbox(HitBoxToCheck)))
					return pEnemyNode->Object;
			}
			else {
				if (IsInCollision(pEnemyNode->Object->GetRightSideHitbox(), GetPlayerHitbox(HitBoxToCheck)))
				{
					if (HitBoxToCheck == HB_FEET)
					{
						if (!GetInput(MOVE_RIGHT) && !GetInput(MOVE_LEFT) && !CheckCollisionWithWall(GetPlayerHitbox(HB_RIGHT_LIMIT)))
							Position.x += 10;
						if (GetInput(MOVE_RIGHT) && !CheckCollisionWithWall(GetPlayerHitbox(HB_RIGHT_LIMIT)))
							Position.x += 10;
						if (GetInput(MOVE_LEFT) && !CheckCollisionWithWall(GetPlayerHitbox(HB_LEFT_LIMIT)))
							Position.x -= 10;

					}
					return pEnemyNode->Object;
				}
				if (IsInCollision(pEnemyNode->Object->GetLeftSideHitbox(), GetPlayerHitbox(HitBoxToCheck)))
				{
					if (HitBoxToCheck == HB_FEET)
					{
						if (!GetInput(MOVE_RIGHT) && !GetInput(MOVE_LEFT) && !CheckCollisionWithWall(GetPlayerHitbox(HB_LEFT_LIMIT)))
							Position.x -= 10;
						if (GetInput(MOVE_RIGHT) && !CheckCollisionWithWall(GetPlayerHitbox(HB_RIGHT_LIMIT)))
							Position.x += 10;
						if (GetInput(MOVE_LEFT) && !CheckCollisionWithWall(GetPlayerHitbox(HB_LEFT_LIMIT)))
							Position.x -= 10;
					}
					return pEnemyNode->Object;
				}
			}
		}
		pEnemyNode = pEnemyNode->next;
	}
	return nullptr;
}

//*****************************************************************************
//GetHalo関数
//ハローのポインターを戻す
//引数：void
//戻：GameObject*
//*****************************************************************************
GameObject * Player::GetHalo()
{
	return p_Halo;
}

//*****************************************************************************
//GetFloor関数
//床のポインターを戻す
//引数：void
//戻：GameObject*
//*****************************************************************************
GameObject * Player::GetFloor()
{
	return goCurrentFloor;
}

//*****************************************************************************
//GetDebugAim関数
//デバッグアイムのポインターを戻す
//引数：void
//戻：GameObject*
//*****************************************************************************
GameObject * Player::GetDebugAim()
{
	return p_DebugAim;
}

//*****************************************************************************
//SetCatchedEnemy関数
//取った敵のポインターを設定する
//引数：void
//戻：void
//*****************************************************************************
void Player::SetCatchedEnemy(Enemy * pEnemyCatched)
{
	enCatchedEnemy = pEnemyCatched;
}

//*****************************************************************************
//GetCatchedEnemy関数
//取った敵のポインターを戻す
//引数：void
//戻：Enemy*
//*****************************************************************************
Enemy * Player::GetCatchedEnemy()
{
	return enCatchedEnemy;
}

//*****************************************************************************
//ActivateAuraEffect関数
//オーラエフェクトを使う
//引数：void
//戻：void
//*****************************************************************************
void Player::ActivateAuraEffect()
{
	p_goAuraEffect->SetSpriteSize(1, 1);
	p_goAuraEffect->SetUse(true);
}

//*****************************************************************************
//GetCurrentWeapon関数
//現在の武器を戻す
//引数：void
//戻：Weapon*
//*****************************************************************************
Weapon * Player::GetCurrentWeapon()
{
	return weapons[nCurrentWeapon];
}

//*****************************************************************************
//SetPlayerTranslation関数
//プレイヤーを動かせる
//引数：float, float
//戻：void
//*****************************************************************************
void Player::SetPlayerTranslation(float x, float y)
{
	Position.x += x;
	Position.y += y;
	if (p_Halo)
	{
		XMFLOAT3 haloPos = p_Halo->GetLocalPosition();
		haloPos.x += x;
		haloPos.y += y;
		p_Halo->SetLocalPosition(haloPos.x, haloPos.y);
	}
}

//*****************************************************************************
//SetDrawWeapon関数
//武器をレンダリングすることを設定する
//引数：bool
//戻：void
//*****************************************************************************
void Player::SetDrawWeapon(bool draweapon)
{
	bDrawWeapon = draweapon;
}

//*****************************************************************************
//ResetPlayerEnemyList関数
//敵のリストをリセットする
//引数：void
//戻：void
//*****************************************************************************
void Player::ResetPlayerEnemyList()
{
	Enemies = nullptr;
}

//*****************************************************************************
//InvinsibilityFramesOn関数
//ダメージがもらえないフレームを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Player::InvinsibilityFramesOn()
{
	return nInvinsibilityFramesAfterDamage>0;
}

//*****************************************************************************
//IsDebugAimOn関数
//デバッグアイム/レベルエディターを使ってることを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Player::IsDebugAimOn()
{
	if(p_DebugAim)
		return p_DebugAim->IsInUse();
	return false;
}

//*****************************************************************************
//GetLastPosBeforeJump関数
//ジャンプの前の経典を戻す
//引数：void
//戻：XMFLOAT2
//*****************************************************************************
XMFLOAT2 Player::GetLastPosBeforeJump()
{
	return xm2LastPosBeforeJump;
}

//*****************************************************************************
//ResetPlayerLists関数
//プレイヤーのリセットリスト
//引数：void
//戻：void
//*****************************************************************************
void Player::ResetPlayerLists()
{
	Floors = nullptr;
	Walls = nullptr;
	Enemies = nullptr;
	enCatchedEnemy = nullptr;
	goCurrentFloor = nullptr;
	bIsOnTheFloor = false;
}

//*****************************************************************************
//SetPlayerUVnum関数
//プレイヤーのUVを設定する
//引数：int
//戻：void
//*****************************************************************************
void Player::SetPlayerUVnum(int newUV)
{
	nCurrentUV = newUV;
}

//*****************************************************************************
//SetPlayerPointer関数
//グローバルのプレイヤーポインターを設定する
//引数：Player*
//戻：void
//*****************************************************************************
void SetPlayerPointer(Player * pPlayer)
{
	thisPlayer = pPlayer;
}

//*****************************************************************************
//GetPlayerPointer関数
//グローバルのプレイヤーポインターを戻す
//引数：void
//戻：Player*
//*****************************************************************************
Player * GetPlayerPointer()
{
	return thisPlayer;
}

//*****************************************************************************
//GetWeapons関数
//武器のポインターを戻す
//引数：Player*
//戻：void
//*****************************************************************************
Weapon ** GetWeapons()
{
	return weapons;
}

//*****************************************************************************
//PausePlayerUpdate関数
//プレイヤーをポースする
//引数：bool
//戻：void
//*****************************************************************************
void PausePlayerUpdate(bool bPause)
{
	bPausePlayerUpdate = bPause;
}

//*****************************************************************************
//IsPlayerPaused関数
//プレイヤーはポースされている状態を確認する
//引数：void
//戻：bool
//*****************************************************************************
bool IsPlayerPaused()
{
	return bPausePlayerUpdate;
}
