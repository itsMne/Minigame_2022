//*****************************************************************************
// C_DXWindow.cpp
// �E�C���h�E�Y�̊Ǘ�
// Author : Mane
//*****************************************************************************
#include "C_DXWindow.h"
#include "Sprite.h"
#include "Texture.h"
#include "TextureManager.h"
#include "SceneManager.h"
#include "input.h"
#include "UniversalVariables.h"
#include "TransitionControl.h"
#include <io.h>
#include <fcntl.h>
#include "sound.h"

//-------- ���C�u�����̃����N
#pragma comment(lib, "winmm")
#pragma comment(lib, "imm32")
#pragma comment(lib, "d3d11")

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define USE_COM true
#define CLASS_NAME				_T("AppClass")					// �E�C���h�E�̃N���X��
#define WINDOW_NAME				_T("�L���������E�w���E�X�e�[�V����")				// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int		OnCreate(HWND hWnd, LPCREATESTRUCT lpcs);
HRESULT InitWindow(HWND hWnd, BOOL bWindow);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
HWND						g_hWnd;					// ���C�� �E�B���h�E �n���h��
HINSTANCE					g_hInst;				// �C���X�^���X �n���h��
ID3D11Device*				g_pDevice;				// �f�o�C�X
ID3D11DeviceContext*		g_pDeviceContext;		// �f�o�C�X �R���e�L�X�g
IDXGISwapChain*				g_pSwapChain;			// �X���b�v�`�F�[��
ID3D11RenderTargetView*		g_pRenderTargetView;	// �t���[���o�b�t�@
UINT						g_uSyncInterval = 1;	// �������� (0:��, 1:�L)
ID3D11BlendState*			g_pBlendState;			// �u�����h �X�e�[�g
bool						bGameIsEnd;
MSG							msg;
Color						BackgroundColor;

//*****************************************************************************
//�R���X�g���N�^
//*****************************************************************************
C_DXWindow::C_DXWindow()
{
	BackgroundColor = { 0,0,0,0 };
#ifdef USE_CONSOLE
	ActivateConsole();
#endif
	nFPS = 0;
}


C_DXWindow::~C_DXWindow()
{
}
//*****************************************************************************
//Init�֐�
//�������̊֐�
//�����FHINSTANCE, HINSTANCE, LPTSTR, int
//�߁Fint
//*****************************************************************************
int C_DXWindow::Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	bGameIsEnd = false;
	UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	WNDCLASSEX wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINFRM)),
		LoadCursor(nullptr, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		nullptr,
		CLASS_NAME,
		nullptr
	};


	// �C���X�^���X �n���h���ۑ�
	g_hInst = hInstance;

	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �N���C�A���g�̈�T�C�Y����E�B���h�E �T�C�Y�Z�o
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION
		| WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX;
	DWORD dwExStyle = 0;
	RECT rc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRectEx(&rc, dwStyle, FALSE, dwExStyle);

	// �E�B���h�E�̍쐬
	g_hWnd = CreateWindowEx(dwExStyle,
		CLASS_NAME,
		WINDOW_NAME,
		dwStyle,
		CW_USEDEFAULT,		// �E�B���h�E�̍����W
		CW_USEDEFAULT,		// �E�B���h�E�̏���W
		rc.right - rc.left,	// �E�B���h�E����
		rc.bottom - rc.top,	// �E�B���h�E�c��
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	timeBeginPeriod(1);
	dwExecLastTime = dwFPSLastTime = timeGetTime();
	dwCurrentTime = dwFrameCount = 0;
	// �E�C���h�E�̕\��
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	
	// DirectX�̏�����(�E�B���h�E���쐬���Ă���s��)
	if (FAILED(InitWindow(g_hWnd, true))) {
		return -1;
	}

	// ���b�Z�[�W���[�v
	
	PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
	//COM������
	if (USE_COM) {
		if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
		{
			MessageBoxA(NULL, "COMNO�������Ɏ��s���܂����B", "error", MB_OK);
			return -1;
		}
	}
}
//*****************************************************************************
//Update�֐�
//�E�C���h�E�Y�̃A�b�v�f�[�g�֐�
//�����Fvoid
//�߁FUINT
//*****************************************************************************
UINT C_DXWindow::Update()
{
		

		for (;;) {
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				if (msg.message == WM_QUIT) {
					// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
					break;
				}
				else {
					// ���b�Z�[�W�̖|��ƃf�B�X�p�b�`
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			else {

				dwCurrentTime = timeGetTime();
				if ((dwCurrentTime - dwFPSLastTime) >= 2000)
				{
					nFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);
					dwFPSLastTime = dwCurrentTime;
					dwFrameCount = 0;
					SetFPS(nFPS);
				}
				if ((dwCurrentTime - dwExecLastTime) >= (2000 / 120))
				{
					dwExecLastTime = dwCurrentTime;

				UpdateInput();
				UpdateFade();
				// �X�V����
				UpdateScenes();
				}
				// �`�揈��
				Draw();
				dwFrameCount++;
			}

		}
		return WM_QUIT;
}


