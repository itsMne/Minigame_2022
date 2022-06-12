//*****************************************************************************
// GameManager.cpp
// ゲーム中の管理
// Author : Mane
//*****************************************************************************
#include "GameManager.h"
#include "UniversalVariables.h"
#include "Player.h"
#include "UI_Manager.h"
#include "Camera.h"
#include "RankManager.h"
#include "TransitionControl.h"
#include "ObjectManager.h"
#include "GameInputManager.h"
#include "sound.h"
#include <stdio.h>
#include "EventHandler.h"
#include "MainMenuControl.h"
#include "TextureManager.h"
#include "PauseMenuControl.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#if DEBUG_MODE
#define DEBUG_VISIBILITY true
#else
#define DEBUG_VISIBILITY false
#endif
#define TILE_SIZE_FLOOR2 320
//*****************************************************************************
// グローバル変数
//*****************************************************************************
bool		bGameIsPaused;
bool		bSetDeathScore;
int			nCurrentLevel;
int			nCurrent_SubLevel;
int			nScore;
int			nGems;
int			nMaxGems;
float		fLevelScale;
Enemy_List* Enemies = nullptr;
Go_List*	BackgroundObjects = nullptr;
Go_List*	PickableGems = nullptr;
Go_List*	Floors = nullptr;
Go_List*	Walls = nullptr;
Go_List*	LevelObjects = nullptr;
Player*		g_Player = nullptr;
Color		ColorBackground = { 0,0,0,1 };

void InGame::Init()
{
	if (!g_Player)
		g_Player = new Player();
	if (!Enemies)
		Enemies = new Enemy_List();
	if (!Floors)
		Floors = new Go_List();
	if (!Walls)
		Walls = new Go_List();
	if (!LevelObjects)
		LevelObjects = new Go_List();
	if (!BackgroundObjects)
		BackgroundObjects = new Go_List();
	if (!PickableGems)
		PickableGems = new Go_List();
	g_Player->Init();
	InitCamera();
	SetFocalPoint(g_Player);
	SetCameraAdjustment({ 0,100,0 });
	InitUIManager();
	InitPauseScreen();
	switch (nCurrentLevel)
	{
	case DEBUG_ROOM:
		SetVolume(1);
		PlaySound(SOUND_LABEL_BGM_DEBUGROOM);
		ColorBackground = { 0.125f, 0.35f, 0.45f, 1 };
		DebugRoomObjects();
		SetLevelScale(0.75f);
		break;
	case TUTORIAL_ROOM:
		SetVolume(1);
		PlaySound(SOUND_LABEL_BGM_TUTORIALROOM);
		ColorBackground = { 0.125f, 0.35f, 0.45f, 1 };
		SetSubLevel(TUTORIAL_R);
		SetLevelScale(0.90f);
		break;
	case LEVEL1_ROOM:
		SetVolume(1);
		PlaySound(SOUND_LABEL_BGM_LEVEL1ROOM);
		ColorBackground = { 0.525f, 0.55f, 0.85f, 1 };
		SetSubLevel(SUBLEVEL1_1);
		SetLevelScale(1);
		break;
	default:
		break;
	}
	EventHandler::Init();
	RankManager::Init();
	nMaxGems = nGems = nScore = 0;
	bGameIsPaused = false;
	bSetDeathScore = false;
}

