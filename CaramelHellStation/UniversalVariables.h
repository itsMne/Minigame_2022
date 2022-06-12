//*****************************************************************************
// UniversalVariables.h
// 環境の変数
// Author : Mane
//*****************************************************************************
#ifndef UNIVERSALVARIABLES_H
#define UNIVERSALVARIABLES_H

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define WINDOW_WIDTH			1280// ウインドウの幅
#define WINDOW_HEIGHT			720	// ウインドウの高さ
#define DEBUG_MODE				true
#define USE_FLOWER_SOUND		true
#define COMMON_UNIVERSAL_SCALE	1.0f
#define NO_DYNAMIC_ZOOM			false
#define TILE_SIZE_FLOOR2 320
#if DEBUG_MODE
//#define USE_CONSOLE
#define SHOW_HITBOXES			false
#define SKIP_LOGOS				true
#define DEBUG_ROOM_ACTIVE		true
#define DEBUG_IFRAMES_JUMP		false
#define DEBUG_OFFSET			false
#define USE_RANK_COLOR			true

#else
#define SHOW_HITBOXES			false
#define SKIP_LOGOS				true
#define DEBUG_ROOM_ACTIVE		false
#define DEBUG_IFRAMES_JUMP		false
#define DEBUG_OFFSET			false
#define USE_RANK_COLOR			true
#endif

//*****************************************************************************
// エナム定義
//*****************************************************************************
enum ROOMS
{
	DEBUG_ROOM = 0,
	TUTORIAL_ROOM,
	LEVEL1_ROOM,
	MAX_ROOMS
};

enum CurrentGameState
{
	LOGOS_STATE = 0,
	TITLE_SCREEN_STATE,
	MAIN_MENU,
	INGAME_STATE,
	GAME_OVER_STATE,
	MAX_GAME_STATE
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct tUV
{
	float U;
	float V;
};

bool IsNoStyleNoGameOn();
void SetNoStyleNoGame(bool nsng);
void SetShowFPS(bool show);
void SetHellsMode(bool set);
bool IsHellsTouchMode();
bool ShowFPS();

#endif // !UNIVERSALVARIABLES_H
