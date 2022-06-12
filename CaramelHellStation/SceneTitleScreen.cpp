//*****************************************************************************
// SceneTitleScreen.cpp
// �^�C�g���̃V�[�����Ǘ�����
// Author : Mane
//*****************************************************************************
#include "SceneTitleScreen.h"
#include "TextureManager.h"
#include "GameInputManager.h"
#include "TransitionControl.h"
#include "sound.h"

//*****************************************************************************
// �R���X�g���N�^�֐�
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
//Init�֐�
//�������֐�
//�����Fvoid
//�߁Fvoid
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
//Update�֐�
//�ύX�֐�
//�����Fvoid
//�߁Fvoid
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
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void SceneTitleScreen::Draw()
{
	if (goTitle_Screen)
		goTitle_Screen->Draw();
}

//*****************************************************************************
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void SceneTitleScreen::End()
{
	SAFE_DELETE(goTitle_Screen);
}