void InGame::TutorialObjects()
{
	SetBackgroundColor(ColorBackground);
	PickableGems->AddGemsFromFile("data/f_Gems_Tutorial");
	GameObject* LastGOAdded = nullptr;
	Enemy* LastEnemyAdded = nullptr;
	LevelObjects->AddSpike({ 4557.190f,-1902.920f }, 1060);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH * 2,50,false,nullptr }, TILE_SIZE_FLOOR2, { 0, -(WINDOW_HEIGHT / 2) + 120 }, { WINDOW_WIDTH * 2, 270 });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,3461.000f,false,nullptr }, 150.000f, { -1343.000f, 1316.000f }, { 150.000f,3461.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,306.000f,false,nullptr }, 150.000f, { 1342.000f, -219.000f }, { 150.000f,306.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,989.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 1762.000f, 46.000f }, { 989.000f,270.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,162.000f,false,nullptr }, 150.000f, { 2158.000f, 37.000f }, { 150.000f,162.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,150.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 2158.000f, 207.000f }, { 150.000f,270.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,157.000f,false,nullptr }, 150.000f, { 3297.000f, 343.000f }, { 150.000f,157.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,392.000f,false,nullptr }, 150.000f, { 2305.000f, 107.000f }, { 150.000f,392.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,2886.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 3665.000f, 437.000f }, { 2886.000f,270.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,2505.000f,false,nullptr }, 150.000f, { 3295.000f, 1562.000f }, { 150.000f,2505.000f });
	LevelObjects->AddPunchStar({ 2962.00f, 518.00f }, { 2902.00f, 3153.00f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,3305.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 3153.000f, 2949.000f }, { 3305.000f,270.000f });
	GameObject* WallLeft = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,3286.000f,false,nullptr }, 150.000f, { 1507.000f, 4460.000f }, { 150.000f,3286.000f });
	GameObject* WallRight = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,3286.000f,false,nullptr }, 150.000f, { 4812.000f, 4460.000f }, { 150.000f,3286.000f });
	LastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 4161.000f,3070.000f }, ENEMY_TUTORIAL1);
	LastEnemyAdded->SetAsIntroductionEnemy();
	LastEnemyAdded->SetWallLimits(WallRight, WallLeft);
	LastGOAdded = LevelObjects->AddSpike({ 3167.575f,6239.787f }, 41);
	LastGOAdded->SetRotation({ 0, 0, 180 });
	LevelObjects->AddTeleporter({ 4546.19f, 3359.08f }, { 3541.19f, 2669.08f }, TELEPORTER_TUTORIAL_01);
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,2262.000f,false,nullptr }, 150.000f, { 3791.187f, 1694.082f }, { 150.000f,2262.000f });
	LevelObjects->AddRecoveryPickable({ 7213.19f, 438.08f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,674.000f,50,false,nullptr },  TILE_SIZE_FLOOR2, { 5646.190f, 437.125f }, { 674.000f,270.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,674.000f,50,false,nullptr },  TILE_SIZE_FLOOR2, { 7311.190f, 437.125f }, { 674.000f,270.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,331.000f,50,false,nullptr },  TILE_SIZE_FLOOR2, { 6600.190f, 437.322f }, { 331.000f,270.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,4265.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 10067.189f, 2912.869f }, { 4265.000f,270.000f }, BOUNCING_TUTORIAL_FIRSTFLOOR);
	LevelObjects->AddTeleporter({ 7597.19f, 597.87f }, { 8122.19f, 3117.87f });
	WallLeft = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,5235.000f,false,nullptr }, 150.000f, { 7952.189f, 5342.870f }, { 150.000f,5235.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,5235.000f,false,nullptr }, 150.000f, { 12272.189f, 5342.870f }, { 150.000f,5235.000f });
	//BOUNCINGチュートリアル
	WallRight = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,748.000f,false,nullptr }, 150.000f, { 9961.084f, 3278.543f }, { 150.000f,748.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,142.000f,false,nullptr }, 150.000f, { 9962.884f, 2837.870f }, { 150.000f,142.000f });
	LastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 9686.084f,3313.543f }, ENEMY_TUTORIAL2_DOUBLEJUMPEN1);
	LastEnemyAdded->SetWallLimits(WallRight, WallLeft);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,1927.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 10485.189f, 3754.258f }, { 1927.000f,270.000f }, BOUNCING_TUTORIAL_SECONDFLOOR);
	WallLeft = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,2395.000f,false,nullptr }, 150.000f, { 9475.189f, 4814.258f }, { 150.000f,2395.000f });
	WallRight = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,770.000f,false,nullptr }, 150.000f, { 11372.189f, 4134.258f }, { 150.000f,770.000f });
	LastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 10972.189f,3859.258f }, ENEMY_TUTORIAL2_DOUBLEJUMPEN2);
	LastEnemyAdded->SetWallLimits(WallRight, WallLeft);
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,142.000f,false,nullptr }, 150.000f, { 11372.884f, 3702.870f }, { 150.000f,142.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,1948.000f,false,nullptr }, 150.000f, { 10878.383f, 5445.019f }, { 150.000f,1948.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,1539.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 11575.000f, 4607.500f }, { 1539.000f,270.000f }, BOUNCING_TUTORIAL_THIRDFLOOR);
	LastGOAdded = LevelObjects->AddSpike({ 10114.541f,6468.000f }, 50);
	LastGOAdded->SetRotation({ 0, 0, 180 });
	LastGOAdded = LevelObjects->AddSpike({ 9404.541f,6048.000f }, 35);
	LastGOAdded->SetRotation({ 0, 0, 180 });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,5349.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 10110.000f, 8051.000f }, { 5349.000f,270.000f });
	LevelObjects->AddTeleporter({ 12085.00f, 4811.00f }, { 7925.00f, 8261.00f });

	//スイッチチュートリアル
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,4953.000f,false,nullptr }, 150.000f, { 7440.000f, 10381.000f }, { 150.000f,4953.000f });
	WallRight = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,4953.000f,false,nullptr }, 150.000f, { 12715.000f, 10381.000f }, { 150.000f,4953.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,4941.000f,false,nullptr }, 150.000f, { 8470.000f, 10377.000f }, { 150.000f,4941.000f });
	LevelObjects->AddSwitchBulb({ 8286.00f, 8127.00f }, SWITCH_TUTORIAL_No1, true);
	LevelObjects->AddSwitchBulb({ 7616.00f, 8503.00f }, SWITCH_TUTORIAL_No2, true);
	LevelObjects->AddSwitchBulb({ 8315.00f, 8829.00f }, SWITCH_TUTORIAL_No3, true);
	LevelObjects->AddSwitchBulb({ 7599.79f, 9491.00f }, SWITCH_TUTORIAL_No4, false);
	LevelObjects->AddSwitchBulb({ 8329.00f, 10157.29f }, SWITCH_TUTORIAL_No5, false);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,1057.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 7925.000f, 8427.000f }, { 1057.000f,270.000f }, SWITCH_TUTORIAL_EXPANDABLE1);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,1118.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 7952.000f, 8753.000f }, { 1118.000f,270.000f }, SWITCH_TUTORIAL_EXPANDABLE2);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,1066.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 7932.000f, 9079.000f }, { 1066.000f,270.000f }, SWITCH_TUTORIAL_EXPANDABLE3);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,1115.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 7925.000f, 9415.000f }, { 1115.000f,270.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,1115.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 7935.000f, 9746.000f }, { 1115.000f,270.000f }, SWITCH_TUTORIAL_EXPANDABLE4);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,1115.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 7935.000f, 10082.000f }, { 1115.000f,270.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,1115.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 7950.000f, 10413.000f }, { 1115.000f,270.000f }, SWITCH_TUTORIAL_EXPANDABLE5);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,1115.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 7940.000f, 10754.000f }, { 1115.000f,270.000f });
	LevelObjects->AddTeleporter({ 7937.00f, 11080.00f }, { 8725.00f, 12466.00f });
	WallLeft = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,4935.000f,false,nullptr }, 150.000f, { 8982.000f, 10790.000f }, { 150.000f,4935.000f });
	LastGOAdded = LevelObjects->AddSpike({ 10037.000f,12915.000f }, 62);
	LastGOAdded->SetRotation({ 0, 180, 0 });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,3705.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 10846.000f, 8456.225f }, { 3705.000f,270.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,446.000f,false,nullptr }, 150.000f, { 9706.000f, 8141.225f }, { 150.000f,446.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,162.000f,false,nullptr }, 150.000f, { 9558.000f, 8042.000f }, { 150.000f,162.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,150.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 9558.000f, 8192.000f }, { 150.000f,270.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,448.000f,false,nullptr }, 150.000f, { 10279.000f, 8562.000f }, { 150.000f,448.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,3680.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 10819.000f, 8892.000f }, { 3680.000f,270.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,168.000f,false,nullptr }, 150.000f, { 10129.000f, 8457.000f }, { 150.000f,168.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,150.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 10129.000f, 8622.000f }, { 150.000f,270.000f });

	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,154.000f,false,nullptr }, 150.000f, { 10688.000f, 8878.000f }, { 150.000f,154.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,150.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 10683.000f, 9038.000f }, { 150.000f,270.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,3725.000f,50,false,nullptr },TILE_SIZE_FLOOR2, { 10888.000f, 9333.000f }, { 3725.000f,270.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,458.000f,false,nullptr }, 150.000f, { 10837.000f, 9003.000f }, { 150.000f,458.000f });

	LevelObjects->AddRecoveryPickable({ 9226.50f, 8476.00f });
	LastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 10250.000f,9459.000f }, ENEMY_TUTORIAL3_EVENT1);
	LastEnemyAdded->SetWallLimits(WallRight, WallLeft);
	LastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 9515.000f,9459.000f  }, ENEMY_TUTORIAL3_EVENT2);
	LastEnemyAdded->SetWallLimits(WallRight, WallLeft);
	LastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 11390.000f,9459.000f }, ENEMY_TUTORIAL3_EVENT3);
	LastEnemyAdded->SetWallLimits(WallRight, WallLeft);
	LevelObjects->AddEndPoint({ 12497.94f, 9666.87f }, TUTORIAL_ENDPOINT);
#ifdef USE_CONSOLE
	printf("LEVEL OBJECTS: %d\n", LevelObjects->GetNumberOfObjects());
	printf("WALLS: %d\n", Walls->GetNumberOfObjects());
	printf("FLOORS: %d\n", Floors->GetNumberOfObjects());
	printf("GEMS: %d\n", PickableGems->GetNumberOfObjects());
#endif
}

