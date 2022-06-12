//*****************************************************************************
// SceneGameOver.cpp
// シーンゲームオーバーを管理する
// Author : Mane
//*****************************************************************************
#include "SceneGameOver.h"
#include "TextureManager.h"
#include "GameInputManager.h"
#include "TransitionControl.h"

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
SceneGameOver::SceneGameOver()
{
	Init();
}


SceneGameOver::~SceneGameOver()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void SceneGameOver::Init()
{
	GameOverScreen = new GameObject();
	GameOverMessage = new GameObject();
	GameOverScreen->Init(GetTexture(TEXTURE_REDNOISE), NOISE_TYPE);
	GameOverMessage->Init(GetTexture(TEXTURE_GAMEOVER_MESSAGE), MAX_GOTYPE);
	GameOverScreen->SetUIObject(true);
	GameOverMessage->SetUIObject(true);
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void SceneGameOver::Update()
{
	SetBackgroundColor({ 0, 0, 0, 1 });
	GameOverScreen->Update();
	GameOverMessage->Update();
	if (GetInput(GAME_START))
		SetStickerTransition(TITLE_SCREEN_STATE);
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void SceneGameOver::Draw()
{
	GameOverScreen->SetSpriteSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	GameOverScreen->SetLocalPosition(0, 0);
	GameOverScreen->Draw();
	GameOverMessage->SetSpriteSize(548, 82);
	GameOverMessage->SetLocalPosition(0, 0);
	GameOverMessage->Draw();
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void SceneGameOver::End()
{
	SAFE_DELETE(GameOverScreen);
	SAFE_DELETE(GameOverMessage);
}
