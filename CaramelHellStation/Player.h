//*****************************************************************************
// Player.h
// Author : Mane
//*****************************************************************************
#ifndef	PLAYER_H
#define	PLAYER_H
#include "Sprite.h"
#include "SpotLight.h"
#include "GameObject.h"
#include "GameObject_List.h"
#include "UniversalVariables.h"
#include "Weapon.h"
#include "Floor.h"
#include "Enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PLAYER_INPUT 8
#define MAX_FLOWERS 16

//*****************************************************************************
// エナム定義
//*****************************************************************************
enum PlayerStates
{
	PLAYER_IDLE_STATE=0,
	PLAYER_MOVING_STATE,
	PLAYER_DOWN_STATE,
	PLAYER_ATTACKING_STATE,
	PLAYER_SHOOTING_STATE,
	PLAYER_TAUNTING_STATE,
	PLAYER_ATTACKED_STATE,
	PLAYER_TELEPORTING,
	MAX_PLAYER_STATES
};

enum PlayerUVs
{
	PLAYER_IDLE_UV = 0,
	PLAYER_MOVING_UV,
	PLAYER_LOCKON_UV,
	PLAYER_DOWN_UV,
	PLAYER_FALLING_UV,
	PLAYER_JUMPING_UV,
	PLAYER_ATTACK_A_UV,
	PLAYER_ATTACK_B_UV,
	PLAYER_ATTACK_STRONG_UV,
	PLAYER_DASH_UV,
	PLAYER_DASHGUITAR_UV,
	PLAYER_ATTACKED_UV,
	PLAYER_TAUNT_UV,
	PLAYER_AIR_TAUNT_UV,
	MAX_PLAYER_UV
};

enum HitBoxes
{
	HB_BODY=0,
	HB_FEET,
	HB_HEAD,
	HB_RIGHT_LIMIT,
	HB_LEFT_LIMIT,
	HB_ENEMY_DETECTION_ZONE,
	HB_MAX
};

//*****************************************************************************
// クラス
//*****************************************************************************
class Player :
	public Sprite
{
private:
	int			nHP, nMaxHP;
	int			nFrame_X;
	int			nFrame_Y;
	tUV			uv;
	int			nCurrentState;
	int			uvAnim;
	int			nDirection;
	int			nCurrentUV;
	int			nGravityCancelFrames;
	GameObject	go_Flowers[MAX_FLOWERS];
	int			nFlowerSpawnTimer;
	GameObject* p_Halo;
	bool		bLockOn;
	int			nCurrentWeapon;
	HitBox		hitBoxes[HB_MAX];
	Go_List*	Floors;
	Go_List*	Walls;
	Enemy_List* Enemies;
	Enemy*		enCatchedEnemy;
	Floor*		goCurrentFloor;
	bool		bIsOnTheFloor;
	float		f_yForce;
	int			nAirResist;
	char		szInputs[MAX_PLAYER_INPUT+1];
	int			nResetInputTimer;
	bool		bAddedEmptySpace;
	bool		bLockInputs;
	bool		bFallAttack;
	bool		bJumpAttack;
	bool		bJumpAttackLimit;
	bool		bDrawWeapon;
	int			nExternalMoveDistance;
	int			nExternalMoveFrames;
	//ダメージをもらうことに関する変数
	int			nAttackedStateTimer;
	int			nColorSwapFrame;
	int			nInvinsibilityFramesAfterDamage;
	XMFLOAT2	xm2LastPosBeforeJump;
	XMFLOAT2	xm2NewTeleportPos;
	float		fTeleportAcceleration;
	bool		bStateAfterTeleportIsDamage;
	bool		bTeleportingToPos;
	int			nInvinsibilityFramesAfterJumping;
	GameObject* p_goAuraEffect;
	//デバッグの変数
	GameObject* p_DebugAim;
	//スポットライト
	SpotLight* p_Spotlight;
	//TAUNT
	bool		bCanAirTaunt;
	int			nOffsetInputFrames;
	int			nLockInputWhileTauntCounter;
	int			nUVGroundTauntChange;
	int			nTauntFrames;
public:
	Player();
	~Player();
	void		Init();
	void		Update();
	void		InvinsibilityFramesControl();
	void		SetLastPosBeforeJump();
	void		AuraControl();
	void		TeleportControl();
	void		DeadControl();
	void		MoveStateControl(float *fLockOnSpeed);
	void		FlowerSpawnControl();
	bool		AttackPlayer(int nDamage);
	bool		AttackPlayer(int nDamage, bool IsAfterTeleport);
	void		GeneralInputsControl();
	void		ExternalMoveControl();
	void		ShootingControl(float &fLockOnSpeed);
	void		End();
	void		ResetInputs();
	void		AddInput(char A);
	void		WeaponsControl();
	void		HaloControl();
	void		Jump(float jumpForce);
	void		GravityControl();
	void		ControlUVAnimation();
	void		ControlDirection();
	void		Draw();
	void		SetHalo(GameObject* goHalo);
	void		SetPlayerState(int newState);
	void		SetPlayerUVnum(int newUV);
	int			GetDirection();
	int			GetPlayerState();
	float		GetYForce();
	void		SetYForce(float newYForce);
	HitBox		GetPlayerHitbox(int hitBoxToget);
	void		SetLockInputs(bool inputsLock);
	bool		IsPlayerInputForward(bool isTrigger);
	bool		IsPlayerInputBackward(bool isTrigger);
	bool		IsPlayerOnTheGround();
	bool		IsPlayerDead();
	bool		IsPlayerLockingOn();
	void		SetFallAttack(bool fAtt);
	void		SetJumpAttack(bool fAtt);
	void		SetJumpAttackLimited(bool setBool);
	bool		GetJumpAttackLimited();
	void		SetExternalMovement(int NumOfFrames, int nDistance);
	void		SetTeleportingDamageAfterJumping(int Damage);
	void		SetPlayerTeleporting(XMFLOAT2 newPosition);
	void		CancelExternalMovement();
	void		RecoverHP(int hpToRecover);
	void		SetGravityCancelFrames(int newFrames);
	int			GetHP();
	int			GetMaxHP();
	bool		CheckCollisionWithWall(HitBox HB_HitBoxToCheck);
	Enemy*		CheckCollisionWithEnemy(int HitBoxToCheck);
	GameObject* GetHalo();
	GameObject* GetFloor();
	GameObject* GetDebugAim();
	void		SetCatchedEnemy(Enemy* pEnemyCatched);
	Enemy*		GetCatchedEnemy();
	void		ActivateAuraEffect();
	Weapon*		GetCurrentWeapon();
	void		SetPlayerTranslation(float x, float y);
	void		SetDrawWeapon(bool draweapon);
	void		ResetPlayerEnemyList();
	bool		InvinsibilityFramesOn();
	bool		IsDebugAimOn();
	XMFLOAT2	GetLastPosBeforeJump();
	void		ResetPlayerLists();
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void	 SetPlayerPointer(Player* pPlayer);
Player*  GetPlayerPointer();
Weapon** GetWeapons();
void	 PausePlayerUpdate(bool bPause);
bool	 IsPlayerPaused();

#endif