void InGame::Level1_1Objects()
{
	SetBackgroundColor(ColorBackground);
	GameObject* LastFloorAdded;
	GameObject* LastGOAdded = nullptr;
	Enemy * pLastEnemyAdded = nullptr;
	GameObject* WallLimit[2] = { nullptr };
	GameObject* p_FirstWall = LastGOAdded = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0, 150,150 * 30 }, 150, { -640,-720 }, { 150, 150 * 30 });
	LastGOAdded = LevelObjects->AddSpike({ 0,-3900 }, 3000);
	LastGOAdded = LevelObjects->AddSpike({ 0,-500 }, 3000);

	LevelObjects->AddRecoveryPickable({ 5110.00f, 961.00f }  );
	LevelObjects->AddRecoveryPickable({ 8588.00f, 3452.97f } );
	LevelObjects->AddRecoveryPickable({ 10303.00f, 5675.50f });
	LevelObjects->AddRecoveryPickable({ 22213.00f, 7392.97f });
	PickableGems->AddGemsFromFile("data/f_Gems_Level1");
	//BackgroundObjects->AddFloor("data/TEXTURE/bg1.tga", { 0,0,0,0,false,nullptr }, 1280*2.5f, { 0, 550 }, { WINDOW_WIDTH * 500, 720*2.5f }, LEVEL1_ROOM);
	//BackgroundObjects->AddFloor("data/TEXTURE/bg2.tga", { 0,0,0,0,false,nullptr }, 1280, { 0, 6790 }, { WINDOW_WIDTH * 500, 720 }, LEVEL1_ROOM);

	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH,50,false,nullptr }, TILE_SIZE_FLOOR2, { 0, -(WINDOW_HEIGHT / 2) + 120 }, { WINDOW_WIDTH, 270 });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0, 150,150 * 2 }, 150, { 640,-180 }, { 150, 150 * 2 });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH,50,false,nullptr }, TILE_SIZE_FLOOR2, { 1210, -(WINDOW_HEIGHT / 2) + 420 }, { WINDOW_WIDTH, 270 });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,400,50,false,nullptr }, TILE_SIZE_FLOOR2, { 1210, -(WINDOW_HEIGHT / 2) + 720 }, { 400, 270 } );
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,400,50,false,nullptr }, TILE_SIZE_FLOOR2, { 810, -(WINDOW_HEIGHT / 2) + 1020 }, { 400, 270 } );
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,400,50,false,nullptr }, TILE_SIZE_FLOOR2, { 1600, -(WINDOW_HEIGHT / 2) + 1020 }, { 400, 270 });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,400,50,false,nullptr }, TILE_SIZE_FLOOR2, { 1210, -(WINDOW_HEIGHT / 2) + 1320 }, { 400, 270 });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH,50,false,nullptr }, TILE_SIZE_FLOOR2, { 1210, -(WINDOW_HEIGHT / 2) + 1670 }, { WINDOW_WIDTH, 270 });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0, 150,150 * 2 }, 150, { 500, 1386 }, { 150, 150 * 2 } );
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0, 150,150 * 2 }, 150, { 1852, 1386 }, { 150, 150 * 2 });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH,50,false,nullptr }, TILE_SIZE_FLOOR2, { -85,  1620 }, { WINDOW_WIDTH, 270 });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH,50,false,nullptr }, TILE_SIZE_FLOOR2, { 2405, 1620 }, { WINDOW_WIDTH, 270 });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH,50,false,nullptr }, TILE_SIZE_FLOOR2, { 1210, 1970 }, { WINDOW_WIDTH, 270 });
	WallLimit[0] = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0, 150,150 * 50 }, 150, { -798, 3050 + (150 * 15) }, { 150, 150 * 50 });//Left
	WallLimit[1] = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0, 150,150 * 50 }, 150, { 3116, 3050 + (150 * 15) }, { 150, 150 * 50 });//Right
	LevelObjects->AddTeleporter({ 1275, 2335 }, { -100,310 });
	pLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 476.0f, 1985.0f });
	pLastEnemyAdded->SetCanFallFromFloor(true);
	pLastEnemyAdded->SetWallLimits(WallLimit[1], WallLimit[0]);
	pLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 1863.0f, 1696.0f });
	pLastEnemyAdded->SetCanFallFromFloor(true);
	pLastEnemyAdded->SetWallLimits(WallLimit[1], WallLimit[0]);
	LevelObjects->AddPunchStar({ 728.75f, 2200.0f }, { -142.75f, 6996.0f });
	LevelObjects->AddPunchStar({ 1800.0f, 2200.0f }, { 2594.0f, 6996.0f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH / 2,50,false,nullptr }, TILE_SIZE_FLOOR2, { -188.501465f, 6430.250000f }, { WINDOW_WIDTH / 2, 270 });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH,50,false,nullptr },     TILE_SIZE_FLOOR2, { -188.501465f, 6430.250000f }, { WINDOW_WIDTH, 270 });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH,50,false,nullptr },     TILE_SIZE_FLOOR2, { 2510.339844f, 6430.250000f }, { WINDOW_WIDTH, 270 });
	LevelObjects->AddSwitchBulb({ 897.5f, 6505.5f }, SWITCH_L1_No1, false);
	LevelObjects->AddSwitchBulb({ 1568.5f, 6505.5f }, SWITCH_L1_No2, false);
	LevelObjects->AddPunchStar({ 1780.0f, 136.0f }, { 4883.0f, 1150.0f }, STAR_L1_No1);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH,50,false,nullptr }, TILE_SIZE_FLOOR2, { 5180.0f, 970.0f }, { WINDOW_WIDTH, 270 });
	LevelObjects->AddSpike({ 7777,800 }, 55);
	Floors->AddFloor(REGULAR_FLOOR_YELLOW_TEXTURE, { 0,-80,0,50,false,nullptr }, 270, { 8043.0f, 1028.988647f }, { 0, 270 }, FLOOR_LV1_EXPANDABLE1);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH*0.34f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 6038.0f, 1249.521118f }, { WINDOW_WIDTH*0.34f, 270 });
	Floors->AddFloor(REGULAR_FLOOR_YELLOW_TEXTURE, { 0,-80,WINDOW_WIDTH*0.34f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 8494.0f, 1352.5f }, { WINDOW_WIDTH*0.34f, 270 }, FLOOR_LV1_EXPANDABLE2);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH*0.34f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 8494.0f, 1652.5f }, { WINDOW_WIDTH*0.34f, 270 });
	Floors->AddFloor(REGULAR_FLOOR_YELLOW_TEXTURE, { 0,-80,WINDOW_WIDTH,50,false,nullptr }, 270, { 8494.0f, 1962.5f }, { WINDOW_WIDTH, 270 }, FLOOR_LV1_EXPANDABLE3);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH*0.34f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 8494.0f, 2252.5f }, { WINDOW_WIDTH*0.34f, 270 });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0, 150,150 * 105 }, 150, { 10036.0f, 1065.06f }, { 150, 150 * 105 }, WALL_1_UPLEFT);
	Floors->AddFloor(REGULAR_FLOOR_YELLOW_TEXTURE, { 0,-80,WINDOW_WIDTH,50,false,nullptr }, 270, { 8494.0f, 2552.5f }, { WINDOW_WIDTH, 270 },FLOOR_LV1_EXPANDABLE4);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH*0.34f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 8494.0f, 2852.5f }, { WINDOW_WIDTH*0.34f, 270 });
	Floors->AddFloor(REGULAR_FLOOR_YELLOW_TEXTURE, { 0,-80,WINDOW_WIDTH,50,false,nullptr }, 270, { 8494.0f, 3152.5f }, { WINDOW_WIDTH, 270 }, FLOOR_LV1_EXPANDABLE5);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH*0.34f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 8494.0f, 3452.5f }, { WINDOW_WIDTH*0.34f, 270 });
	LevelObjects->AddSwitchBulb({ 6024.0f, 1400.5f }, SWITCH_L1_No3, true);
	LevelObjects->AddSwitchBulb({ 8436.0f, 1100.5f }, SWITCH_L1_No4, true);
	LevelObjects->AddSwitchBulb({ 8436.0f, 1720.5f }, SWITCH_L1_No5, true);
	LevelObjects->AddSwitchBulb({ 8436.0f, 2320.5f }, SWITCH_L1_No6, true);
	LastGOAdded = LevelObjects->AddSwitchBulb({ 7780.0f, 2920.5f }, SWITCH_L1_No7, false);
	LastGOAdded->SetSwitchResistance(4);
	LastGOAdded = LevelObjects->AddSpike({ 8465.0f, 1980.3f }, 4);
	LastGOAdded->SetRotation({ 0,0,180 });
	LevelObjects->AddSpike({ 7886.0f, 2038.05f }, 3);
	LevelObjects->AddSpike({ 9119.0f, 2038.05f }, 3);
	LastGOAdded = LevelObjects->AddSpike({ 8160.0f, 2628.50f }, 12);
	LastGOAdded->SetRotation({ 0,0,180 });
	LastGOAdded = LevelObjects->AddSpike({ 9007.0f, 2628.50f }, 3);
	LastGOAdded->SetRotation({ 0,0,180 });
	LastGOAdded = LevelObjects->AddSpike({ 7860.0f, 3228.50f }, 6);
	LastGOAdded->SetRotation({ 0,0,180 });
	LastGOAdded = LevelObjects->AddSpike({ 8907.0f, 3228.50f }, 12);
	LastGOAdded->SetRotation({ 0,0,180 });
	LevelObjects->AddTeleporter({ 8531.00f, 3813.50f }, { -371.00f, -1294.05f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH * 3,50,false,nullptr }, TILE_SIZE_FLOOR2, { WINDOW_WIDTH, -2244.05f }, { WINDOW_WIDTH * 3, 270 });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0, 150,150 * 1.55f }, 150, { 1599.00f,  -2268.05f + 58 }, { 150, 150 * 1.55f });
	LastGOAdded = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0, 150,150 * 25 }, 150, { 3177.00f, -1968.05f - 450 }, { 150, 150 * 25 });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,155,50,false,nullptr }, TILE_SIZE_FLOOR2, { 1599.00f, -2009.05f }, { 155, 270 });
	pLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 798.00f,  -1968.05f },ENEMY_LV1_01);
	pLastEnemyAdded->SetWallLimits(LastGOAdded, p_FirstWall);
	pLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 97.00f,   -1968.05f },ENEMY_LV1_02);
	pLastEnemyAdded->SetWallLimits(LastGOAdded, p_FirstWall);
	pLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 1369.00f, -1968.05f },ENEMY_LV1_03);
	pLastEnemyAdded->SetWallLimits(LastGOAdded, p_FirstWall);
	pLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 1923.00f, -1968.05f },ENEMY_LV1_04);
	pLastEnemyAdded->SetWallLimits(LastGOAdded, p_FirstWall);
	pLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 2490.00f, -1968.05f },ENEMY_LV1_05);
	pLastEnemyAdded->SetWallLimits(LastGOAdded, p_FirstWall);
	LastGOAdded = LevelObjects->AddTeleporter({ 3025.05f, -1437.82f }, { 10679.00f, 5206.50f }, TELEPORTER_LVL1_01);
	pLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 2269.68f, -2168.05f }, ENEMY_LV1_06);
	pLastEnemyAdded->SetWallLimits(LastGOAdded, p_FirstWall);
	pLastEnemyAdded->SetUse(false);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH,50,false,nullptr }, TILE_SIZE_FLOOR2, { 11024.00f, 5008.50f }, { WINDOW_WIDTH, 270 });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0, 0,0 }, 150, { 10227.00f - 50, 4959.50f + 150 }, { 150, 150 * 5 });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { -150,0, 150 * 3,150 * 5 }, 150, { 10367.00f - 50, 4959.50f + 150 }, { 150, 150 * 5 }, WALL_1_DOWNLEFT);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,300,50,false,nullptr }, TILE_SIZE_FLOOR2, { 10240.0f, 5604.50f }, { 300, 270 } );
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 150,0, 150 * 3,150 * 5 }, 150, { 11768.00f - 30, 4959.50f + 150 }, { 150, 150 * 5 }, WALL_1_DOWNRIGHT);
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,0,0 }, 150, { 11768.00f + 120, 4959.50f + 150 }, { 150, 150 * 5 });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,300,50,false,nullptr }, TILE_SIZE_FLOOR2, { 11815.00f, 5604.50f }, { 300, 270 }, FLOOR_LV1_REGULAR_A);
	pLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 11808.00f, 5649.50f }, ENEMY_LV1_07);
	pLastEnemyAdded->SetCanFallFromFloor(true);
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0, 150,150 * 25 }, 150, { 12038.00f, 5083.57f }, { 150, 150 * 25 }, WALL_1_UPRIGHT);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH * 2,50,false,nullptr }, TILE_SIZE_FLOOR2, { 12038.33f, 6964.90f + 75 }, { WINDOW_WIDTH * 2, 270 });
	LevelObjects->AddPunchStar({ 11834.00f, 5924.50f }, { 11834.00f, 7219.50f });
	LevelObjects->AddSpike({ 10416.80f, 6974.95f }, 8);
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH*0.2f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 10213.00f, 7144.92f }, { WINDOW_WIDTH*0.2f, 270 });
	LevelObjects->AddSpike({ 19823.95f, 6900.50f }, 77 * 2);

	//秘密
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,3135.000f,false,nullptr }, 150.000f, { -6050.000f, -2214.000f }, { 150.000f,3135.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,3135.000f,false,nullptr }, 150.000f, { -5525.000f, -2214.000f }, { 150.000f,3135.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,673.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { -5785.000f, -3689.000f }, { 673.000f,270.000f });
	LevelObjects->AddTeleporter({ -5790.00f, -3609.00f }, { 8489.00f, 3598.00f });
	LevelObjects->AddTeleporter({ 7392.00f, 3468.00f }, { -5823.00f, -952.00f });

	//動ける床の行事
	LastFloorAdded = Floors->AddFloor(REGULAR_FLOOR_RED_TEXTURE, { 0,-80,WINDOW_WIDTH*0.6f,50,false,nullptr }, 270, { 13785.00f + 2000, 7115.58f }, { WINDOW_WIDTH*0.6f, 270 });
	LastFloorAdded->SetUpMoveFloorControlledBySwitch({ 13785.00f,7115.58f }, { 17886.95f,7115.58f }, { 5 ,5 }, { 13074.95f, 7150.66f });
	LastGOAdded = LastFloorAdded->GetControlSwitchForMoveablePlatform();
	if (LastGOAdded)
		LastGOAdded->SetLocalPosition(LastGOAdded->GetLocalPosition().x + 300, LastGOAdded->GetLocalPosition().y);

	//動けるスパイク1
	GameObject* BlockWithSpikes = nullptr;
	BlockWithSpikes = Walls->AddWall(EVIL_BLOCK2_TEXTURE, { 0,0, 150,150 * 1 }, 150, { 15234.95f, 7115.61f }, { 150, 150 * 1 });
	BlockWithSpikes->SetUpMoveControl({ 15234.95f, 7115.61f }, { 15234.95f, 7915.61f }, { 0, 10 });
	LastGOAdded = LevelObjects->AddSpike({ 0,-120 }, 2);
	LastGOAdded->SetRotation({ 0, 0, 180 });
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded = LevelObjects->AddSpike({ 0,120 }, 2);
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded = Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,150,50,false,nullptr }, TILE_SIZE_FLOOR2, { 0, 125 }, { 150, 270 });
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded->SetVisibility(false);

	//動けるスパイク2
	BlockWithSpikes = nullptr;
	BlockWithSpikes = Walls->AddWall(EVIL_BLOCK2_TEXTURE, { 0,0, 150,150 * 1 }, 150, { 16234.95f, 7115.61f }, { 150, 150 * 1 });
	BlockWithSpikes->SetUpMoveControl({ 16234.95f, 7115.61f }, { 16234.95f, 7915.61f }, 12);
	LastGOAdded = LevelObjects->AddSpike({ 0,-120 }, 2);
	LastGOAdded->SetRotation({ 0, 0, 180 });
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded = LevelObjects->AddSpike({ 0,120 }, 2);
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded = Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,150,50,false,nullptr }, TILE_SIZE_FLOOR2, { 0, 125 }, { 150, 270 });
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded->SetVisibility(false);

	//動けるスパイク3
	BlockWithSpikes = nullptr;
	BlockWithSpikes = Walls->AddWall(EVIL_BLOCK2_TEXTURE, { 0,0, 150,150 * 1 }, 150, { 17234.95f, 7115.61f }, { 150, 150 * 1 });
	BlockWithSpikes->SetUpMoveControl({ 17234.95f, 7115.61f }, { 17234.95f, 7915.61f }, 15);
	LastGOAdded = LevelObjects->AddSpike({ 0,-120 }, 2);
	LastGOAdded->SetRotation({ 0, 0, 180 });
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded = LevelObjects->AddSpike({ 0,120 }, 2);
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded = Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,150,50,false,nullptr }, TILE_SIZE_FLOOR2, { 0, 125 }, { 150, 270 });
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded->SetVisibility(false);

	//動けるスパイク4
	BlockWithSpikes = nullptr;
	BlockWithSpikes = Walls->AddWall(EVIL_BLOCK2_TEXTURE, { 0,0, 150,150 * 1 }, 150, { 20234.95f + 1500, 7115.61f }, { 150, 150 * 1 });
	BlockWithSpikes->SetUpMoveControl({ 20234.95f + 1500, 7115.61f }, { 20234.95f + 1500, 7915.61f }, 20);
	LastGOAdded = LevelObjects->AddSpike({ 0,-120 }, 2);
	LastGOAdded->SetRotation({ 0, 0, 180 });
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded = LevelObjects->AddSpike({ 0,120 }, 2);
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded = Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,150,50,false,nullptr }, 270, { 0, 125 }, { 150, 270 });
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded->SetVisibility(false);

	//動けるスパイク5
	BlockWithSpikes = nullptr;
	BlockWithSpikes = Walls->AddWall(EVIL_BLOCK2_TEXTURE, { 0,0, 150,150 * 1 }, 150, { 20734.95f + 1500, 7115.61f }, { 150, 150 * 1 });
	BlockWithSpikes->SetUpMoveControl({ 20734.95f + 1500, 7115.61f }, { 20734.95f + 1500, 7915.61f }, 20);
	LastGOAdded = LevelObjects->AddSpike({ 0,-120 }, 2);
	LastGOAdded->SetRotation({ 0, 0, 180 });
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded = LevelObjects->AddSpike({ 0,120 }, 2);
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded = Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,150,50,false,nullptr }, 270, { 0, 125 }, { 150, 270 });
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded->SetVisibility(false);

	//動けるスパイク6
	BlockWithSpikes = nullptr;
	BlockWithSpikes = Walls->AddWall(EVIL_BLOCK2_TEXTURE, { 0,0, 150,150 * 1 }, 150, { 20734.95f + 2000, 7115.61f }, { 150, 150 * 1 });
	BlockWithSpikes->SetUpMoveControl({ 20734.95f + 2000, 7115.61f }, { 20734.95f + 2000, 7915.61f }, 20);
	LastGOAdded = LevelObjects->AddSpike({ 0,-120 }, 2);
	LastGOAdded->SetRotation({ 0, 0, 180 });
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded = LevelObjects->AddSpike({ 0,120 }, 2);
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded = Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,150,50,false,nullptr }, 270, { 0, 125 }, { 150, 270 });
	LastGOAdded->SetParent(BlockWithSpikes, false);
	LastGOAdded->SetVisibility(false);

	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,2000,50,false,nullptr }, TILE_SIZE_FLOOR2, { 18335.95f, 7195.84f + 250 }, { 2000, 270 });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0, 150,150 * 2 }, 150, { 18335.95f, 7195.84f }, { 150, 150 * 2 });

	Enemies->AddEnemy(BUNBUN_ENEMY, { 18332.95f, 7500.08f }, ENEMY_LV1_05);

	//動ける床2の行事
	LastFloorAdded = Floors->AddFloor(REGULAR_FLOOR_RED_TEXTURE, { 0,-80,WINDOW_WIDTH*0.6f,50,false,nullptr }, 270, { 21775.95f, 7115.58f }, { WINDOW_WIDTH*0.6f, 270 });
	LastFloorAdded->SetUpMoveFloorControlledBySwitch({ 19354.95f,7115.58f }, { 24374.95f,7115.58f }, { 5 ,5 }, { 19200.90f, 7491.08f + 150 });
	LastGOAdded = LastFloorAdded->GetControlSwitchForMoveablePlatform();
	if (LastGOAdded)
		LastGOAdded->SetLocalPosition(LastGOAdded->GetLocalPosition().x + 300, LastGOAdded->GetLocalPosition().y);
	//最後の部分
	LevelObjects->AddTeleporter({ 24437.63f, 7436.24f }, { 23988.00f, 4547.50f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,WINDOW_WIDTH * 2,50,false,nullptr }, TILE_SIZE_FLOOR2, { 25123.00f, 4471.50f }, { WINDOW_WIDTH * 2, 270 }, FLOOR_LV1_REDUCEABLE_FLOOR_1);
	//秘密
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,1242.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 7315.000f, -3669.000f }, { 1242.000f,270.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,2533.000f,false,nullptr }, 150.000f, { 6640.000f, -2659.000f }, { 150.000f,2533.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,2533.000f,false,nullptr }, 150.000f, { 8000.000f, -2654.000f }, { 150.000f,2533.000f });
	LevelObjects->AddSpike({ 7290.000f,-1359.000f }, 21);
	LevelObjects->AddTeleporter({ 10220.00f, 5921.00f }, { 7065.00f, -1859.00f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,478.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 7718.000f, -3358.000f }, { 478.000f,270.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,478.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 6918.000f, -3358.000f }, { 478.000f,270.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,973.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 7323.000f, -3033.000f }, { 973.000f,270.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,693.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 6948.000f, -2723.000f }, { 693.000f,270.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,693.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 7673.000f, -2258.000f }, { 693.000f,270.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,1117.000f,50,false,nullptr },TILE_SIZE_FLOOR2, { 7238.000f, -1928.000f }, { 1117.000f,270.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,132.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 7162.000f, -2432.000f }, { 132.000f,270.000f });
	Enemies->AddEnemy(BUNBUN_ENEMY, { 7765.000f,-2164.000f }, ENEMY_LV1_08);
	Enemies->AddEnemy(BUNBUN_ENEMY, { 7515.000f,-2164.000f }, ENEMY_LV1_09);
	Enemies->AddEnemy(BUNBUN_ENEMY, { 6865.000f,-2614.000f }, ENEMY_LV1_10);
	Enemies->AddEnemy(BUNBUN_ENEMY, { 7240.000f,-2989.000f }, ENEMY_LV1_11);
	Enemies->AddEnemy(BUNBUN_ENEMY, { 6815.000f,-3564.000f }, ENEMY_LV1_12);
	Enemies->AddEnemy(BUNBUN_ENEMY, { 7190.000f,-3564.000f }, ENEMY_LV1_13);
	Enemies->AddEnemy(BUNBUN_ENEMY, { 7715.000f,-3564.000f }, ENEMY_LV1_14);
	LevelObjects->AddTeleporter({ 7788.54f, -1542.18f }, { 10558.54f, 5412.82f }, TELEPORTER_LVL1_02);
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,4528.000f,false,nullptr }, 150.000f, { 26430.000f, 9112.500f }, { 150.000f,4528.000f });
	//秘密2
	LevelObjects->AddTeleporter({ 9918.00f, 1372.88f }, { 11118.00f, -3237.12f });
	LevelObjects->AddTeleporter({ 12333.00f, -2352.12f }, { 5858.00f, 1397.88f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,2675.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 12323.000f, -3332.119f }, { 2675.000f,270.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,2675.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 12323.000f, -3032.119f }, { 2675.000f,270.000f });
	Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,2675.000f,50,false,nullptr }, TILE_SIZE_FLOOR2, { 12323.000f, -2722.119f }, { 2675.000f,270.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,2570.000f,false,nullptr }, 150.000f, { 10968.000f, -2182.119f }, { 150.000f,2570.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,2570.000f,false,nullptr }, 150.000f, { 13713.000f, -2182.119f }, { 150.000f,2570.000f });
	LevelObjects->AddSpike({ 12333.000f,-862.119f }, 33);

	LastGOAdded = LevelObjects->AddSpike({ 18315.000f,11421.000f }, 193);
	LastGOAdded->SetRotation({0, 0, 180});
	//おわり
	LevelObjects->AddSwitchBulb({ 25577.00f, 4772.50f }, SWITCH_L1_No8, true);
	LastGOAdded = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,-80,150,9271.000f,false,nullptr }, 150.000f, { 26457.000f, 3007.500f }, { 150.000f,9271.000f }, WALL_1_FALLING1);
	LastGOAdded->SetVisibility(DEBUG_VISIBILITY);
	LastGOAdded->SetUse(false);
	LastGOAdded = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,-80,150,9271.000f,false,nullptr }, 150.000f, { 23902.000f, 3007.500f }, { 150.000f,9271.000f }, WALL_1_FALLING2);
	LastGOAdded->SetVisibility(DEBUG_VISIBILITY);
	LastGOAdded->SetUse(false);

