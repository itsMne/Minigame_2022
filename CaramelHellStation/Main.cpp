//*****************************************************************************
// Main.cpp
// Author : Mane
//*****************************************************************************
#include "C_DXWindow.h"

//*****************************************************************************
// メイン
//*****************************************************************************
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	C_DXWindow* Window = new C_DXWindow();//コンストラクタ
	Window->Init(hInstance, hPrevInstance, lpCmdLine, nCmdShow);//初期化
	Window->Update();	//処理のアップデート
	Window->Uninit();	// 終了処理
	delete(Window);
	Window = nullptr;
	return 0;
}