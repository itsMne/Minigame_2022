//*****************************************************************************
// debugproc.cpp
// デバッグ表示の管理
// Author : Mane
//*****************************************************************************
#include <stdarg.h>
#include <string.h>
#include "debugproc.h"
#include "UniversalVariables.h"
#include "Texture.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define FONT_SIZE	16

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static char			g_szDebug[8192] = { '\0' };	// デバッグ情報
static bool			g_bHiragana = false;		// 平仮名フラグ

//*****************************************************************************
// コンストラクタ
//*****************************************************************************
cDebugProc::cDebugProc()
{
}


cDebugProc::~cDebugProc()
{
}

//*****************************************************************************
//InitDebugProc関数
//デバッグプロックの初期化処理関数
//引数：void
//戻：HRESULT
//*****************************************************************************
HRESULT cDebugProc::InitDebugProc()
{
	InitSprite();
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = S_OK;
	hr = CreateTextureFromFile(pDevice, "data/TEXTURE/PressStart2Ph.tga", &gpTexture);
	SetTexture(gpTexture);
	return hr;
}

//*****************************************************************************
//UninitDebugProc関数
//デバッグプロックの処理の終了処理関数
//引数：void
//戻：void
//*****************************************************************************
void cDebugProc::UninitDebugProc()
{
	// テクスチャ解法
	SAFE_RELEASE(gpTexture);
}

//*****************************************************************************
//UpdateDebugProc関数
//デバッグプロックの処理の更新処理関数
//引数：void
//戻：void
//*****************************************************************************
void cDebugProc::UpdateDebugProc()
{
	//ない
}

//*****************************************************************************
//DrawDebugProc関数
//デバッグプロック処理の描画処理関数
//引数：void
//戻：void
//*****************************************************************************
void cDebugProc::DrawDebugProc()
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMFLOAT2 vPos(WINDOW_WIDTH * -0.5f + 16 * 0.5f, WINDOW_HEIGHT * 0.5f - FONT_SIZE * 0.5f);
	SetSpriteFrameSize(8.0f / 128.0f, 8.0f / 128.0f);
	SetSpriteSize(FONT_SIZE, FONT_SIZE);
	for (char* pChr = &g_szDebug[0]; *pChr; ++pChr) {
		if (*pChr == '\n') {
			vPos.x = WINDOW_WIDTH * -0.5f + FONT_SIZE * 0.5f;
			vPos.y -= FONT_SIZE;
			continue;
		}
		SetLocalPosition(vPos.x, vPos.y);
		int nChr = (BYTE)*pChr;
		if (g_bHiragana && (nChr >= 0xa6 && nChr <= 0xaf || nChr >= 0xb1 && nChr <= 0xdd))
			nChr ^= 0x20;
		SetSpriteUV((nChr & 15) / 16.0f, (nChr >> 4) / 16.0f);
		UpdateSprite();
		DrawSpriteUI(pDeviceContext);
		vPos.x += FONT_SIZE;
	}

	// テクスチャ設定を元に戻す
	SetColor(1.0f, 0.5f, 0.5f);
	SetAlpha(1.0f);
	SetSpriteUV(0.0f, 0.0f);
	SetSpriteFrameSize(1.0f, 1.0f);
}

//*****************************************************************************
//StartDebugProc関数
//デバッグプロックの登録開始関数
//引数：bool
//戻：void
//*****************************************************************************
void cDebugProc::StartDebugProc(bool hiragana)
{
	SetHiragana(hiragana);
	g_szDebug[0] = '\0';
}

//*****************************************************************************
//PrintDebugProc関数
//デバッグプロックの登録関数
//引数：const char *fmt, ...
//戻：void
//*****************************************************************************
void cDebugProc::PrintDebugProc(const char *fmt, ...)
{
	va_list list;
	char aBuf[256];
	va_start(list, fmt);
	_vsprintf_p(aBuf, sizeof(aBuf), fmt, list);
	va_end(list);
	if ((strlen(g_szDebug) + strlen(aBuf)) < sizeof(g_szDebug) - 1)
		strcat_s(g_szDebug, sizeof(g_szDebug), aBuf);
}

//*****************************************************************************
//SetHiragana関数
//ひらがなにする関数
//引数：bool
//戻：bool
//*****************************************************************************
bool cDebugProc::SetHiragana(bool hiragana)
{
	bool prev = g_bHiragana;
	g_bHiragana = hiragana;
	return prev;
}
