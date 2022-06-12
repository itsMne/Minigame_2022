//*****************************************************************************
// C_DXWindow.cpp
// ウインドウズの管理
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

//-------- ライブラリのリンク
#pragma comment(lib, "winmm")
#pragma comment(lib, "imm32")
#pragma comment(lib, "d3d11")

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define USE_COM true
#define CLASS_NAME				_T("AppClass")					// ウインドウのクラス名
#define WINDOW_NAME				_T("キャラメル・ヘル・ステーション")				// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int		OnCreate(HWND hWnd, LPCREATESTRUCT lpcs);
HRESULT InitWindow(HWND hWnd, BOOL bWindow);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
HWND						g_hWnd;					// メイン ウィンドウ ハンドル
HINSTANCE					g_hInst;				// インスタンス ハンドル
ID3D11Device*				g_pDevice;				// デバイス
ID3D11DeviceContext*		g_pDeviceContext;		// デバイス コンテキスト
IDXGISwapChain*				g_pSwapChain;			// スワップチェーン
ID3D11RenderTargetView*		g_pRenderTargetView;	// フレームバッファ
UINT						g_uSyncInterval = 1;	// 垂直同期 (0:無, 1:有)
ID3D11BlendState*			g_pBlendState;			// ブレンド ステート
bool						bGameIsEnd;
MSG							msg;
Color						BackgroundColor;

//*****************************************************************************
//コンストラクタ
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
//Init関数
//初期化の関数
//引数：HINSTANCE, HINSTANCE, LPTSTR, int
//戻：int
//*****************************************************************************
int C_DXWindow::Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	bGameIsEnd = false;
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

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


	// インスタンス ハンドル保存
	g_hInst = hInstance;

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// クライアント領域サイズからウィンドウ サイズ算出
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION
		| WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX;
	DWORD dwExStyle = 0;
	RECT rc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRectEx(&rc, dwStyle, FALSE, dwExStyle);

	// ウィンドウの作成
	g_hWnd = CreateWindowEx(dwExStyle,
		CLASS_NAME,
		WINDOW_NAME,
		dwStyle,
		CW_USEDEFAULT,		// ウィンドウの左座標
		CW_USEDEFAULT,		// ウィンドウの上座標
		rc.right - rc.left,	// ウィンドウ横幅
		rc.bottom - rc.top,	// ウィンドウ縦幅
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	timeBeginPeriod(1);
	dwExecLastTime = dwFPSLastTime = timeGetTime();
	dwCurrentTime = dwFrameCount = 0;
	// ウインドウの表示
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	
	// DirectXの初期化(ウィンドウを作成してから行う)
	if (FAILED(InitWindow(g_hWnd, true))) {
		return -1;
	}

	// メッセージループ
	
	PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
	//COM初期化
	if (USE_COM) {
		if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
		{
			MessageBoxA(NULL, "COMNO初期化に失敗しました。", "error", MB_OK);
			return -1;
		}
	}
}
//*****************************************************************************
//Update関数
//ウインドウズのアップデート関数
//引数：void
//戻：UINT
//*****************************************************************************
UINT C_DXWindow::Update()
{
		

		for (;;) {
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				if (msg.message == WM_QUIT) {
					// PostQuitMessage()が呼ばれたらループ終了
					break;
				}
				else {
					// メッセージの翻訳とディスパッチ
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
				// 更新処理
				UpdateScenes();
				}
				// 描画処理
				Draw();
				dwFrameCount++;
			}

		}
		return WM_QUIT;
}


//*****************************************************************************
//WndProc関数
//ウインドウズのプロシージャ関数
//引数：HWND, UINT, WPARAM, LPARAM
//戻：LRESULT
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:					//----- ウィンドウが生成された
		return OnCreate(hWnd, (LPCREATESTRUCT)lParam);
	case WM_DESTROY:				//----- ウィンドウ破棄指示がきた
		PostQuitMessage(0);				// システムにスレッドの終了を要求
		return 0;
	case WM_KEYDOWN:				//----- キーボードが押された
		switch (wParam) {
		case VK_ESCAPE:					// [ESC]キーが押された
			PostMessage(hWnd, WM_CLOSE, 0, 0);	// [x]が押されたように振舞う
			return 0;
		}
		break;
	case WM_MENUCHAR:
		return MNC_CLOSE << 16;			// [Alt]+[Enter]時のBEEPを抑止
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
//OnCreate関数
//ウインドウズの WM_CREATEメッセージハンドラ関数
//引数：HWND, LPCREATESTRUCT
//戻：int
//*****************************************************************************
int OnCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
	// クライアント領域サイズをSCREEN_WIDTH×SCREEN_HEIGHTに再設定.
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

	// IME無効化
	ImmAssociateContext(hWnd, nullptr);

	return 0;	// -1を返すとCreateWindow(Ex)が失敗する.
}

