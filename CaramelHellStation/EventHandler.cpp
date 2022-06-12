//*****************************************************************************
// EventHandler.cpp
// ステージのイベント管理
// Author : Mane
//*****************************************************************************
#include "EventHandler.h"
#include "UniversalVariables.h"
#include "SceneInGame.h"
#include "Camera.h"
#include "GameObject.h"
#include "Player.h"
#include "GameInputManager.h"
#include "sound.h"
#include "UIManager.h"
#include "Floor.h"
#include <stdio.h>

//*****************************************************************************
// エナム定義
//*****************************************************************************
enum EVENT_NUMBERS
{
	EVENT_LVL01_STAREVENT = 0,
	EVENT_LVL01_EXPANDABLEFLOOREVENT,
	EVENT_LVL01_EXPANDABLEFLOOREVENT2,
	EVENT_LVL01_EXPANDABLEFLOOREVENT3,
	EVENT_LVL01_EXPANDABLEFLOOREVENT4,
	EVENT_LVL01_EXPANDABLEFLOOREVENT5,
	EVENT_LVL01_EXPANDABLEFLOOREVENT6,
	EVENT_LVL01_EXPANDABLEFLOOREVENT7,
	EVENT_LVL01_EXPANDABLEFLOOREVENT8,
	EVENT_LVL01_KILLALLENEMIES,
	EVENT_LVL01_KILLALLENEMIES2,
	EVENT_LVL01_KILLALLENEMIES3,
	EVENT_LVL01_KILLALLENEMIES4,
	EVENT_LVL01_KILLALLENEMIES5,
	EVENT_LVL01_FOCUSCAMERAAFTERKILL_01,
	EVENT_LVL01_RANKEDEVENT_FOCUSCAMERAAFTERKILL_02,
	EVENT_LVL01_RANKEDEVENT_FOCUSCAMERAAFTERKILL_03,
	EVENT_LVL01_RANKEDEVENT_FOCUSCAMERAAFTERKILL_04,
	EVENT_LVL01_RANKEDEVENT_FOCUSCAMERAAFTERKILL_05,
	EVENT_LVL01_FLOORMOVEABLE01,
	EVENT_LVL01_FLOORMOVEABLE02,
	EVENT_LVL01_SUBLEVELCHANGE,
	EVENT_LVL01_SCALEOUT,
	EVENT_TUTORIAL_FIGHT1,
	EVENT_TUTORIAL_FIGHT1_CAMERAFOCUSAFTERKILL,
	EVENT_TUTORIAL_EXPANDABLEFLOOR1,
	EVENT_TUTORIAL_EXPANDABLEFLOOR2,
	EVENT_TUTORIAL_EXPANDABLEFLOOR3,
	EVENT_TUTORIAL_EXPANDABLEFLOOR4,
	EVENT_TUTORIAL_EXPANDABLEFLOOR5,
	EVENT_TUTORIAL_FIGHT2,
	EVENT_TUTORIAL_FIGHT2_CAMERAFOCUSAFTERKILL,
	MAX_EVENT_NUMBERS
};
enum RANKED_EVENTS
{
	TUTORIAL_RANKED_EVENT=0,
	EVENT_LVL01_FIRSTEVENT,
	EVENT_LVL01_FIGHT_EVENT1,
	EVENT_LVL01_FIGHT_EVENT2,
	EVENT_LVL01_FIGHT_EVENT3,
	EVENT_LVL01_REACHTHEEND_EVENT,
	EVENT_LVL01_REACHTHEEND_EVENT2,
	MAX_RANKED_EVENTS
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
int			n_CurrentLevel;
GameObject* go_PunchableStars[STAR_MAX];
SwitchGO* go_Switches[SWITCH_MAX];
Floor* go_Floors[FLOOR_ID_MAX];
GameObject* go_Teleporters[MAX_TELEPORTERS_ID];
GameObject* go_Walls[MAX_WALLS_ID];
Enemy*		en_Enemies[ENEMY_ID_MAX];
float		fEventAcceleration[MAX_EVENT_NUMBERS];
int			nFrameCounter[MAX_EVENT_NUMBERS];
bool		bEventControls[MAX_EVENT_NUMBERS];
RankedEvent reRankedEvents[MAX_RANKED_EVENTS];
RankedEvent* p_reCurrentActiveEvent = nullptr;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void ExpandableFloorControl(Floor * FloorToExpandContract, SwitchGO * SwitchToRelyOn, int AccelerationNumberEvent, float MaxSizeFloor, float fContractSpeed);

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void EventHandler::Init()
{
	SceneInGame* pGame = GetCurrentGame();
	pGame = nullptr;
	p_reCurrentActiveEvent = nullptr;
	UIManager::SetOnAirMessageUse(false);
	n_CurrentLevel = 0;
	SearchForLevelObjects();
	for (int i = 0; i < MAX_EVENT_NUMBERS; i++)
	{
		bEventControls[i] = true;
		nFrameCounter[i] = 0;
		fEventAcceleration[i] = 0;
	}
#ifdef USE_CONSOLE
	printf("INITED EVENTS\n");
#endif // USE_CONSOLE
	reRankedEvents[EVENT_LVL01_FIRSTEVENT].InitEvent({ 810.000f,556.000f,106.000f,1400.000f },
		{ 5225.000f,1431.000f,106.000f,1400.000f }, 5000, 3560, 1, LEVEL1_ROOM, SUBLEVEL1_1);
	reRankedEvents[EVENT_LVL01_FIGHT_EVENT1].InitEvent({ 1284.000f,-1420.050f,3815.000f,125.000f },
		{ 0.0f,0.0f,0.0f,0.0f }, 8800, 7500, 0, LEVEL1_ROOM, SUBLEVEL1_1);
	reRankedEvents[TUTORIAL_RANKED_EVENT].InitEvent({ 10837.000f,9738.000f,3795.000f,600.000f }, { 0.0f,0.0f,0.0f,0.0f }, 9500, 5000, 2, TUTORIAL_ROOM, TUTORIAL_R);
	reRankedEvents[EVENT_LVL01_FIGHT_EVENT2].InitEvent({ 7263.515f,-1646.000f,1275.000f,615.000f }, { 0.0f,0.0f,0.0f,0.0f }, 20000, 6522, 1, LEVEL1_ROOM, SUBLEVEL1_1);
	reRankedEvents[EVENT_LVL01_REACHTHEEND_EVENT].InitEvent({ 14326.644f,7772.902f,188.000f,1875.000f }, { 25116.645f,4702.902f,2683.000f,593.000f }, 15000, 500, 0, LEVEL1_ROOM, SUBLEVEL1_1);
	reRankedEvents[EVENT_LVL01_FIGHT_EVENT3].InitEvent({ -4952.000f,6567.000f,1999.000f,600.000f }, { 0.0f,0.0f,0.0f,0.0f }, 15000, 8875, 1, LEVEL1_ROOM, SUBLEVEL1_2);
	reRankedEvents[EVENT_LVL01_REACHTHEEND_EVENT2].InitEvent({ 10800.074f,2066.357f,1170.000f,2880.000f }, { 16365.074f,1931.357f,345.000f,2880.000f }, 10000, 9500, 1, LEVEL1_ROOM, SUBLEVEL1_2);
}

void EventHandler::Update()
{
	SceneInGame* pGame = GetCurrentGame();
	pGame = GetCurrentGame();
	if (!pGame)
		return;
	n_CurrentLevel = pGame->GetCurrentLevel();
	int nSubLevel = pGame->GetCurrentSubLevel();
	switch (n_CurrentLevel)
	{
	case DEBUG_ROOM:
		break;
	case TUTORIAL_ROOM:
		TutorialEventsUpdate();
		break;
	case LEVEL1_ROOM:
		if(nSubLevel== SUBLEVEL1_1)
			Level1_1RoomEventsUpdate();
		if (nSubLevel == SUBLEVEL1_2 && go_Floors[FLOOR_LV1_REGULAR_B])
			Level1_2RoomEventsUpdate();
		break;
	default:
		break;
	}
	if (!p_reCurrentActiveEvent) {
		for (int i = 0; i < MAX_RANKED_EVENTS; i++)
			reRankedEvents[i].Update();
	}
	else 
	{
		p_reCurrentActiveEvent->Update();
	}
}

//*****************************************************************************
//TutorialEventsUpdate関数
//チュートリアルイベントを管理する
//引数：void
//戻：void
//*****************************************************************************
void EventHandler::TutorialEventsUpdate()
{
	Player* pPlayer = GetPlayerPointer();
	if (!pPlayer)
		return;
	KillEveryoneEventControlRevealsTeleporter(EVENT_TUTORIAL_FIGHT1, TELEPORTER_TUTORIAL_01, ENEMY_TUTORIAL1, ENEMY_TUTORIAL1, EVENT_TUTORIAL_FIGHT1_CAMERAFOCUSAFTERKILL, -1);
	if (en_Enemies[ENEMY_TUTORIAL2_DOUBLEJUMPEN1]->GetCurrentFloor() == go_Floors[BOUNCING_TUTORIAL_SECONDFLOOR])
	{
		if (pPlayer->GetFloor() == go_Floors[BOUNCING_TUTORIAL_FIRSTFLOOR])
			en_Enemies[ENEMY_TUTORIAL2_DOUBLEJUMPEN1]->SetWorldPosition( 9739.00f, 3602.40f );
		else if (pPlayer->GetFloor() == go_Floors[BOUNCING_TUTORIAL_SECONDFLOOR])
			en_Enemies[ENEMY_TUTORIAL2_DOUBLEJUMPEN1]->SetWallLimits(nullptr, nullptr);
	}
	if (pPlayer->GetFloor() == go_Floors[BOUNCING_TUTORIAL_FIRSTFLOOR])
	{
		XMFLOAT3 EnPos = en_Enemies[ENEMY_TUTORIAL2_DOUBLEJUMPEN1]->GetWorldPosition();
		while (EnPos.x > 9900.26f)
		{
			en_Enemies[ENEMY_TUTORIAL2_DOUBLEJUMPEN1]->SetWorldPosition(EnPos.x - 1, EnPos.y);
			EnPos = en_Enemies[ENEMY_TUTORIAL2_DOUBLEJUMPEN1]->GetWorldPosition();
		}
	}
	if (en_Enemies[ENEMY_TUTORIAL2_DOUBLEJUMPEN2]->GetCurrentFloor() == go_Floors[BOUNCING_TUTORIAL_THIRDFLOOR] || en_Enemies[ENEMY_TUTORIAL2_DOUBLEJUMPEN2]->GetCurrentFloor() == go_Floors[BOUNCING_TUTORIAL_FIRSTFLOOR])
	{
		if (pPlayer->GetFloor() == go_Floors[BOUNCING_TUTORIAL_SECONDFLOOR])
			en_Enemies[ENEMY_TUTORIAL2_DOUBLEJUMPEN2]->SetWorldPosition( 11100.00f, 4436.00f );
		else if (pPlayer->GetFloor() == go_Floors[BOUNCING_TUTORIAL_THIRDFLOOR])
			en_Enemies[ENEMY_TUTORIAL2_DOUBLEJUMPEN2]->SetWallLimits(nullptr, nullptr);
	}
	ControlEnemyRespawn(en_Enemies[ENEMY_TUTORIAL2_DOUBLEJUMPEN2], 10972.189f, 3859.258f);
	ControlEnemyRespawn(en_Enemies[ENEMY_TUTORIAL2_DOUBLEJUMPEN1], 9686.084f, 3313.543f);
	ExpandableFloorControl(go_Floors[SWITCH_TUTORIAL_EXPANDABLE1], go_Switches[SWITCH_TUTORIAL_No1], EVENT_TUTORIAL_EXPANDABLEFLOOR1, 1057.000f,3);
	ExpandableFloorControl(go_Floors[SWITCH_TUTORIAL_EXPANDABLE2], go_Switches[SWITCH_TUTORIAL_No2], EVENT_TUTORIAL_EXPANDABLEFLOOR2, 1057.000f,6);
	ExpandableFloorControl(go_Floors[SWITCH_TUTORIAL_EXPANDABLE3], go_Switches[SWITCH_TUTORIAL_No3], EVENT_TUTORIAL_EXPANDABLEFLOOR3, 1057.000f,12);
	ExpandableFloorControl(go_Floors[SWITCH_TUTORIAL_EXPANDABLE4], go_Switches[SWITCH_TUTORIAL_No4], EVENT_TUTORIAL_EXPANDABLEFLOOR4, 1057.000f,12);
	ExpandableFloorControl(go_Floors[SWITCH_TUTORIAL_EXPANDABLE5], go_Switches[SWITCH_TUTORIAL_No5], EVENT_TUTORIAL_EXPANDABLEFLOOR5, 1057.000f,12);

	KillEveryoneEventControlRevealsTeleporter(EVENT_TUTORIAL_FIGHT2, TUTORIAL_ENDPOINT, ENEMY_TUTORIAL3_EVENT1, ENEMY_TUTORIAL3_EVENT3, EVENT_TUTORIAL_FIGHT2_CAMERAFOCUSAFTERKILL, TUTORIAL_RANKED_EVENT);
}

//*****************************************************************************
//Level1_2RoomEventsUpdate関数
//レベル1-2イベントを管理する
//引数：void
//戻：void
//*****************************************************************************
void EventHandler::Level1_2RoomEventsUpdate()
{
	SceneInGame* pGame = GetCurrentGame();
	if (!pGame)
		return;
	Player* pPlayer = GetPlayerPointer();
	if (!pPlayer)
		return;
	if (bEventControls[EVENT_LVL01_SUBLEVELCHANGE] == true)
	{
		if (pPlayer->GetFloor() == go_Floors[FLOOR_LV1_REGULAR_B])
		{
			pPlayer->SetWorldPosition(0, 0);
			pPlayer->SetYForce(0);
			go_Floors[FLOOR_LV1_REGULAR_B]->SetUse(false);
			if(go_Walls[WALL_1_FALLING1])
				go_Walls[WALL_1_FALLING1]->SetUse(false);
			if(go_Walls[WALL_1_FALLING2])
				go_Walls[WALL_1_FALLING2]->SetUse(false);
			bEventControls[EVENT_LVL01_SUBLEVELCHANGE] = false;
		}
	}

	if (bEventControls[EVENT_LVL01_SCALEOUT]) {
		if (pPlayer->GetFloor() == go_Floors[FLOOR_LV1_REGULAR_C])
		{
			float currentscale = pGame->GetLevelScale();
			if (currentscale > 0.70f) {
				currentscale -= 0.005f;
				if (currentscale < 0.70f)
					currentscale = 0.70f;
			}
			pGame->SetLevelScale(currentscale);
		}
	}
	ExpandableFloorControl(go_Floors[FLOOR_LV1_EXPANDABLE6], go_Switches[SWITCH_L1_No9], EVENT_LVL01_EXPANDABLEFLOOREVENT6, 4516.000f, 22.75f);
	ExpandableFloorControl(go_Floors[FLOOR_LV1_EXPANDABLE7], go_Switches[SWITCH_L1_No10], EVENT_LVL01_EXPANDABLEFLOOREVENT7, 6215.000f, 22.75f);
	ExpandableFloorControl(go_Floors[FLOOR_LV1_EXPANDABLE8], go_Switches[SWITCH_L1_No11], EVENT_LVL01_EXPANDABLEFLOOREVENT8, 6215.000f, 28.75f);
	KillEveryoneEventControlRevealsTeleporter(EVENT_LVL01_KILLALLENEMIES3, TELEPORTER_LVL1_03, ENEMY_LV1_15, ENEMY_LV1_23, EVENT_LVL01_RANKEDEVENT_FOCUSCAMERAAFTERKILL_03, EVENT_LVL01_FIGHT_EVENT3);
	KillEveryoneEventControlRevealsSwitch(EVENT_LVL01_KILLALLENEMIES4, SWITCH_L1_No13, ENEMY_LV1_24, ENEMY_LV1_28, EVENT_LVL01_RANKEDEVENT_FOCUSCAMERAAFTERKILL_04, -1);
	KillEveryoneEventControlRevealsSwitch(EVENT_LVL01_KILLALLENEMIES5, SWITCH_L1_No14, ENEMY_LV1_29, ENEMY_LV1_31, EVENT_LVL01_RANKEDEVENT_FOCUSCAMERAAFTERKILL_05, -1);
}

//*****************************************************************************
//Level1_1RoomEventsUpdate関数
//レベル1-1イベントを管理する
//引数：void
//戻：void
//*****************************************************************************
void EventHandler::Level1_1RoomEventsUpdate()
{
	SceneInGame* pGame = GetCurrentGame();
	if (!pGame)
		return;
	if (go_Switches[SWITCH_L1_No8])
	{
		if (go_Switches[SWITCH_L1_No8]->IsSwitchOn())
		{
			static float fSwitchRotAcceleration = 0;
			fSwitchRotAcceleration += 0.5f;
			XMFLOAT3 SwitchRotation = go_Switches[SWITCH_L1_No8]->GetRotation();
			if (SwitchRotation.y != 90) {
				SwitchRotation.y += fSwitchRotAcceleration;
				if (SwitchRotation.y > 90)
					SwitchRotation.y = 90;
				go_Switches[SWITCH_L1_No8]->SetRotation(SwitchRotation);
			}
			if(go_Walls[WALL_1_FALLING1])
				go_Walls[WALL_1_FALLING1]->SetUse(true);
			if(go_Walls[WALL_1_FALLING2])
				go_Walls[WALL_1_FALLING2]->SetUse(true);
			bool bReadyToChangeSubLevelFloor = false;
			bool bReadyToChangeSubLevelColor = false;
			Color ColorBackground = pGame->GetCurrentBackgroundColor();
			static float fColorChangeSpeed = 0;
			fColorChangeSpeed += 0.00255f;
			if (ColorBackground.R > 0.05f) {
				ColorBackground.R -= fColorChangeSpeed;
				if (ColorBackground.R < 0.05f)
					ColorBackground.R = 0.05f;
			}
			if (ColorBackground.G > 0.05f) {
				ColorBackground.G -= fColorChangeSpeed;
				if (ColorBackground.G < 0.05f)
					ColorBackground.G = 0.05f;
			}
			if (ColorBackground.B > 0.15f) {
				ColorBackground.B -= fColorChangeSpeed;
				if (ColorBackground.B < 0.15f)
					ColorBackground.B = 0.15f;
			}
			bReadyToChangeSubLevelColor = ColorBackground.B == 0.15f && ColorBackground.G == 0.05f && ColorBackground.R == 0.05f;
			SetBackgroundColor(ColorBackground);
			if (go_Floors[FLOOR_LV1_REDUCEABLE_FLOOR_1] && bReadyToChangeSubLevelColor)
			{
				static float fFloorAcceleration = 0;
				fFloorAcceleration += 15;
				go_Floors[FLOOR_LV1_REDUCEABLE_FLOOR_1]->IncreaseSizeWithHitbox(-fFloorAcceleration, 0);
				bReadyToChangeSubLevelFloor = go_Floors[FLOOR_LV1_REDUCEABLE_FLOOR_1]->GetSize().x == 0;
			}
			if (bReadyToChangeSubLevelFloor && bReadyToChangeSubLevelColor) {
				pGame->SetSubLevel(SUBLEVEL1_2);
				fColorChangeSpeed = 0;
				fSwitchRotAcceleration = 0;
				return;
			}
		}
		else
		{
			static int nColorMirrorB = 1;
			Color ColorBackground = pGame->GetCurrentBackgroundColor();
			ColorBackground.B += 0.00055f*nColorMirrorB;
			if (ColorBackground.B > 0.86f || ColorBackground.B < 0.75f)
				nColorMirrorB *= -1;
			SetBackgroundColor(ColorBackground);
		}
	}

	//レベル１の行事１
	if (bEventControls[EVENT_LVL01_STAREVENT]) {
		if (go_Switches[SWITCH_L1_No1] && go_Switches[SWITCH_L1_No2])
		{
			if (go_Switches[SWITCH_L1_No1]->IsSwitchOn() && go_Switches[SWITCH_L1_No2]->IsSwitchOn())
			{
				float currentSize = go_PunchableStars[STAR_L1_No1]->GetSize().x;
				if (currentSize != STAR_SIZE) {
					SetFocalPoint(go_PunchableStars[STAR_L1_No1]);
					PauseEnemyUpdate(true);
					PausePlayerUpdate(true);
					if (IsCameraOnFocus())
					{
						fEventAcceleration[EVENT_LVL01_STAREVENT] += 0.225f;
						if(go_PunchableStars[STAR_L1_No1])
							go_PunchableStars[STAR_L1_No1]->SetUse(true);
						else {
							Go_List* pLevelObjects = pGame->GetLevelObjects();
							go_PunchableStars[STAR_L1_No1] = pLevelObjects->GetPunchStar(STAR_L1_No1);
						}

						currentSize += fEventAcceleration[EVENT_LVL01_STAREVENT];
						if (currentSize > STAR_SIZE)
							currentSize = STAR_SIZE;
						if (go_PunchableStars[STAR_L1_No1])
							go_PunchableStars[STAR_L1_No1]->SetSpriteSize(currentSize, currentSize);
						else {
							Go_List* pLevelObjects = pGame->GetLevelObjects();
							go_PunchableStars[STAR_L1_No1] = pLevelObjects->GetPunchStar(STAR_L1_No1);
						}
					}
				}
				else {
					nFrameCounter[EVENT_LVL01_STAREVENT]++;
					if (GetInput(INPUT_CONFIRM))
						nFrameCounter[EVENT_LVL01_STAREVENT] =  140;
					if (nFrameCounter[EVENT_LVL01_STAREVENT] >= 140 && GetPlayerPointer() != GetFocalPoint())
					{
						SetFocalPoint(GetPlayerPointer());
						nFrameCounter[EVENT_LVL01_STAREVENT] = 0;
					}
					if (GetPlayerPointer() == GetFocalPoint() && IsCameraOnFocus())
					{
						PauseEnemyUpdate(false);
						PausePlayerUpdate(false);
						bEventControls[EVENT_LVL01_STAREVENT] = false;
					}
				}
			}
			else
			{
				if (go_PunchableStars[STAR_L1_No1]) {
					go_PunchableStars[STAR_L1_No1]->SetSpriteSize(0, 0);
					go_PunchableStars[STAR_L1_No1]->SetUse(false);
				} else {
					Go_List* pLevelObjects = pGame->GetLevelObjects();
					go_PunchableStars[STAR_L1_No1] = pLevelObjects->GetPunchStar(STAR_L1_No1);
				}
			}
		}
	}
	//レベル１のサイズが変える床である
	ExpandableFloorControl(go_Floors[FLOOR_LV1_EXPANDABLE1], go_Switches[SWITCH_L1_No3], EVENT_LVL01_EXPANDABLEFLOOREVENT, WINDOW_WIDTH * 3.0f, 3.75f);
	ExpandableFloorControl(go_Floors[FLOOR_LV1_EXPANDABLE2], go_Switches[SWITCH_L1_No4], EVENT_LVL01_EXPANDABLEFLOOREVENT2, WINDOW_WIDTH * 0.34f, 5.75f);
	ExpandableFloorControl(go_Floors[FLOOR_LV1_EXPANDABLE3], go_Switches[SWITCH_L1_No5], EVENT_LVL01_EXPANDABLEFLOOREVENT3, WINDOW_WIDTH, 6.75f);
	ExpandableFloorControl(go_Floors[FLOOR_LV1_EXPANDABLE4], go_Switches[SWITCH_L1_No6], EVENT_LVL01_EXPANDABLEFLOOREVENT4, WINDOW_WIDTH, 8.75f);
	ExpandableFloorControl(go_Floors[FLOOR_LV1_EXPANDABLE5], go_Switches[SWITCH_L1_No7], EVENT_LVL01_EXPANDABLEFLOOREVENT5, WINDOW_WIDTH, 9.75f);

	//敵を倒すイベント１
	if (bEventControls[EVENT_LVL01_KILLALLENEMIES]) {
		if (en_Enemies[ENEMY_LV1_06])
			en_Enemies[ENEMY_LV1_06]->SetUse(false);
		if (go_Teleporters[TELEPORTER_LVL1_01])
		{
			go_Teleporters[TELEPORTER_LVL1_01]->SetAlpha(0);
			go_Teleporters[TELEPORTER_LVL1_01]->SetUse(false);
		}
		if (en_Enemies[ENEMY_LV1_01] && en_Enemies[ENEMY_LV1_02] && en_Enemies[ENEMY_LV1_03] && en_Enemies[ENEMY_LV1_04] && en_Enemies[ENEMY_LV1_05])
			bEventControls[EVENT_LVL01_KILLALLENEMIES] = !(en_Enemies[ENEMY_LV1_01]->IsEnemyDead() && en_Enemies[ENEMY_LV1_02]->IsEnemyDead() && 
				en_Enemies[ENEMY_LV1_03]->IsEnemyDead() && en_Enemies[ENEMY_LV1_04]->IsEnemyDead() && en_Enemies[ENEMY_LV1_05]->IsEnemyDead());
	} else {
		if (bEventControls[EVENT_LVL01_FOCUSCAMERAAFTERKILL_01])
		{
			if (!go_Teleporters[TELEPORTER_LVL1_01])
			{
				Go_List* pLevelTeleporters = pGame->GetLevelObjects();
				go_Teleporters[TELEPORTER_LVL1_01]=pLevelTeleporters->GetTeleporter(TELEPORTER_LVL1_01);
				return;
			}
			float fCurrentAlpha = go_Teleporters[TELEPORTER_LVL1_01]->GetAlpha();
			if (fCurrentAlpha != 1) {
				SetFocalPoint(go_Teleporters[TELEPORTER_LVL1_01]);
				PauseEnemyUpdate(true);
				PausePlayerUpdate(true);
				go_Teleporters[TELEPORTER_LVL1_01]->SetUse(true);
				if (IsCameraOnFocus())
				{
					fCurrentAlpha += 0.025f;
					if (fCurrentAlpha > 1) {
						fCurrentAlpha = 1;
					}
					if (fCurrentAlpha == 1) {
						reRankedEvents[EVENT_LVL01_FIGHT_EVENT1].SetEventOver();
					}
					go_Teleporters[TELEPORTER_LVL1_01]->SetAlpha(fCurrentAlpha);
				}
			}
			else {
				nFrameCounter[EVENT_LVL01_KILLALLENEMIES]++;
				if (GetInput(INPUT_CONFIRM))
					nFrameCounter[EVENT_LVL01_KILLALLENEMIES] = 100;
				if (nFrameCounter[EVENT_LVL01_KILLALLENEMIES] > 100) {
					SetFocalPoint(GetPlayerPointer());
					PauseEnemyUpdate(false);
					PausePlayerUpdate(false);
					bEventControls[EVENT_LVL01_FOCUSCAMERAAFTERKILL_01] = false;
				}
			}
		}
		else {
			ControlEnemyRespawn(en_Enemies[ENEMY_LV1_06], 2269.68f, -2168.05f);
		}
	}
	//敵を倒すイベント2
	KillEveryoneEventControlRevealsTeleporter(EVENT_LVL01_KILLALLENEMIES2, TELEPORTER_LVL1_02, ENEMY_LV1_08, ENEMY_LV1_14, EVENT_LVL01_RANKEDEVENT_FOCUSCAMERAAFTERKILL_02, EVENT_LVL01_FIGHT_EVENT2);
	//無限な敵「07」の管理
	if (en_Enemies[ENEMY_LV1_07])
	{
		ControlEnemyRespawn(en_Enemies[ENEMY_LV1_07], 11808.00f, 5649.50f);
		
		XMFLOAT3 pos = en_Enemies[ENEMY_LV1_07]->GetLocalPosition();
		if (go_Walls[WALL_1_UPRIGHT] && go_Walls[WALL_1_DOWNRIGHT] && go_Walls[WALL_1_UPLEFT] && go_Walls[WALL_1_DOWNLEFT]
			&& go_Floors[FLOOR_LV1_REGULAR_A])
		{
			if (pos.y < 5504.50f)
			{
				//en_Enemies[ENEMY_LV1_07]->SetWallLimits(go_Walls[WALL_1_UPRIGHT], go_Walls[WALL_1_UPLEFT]);
				en_Enemies[ENEMY_LV1_07]->SetWallLimits(go_Walls[WALL_1_DOWNRIGHT], go_Walls[WALL_1_DOWNLEFT]);
			}
			else {
				//en_Enemies[ENEMY_LV1_07]->SetWallLimits(go_Walls[WALL_1_DOWNRIGHT], go_Walls[WALL_1_DOWNLEFT]);
				en_Enemies[ENEMY_LV1_07]->SetWallLimits(go_Walls[WALL_1_UPRIGHT], go_Walls[WALL_1_UPLEFT]);
			}
			while (pos.y > 6795.19f) {
				pos.y--;
				en_Enemies[ENEMY_LV1_07]->SetLocalPosition(pos.x, pos.y);
			}
		}
	}
}

//*****************************************************************************
//ControlEnemyRespawn関数
//敵の復活を管理する
//引数：Enemy*, float, float
//戻：void
//*****************************************************************************
void EventHandler::ControlEnemyRespawn(Enemy * EnemyToRespawn, float fPosX, float fPosY)
{
	if (EnemyToRespawn)
	{
		EnemyToRespawn->SetUse(true);
		if (EnemyToRespawn->IsEnemyDead()) {
			EnemyToRespawn->Init(BUNBUN_ENEMY);
			EnemyToRespawn->SetNoScoreEnemy(true);
			EnemyToRespawn->SetWorldPosition(fPosX, fPosY);
		}
	}
}

//*****************************************************************************
//KillEveryoneEventControlRevealsTeleporter関数
//敵の倒すあとでテレポートを表す関数
//引数：int, int, int, int, int, int
//戻：void
//*****************************************************************************
void EventHandler::KillEveryoneEventControlRevealsTeleporter(int nEvent, int nTeleporter, int nFirstEnemy, int nEndEnemy, int nRankedEvent, int nFightEvent)
{
	SceneInGame* pGame = GetCurrentGame();
	if (!pGame)
		return;
	if (bEventControls[nEvent])
	{
		if (go_Teleporters[nTeleporter])
		{
			go_Teleporters[nTeleporter]->SetAlpha(0);
			go_Teleporters[nTeleporter]->SetUse(false);
		}
		bEventControls[nEvent] = !CheckIfEventEnemiesAreDead(nFirstEnemy, nEndEnemy);
	}
	else {
		if (bEventControls[nRankedEvent])
		{
			if (!go_Teleporters[nTeleporter]) 
			{
				Go_List* pLevelTeleporters = pGame->GetLevelObjects();
				go_Teleporters[nTeleporter] = pLevelTeleporters->GetTeleporter(nTeleporter);
				return;
			}
			float fCurrentAlpha = go_Teleporters[nTeleporter]->GetAlpha();
			if (fCurrentAlpha != 1) {
				SetFocalPoint(go_Teleporters[nTeleporter]);
				PauseEnemyUpdate(true);
				PausePlayerUpdate(true);
				go_Teleporters[nTeleporter]->SetUse(true);
				if (IsCameraOnFocus())
				{
					fCurrentAlpha += 0.025f;
					if (fCurrentAlpha > 1) {
						fCurrentAlpha = 1;
					}
					if (fCurrentAlpha == 1) {
						if(nFightEvent>=0)
							reRankedEvents[nFightEvent].SetEventOver();
					}
					go_Teleporters[nTeleporter]->SetAlpha(fCurrentAlpha);
				}
			}
			else {
				nFrameCounter[nEvent]++;
				if (GetInput(INPUT_CONFIRM))
					nFrameCounter[nEvent] = 100;
				if (nFrameCounter[nEvent] > 100) {
					SetFocalPoint(GetPlayerPointer());
					PauseEnemyUpdate(false);
					PausePlayerUpdate(false);
					bEventControls[nRankedEvent] = false;
				}
			}
		}
	}
}

//*****************************************************************************
//KillEveryoneEventControlRevealsSwitch関数
//敵の倒すあとでスイッチを表す関数
//引数：int, int, int, int, int, int
//戻：void
//*****************************************************************************
void EventHandler::KillEveryoneEventControlRevealsSwitch(int nEvent, int nLevelObject, int nFirstEnemy, int nEndEnemy, int nRankedEvent, int nFightEvent)
{
	SceneInGame* pGame = GetCurrentGame();
	if (bEventControls[nEvent])
	{
		if (go_Switches[nLevelObject])
		{
			go_Switches[nLevelObject]->SetAlpha(0);
			go_Switches[nLevelObject]->SetUse(false);
		}
		bEventControls[nEvent] = !CheckIfEventEnemiesAreDead(nFirstEnemy, nEndEnemy);
	}
	else {
		if (bEventControls[nRankedEvent])
		{
			if (!go_Switches[nLevelObject])
			{
				Go_List* pLevelObjects = pGame->GetLevelObjects();
				go_Switches[nLevelObject] = (SwitchGO*)pLevelObjects->GetSwitch(nLevelObject);
				return;
			}
			float fCurrentAlpha = go_Switches[nLevelObject]->GetAlpha();
			if (fCurrentAlpha != 1) {
				SetFocalPoint(go_Switches[nLevelObject]);
				PauseEnemyUpdate(true);
				PausePlayerUpdate(true);
				go_Switches[nLevelObject]->SetUse(true);
				if (IsCameraOnFocus())
				{
					fCurrentAlpha += 0.025f;
					if (fCurrentAlpha > 1) {
						fCurrentAlpha = 1;
					}
					if (fCurrentAlpha == 1) {
						if (nFightEvent >= 0)
							reRankedEvents[nFightEvent].SetEventOver();
					}
					go_Switches[nLevelObject]->SetAlpha(fCurrentAlpha);
				}
			}
			else {
				nFrameCounter[nEvent]++;
				if (GetInput(INPUT_CONFIRM))
					nFrameCounter[nEvent] = 100;
				if (nFrameCounter[nEvent] > 100) {
					SetFocalPoint(GetPlayerPointer());
					PauseEnemyUpdate(false);
					PausePlayerUpdate(false);
					bEventControls[nRankedEvent] = false;
				}
			}
		}
	}
}

//*****************************************************************************
//CheckIfEventEnemiesAreDead関数
//全部の敵を倒したことを確認する
//引数：int, int
//戻：bool
//*****************************************************************************
bool EventHandler::CheckIfEventEnemiesAreDead(int nStartEnemy, int nEndEnemy)
{
	bool AreEnemiesDead = false;
	for (int i = nStartEnemy; i <= nEndEnemy; i++)
	{
		if (en_Enemies[i])
		{
			AreEnemiesDead = en_Enemies[i]->IsEnemyDead();
			if (!AreEnemiesDead)
				break;
		}
	}
	return AreEnemiesDead;
}

//*****************************************************************************
//ExpandableFloorControl関数
//伸びれる床を管理する
//引数：Floor*, GameObject*, int, float, float
//戻：void
//*****************************************************************************
void ExpandableFloorControl(Floor * FloorToExpandContract, SwitchGO * SwitchToRelyOn, int AccelerationNumberEvent, float MaxSizeFloor, float fContractSpeed)
{
	if (FloorToExpandContract && SwitchToRelyOn) {
		FloorToExpandContract->SetFloorAsSafe(false);
		if (SwitchToRelyOn->IsSwitchOn())
		{
			fEventAcceleration[AccelerationNumberEvent]++;
			float FloorSizeX = FloorToExpandContract->GetSize().x;
			FloorSizeX += fEventAcceleration[AccelerationNumberEvent];
			if (FloorSizeX > MaxSizeFloor)
			{
				SwitchToRelyOn->SetSwitchOn(false);
				FloorSizeX = MaxSizeFloor;
			}
			FloorToExpandContract->SetSpriteSize(FloorSizeX, FloorToExpandContract->GetSize().y);
			HitBox newHB = FloorToExpandContract->GetHitBoxWithoutPosition();
			newHB.w = FloorSizeX;
			FloorToExpandContract->SetNewHitbox(newHB);
		}
		else
		{
			fEventAcceleration[AccelerationNumberEvent] = 0;
			float FloorSizeX = FloorToExpandContract->GetSize().x;
			FloorSizeX -= fContractSpeed;
			if (FloorSizeX < 0)
				FloorSizeX = 0;
			FloorToExpandContract->SetSpriteSize(FloorSizeX, FloorToExpandContract->GetSize().y);
			HitBox newHB = FloorToExpandContract->GetHitBoxWithoutPosition();
			newHB.w = FloorSizeX;
			FloorToExpandContract->SetNewHitbox(newHB);
		}
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void EventHandler::Draw()
{
	//なし
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void EventHandler::End()
{
	//なし
}

//*****************************************************************************
//SearchForLevelObjects関数
//レベルのオブジェクトを探す関数
//引数：void
//戻：void
//*****************************************************************************
void SearchForLevelObjects()
{
	SceneInGame* pGame = GetCurrentGame();
	if (!pGame)
		return;
	for (int i = 0; i < ENEMY_ID_MAX; i++)
	{
		en_Enemies[i] = nullptr;
		Enemy_List* pLevelObjects = pGame->GetEnemies();
		if (pLevelObjects)
			en_Enemies[i] = pLevelObjects->GetEnemy(i);
	}
	for (int i = 0; i < STAR_MAX; i++)
	{
		go_PunchableStars[i] = nullptr;
		Go_List* pLevelObjects = pGame->GetLevelObjects();
		if (pLevelObjects)
			go_PunchableStars[i] = pLevelObjects->GetPunchStar(i);
	}
	for (int i = 0; i < SWITCH_MAX; i++)
	{
		go_Switches[i] = nullptr;
		Go_List* pLevelObjects = pGame->GetLevelObjects();
		if (pLevelObjects)
			go_Switches[i] = (SwitchGO*)pLevelObjects->GetSwitch(i);
	}
	for (int i = 0; i < FLOOR_ID_MAX; i++)
	{
		go_Floors[i] = nullptr;
		Go_List* pLevelFloors = pGame->GetFloors();
		if (pLevelFloors)
			go_Floors[i] = (Floor*)pLevelFloors->GetFloor(i);
	}

	for (int i = 0; i < MAX_TELEPORTERS_ID; i++)
	{
		go_Teleporters[i] = nullptr;
		Go_List* pLevelTeleporters = pGame->GetLevelObjects();
		if (pLevelTeleporters)
			go_Teleporters[i] = pLevelTeleporters->GetTeleporter(i);
	}

	for (int i = 0; i < MAX_WALLS_ID; i++)
	{
		go_Walls[i] = nullptr;
		Go_List* pLevelWalls = pGame->GetWalls();
		if (go_Walls)
			go_Walls[i] = pLevelWalls->GetWall(i);
	}
}

//*****************************************************************************
//GetEventEnemy関数
//イベントの敵を戻す
//引数：int
//戻：Enemy*
//*****************************************************************************
Enemy* GetEventEnemy(int nEn)
{
	SceneInGame* pGame = GetCurrentGame();
	if (!en_Enemies[nEn]) {
		en_Enemies[nEn] = nullptr;
		Enemy_List* pLevelObjects = pGame->GetEnemies();
		if (pLevelObjects)
			en_Enemies[nEn] = pLevelObjects->GetEnemy(nEn);
	}
	return en_Enemies[nEn];
}

//*****************************************************************************
//GetCurrentActiveEvent関数
//行っているイベントを戻す
//引数：void
//戻：RankedEvent*
//*****************************************************************************
RankedEvent * GetCurrentActiveEvent()
{
	return p_reCurrentActiveEvent;
}

//*****************************************************************************
//コンストラクタ関数
//*****************************************************************************
RankedEvent::RankedEvent()
{
	fCurrentTime = 0;
	nDamageDone = 0;
	nCurrentComboScore = 0;
	nDamageDone = 0;
	hbStartEvent= {0,0,0,0,false, nullptr};
	hbEndEvent  = {0,0,0,0,false, nullptr};
	bEventIsActive=false;
	bEventIsOver = false;
	nEventType = 0;
	fMinTime = 0;
	fMinComboScore = 0;
	nMinDamageReceived = 0;
	nLevel = 0;
	nSubLevel = 0;
	cTimeRank = 'Z';
	cComboRank = 'Z';
	cDamageRank = 'Z';
	cOverralRank = 'Z';
}

RankedEvent::~RankedEvent()
{
	//なし
}

//*****************************************************************************
//InitEvent関数
//初期化関数
//引数：void
//戻：HitBox, HitBox, float, float, float, int, int
//*****************************************************************************
void RankedEvent::InitEvent(HitBox StartEvent, HitBox EndEvent, float MinTime, float MinCombo, float MinDamage, int Level, int SubLevel)
{
	fCurrentTime = 0;
	nCurrentComboScore = 0;
	nDamageDone = 0;
	bEventIsActive = false;
	bEventIsOver = false;
	hbStartEvent = StartEvent;
	hbEndEvent = EndEvent;
	fMinTime = MinTime;
	fMinComboScore = MinCombo;
	nMinDamageReceived = (int)MinDamage;
	nLevel = Level;
	nSubLevel = SubLevel;
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void RankedEvent::Update()
{
	SceneInGame* pGame = GetCurrentGame();
	pGame = GetCurrentGame();
	if (!pGame)
		return;
	if (bEventIsOver || nLevel!= pGame->GetCurrentLevel() || nSubLevel != pGame->GetCurrentSubLevel())
		return;
	Player* pPlayer = GetPlayerPointer();
	if (!bEventIsActive)
	{
		if (IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), hbStartEvent))
		{
			SetEventActive();
			return;
		}
	}
	if(!(IsPlayerPaused()))
		fCurrentTime++;
	
	if (pPlayer)
	{
		if (IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), hbEndEvent) && hbEndEvent.x >0 && hbEndEvent.y>0)
		{
			SetEventOver();
		}
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void RankedEvent::Draw()
{
	//なし
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void RankedEvent::End()
{
	//なし
}

//*****************************************************************************
//SetEventActive関数
//イベントを有効します
//引数：void
//戻：void
//*****************************************************************************
void RankedEvent::SetEventActive()
{
	UIManager::SetStartEventMessageOn();
	p_reCurrentActiveEvent = this;
	bEventIsActive = true;
	UIManager::SetOnAirMessageUse(true);
}

//*****************************************************************************
//SetEventOver関数
//イベントを無効します
//引数：void
//戻：void
//*****************************************************************************
void RankedEvent::SetEventOver()
{
	SceneInGame* pGame = GetCurrentGame();
	pGame = GetCurrentGame();
	if (!pGame)
		return;
	cTimeRank = CalculateTimeRank();
	cComboRank = CalculateComboRank();
	cDamageRank = CalculateDamageRank();
	cOverralRank = CalculateOverralRank(cTimeRank, cComboRank, cDamageRank);
#ifdef USE_CONSOLE
	printf("TIME: %f RANK: %c \n", fCurrentTime, cTimeRank);
	printf("COMBO: %d RANK: %c \n", nCurrentComboScore, cComboRank);
	printf("DAMAGEDONE: %d RANK: %c \n", nDamageDone, cDamageRank);
	printf("OVERRALL: %c \n", cOverralRank);
#endif
	p_reCurrentActiveEvent = nullptr;
	bEventIsOver = true;
	switch (cOverralRank)
	{
	case 'S':
		pGame->AddToScore(10000);
		break;
	case 'A':
		pGame->AddToScore(5000);
		break;
	case 'B':
		pGame->AddToScore(2500);
		break;
	case 'D':
		pGame->AddToScore(500);
		break;
	default:
		break;
	}
	UIManager::SetEndEventMessageOn(cTimeRank, cComboRank, cDamageRank, cOverralRank);
	UIManager::SetOnAirMessageUse(false);
}

//*****************************************************************************
//CalculateTimeRank関数
//イベントをタイムランクを数える
//引数：void
//戻：char
//*****************************************************************************
char RankedEvent::CalculateTimeRank()
{
	if (fCurrentTime <= fMinTime)
		return 'S';
	if (fCurrentTime > fMinTime && (fCurrentTime <= fMinTime + (fMinTime*0.25f)))
		return 'A';
	if (fCurrentTime > fMinTime + (fMinTime*0.25f) && (fCurrentTime <= fMinTime + (fMinTime*0.5f)))
		return 'B';
	if (fCurrentTime > fMinTime + (fMinTime*0.5f) && (fCurrentTime <= fMinTime + (fMinTime*0.75f)))
		return 'C';
	if (fCurrentTime > fMinTime + (fMinTime*0.75f))
		return 'D';
	return 0;
}

//*****************************************************************************
//CalculateComboRank関数
//イベントをコンボランクを数える
//引数：void
//戻：char
//*****************************************************************************
char RankedEvent::CalculateComboRank()
{
	if (nCurrentComboScore >= fMinComboScore)
		return 'S';
	if (nCurrentComboScore < fMinComboScore && (nCurrentComboScore >= fMinComboScore - (fMinComboScore*0.25f)))
		return 'A';
	if (nCurrentComboScore < fMinComboScore - (fMinComboScore*0.25f) && (nCurrentComboScore >= fMinComboScore - (fMinComboScore*0.5f)))
		return 'B';		   
	if (nCurrentComboScore < fMinComboScore - (fMinComboScore*0.5f) && (nCurrentComboScore >= fMinComboScore - (fMinComboScore*0.75f)))
		return 'C';		   
	if (nCurrentComboScore < fMinComboScore - (fMinComboScore*0.75f))
		return 'D';
	return 0;
}

//*****************************************************************************
//CalculateDamageRank関数
//イベントをダメージランクを数える
//引数：void
//戻：char
//*****************************************************************************
char RankedEvent::CalculateDamageRank()
{
	if (nDamageDone == 0)
		return 'S';
	if (nDamageDone <= nMinDamageReceived)
		return 'A';
	if (nDamageDone == nMinDamageReceived+1)
		return 'B';
	if (nDamageDone == nMinDamageReceived+2)
		return 'C';
	if (nDamageDone > nMinDamageReceived+2)
		return 'D';
	return 0;
}

//*****************************************************************************
//CalculateOverralRank関数
//イベントを全体ランクを数える
//引数：void
//戻：char
//*****************************************************************************
char RankedEvent::CalculateOverralRank(char TimeRank, char ComboRank, char DamageRank)
{
	int nTimeCalc = 0;
	if (TimeRank == 'S')
		nTimeCalc = 10;
	if (TimeRank == 'A')
		nTimeCalc = 8;
	if (TimeRank == 'B')
		nTimeCalc = 5;
	if (TimeRank == 'C')
		nTimeCalc = 3;
	if (TimeRank == 'D')
		nTimeCalc = 1;

	int nComboCalc = 0;
	if (ComboRank == 'S')
		nComboCalc = 10;
	if (ComboRank == 'A')
		nComboCalc = 8;
	if (ComboRank == 'B')
		nComboCalc = 5;
	if (ComboRank == 'C')
		nComboCalc = 3;
	if (ComboRank == 'D')
		nComboCalc = 1;

	int nDamageCalc = 0;
	if (DamageRank == 'S')
		nDamageCalc = 10;
	if (DamageRank == 'A')
		nDamageCalc = 8;
	if (DamageRank == 'B')
		nDamageCalc = 5;
	if (DamageRank == 'C')
		nDamageCalc = 3;
	if (DamageRank == 'D')
		nDamageCalc = 1;
	
	int OverrallRank = nTimeCalc + nComboCalc + nDamageCalc;
	if (OverrallRank >= 28)
		return 'S';
	if (OverrallRank < 28 && OverrallRank >= 21)
		return 'A';
	if (OverrallRank < 21 && OverrallRank >= 13)
		return 'B';
	if (OverrallRank < 13 && OverrallRank >= 7)
		return 'C';
	if (OverrallRank < 7)
		return 'D';
	return 0;
}

//*****************************************************************************
//GetComboRank関数
//コンボランクを戻す
//引数：void
//戻：char
//*****************************************************************************
char RankedEvent::GetComboRank()
{
	return cComboRank;
}

//*****************************************************************************
//GetDamageRank関数
//ダメージを戻す
//引数：void
//戻：char
//*****************************************************************************
char RankedEvent::GetDamageRank()
{
	return cDamageRank;
}

//*****************************************************************************
//GetTimeRank関数
//タイムランクを戻す
//引数：void
//戻：char
//*****************************************************************************
char RankedEvent::GetTimeRank()
{
	return cTimeRank;
}

//*****************************************************************************
//GetOverralRank関数
//全体ランクを戻す
//引数：void
//戻：char
//*****************************************************************************
char RankedEvent::GetOverralRank()
{
	return cOverralRank;
}

//*****************************************************************************
//AddComboScore関数
//コンボのスコアを増やす
//引数：int
//戻：void
//*****************************************************************************
void RankedEvent::AddComboScore(int nComboToAdd)
{
	nCurrentComboScore += nComboToAdd;
}

//*****************************************************************************
//AddDamage関数
//ダメージカウンターを増やす
//引数：int
//戻：void
//*****************************************************************************
void RankedEvent::AddDamage(int nDamage)
{
	nDamageDone += nDamage;
}

//*****************************************************************************
//GetStartHitbox関数
//イベントのスタートヒットボックスを戻す
//引数：void
//戻：HitBox
//*****************************************************************************
HitBox RankedEvent::GetStartHitbox()
{
	return hbStartEvent;
}

//*****************************************************************************
//GetEndHitbox関数
//イベントの終了ヒットボックスを戻す
//引数：void
//戻：HitBox
//*****************************************************************************
HitBox RankedEvent::GetEndHitbox()
{
	return hbEndEvent;
}
