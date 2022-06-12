//*****************************************************************************
// EventHandler.h
// Author : Mane
//*****************************************************************************
#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "Sprite.h"
#include "Enemy.h"

//*****************************************************************************
// エナム定義
//*****************************************************************************
enum SWITCHES_ID
{
	SWITCH_L1_No1=0,
	SWITCH_L1_No2,
	SWITCH_L1_No3,
	SWITCH_L1_No4,
	SWITCH_L1_No5,
	SWITCH_L1_No6,
	SWITCH_L1_No7,
	SWITCH_L1_No8,
	SWITCH_L1_No9,
	SWITCH_L1_No10,
	SWITCH_L1_No11,
	SWITCH_L1_No12,
	SWITCH_L1_No13,
	SWITCH_L1_No14,
	SWITCH_TUTORIAL_No1,
	SWITCH_TUTORIAL_No2,
	SWITCH_TUTORIAL_No3,
	SWITCH_TUTORIAL_No4,
	SWITCH_TUTORIAL_No5,
	SWITCH_MAX
};

enum STAR_ID
{
	STAR_L1_No1 = 0,
	STAR_MAX
};

enum FLOORS_ID
{
	FLOOR_LV1_EXPANDABLE1=0,
	FLOOR_LV1_EXPANDABLE2,
	FLOOR_LV1_EXPANDABLE3,
	FLOOR_LV1_EXPANDABLE4,
	FLOOR_LV1_EXPANDABLE5,
	FLOOR_LV1_EXPANDABLE6,
	FLOOR_LV1_EXPANDABLE7,
	FLOOR_LV1_EXPANDABLE8,
	FLOOR_LV1_REGULAR_A,
	FLOOR_LV1_REDUCEABLE_FLOOR_1,
	FLOOR_LV1_REGULAR_B,
	FLOOR_LV1_REGULAR_C,
	BOUNCING_TUTORIAL_FIRSTFLOOR,
	BOUNCING_TUTORIAL_SECONDFLOOR,
	BOUNCING_TUTORIAL_THIRDFLOOR,
	SWITCH_TUTORIAL_EXPANDABLE1,
	SWITCH_TUTORIAL_EXPANDABLE2,
	SWITCH_TUTORIAL_EXPANDABLE3,
	SWITCH_TUTORIAL_EXPANDABLE4,
	SWITCH_TUTORIAL_EXPANDABLE5,
	FLOOR_ID_MAX
};
enum ENEMY_ID
{
	ENEMY_TUTORIAL1=0,
	ENEMY_TUTORIAL2_DOUBLEJUMPEN1,
	ENEMY_TUTORIAL2_DOUBLEJUMPEN2,
	ENEMY_TUTORIAL3_EVENT1,
	ENEMY_TUTORIAL3_EVENT2,
	ENEMY_TUTORIAL3_EVENT3,
	ENEMY_LV1_01,
	ENEMY_LV1_02,
	ENEMY_LV1_03,
	ENEMY_LV1_04,
	ENEMY_LV1_05,
	ENEMY_LV1_06,
	ENEMY_LV1_07,
	ENEMY_LV1_08,
	ENEMY_LV1_09,
	ENEMY_LV1_10,
	ENEMY_LV1_11,
	ENEMY_LV1_12,
	ENEMY_LV1_13,
	ENEMY_LV1_14,

	ENEMY_LV1_15,
	ENEMY_LV1_16,
	ENEMY_LV1_17,
	ENEMY_LV1_18,
	ENEMY_LV1_19,
	ENEMY_LV1_20,
	ENEMY_LV1_21,
	ENEMY_LV1_22,
	ENEMY_LV1_23,

	ENEMY_LV1_24,
	ENEMY_LV1_25,
	ENEMY_LV1_26,
	ENEMY_LV1_27,
	ENEMY_LV1_28,

	ENEMY_LV1_29,
	ENEMY_LV1_30,
	ENEMY_LV1_31,
	ENEMY_ID_MAX
};

enum TELEPORTERS_ID
{
	TELEPORTER_TUTORIAL_01 = 0,
	TUTORIAL_ENDPOINT,
	TELEPORTER_LVL1_01,
	TELEPORTER_LVL1_02,
	TELEPORTER_LVL1_03,
	MAX_TELEPORTERS_ID
};
enum WALLS_ID
{
	WALL_1_UPRIGHT = 0,
	WALL_1_DOWNRIGHT,
	WALL_1_UPLEFT,
	WALL_1_DOWNLEFT,
	WALL_1_FALLING1,
	WALL_1_FALLING2,
	MAX_WALLS_ID
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
namespace EventHandler
{
	void Init();
	void Update();
	void TutorialEventsUpdate();
	void Level1_2RoomEventsUpdate();
	void Level1_1RoomEventsUpdate();
	void ControlEnemyRespawn(Enemy * EnemyToRespawn, float fPosX, float fPosY);
	void KillEveryoneEventControlRevealsTeleporter(int nEvent, int nTeleporter, int nFirstEnemy, int nEndEnemy, int nRankedEvent, int nFightEvent);
	void KillEveryoneEventControlRevealsSwitch(int nEvent, int nLevelObject, int nFirstEnemy, int nEndEnemy, int nRankedEvent, int nFightEvent);
	bool CheckIfEventEnemiesAreDead(int nStartEnemy, int nEndEnemy);
	void Draw();
	void End();

};
void SearchForLevelObjects();
Enemy* GetEventEnemy(int nEn);
//*****************************************************************************
// クラス
//*****************************************************************************
class RankedEvent
{
private:
	HitBox	hbStartEvent;
	HitBox	hbEndEvent;
	bool	bEventIsActive;
	bool	bEventIsOver;
	int		nEventType;//TEMP
	//行事の結果に関する変数
	float	fMinTime;
	float	fMinComboScore;
	int		nMinDamageReceived;
	char	cTimeRank;
	char	cComboRank;
	char	cDamageRank;
	char	cOverralRank;
	//アップデートで管理の変数
	float	fCurrentTime;
	int		nCurrentComboScore;
	int		nDamageDone;
	int		nLevel;
	int		nSubLevel;
public:
	RankedEvent();
	~RankedEvent();
	void InitEvent(HitBox StartEvent, HitBox EndEvent, float MinTime, float MinCombo, float MinDamage, int Level, int SubLevel);
	void Update();
	void Draw();
	void End();
	void SetEventActive();
	void SetEventOver();
	char CalculateTimeRank();
	char CalculateComboRank();
	char CalculateDamageRank();
	char CalculateOverralRank(char TimeRank, char ComboRank, char DamageRank);
	char GetComboRank();
	char GetDamageRank();
	char GetTimeRank();
	char GetOverralRank();
	void AddComboScore(int nComboToAdd);
	void AddDamage(int nDamage);
	HitBox GetStartHitbox();
	HitBox GetEndHitbox();
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
RankedEvent* GetCurrentActiveEvent();
#endif // !EVENT_HANDLER_H