#ifdef USE_CONSOLE
	printf("LEVEL OBJECTS: %d\n", LevelObjects->GetNumberOfObjects());
	printf("WALLS: %d\n", Walls->GetNumberOfObjects());
	printf("FLOORS: %d\n", Floors->GetNumberOfObjects());
	printf("GEMS: %d\n", PickableGems->GetNumberOfObjects());
#endif
}

void InGame::Level1_2Objects()
{
	GameObject* LastGOAdded = nullptr;
	GameObject* goWallLimitRight = nullptr;
	GameObject* goWallLimitLeft = nullptr;
	GameObject* goLastFloorAdded = nullptr;
	Enemy* enLastEnemyAdded = nullptr;
	LevelObjects->AddSpike({ -1680.000f,-4004.954f }, 784);
	PickableGems->AddGemsFromFile("data/f_Gems_Level1_2");
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,2696.000f,75,false,nullptr }, 270.000f, { 0, 0 }, { 2696.000f,270.000f },  FLOOR_LV1_REGULAR_C);
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,8033.000f,75,false,nullptr }, 270.000f, { 25032.000f, -902.500f }, { 8033.000f,270.000f },  FLOOR_LV1_REGULAR_B);
	LastGOAdded = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,-80,150,9271.000f,false,nullptr }, 150.000f, { 26457.000f, 3007.500f }, { 150.000f,9271.000f }, WALL_1_FALLING1);
	LastGOAdded->SetVisibility(DEBUG_VISIBILITY);
	LastGOAdded = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,-80,150,9271.000f,false,nullptr }, 150.000f, { 23902.000f, 3007.500f }, { 150.000f,9271.000f }, WALL_1_FALLING2);
	LastGOAdded->SetVisibility(DEBUG_VISIBILITY);
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,308.000f,false,nullptr }, 150.000f, { -1408.000f, 38.000f }, { 150.000f,308.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,308.000f,false,nullptr }, 150.000f, { 1417.000f, 38.000f }, { 150.000f,308.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,1064.000f,75,false,nullptr }, 270.000f, { 1871.000f, 298.000f }, { 1064.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,1064.000f,75,false,nullptr }, 270.000f, { -1864.000f, 303.000f }, { 1064.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_YELLOW_TEXTURE, { 0,-80,0,75,false,nullptr }, 270.000f, { -4341.000f, 536.000f }, { 0,270.000f }, FLOOR_LV1_EXPANDABLE6);
	LevelObjects->AddSwitchBulb({ -2326.00f, 636.00f }, SWITCH_L1_No9, true);
	Floors->AddFloor(DOMINOS_FLOOR_YELLOW_TEXTURE, { 0,-80,0,75,false,nullptr }, 270.000f, { -6901.000f, 921.000f }, { 0,270.000f }, FLOOR_LV1_EXPANDABLE7);
	LevelObjects->AddSwitchBulb({ -4700.00f, 605.00f }, SWITCH_L1_No10, true);
	LevelObjects->AddTeleporter({ -10884.00f, 1675.69f }, { -4952.00f, 6577.00f });

	LevelObjects->AddSwitchBulb({ -7686.00f, 1008.88f }, SWITCH_L1_No11, true);
	Floors->AddFloor(DOMINOS_FLOOR_YELLOW_TEXTURE, { 0,-80,5255.000f,75,false,nullptr }, 270.000f, { -9649.000f, 1249.513f }, { 5255.000f,270.000f }, FLOOR_LV1_EXPANDABLE8);

	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,1944.000f,75,false,nullptr }, 270.000f, { 0.000f, 4532.000f }, { 1944.000f,270.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,2418.000f,false,nullptr }, 150.000f, { -955.000f, 5597.000f }, { 150.000f,2418.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,2418.000f,false,nullptr }, 150.000f, { 915.000f, 5597.000f }, { 150.000f,2418.000f });
	
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,571.000f,75,false,nullptr }, 270.000f, { 627.000f, 4861.000f }, { 571.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,571.000f,75,false,nullptr }, 270.000f, { -748.000f, 4861.000f }, { 571.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,571.000f,75,false,nullptr }, 270.000f, { -748.000f, 5241.000f }, { 571.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,571.000f,75,false,nullptr }, 270.000f, { 702.000f, 5241.000f }, { 571.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,571.000f,75,false,nullptr }, 270.000f, { -538.000f, 4846.000f }, { 571.000f,270.000f });
	LevelObjects->AddRecoveryPickable({ 732.00f, 5306.00f });
	LevelObjects->AddTeleporter({ 762.00f, 5536.00f }, { 0,100 });
	LevelObjects->AddTeleporter({ 1572.99f, -175.79f }, { -135.00f, 5122.00f });

	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,2235.000f,false,nullptr }, 150.000f, { 2670.000f, -879.000f }, { 150.000f,2235.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,2235.000f,false,nullptr }, 150.000f, { 3235.000f, -879.000f }, { 150.000f,2235.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,2400.000f,75,false,nullptr }, 270.000f, { 3900.000f, -2369.000f }, { 2400.000f,270.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,540.000f,false,nullptr }, 150.000f, { 2670.000f, -2264.000f }, { 150.000f,540.000f });
	Enemies->AddEnemy(BUNBUN_ENEMY, { 3600.000f,-2184.000f }, 1);
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,180.000f,75,false,nullptr }, 270.000f, { 2674.000f, 311.000f }, { 180.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,180.000f,75,false,nullptr }, 270.000f, { 3239.000f, 311.000f }, { 180.000f,270.000f });
	LevelObjects->AddSpike({ 3724.000f,276.000f }, 10); 
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,370.000f,75,false,nullptr }, 270.000f, { 4229.000f, 446.000f }, { 370.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,64.000f,75,false,nullptr }, 270.000f, { 3829.000f, 431.000f }, { 64.000f,270.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,1880.000f,false,nullptr }, 150.000f, { 4449.000f, 1266.000f }, { 150.000f,1880.000f });
	//秘密1
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,2150.000f,75,false,nullptr }, 270.000f, { -4911.000f, 4551.000f }, { 2150.000f,270.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,4475.000f,false,nullptr }, 150.000f, { -6017.000f, 6641.000f }, { 150.000f,4475.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,4475.000f,false,nullptr }, 150.000f, { -3902.000f, 6641.000f }, { 150.000f,4475.000f });
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,150.000f,false,nullptr }, 150.000f, { 2672.175f, -2521.343f }, { 150.000f,150.000f });
	LevelObjects->AddSpike({ -4061.000f,4536.000f }, 2); 
	LevelObjects->AddSpike({ -5861.000f,4536.000f }, 2); 
	LevelObjects->AddSpike({ -4954.624f,8917.000f }, 27);
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,1052.000f,75,false,nullptr }, 270.000f, { -4926.000f, 4776.000f }, { 1052.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,1052.000f,75,false,nullptr }, 270.000f, { -4951.000f, 5571.000f }, { 1052.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,347.000f,75,false,nullptr }, 270.000f, { -5876.000f, 4976.000f }, { 347.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,347.000f,75,false,nullptr }, 270.000f, { -4026.000f, 4976.000f }, { 347.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,347.000f,75,false,nullptr }, 270.000f, { -4306.000f, 5281.000f }, { 347.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,347.000f,75,false,nullptr }, 270.000f, { -5621.000f, 5281.000f }, { 347.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,249.000f,75,false,nullptr }, 270.000f, { -5502.306f, 5864.347f }, { 249.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,249.000f,75,false,nullptr }, 270.000f, { -4352.306f, 5864.347f }, { 249.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,1037.000f,75,false,nullptr }, 270.000f, { -4377.306f, 6164.347f }, { 1037.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,1037.000f,75,false,nullptr }, 270.000f, { -5552.306f, 6164.347f }, { 1037.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,165.000f,75,false,nullptr }, 270.000f, { -4961.974f, 6432.000f }, { 165.000f,270.000f });
	Enemies->AddEnemy(BUNBUN_ENEMY, { -4943.000f,5731.000f }, ENEMY_LV1_15);
	Enemies->AddEnemy(BUNBUN_ENEMY, { -4943.000f,5021.000f }, ENEMY_LV1_16);
	Enemies->AddEnemy(BUNBUN_ENEMY, { -4418.000f,4646.000f }, ENEMY_LV1_17);
	Enemies->AddEnemy(BUNBUN_ENEMY, { -5543.000f,4646.000f }, ENEMY_LV1_18);
	Enemies->AddEnemy(BUNBUN_ENEMY, { -4593.000f,5746.000f }, ENEMY_LV1_19);
	Enemies->AddEnemy(BUNBUN_ENEMY, { -5507.000f,6356.000f }, ENEMY_LV1_20);
	Enemies->AddEnemy(BUNBUN_ENEMY, { -4582.000f,6356.000f }, ENEMY_LV1_21);
	Enemies->AddEnemy(BUNBUN_ENEMY, { -5807.000f,6356.000f }, ENEMY_LV1_22);
	Enemies->AddEnemy(BUNBUN_ENEMY, { -4232.000f,6356.000f }, ENEMY_LV1_23);
	LevelObjects->AddTeleporter({ -4979.81f, 6667.00f }, { 0,100 }, TELEPORTER_LVL1_03);
	LevelObjects->AddTeleporter({ 11046.00f, -1824.00f }, { 8736.00f, -549.00f });
	//下
	LevelObjects->AddPunchStar({ 3775.45f, -2101.01f }, { 3775.45f, -716.01f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,1040.000f,75,false,nullptr }, 270.000f, { 5363.529f, -1996.207f }, { 1040.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,1040.000f,75,false,nullptr }, 270.000f, { 6138.529f, -1646.207f }, { 1040.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,1040.000f,75,false,nullptr }, 270.000f, { 7013.529f, -1346.207f }, { 1040.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,3890.000f,75,false,nullptr }, 270.000f, { 9263.529f, -1021.207f }, { 3890.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,1040.000f,75,false,nullptr }, 270.000f, { 5363.529f, -1996.207f }, { 1040.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,1040.000f,75,false,nullptr }, 270.000f, { 6138.529f, -1646.207f }, { 1040.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,1040.000f,75,false,nullptr }, 270.000f, { 7013.529f, -1346.207f }, { 1040.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,3890.000f,75,false,nullptr }, 270.000f, { 9263.529f, -1021.207f }, { 3890.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,480.000f,75,false,nullptr }, 270.000f, { 10969.529f, -773.207f }, { 480.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,480.000f,75,false,nullptr }, 270.000f, { 10429.529f, -573.207f }, { 480.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,480.000f,75,false,nullptr }, 270.000f, { 10979.529f, -358.207f }, { 480.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,480.000f,75,false,nullptr }, 270.000f, { 10454.529f, -138.207f }, { 480.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,480.000f,75,false,nullptr }, 270.000f, { 10939.529f, 111.793f }, { 480.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,480.000f,75,false,nullptr }, 270.000f, { 10409.529f, 311.793f }, { 480.000f,270.000f });

	goLastFloorAdded = Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,6075.000f+200,75,false,nullptr }, 270.000f, { 13339.529f, 636.793f }, { 6075.000f+200,270.000f });//敵と戦う床
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,2790.000f,false,nullptr }, 150.000f, { 11259.529f, -808.207f }, { 150.000f,2790.000f });
	LastGOAdded = LevelObjects->AddSpike({ 10161.000f,-1192.946f }, 25);
	LastGOAdded->SetRotation({ 0, 0, 180 });
	goWallLimitLeft = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,4347.000f,false,nullptr }, 150.000f, { 10150.525f, 1492.000f }, { 150.000f,4347.000f });
	LastGOAdded = LevelObjects->AddSwitchBulb({ 8335.92f, -718.44f }, SWITCH_L1_No12, true);	
	LastGOAdded->SetSwitchAsScaleInverter();
	LastGOAdded = LevelObjects->AddSwitchBulb({ 12548.00f, 2064.50f }, SWITCH_L1_No13, true);	
	LastGOAdded->SetSwitchAsScaleInverter();
	LastGOAdded = LevelObjects->AddSwitchBulb({ 15678.00f, 1679.50f }, SWITCH_L1_No14, true);	
	LastGOAdded->SetSwitchAsScaleInverter();
	LastGOAdded = LevelObjects->AddSwitchBulb({ 20954.90f, -431.27f }, SWITCH_L1_No14, true);
	LastGOAdded->SetSwitchAsScaleInverter();

	LastGOAdded = Walls->AddWall(EVIL_BLOCK3_TEXTURE, { 0,0,150,5865.000f,false,nullptr }, 150.000f, { 9030.355f, 741.310f }, { 150.000f,5865.000f });
	LastGOAdded->SetWallDeactivateableWhenScaleIsNegative(true);
	LevelObjects->AddSpike({ 14424.025f,2791.728f }, 40); 
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,995.000f,75,false,nullptr }, 270.000f, { 11500.000f, 971.000f }, { 995.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,995.000f,75,false,nullptr }, 270.000f, { 11500.000f, 1271.000f }, { 995.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,995.000f,75,false,nullptr }, 270.000f, { 11500.000f, 1571.000f }, { 995.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,275.000f,75,false,nullptr }, 270.000f, { 12350.000f, 1796.000f }, { 275.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,275.000f,75,false,nullptr }, 270.000f, { 10700.000f, 1846.000f }, { 275.000f,270.000f });
	goWallLimitRight = Walls->AddWall(EVIL_BLOCK3_TEXTURE, { 0,0,150,2270.000f,false,nullptr }, 150.000f, { 12824.025f, 1606.728f }, { 150.000f,2270.000f });
	goWallLimitRight->SetWallDeactivateableWhenScaleIsPositive(true);
	LastGOAdded = Walls->AddWall(EVIL_BLOCK3_TEXTURE, { 0,0,150,2270.000f,false,nullptr }, 150.000f, { 16044.025f, 1606.728f }, { 150.000f,2270.000f });
	LastGOAdded->SetWallDeactivateableWhenScaleIsNegative(true);
	LevelObjects->AddRecoveryPickable({ 10695.00f, 1981.00f });

	enLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 11490.000f,1386.000f }, ENEMY_LV1_24);
	enLastEnemyAdded->SetWallLimits(goWallLimitRight, goWallLimitLeft);
	enLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 11840.000f,1716.000f }, ENEMY_LV1_25);
	enLastEnemyAdded->SetWallLimits(goWallLimitRight, goWallLimitLeft);
	enLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 12299.025f,866.728f } , ENEMY_LV1_26);
	enLastEnemyAdded->SetWallLimits(goWallLimitRight, goWallLimitLeft);		   
	enLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 11774.025f,866.728f } , ENEMY_LV1_27);
	enLastEnemyAdded->SetWallLimits(goWallLimitRight, goWallLimitLeft);		   
	enLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 11374.025f,866.728f } , ENEMY_LV1_28);
	enLastEnemyAdded->SetWallLimits(goWallLimitRight, goWallLimitLeft);

	enLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 13224.025f,791.728f }, ENEMY_LV1_29);
	enLastEnemyAdded->SetWallLimits(LastGOAdded, goWallLimitRight);
	enLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 13624.025f,791.728f }, ENEMY_LV1_30);
	enLastEnemyAdded->SetWallLimits(LastGOAdded, goWallLimitRight);
	enLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 14024.025f,791.728f }, ENEMY_LV1_31);
	enLastEnemyAdded->SetWallLimits(LastGOAdded, goWallLimitRight);


	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,885.000f,75,false,nullptr }, 270.000f, { 13894.025f, 1046.728f }, { 885.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,885.000f,75,false,nullptr }, 270.000f, { 14944.025f, 1471.728f }, { 885.000f,270.000f });

	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,885.000f,75,false,nullptr }, 270.000f, { 13894.025f, 1046.728f }, { 885.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,885.000f,75,false,nullptr }, 270.000f, { 14944.025f, 1471.728f }, { 885.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,885.000f,75,false,nullptr }, 270.000f, { 17089.025f, 276.728f }, { 885.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,885.000f,75,false,nullptr }, 270.000f, { 17989.025f, -223.272f }, { 885.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,885.000f,75,false,nullptr }, 270.000f, { 18864.025f, -723.272f }, { 885.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,885.000f,75,false,nullptr }, 270.000f, { 19939.025f, -723.272f }, { 885.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,1217.000f,75,false,nullptr }, 270.000f, { 21180.000f, -724.000f }, { 1217.000f,270.000f });

	LastGOAdded = Walls->AddWall(EVIL_BLOCK3_TEXTURE, { 0,0,150,5465.000f,false,nullptr }, 150.000f, { 21245.000f, 1391.000f }, { 150.000f,5465.000f });
	LastGOAdded->SetWallDeactivateableWhenScaleIsPositive(true);
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,725.000f,75,false,nullptr }, 270.000f, { 22120.000f, -449.000f }, { 725.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,725.000f,75,false,nullptr }, 270.000f, { 22695.000f, -149.000f }, { 725.000f,270.000f });

	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,2360.000f,75,false,nullptr }, 270.000f, { 10152.000f, -2089.000f }, { 2360.000f,270.000f });
	LastGOAdded = LevelObjects->AddSpike({ 10157.000f,-2249.000f }, 28);
	LastGOAdded->SetRotation({ 0, 0, 180 });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,319.000f,75,false,nullptr }, 270.000f, { 13151.750f, 975.000f }, { 319.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,319.000f,75,false,nullptr }, 270.000f, { 14339.000f, 1260.000f }, { 319.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,125.000f,75,false,nullptr }, 270.000f, { 17648.572f, 55.047f }, { 125.000f,270.000f });
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,125.000f,75,false,nullptr }, 270.000f, { 18543.572f, -459.953f }, { 125.000f,270.000f });
	//終わり
	LevelObjects->AddEndPoint({ 22999.03f, 179.18f });

