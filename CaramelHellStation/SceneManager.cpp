//*****************************************************************************
// SceneManager.cpp
// ゲームの状況を管理する
// Author : Mane
//*****************************************************************************
#include "SceneManager.h"
#include "debugproc.h"
#include "TransitionControl.h"
#include "SceneGameOver.h"
#include "SceneInGame.h"
#include "SceneLogos.h"
#include "SceneMainMenu.h"
#include "SceneTitleScreen.h"

//*****************************************************************************
// グローバル変数
//*****************************************************************************
int			g_nFPS;
int			nLevel = 0;
#if SKIP_LOGOS
int			nCurrentGameState = TITLE_SCREEN_STATE;
#else
int			nCurrentGameState = LOGOS_STATE;
#endif
SceneBase* pCurrentScene = nullptr;
cDebugProc  DebugLogTest;

//*****************************************************************************
//InitScenes関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void InitScenes()
{	
	DebugLogTest.InitDebugProc();
	EndTextures();
	if (pCurrentScene)
		return;
	switch (nCurrentGameState)
	{
	case LOGOS_STATE:
		pCurrentScene = new SceneLogos();
		break;
	case TITLE_SCREEN_STATE:
		pCurrentScene = new SceneTitleScreen();
		break;
	case INGAME_STATE:
		pCurrentScene = new SceneInGame(nLevel);
		break;
	case MAIN_MENU:
		pCurrentScene = new SceneMainMenu();
		break;
	case GAME_OVER_STATE:
		pCurrentScene = new SceneGameOver();
		break;
	default:
		break;
	}
	
}

//*****************************************************************************
//UpdateScenes関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void UpdateScenes()
{
	if (!IsTransitionOver())
		return;
	if (!pCurrentScene)
		return;
	pCurrentScene->Update();
	
	DebugLogTest.StartDebugProc();
	DebugLogTest.UpdateDebugProc();
	if(ShowFPS())
		DebugLogTest.PrintDebugProc("FPS: %d", g_nFPS);
#if SHOW_FPS
	DebugLogTest.PrintDebugProc("\nGP51A176-02", g_nFPS);
	DebugLogTest.PrintDebugProc("\nGO: %d", GetNumOfGameObjectsSetInMemory());
	DebugLogTest.PrintDebugProc("\nENEMIES: %d", GetEnemyCountInMemory());
	DebugLogTest.PrintDebugProc("\nPOLYGONS: %d", GetSpriteNumInMemory());
	DebugLogTest.PrintDebugProc("\nTEXTURES: %d", GetTextureCountInMemory());
#endif
}

//*****************************************************************************
//SetNewState関数
//新しいシーンの状態を設定する
//引数：int
//戻：void
//*****************************************************************************
void SetNewState(int newState)
{
	EndScenes();
	SAFE_DELETE(pCurrentScene);
	nCurrentGameState = newState;
	InitScenes();
	
}

//*****************************************************************************
//DrawScenes関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void DrawScenes()
{
	if (!pCurrentScene)
		return;
	pCurrentScene->Draw();
	
	DebugLogTest.DrawDebugProc();
}

//*****************************************************************************
//EndScenes関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void EndScenes()
{
	SAFE_DELETE(pCurrentScene);
}

//*****************************************************************************
//SetFPS関数
//FPSを設定する
//引数：int
//戻：void
//*****************************************************************************
void SetFPS(int newFPS)
{
	g_nFPS = newFPS;
}

//*****************************************************************************
//GetCurrentState関数
//状態を戻す
//引数：void
//戻：int
//*****************************************************************************
int GetCurrentState()
{
	return nCurrentGameState;
}

//*****************************************************************************
//SetGameLevel関数
//ゲームレベルを設定する
//引数：void
//戻：int
//*****************************************************************************
void SetGameLevel(int Level)
{
	nLevel = Level;
}

