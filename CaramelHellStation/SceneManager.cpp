//*****************************************************************************
// SceneManager.cpp
// �Q�[���̏󋵂��Ǘ�����
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
// �O���[�o���ϐ�
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
//InitScenes�֐�
//�������֐�
//�����Fvoid
//�߁Fvoid
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
//UpdateScenes�֐�
//�ύX�֐�
//�����Fvoid
//�߁Fvoid
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
//SetNewState�֐�
//�V�����V�[���̏�Ԃ�ݒ肷��
//�����Fint
//�߁Fvoid
//*****************************************************************************
void SetNewState(int newState)
{
	EndScenes();
	SAFE_DELETE(pCurrentScene);
	nCurrentGameState = newState;
	InitScenes();
	
}

//*****************************************************************************
//DrawScenes�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void DrawScenes()
{
	if (!pCurrentScene)
		return;
	pCurrentScene->Draw();
	
	DebugLogTest.DrawDebugProc();
}

//*****************************************************************************
//EndScenes�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void EndScenes()
{
	SAFE_DELETE(pCurrentScene);
}

//*****************************************************************************
//SetFPS�֐�
//FPS��ݒ肷��
//�����Fint
//�߁Fvoid
//*****************************************************************************
void SetFPS(int newFPS)
{
	g_nFPS = newFPS;
}

//*****************************************************************************
//GetCurrentState�֐�
//��Ԃ�߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int GetCurrentState()
{
	return nCurrentGameState;
}

//*****************************************************************************
//SetGameLevel�֐�
//�Q�[�����x����ݒ肷��
//�����Fvoid
//�߁Fint
//*****************************************************************************
void SetGameLevel(int Level)
{
	nLevel = Level;
}