#ifdef USE_CONSOLE
	printf("LEVEL OBJECTS: %d\n", LevelObjects->GetNumberOfObjects());
	printf("WALLS: %d\n", Walls->GetNumberOfObjects());
	printf("FLOORS: %d\n", Floors->GetNumberOfObjects());
	printf("GEMS: %d\n", PickableGems->GetNumberOfObjects());
#endif
}

void InGame::DebugRoomObjects()
{
	Enemy * pLastEnemyAdded = nullptr;
	SetBackgroundColor(ColorBackground);
	//LevelObjects->AddSpike({ 0,-900 }, 300);
	Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-60,WINDOW_WIDTH * 8,50,false,nullptr }, 270, { 0, -(WINDOW_HEIGHT / 2) + 100 }, { WINDOW_WIDTH * 8, 270 }, DEBUG_ROOM);
	/*Floors->AddFloor(DEBUG_ROOM_TEXTURE, { 0,-60,1000,50,false,nullptr }, 270, { 0, -(WINDOW_HEIGHT / 2) + 4800 }, { 1000, 270 }, DEBUG_ROOM);
	Floors->AddFloor(DEBUG_ROOM_TEXTURE, { 0,-60,500,50,false,nullptr }, 270, { 0, -(WINDOW_HEIGHT / 2) + 400 }, { 500, 270 }, DEBUG_ROOM);
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0, 150,150 * 7 }, 150, { 420,4950 }, { 150, 150 * 7 }, DEBUG_ROOM);
	Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0, 150,150 * 7 }, 150, { -420,4950 }, { 150, 150 * 7 }, DEBUG_ROOM);
	pLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { 500,150 }, DEBUG_ROOM);
	pLastEnemyAdded->SetCanFallFromFloor(true);
	pLastEnemyAdded = Enemies->AddEnemy(BUNBUN_ENEMY, { -500,0 }, DEBUG_ROOM);
	pLastEnemyAdded->SetCanFallFromFloor(true);
	LevelObjects->AddTeleporter({ 500,310 }, { 0,5000 });
	LevelObjects->AddTeleporter({ 0,4900 }, { 0,310 });
	LevelObjects->AddPunchStar({ 0,250 }, { 0, 5000 });*/
}

