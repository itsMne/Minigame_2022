//*****************************************************************************
// Weapon.h
// Author : Mane
//*****************************************************************************
#ifndef	WEAPON_H
#define WEAPON_H
#include "Sprite.h"
#include "UniversalVariables.h"
#include "GameObject.h"
#include "Bullet.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_BULLETS 10
#define MAX_CATCHBULLETS 1

//*****************************************************************************
// エナム定義
//*****************************************************************************
enum WEAPON_TYPE 
{
	WEAPON_GUITAR=0,
	WEAPON_HANDS,
	WEAPON_MAX
};

enum HANDS_STATE {
	HAND_PUNCH_A_STATE = 0,
	HAND_PUNCH_B_STATE,
	HAND_DOWNSTOGROUND_STATE,
	HAND_DUAL_A_STATE,
	HAND_DUAL_B_STATE,
	HAND_GROUND_A_STATE,
	HAND_SPIN_STATE,
	HAND_DOWN_IDLE_STATE,
	HAND_IDLE_STATE,
	HAND_UPPERCUT,
	DOWN_DASH_ATTACK,
	HAND_STOMP,
	HAND_STRONGPUNCH_A,
	HAND_STRONGPUNCH_B,
	HAND_SENDDOWN,
	HAND_STRONGPUNCH_A_ALT,
	HAND_UPPERCUT_ALT,
	HAND_AIR_DASH,
	MAX_HAND_STATES
};

enum GUITAR_STATES
{
	GUITAR_FRONTAL = 0,
	GUITAR_DIAGONAL_UP,
	GUITAR_DIAGONAL_DOWN,
	GUITAR_UP,
	GUITAR_DOWN,
	MAX_GUITAR_STATES
};

//*****************************************************************************
// クラス
//*****************************************************************************
class Weapon :
	public Sprite
{
private:
	int		nType;
	int		nState;
	int		nDirection;
	float	nFrame_X;
	float	nFrame_Y;
	bool	bShowWeapon;
	bool	bIsAttacking;
	bool	bEndAttack;
	int		nAnimationTimer;
	int		nIdleState;
	int		nDashTimer;
	float	fDashAttackSpeed;
	int		nCurrentAttackPlaying;
	bool	bDontShowWeaponDuringAttack;
	int		uvMirrorDirection;
	int		nBulletCoolDown;
	int		nCoolDownDashAttack;
	bool	bAirDashExecutedOnce;
	Sprite  *spPlayer;
	tUV		uv;
	Bullet* pBullets[MAX_BULLETS];
	Bullet* pCatchBullets[MAX_CATCHBULLETS];
public:
	Weapon();
	~Weapon();
	void Init(int Type);
	void Update();
	void UpdateGuitarControl();
	void HandsUpdateControl();
	void Draw();
	void ControlDirection();
	void ShowWeapon(bool showweapon);
	void ExecuteAttack(char szInputs[256]);
	void ExecuteAttack(char szInputs[256], int RecursionNum, bool bVerifyLastChars);
	void ResetToIdle();
	void SetPlayerPointer(Sprite *sp_Player);
	void SetState(int NewState);
	int  GetCurrentAttackPlaying();
	bool IsWeaponShowing();
	void SetDashTime(int newTime);
	bool IsWeaponAttacking();
	void ResetWeaponState();
	void ResetAirDash();
	void SetCoolDownDashFrames(int newDashFrames);
	Sprite* GetOwner();
	Bullet** GetBullets();
	Bullet** GetCatchBullets();
	void End();
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
Bullet** GetBullets();
Bullet** GetCatchBullets();

#endif