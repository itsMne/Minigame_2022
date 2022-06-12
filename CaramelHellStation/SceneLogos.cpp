//*****************************************************************************
// SceneLogos.cpp
// シーンロゴを管理する
// Author : Mane
//*****************************************************************************
#include "SceneLogos.h"
#include "TextureManager.h"
#include "GameInputManager.h"
#include "TransitionControl.h"
#include "UniversalVariables.h"

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
SceneLogos::SceneLogos()
{
	Init();
}


SceneLogos::~SceneLogos()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void SceneLogos::Init()
{
	SetBackgroundColor({ 0, 0, 0, 1 });
	pLogo = new GameObject();
	pLogo->Init(GetTexture(TEXTURE_LOGO), ANIMATED_LOGO_TYPE);
	pLogo->SetUIObject(true);
	pLogo->SetLocalPosition(20, 0);
	pLogo->SetFramesForUV(1, 30);
	pLogo->SetSpriteSize(960, 540);
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void SceneLogos::Update()
{
	if(pLogo)
		pLogo->Update();
	if (GetInput(GAME_START))
		SetFadeTransitionToNewState(TITLE_SCREEN_STATE);
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void SceneLogos::Draw()
{
	if(pLogo)
		pLogo->Draw();
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void SceneLogos::End()
{
	SAFE_DELETE(pLogo);
}