void InGame::Update()
{
	UpdateUIManager();
	if (bGameIsPaused) {
		UpdatePauseScreen();
		return;
	}
	if (IsResultScreenUp()) {
		if (GetInput(MOVE_JUMP))
			SetStickerTransition(TITLE_SCREEN_STATE);
		return;
	}
	UpdateCamera();
	g_Player->Update();
	Enemies->Update();
	Floors->Update();
	Walls->Update();
	PickableGems->Update();
	RankManager::Update();
	LevelObjects->Update();
	EventHandler::Update();
	BackgroundObjects->Update();
	if (g_Player->IsPlayerDead()) {
		if (!bSetDeathScore) {//死んだ時、スコアを保存する
			AddScoreToTopRankings(GetScore(), GetCurrentLevel());
			bSetDeathScore = true;
		}
		SetNoiseTransitionToNewState(GAME_OVER_STATE);
	}
}

void InGame::Draw()
{
	BackgroundObjects->Draw();
	Floors->Draw();
	Enemies->Draw();
	Walls->Draw();
	LevelObjects->Draw();
	g_Player->Draw();
	PickableGems->Draw();
	DrawUIManager();
	if(bGameIsPaused)
		DrawPauseScreen();
	EventHandler::Draw();
}

void InGame::End()
{
	if (g_Player) {
#ifdef USE_CONSOLE
		printf("ENDING PLAYER\n");
#endif // USE_CONSOLE
		delete(g_Player);
		g_Player = nullptr;
	}

	if (Floors) {
#ifdef USE_CONSOLE
		printf("ENDING FLOOR\n");
#endif // USE_CONSOLE
		delete(Floors);
		Floors = nullptr;
	}

	if (Enemies) {
#ifdef USE_CONSOLE
		printf("ENDING ENEMIES\n");
#endif // USE_CONSOLE
		delete(Enemies);
		Enemies = nullptr;
	}

	if (LevelObjects) {
#ifdef USE_CONSOLE
		printf("ENDING LEVELOBJECTS\n");
#endif // USE_CONSOLE
		LevelObjects->End();
		delete(LevelObjects);
		LevelObjects = nullptr;
	}

	if (Walls) {
#ifdef USE_CONSOLE
		printf("ENDING WALLS\n");
#endif // USE_CONSOLE
		Walls->End();
		delete(Walls);
		Walls = nullptr;
	}

	if (BackgroundObjects) {
#ifdef USE_CONSOLE
		printf("ENDING BACKGROUNDOBJECTS\n");
#endif // USE_CONSOLE
		delete(BackgroundObjects);
		BackgroundObjects = nullptr;
	}
	
	if (PickableGems) {
#ifdef USE_CONSOLE
		printf("ENDING PICKABLES\n");
#endif // USE_CONSOLE
		delete(PickableGems);
		PickableGems = nullptr;
	}
	//カメラのリセット
	SetFocalPoint(nullptr);
	ResetCameraToOrigin();
	EndUIManager();
	EndPauseScreen();
	EventHandler::End();
}