//*****************************************************************************
//CreateBackBuffer関数
//ウインドウズのバックバッファ生成関数
//引数：void
//戻：HRESULT
//*****************************************************************************
HRESULT CreateBackBuffer()
{
	// レンダーターゲットビュー生成
	ID3D11Texture2D* pBackBuffer = nullptr;
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	g_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
	SAFE_RELEASE(pBackBuffer);

	// 各ターゲットビューをレンダーターゲットに設定
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

	// ビューポート設定
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
//InitWindow関数
//ウインドウズの初期化処理関数
//引数：HWND, BOOL
//戻：HRESULT
//*****************************************************************************
HRESULT InitWindow(HWND hWnd, BOOL bWindow)
{
	HRESULT hr = S_OK;

	// デバイス、スワップチェーンの作成
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

	// バックバッファ生成
	hr = CreateBackBuffer();
	if (FAILED(hr)) {
		return hr;
	}

	// ラスタライズ設定
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

	// ブレンド ステート生成
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
//InitWindow関数
//ウインドウズのバックバッファ解放関数
//引数：void
//戻：void
//*****************************************************************************
void ReleaseBackBuffer()
{
	if (g_pDeviceContext) {
		g_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	}
	SAFE_RELEASE(g_pRenderTargetView);
}

//*****************************************************************************
//Uninit関数
//ウインドウズの終了処理関数
//引数：void
//戻：void
//*****************************************************************************
void C_DXWindow::Uninit()
{
	UninitSound();
	UninitInput();
	EndTextures();
	EndScenes();
	EndFade();
	// ブレンド ステート解放
	SAFE_RELEASE(g_pBlendState);

	// バックバッファ解放
	ReleaseBackBuffer();

	// スワップチェーン解放
	SAFE_RELEASE(g_pSwapChain);

	// デバイス コンテキストの開放
	SAFE_RELEASE(g_pDeviceContext);

	// デバイスの開放
	SAFE_RELEASE(g_pDevice);

	timeEndPeriod(1);

	if(USE_COM)
		CoUninitialize();
}


//*****************************************************************************
//ActivateConsole関数
//コンソルを活性化する関数
//引数：void
//戻：void
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
	printf("こんにちは！コンソールはONです！\n");
}

//*****************************************************************************
//GetMainWnd関数
//メイン ウィンドウ ハンドル取得関数
//引数：void
//戻：HWND
//*****************************************************************************
HWND GetMainWnd()
{
	return g_hWnd;
}

//*****************************************************************************
//GetInstance関数
//インスタンス ハンドル取得関数
//引数：void
//戻：HINSTANCE
//*****************************************************************************
HINSTANCE GetInstance()
{
	return g_hInst;
}

//*****************************************************************************
//GetDevice関数
//デバイス取得関数
//引数：void
//戻：ID3D11Device*
//*****************************************************************************
ID3D11Device* GetDevice()
{
	return g_pDevice;
}

//*****************************************************************************
//GetDeviceContext関数
// デバイス コンテキスト取得関数
//引数：void
//戻：ID3D11DeviceContext*
//*****************************************************************************
ID3D11DeviceContext* GetDeviceContext()
{
	return g_pDeviceContext;
}


//*****************************************************************************
//Draw関数
//描画処理関数
//引数：void
//戻：void
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
//SetBackgroundColor関数
//バックグラウンドの色を変える関数
//引数：Color
//戻：void
//*****************************************************************************
void SetBackgroundColor(Color newColor)
{
	BackgroundColor = newColor;
}

//*****************************************************************************
//EndCurrentGame関数
//ゲームが終わる変数を管理する
//引数：void
//戻：void
//*****************************************************************************
void EndCurrentGame()
{
	bGameIsEnd = true;
}
