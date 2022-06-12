//*****************************************************************************
// SceneGameOver.cpp
// �V�[���Q�[���I�[�o�[���Ǘ�����
// Author : Mane
//*****************************************************************************
#include "SceneGameOver.h"
#include "TextureManager.h"
#include "GameInputManager.h"
#include "TransitionControl.h"

//*****************************************************************************
// �R���X�g���N�^�֐�
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
//Init�֐�
//�������֐�
//�����Fvoid
//�߁Fvoid
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
//Update�֐�
//�ύX�֐�
//�����Fvoid
//�߁Fvoid
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
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
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
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void SceneGameOver::End()
{
	SAFE_DELETE(GameOverScreen);
	SAFE_DELETE(GameOverMessage);
}