//*****************************************************************************
//WndProc�֐�
//�E�C���h�E�Y�̃v���V�[�W���֐�
//�����FHWND, UINT, WPARAM, LPARAM
//�߁FLRESULT
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:					//----- �E�B���h�E���������ꂽ
		return OnCreate(hWnd, (LPCREATESTRUCT)lParam);
	case WM_DESTROY:				//----- �E�B���h�E�j���w��������
		PostQuitMessage(0);				// �V�X�e���ɃX���b�h�̏I����v��
		return 0;
	case WM_KEYDOWN:				//----- �L�[�{�[�h�������ꂽ
		switch (wParam) {
		case VK_ESCAPE:					// [ESC]�L�[�������ꂽ
			PostMessage(hWnd, WM_CLOSE, 0, 0);	// [x]�������ꂽ�悤�ɐU����
			return 0;
		}
		break;
	case WM_MENUCHAR:
		return MNC_CLOSE << 16;			// [Alt]+[Enter]����BEEP��}�~
	default:
		break;
	}
	if (bGameIsEnd)
	{
		PostMessage(hWnd, WM_CLOSE, 0, 0);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//*****************************************************************************
//OnCreate�֐�
//�E�C���h�E�Y�� WM_CREATE���b�Z�[�W�n���h���֐�
//�����FHWND, LPCREATESTRUCT
//�߁Fint
//*****************************************************************************
int OnCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
	// �N���C�A���g�̈�T�C�Y��SCREEN_WIDTH�~SCREEN_HEIGHT�ɍĐݒ�.
	RECT rcClnt;
	GetClientRect(hWnd, &rcClnt);
	rcClnt.right -= rcClnt.left;
	rcClnt.bottom -= rcClnt.top;
	if (rcClnt.right != WINDOW_WIDTH || rcClnt.bottom != WINDOW_HEIGHT) {
		RECT rcWnd;
		GetWindowRect(hWnd, &rcWnd);
		SIZE sizeWnd;
		sizeWnd.cx = (rcWnd.right - rcWnd.left) - rcClnt.right + WINDOW_WIDTH;
		sizeWnd.cy = (rcWnd.bottom - rcWnd.top) - rcClnt.bottom + WINDOW_HEIGHT;
		SetWindowPos(hWnd, nullptr, 0, 0, sizeWnd.cx, sizeWnd.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}

	// IME������
	ImmAssociateContext(hWnd, nullptr);

	return 0;	// -1��Ԃ���CreateWindow(Ex)�����s����.
}

//*****************************************************************************
//CreateBackBuffer�֐�
//�E�C���h�E�Y�̃o�b�N�o�b�t�@�����֐�
//�����Fvoid
//�߁FHRESULT
//*****************************************************************************
HRESULT CreateBackBuffer()
{
	// �����_�[�^�[�Q�b�g�r���[����
	ID3D11Texture2D* pBackBuffer = nullptr;
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	g_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
	SAFE_RELEASE(pBackBuffer);

	// �e�^�[�Q�b�g�r���[�������_�[�^�[�Q�b�g�ɐݒ�
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

	// �r���[�|�[�g�ݒ�
	D3D11_VIEWPORT vp;
	vp.Width = (float)WINDOW_WIDTH;
	vp.Height = (float)WINDOW_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pDeviceContext->RSSetViewports(1, &vp);

	return S_OK;
}

//*****************************************************************************
//InitWindow�֐�
//�E�C���h�E�Y�̏����������֐�
//�����FHWND, BOOL
//�߁FHRESULT
//*****************************************************************************
HRESULT InitWindow(HWND hWnd, BOOL bWindow)
{
	HRESULT hr = S_OK;

	// �f�o�C�X�A�X���b�v�`�F�[���̍쐬
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 1;
	scd.BufferDesc.Width = WINDOW_WIDTH;
	scd.BufferDesc.Height = WINDOW_HEIGHT;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = bWindow;

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, 0, featureLevels, _countof(featureLevels), D3D11_SDK_VERSION, &scd,
		&g_pSwapChain, &g_pDevice, nullptr, &g_pDeviceContext);
	if (FAILED(hr)) {
		return hr;
	}

	// �o�b�N�o�b�t�@����
	hr = CreateBackBuffer();
	if (FAILED(hr)) {
		return hr;
	}

	// ���X�^���C�Y�ݒ�
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.CullMode = D3D11_CULL_NONE;
	//D3D_CULL_BACK
	//D3D_CULL_FRONT
	//D3D_CULL_NONE
	rd.FillMode = D3D11_FILL_SOLID;
	ID3D11RasterizerState* pRs = nullptr;
	g_pDevice->CreateRasterizerState(&rd, &pRs);
	g_pDeviceContext->RSSetState(pRs);
	SAFE_RELEASE(pRs);

	// �u�����h �X�e�[�g����
	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(BlendDesc));
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_pDevice->CreateBlendState(&BlendDesc, &g_pBlendState);
	g_pDeviceContext->OMSetBlendState(g_pBlendState, blendFactor, 0xffffffff);

	hr = InitSound(g_hWnd);
	if (FAILED(hr)) {
		return hr;
	}
	SetVolume(0.0f);
	SetDevice(g_pDevice);
	SetHR(&hr);
	InitScenes();
	InitFade();
	hr = InitInput();

	return hr;
}

