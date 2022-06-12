//*****************************************************************************
// C_DXWindow.h
// Author : Mane
//*****************************************************************************
#ifndef C_DXWINDOW_H
#define C_DXWINDOW_H

#include <windows.h>
#include <tchar.h>
#include <mmsystem.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "resource.h"

// 本来はヘッダに書かない方が良い
using namespace DirectX;

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) {if(x){(x)->Release();x=nullptr;}}
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(x) {if(x){delete(x);x=nullptr;}}
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) {if(x){delete[](x);x=nullptr;}}
#endif
#define SCREEN_CENTER_X	(SCREEN_WIDTH/2)	// ウインドウの中心Ｘ座標
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT/2)	// ウインドウの中心Ｙ座標
#define	NUM_VERTEX		(4)					// 頂点数
#define	NUM_POLYGON		(2)					// ポリゴン数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct VERTEX_2D {	    // 頂点フォーマット( 頂点座標[2D] / 反射光 / テクスチャ座標 )
	XMFLOAT3 vtx = {0,0,0};		// 頂点座標
	XMFLOAT4 diffuse = {0,0,0,0};	// 拡散反射光
	XMFLOAT2 tex = {0,0};		// テクスチャ座標
};

struct Color
{
	float R = 0, G = 0, B = 0, A = 0;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HWND				 GetMainWnd();
HINSTANCE			 GetInstance();
ID3D11Device*		 GetDevice();
ID3D11DeviceContext* GetDeviceContext();

//*****************************************************************************
// クラス
//*****************************************************************************
class C_DXWindow
{
private:
	int		nFPS;
	DWORD	dwExecLastTime;
	DWORD	dwFPSLastTime;
	DWORD	dwCurrentTime;
	DWORD	dwFrameCount;
public:
	C_DXWindow();
	~C_DXWindow();
	int Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
	UINT Update();
	void Draw();
	void Uninit();
	void ActivateConsole();
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SetBackgroundColor(Color newColor);
void EndCurrentGame();

#endif