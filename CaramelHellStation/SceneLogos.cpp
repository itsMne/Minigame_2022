//*****************************************************************************
// SceneLogos.cpp
// �V�[�����S���Ǘ�����
// Author : Mane
//*****************************************************************************
#include "SceneLogos.h"
#include "TextureManager.h"
#include "GameInputManager.h"
#include "TransitionControl.h"
#include "UniversalVariables.h"

//*****************************************************************************
// �R���X�g���N�^�֐�
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
//Init�֐�
//�������֐�
//�����Fvoid
//�߁Fvoid
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
//Update�֐�
//�ύX�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void SceneLogos::Update()
{
	if(pLogo)
		pLogo->Update();
	if (GetInput(GAME_START))
		SetFadeTransitionToNewState(TITLE_SCREEN_STATE);
}

//*****************************************************************************
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void SceneLogos::Draw()
{
	if(pLogo)
		pLogo->Draw();
}

//*****************************************************************************
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void SceneLogos::End()
{
	SAFE_DELETE(pLogo);
}