//*****************************************************************************
//InitWindow�֐�
//�E�C���h�E�Y�̃o�b�N�o�b�t�@����֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void ReleaseBackBuffer()
{
	if (g_pDeviceContext) {
		g_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	}
	SAFE_RELEASE(g_pRenderTargetView);
}

//*****************************************************************************
//Uninit�֐�
//�E�C���h�E�Y�̏I�������֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void C_DXWindow::Uninit()
{
	UninitSound();
	UninitInput();
	EndTextures();
	EndScenes();
	EndFade();
	// �u�����h �X�e�[�g���
	SAFE_RELEASE(g_pBlendState);

	// �o�b�N�o�b�t�@���
	ReleaseBackBuffer();

	// �X���b�v�`�F�[�����
	SAFE_RELEASE(g_pSwapChain);

	// �f�o�C�X �R���e�L�X�g�̊J��
	SAFE_RELEASE(g_pDeviceContext);

	// �f�o�C�X�̊J��
	SAFE_RELEASE(g_pDevice);

	timeEndPeriod(1);

	if(USE_COM)
		CoUninitialize();
}


//*****************************************************************************
//ActivateConsole�֐�
//�R���\��������������֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void C_DXWindow::ActivateConsole()
{
	AllocConsole();
	HANDLE stdHandle;
	int hConsole;
	FILE* fp;
	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsole = _open_osfhandle((long)stdHandle, _O_TEXT);
	fp = _fdopen(hConsole, "w");
	freopen_s(&fp, "CONOUT$", "w", stdout);
	printf("����ɂ��́I�R���\�[����ON�ł��I\n");
}

//*****************************************************************************
//GetMainWnd�֐�
//���C�� �E�B���h�E �n���h���擾�֐�
//�����Fvoid
//�߁FHWND
//*****************************************************************************
HWND GetMainWnd()
{
	return g_hWnd;
}

//*****************************************************************************
//GetInstance�֐�
//�C���X�^���X �n���h���擾�֐�
//�����Fvoid
//�߁FHINSTANCE
//*****************************************************************************
HINSTANCE GetInstance()
{
	return g_hInst;
}

//*****************************************************************************
//GetDevice�֐�
//�f�o�C�X�擾�֐�
//�����Fvoid
//�߁FID3D11Device*
//*****************************************************************************
ID3D11Device* GetDevice()
{
	return g_pDevice;
}

//*****************************************************************************
//GetDeviceContext�֐�
// �f�o�C�X �R���e�L�X�g�擾�֐�
//�����Fvoid
//�߁FID3D11DeviceContext*
//*****************************************************************************
ID3D11DeviceContext* GetDeviceContext()
{
	return g_pDeviceContext;
}


//*****************************************************************************
//Draw�֐�
//�`�揈���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void C_DXWindow::Draw()
{
	float ClearColor[4] = { BackgroundColor.R, BackgroundColor.G, BackgroundColor.B, BackgroundColor.A };
	g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

	DrawScenes();
	DrawFade();
	g_pSwapChain->Present(g_uSyncInterval, 0);
}

//*****************************************************************************
//SetBackgroundColor�֐�
//�o�b�N�O���E���h�̐F��ς���֐�
//�����FColor
//�߁Fvoid
//*****************************************************************************
void SetBackgroundColor(Color newColor)
{
	BackgroundColor = newColor;
}

//*****************************************************************************
//EndCurrentGame�֐�
//�Q�[�����I���ϐ����Ǘ�����
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void EndCurrentGame()
{
	bGameIsEnd = true;
}