void SetSubLevel(int SubLevel)
{
	if (Enemies) {
		Enemies->End();
		delete(Enemies);
		Enemies = new Enemy_List();
	}
	EndEnemyTextures();
	if (Floors)
		Floors->End(true);
	EndFloorTextures();
	if (Walls)
		Walls->End(true);
	EndWallTextures();
	if (LevelObjects)
		LevelObjects->End(true);
	if (PickableGems)
		PickableGems->End(true);
	EndLevelObjectsTextures();
	nCurrent_SubLevel = SubLevel;
	switch (SubLevel)
	{
	case TUTORIAL_R:
		InGame::TutorialObjects();
		break;
	case SUBLEVEL1_1:
		InGame::Level1_1Objects();
		break;
	case SUBLEVEL1_2:
		InGame::Level1_2Objects();
		break;
	default:
		break;
	}
	SearchForLevelObjects();
}

void SetLevel(int newLevel)
{
	nCurrentLevel = newLevel;
}

void AddToScore(int nToAdd)
{
	nScore += nToAdd;
}

void AddToScoreWithMultiplier(int nToAdd)
{
	nScore += (nToAdd*GetRank());
	RankedEvent* pCurrentEvent = GetCurrentActiveEvent();
	if (pCurrentEvent)
		pCurrentEvent->AddComboScore(nToAdd*GetRank());
}

