//*****************************************************************************
// Main.cpp
// Author : Mane
//*****************************************************************************
#include "C_DXWindow.h"

//*****************************************************************************
// ���C��
//*****************************************************************************
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	C_DXWindow* Window = new C_DXWindow();//�R���X�g���N�^
	Window->Init(hInstance, hPrevInstance, lpCmdLine, nCmdShow);//������
	Window->Update();	//�����̃A�b�v�f�[�g
	Window->Uninit();	// �I������
	delete(Window);
	Window = nullptr;
	return 0;
}