//*****************************************************************************
// SceneTitleScreen.cpp
// タイトルのシーンを管理する
// Author : Mane
//*****************************************************************************
#include "SceneTitleScreen.h"
#include "TextureManager.h"
#include "GameInputManager.h"
#include "TransitionControl.h"
#include "sound.h"

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
SceneTitleScreen::SceneTitleScreen()
{
	Init();
}


SceneTitleScreen::~SceneTitleScreen()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void SceneTitleScreen::Init()
{
	goTitle_Screen = new GameObject();
	SetBackgroundColor({ 0.125f, 0.35f, 0.45f, 1 });
	goTitle_Screen->Init(GetTexture(TEXTURE_TITLE), ANIMATED_TITLE_TYPE);
	goTitle_Screen->SetLocalPosition(0, 100);
	bFirstInit = true;
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void SceneTitleScreen::Update()
{
	if (!goTitle_Screen)
		return;
	goTitle_Screen->Update();
#if SKIP_LOGOS
	if (bFirstInit) {
		SetVolume(1);
		PlaySound(SOUND_LABEL_BGM_TITLESCREEN);
		bFirstInit = false;
	}
#endif
	if (GetInput(GAME_START) || GetInput(MOVE_JUMP)) {
		SetTransitionToMenu();
		SetVolume(1);
		PlaySound(SOUND_LABEL_SE_SELECTED);
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void SceneTitleScreen::Draw()
{
	if (goTitle_Screen)
		goTitle_Screen->Draw();
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void SceneTitleScreen::End()
{
	SAFE_DELETE(goTitle_Screen);
}