void AddGem(int newGemNum)
{
	nGems += newGemNum;
}

void AddMaxGemNum(int newGemNum)
{
	nMaxGems += newGemNum;
}

void SetLevelScale(float newScale)
{
	fLevelScale = newScale;
	SetUniversalScale(newScale);
}

void PauseGame()
{
	PlaySound(SOUND_LABEL_SE_PASTE_STICKER);
	bGameIsPaused = true;
}

void UnpauseGame()
{
	PlaySound(SOUND_LABEL_SE_REMOVE_STICKER);
	bGameIsPaused = false;
}

float GetLevelScale()
{
	return fLevelScale;
}

int GetScore()
{
	return nScore;
}

int GetGems()
{
	return nGems;
}

int GetCurrentLevel()
{
	return nCurrentLevel;
}

int GetCurrentSubLevel()
{
	return nCurrent_SubLevel;
}

bool IsGamePaused()
{
	return bGameIsPaused;
}

Color GetCurrentBackgroundColor()
{
	return ColorBackground;
}

Go_List * GetFloors()
{
	return Floors;
}

Go_List * GetWalls()
{
	return Walls;
}

Go_List * GetLevelObjects()
{
	return LevelObjects;
}

Go_List * GetPickableGems()
{
	return PickableGems;
}

Enemy_List * GetEnemies()
{
	return Enemies;
}
