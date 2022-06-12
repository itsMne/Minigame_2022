//*****************************************************************************
// PauseMenuControl.cpp
// ポースメニューを管理する
// Author : Mane
//*****************************************************************************
#include "PauseMenuControl.h"
#include "SceneInGame.h"
#include "GameInputManager.h"
#include "TransitionControl.h"
#include "SceneManager.h"
#include "UI_Object.h"
#include "sound.h"

//*****************************************************************************
// エナム
//*****************************************************************************
enum PAUSE_MENU_SELECTIONS
{
	PM_CONTINUE = 0,
	PM_CONTROLS,
	PM_COMMANDS,
	PM_BACKTOTITLE,
	PM_MAX
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
UI_Object*	p_UIPauseScreen = nullptr;
UI_Object*  p_UIContinue = nullptr;
UI_Object*  p_UIControls = nullptr;
UI_Object*  p_UICommands = nullptr;
UI_Object*  p_UIBackToTitle = nullptr;
int nSelection;
bool bShowControls;
bool bShowCommands;

//*****************************************************************************
//InitPauseScreen関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void InitPauseScreen()
{
	bShowControls = bShowCommands = false;
	nSelection = PM_CONTINUE;
	if (!p_UIPauseScreen)
		p_UIPauseScreen = new UI_Object();
	if (!p_UIContinue)
		p_UIContinue = new UI_Object();	
	if (!p_UIControls)
		p_UIControls = new UI_Object();
	if (!p_UICommands)
		p_UICommands = new UI_Object();	
	if (!p_UIBackToTitle)
		 p_UIBackToTitle = new UI_Object();
	p_UIPauseScreen->Init(UI_PAUSE_SCREEN);
	p_UIContinue->Init(UI_PAUSE_MENU_OPTIONS_CONTINUE);
	p_UIControls->Init(UI_PAUSE_MENU_OPTIONS_CONTROLS);
	p_UICommands->Init(UI_PAUSE_MENU_OPTIONS_COMMANDS);
	p_UIBackToTitle->Init(UI_PAUSE_MENU_OPTIONS_RETURNTITLE);
}

//*****************************************************************************
//UpdatePauseScreen関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void UpdatePauseScreen()
{
	if (p_UIPauseScreen)
		p_UIPauseScreen->Update();
	if (p_UIContinue) 
	{
		p_UIContinue->Update();
		p_UIContinue->SetAsSelectedOption(nSelection == PM_CONTINUE);
	}
	if (p_UIControls) 
	{
		p_UIControls->Update();
		p_UIControls->SetAsSelectedOption(nSelection == PM_CONTROLS);
	}
	if (p_UICommands)
	{
		p_UICommands->Update();
		p_UICommands->SetAsSelectedOption(nSelection == PM_COMMANDS);
	}
	if (p_UIBackToTitle) 
	{
		p_UIBackToTitle->Update();
		p_UIBackToTitle->SetAsSelectedOption(nSelection == PM_BACKTOTITLE);
	}
	if (GetInput(INPUT_PAUSE)) {
		if (nSelection == PM_BACKTOTITLE) {
			SetStickerTransition(TITLE_SCREEN_STATE);
			return;
		}
		nSelection = PM_CONTINUE;
		bShowControls = bShowCommands = false;
		SceneInGame* pCurrentGame = GetCurrentGame();
		if(pCurrentGame)
			pCurrentGame->UnpauseGame();
		return;
	}
	if (bShowControls)
	{
		if (GetInput(INPUT_CONFIRM)) {
			bShowControls = false;
			p_UIPauseScreen->SetPauseInstructions(IT_MAX);
		}
		return;
	}
	if (bShowCommands)
	{
		if (GetInput(INPUT_CONFIRM)) {
			bShowCommands = false;
			p_UIPauseScreen->SetPauseInstructions(IT_MAX);
		}
		return;
	}
	if (GetInput(INPUT_DOWN))
		nSelection++;
	if (GetInput(INPUT_UP))
		nSelection--;
	if (nSelection < 0)
		nSelection = PM_MAX - 1;
	if (nSelection == PM_MAX)
		nSelection = 0;
	switch (nSelection)
	{
	case PM_CONTINUE:
		if (GetInput(INPUT_CONFIRM)) {
			SceneInGame* pCurrentGame = GetCurrentGame();
			if (pCurrentGame)
				pCurrentGame->UnpauseGame();
		}
		p_UIPauseScreen->SetPauseInstructions(IT_MAX);
		break;
	case PM_CONTROLS:
		if (GetInput(INPUT_CONFIRM)) {
			PlaySound(SOUND_LABEL_SE_ON);
			p_UIPauseScreen->SetPauseInstructions(IT_CONTROLS);
			bShowControls = true;
			return;
		}
		break;
	case PM_COMMANDS:
		if (GetInput(INPUT_CONFIRM)) {
			PlaySound(SOUND_LABEL_SE_ON);
			p_UIPauseScreen->SetPauseInstructions(IT_COMBOS);
			bShowCommands = true;
			return;
		}
		break;
	case PM_BACKTOTITLE:
		if (GetInput(INPUT_CONFIRM)) {
			SetStickerTransition(TITLE_SCREEN_STATE);
			return;
		}
		break;
	default:
		break;
	}
}

//*****************************************************************************
//DrawPauseScreen関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void DrawPauseScreen()
{
	if (p_UIPauseScreen)
		p_UIPauseScreen->Draw();
	if (bShowControls || bShowCommands)
		return;
	if (p_UIContinue)
		p_UIContinue->Draw();
	if (p_UIControls)
		p_UIControls->Draw();	
	if (p_UICommands)
		p_UICommands->Draw();
	if (p_UIBackToTitle)
		p_UIBackToTitle->Draw();
}

//*****************************************************************************
//DrawPauseScreen関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void EndPauseScreen()
{
	if (p_UIPauseScreen)
	{
		p_UIPauseScreen->End();
		delete(p_UIPauseScreen);
		p_UIPauseScreen = nullptr;
	}
	if (p_UIContinue)
	{
		p_UIContinue->End();
		delete(p_UIContinue);
		p_UIContinue = nullptr;
	}
	if (p_UIControls)
	{
		p_UIControls->End();
		delete(p_UIControls);
		p_UIControls = nullptr;
	}	
	if (p_UICommands)
	{
		p_UICommands->End();
		delete(p_UICommands);
		p_UICommands = nullptr;
	}	
	if (p_UIBackToTitle)
	{
		p_UIBackToTitle->End();
		delete(p_UIBackToTitle);
		p_UIBackToTitle = nullptr;
	}
}
