//*****************************************************************************
// Enemy.h
// Author : Mane
//*****************************************************************************
#ifndef	ENEMY_H
#define ENEMY_H
#include "Sprite.h"
#include "UniversalVariables.h"
#include "GameObject.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_MINIAURA_EFFECTS 7 //7
#define BOUNCING_STATE -1
#define CATCHED_STATE -2
#define BUNBUN_SIZE 600
#define MAX_BUN_BUN_HP	380
//*****************************************************************************
// �G�i����`
//*****************************************************************************
enum EnemyType
{
	BUNBUN_ENEMY=0,
	MAX_ENEMY_TYPE
};

//*****************************************************************************
// �\����
//*****************************************************************************
struct SA_CooldownTimer
{
	float current = 0;
	float speed = 0;
	float max = 0;
};

//*****************************************************************************
// �N���X
//*****************************************************************************
class Enemy :
	public Sprite
{
private: 
	int nEnemyType;
	int nFrame_X;
	int nFrame_Y;
	int nEnemyState;
	tUV uv;
	int mirrorUv;
	int nAnimeTimer;
	int nDirection;
	int nAnimeMirror;
	int nCounter;
	int nRoomID;
	int nIdleState;
	int nEnemyID;
	float fSizeChangeAcceleration;
	bool bSizeSet;
	bool bIsInUse;
	bool bEnemyDyingSoundEffect;
	//�����蔻��
	int nHitAttackState;
	int nAttackPlayerWeapon;
	bool bHitCollision;
	bool bBulletCollision;
	bool bBulletCatchCollision;
	bool bNoScoreEnemy;
	int nRecoveringFramesFromAttack;
	int nFramesSinceAttack;
	float fAttackedAcceleration;
	float fyForce;
	HitBox FeetHitbox;
	HitBox RightSideHitbox;
	HitBox LeftSideHitbox;
	XMFLOAT2 x2MaxSize;
	XMFLOAT2 x2InitialPosition;
	GameObject* p_goCurrentFloor;
	int nLockedAnimationFrames;
	GameObject* p_goMiniAuraEffect[MAX_MINIAURA_EFFECTS];
	//�X�[�p�[�A�[�}�[�Ɋւ���ϐ�
	GameObject* p_goAuraEffect;
	GameObject* p_ShieldEffect;
	float fMaxSuperArmor;
	float fSuperArmor;
	bool bSuperArmorBroken;
	XMFLOAT2 xm2ImpulseDirection;
	int nBounces;
	bool bBlockGravity;
	SA_CooldownTimer SA_RecoveryTimer;
	bool bCatchedPositionWithX;
	bool bCatchedPositionWithY;
	//�L���b�`�̕ϐ�
	float fCatchedAcceleration;
	//AI�̕ϐ�
	bool bDetectedByPlayer;
	bool bCanFallFromFloor;
	float fEnemySpeed;
	int nWaitingFrames;
	int nHP;
	int nMaxHP;
	int nFramesForPlayerDetectionVerification;
	HitBox AttackHitbox;
	Sprite* AttackPolygonHitbox;
	//�ꏊ�̊Ǘ�
	GameObject* pWallLimitLeft;
	GameObject* pWallLimitRight;
	GameObject* pFloorLimit;
	//�G�̏Љ�S�Ɋւ���ϐ�
	GameObject* pIntroductionLogo;
	bool bIsIntroductionEnemy;
	
public:
	Enemy();
	~Enemy();
	void		Init(int nEnemyType);
	void		Update();
	void		ActivateMiniAuraEffect();
	void		CatchedStateControl();
	void		SuperArmorRecoveryControl();
	void		GravityControl();
	void		UpdateBunBun();
	void		BunBunAIControl();
	void		SetNoScoreEnemy(bool bNoScore);
	void		ControlAICollisionWithWall(bool bChangeDirection);
	bool		CheckCollisionWithSpikes();
	bool		IsNoScoreEnemy();
	void		AttackCollisionControl();
	void		ActivateAuraEffect(bool bNormalAura);
	void		RecoveryFramesControl();
	void		SetDamage(int damage);
	void		End();
	bool		CheckBounceWithFloor();
	void		SetRoom(int newID_Room);
	HitBox		GetFeetHitbox();
	HitBox		GetRightSideHitbox();
	HitBox		GetLeftSideHitbox();
	int			GetState();
	GameObject* GetCurrentFloor();
	GameObject* GetIntroLogoGameObject();
	void		SetBouncingState(XMFLOAT2 ImpulseDirection, int nBounces);
	void		SetEnemyAsCatched();
	void		SetCanFallFromFloor(bool bCanfall);
	void		SetEnemyID(int newID);
	void		Draw();
	bool		IsEnemyDead();
	bool		IsEnemyInUse();
	bool		IsSuperArmorBroken();
	void		SetUse(bool bUse);
	int			GetEnemyID();
	int			GetEnemyHP();
	void		SetWallLimits(GameObject* pWallR, GameObject* pWallL);
	void		SetFloorLimit(GameObject* FloorLimit);
	void		SetAsIntroductionEnemy();
	bool		CheckCollisionWithWall(HitBox HB_HitBoxToCheck);
	void		SetInitialPosition(XMFLOAT2 InitPos);
	int			GetEnemyType();
	XMFLOAT2	GetInitialPos();
};

//*****************************************************************************
// �N���X-���X�g
//*****************************************************************************
struct enemy_node {//���X�g�̊Ǘ��ׂ̈ɁA���̍\���̂��v��
	Enemy* Object;
	enemy_node *next;
};
struct EnemyContainer {//���X�g�̊Ǘ��ׂ̈ɁA���̍\���̂��v��
	int nEnemyType; 
	XMFLOAT2 x2Position; 
	int nEnemy_ID;
};

class Enemy_List
{
private:

public:
	enemy_node * HeadNode;
	Enemy_List();
	~Enemy_List();
	void AddEnemy(Enemy newObject);
	Enemy* AddEnemy(int nEnemyType, XMFLOAT2 x2Position);
	Enemy* AddEnemy(int nEnemyType, XMFLOAT2 x2Position, int nEnemy_ID);
	Enemy* GetEnemy(int nIdToGet);
	void Save(const char* szFilename);
	void Load(const char* szFilename);
	void DeleteLastPosObject();
	void Update();
	void Draw();
	void DeleteObject(Enemy*);
	void ResetList();
	void End();
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void DynamicZoomControl();
void PauseEnemyUpdate(bool bPause);
int GetEnemyCountInMemory();

#endif