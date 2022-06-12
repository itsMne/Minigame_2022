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

// �{���̓w�b�_�ɏ����Ȃ������ǂ�
using namespace DirectX;

//*****************************************************************************
// �}�N����`
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
#define SCREEN_CENTER_X	(SCREEN_WIDTH/2)	// �E�C���h�E�̒��S�w���W
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT/2)	// �E�C���h�E�̒��S�x���W
#define	NUM_VERTEX		(4)					// ���_��
#define	NUM_POLYGON		(2)					// �|���S����

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct VERTEX_2D {	    // ���_�t�H�[�}�b�g( ���_���W[2D] / ���ˌ� / �e�N�X�`�����W )
	XMFLOAT3 vtx = {0,0,0};		// ���_���W
	XMFLOAT4 diffuse = {0,0,0,0};	// �g�U���ˌ�
	XMFLOAT2 tex = {0,0};		// �e�N�X�`�����W
};

struct Color
{
	float R = 0, G = 0, B = 0, A = 0;
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HWND				 GetMainWnd();
HINSTANCE			 GetInstance();
ID3D11Device*		 GetDevice();
ID3D11DeviceContext* GetDeviceContext();

//*****************************************************************************
// �N���X
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
// �v���g�^�C�v�錾
//*****************************************************************************
void SetBackgroundColor(Color newColor);
void EndCurrentGame();

#endif