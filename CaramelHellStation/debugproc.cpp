//*****************************************************************************
// debugproc.cpp
// �f�o�b�O�\���̊Ǘ�
// Author : Mane
//*****************************************************************************
#include <stdarg.h>
#include <string.h>
#include "debugproc.h"
#include "UniversalVariables.h"
#include "Texture.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define FONT_SIZE	16

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static char			g_szDebug[8192] = { '\0' };	// �f�o�b�O���
static bool			g_bHiragana = false;		// �������t���O

//*****************************************************************************
// �R���X�g���N�^
//*****************************************************************************
cDebugProc::cDebugProc()
{
}


cDebugProc::~cDebugProc()
{
}

//*****************************************************************************
//InitDebugProc�֐�
//�f�o�b�O�v���b�N�̏����������֐�
//�����Fvoid
//�߁FHRESULT
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
//UninitDebugProc�֐�
//�f�o�b�O�v���b�N�̏����̏I�������֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void cDebugProc::UninitDebugProc()
{
	// �e�N�X�`����@
	SAFE_RELEASE(gpTexture);
}

//*****************************************************************************
//UpdateDebugProc�֐�
//�f�o�b�O�v���b�N�̏����̍X�V�����֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void cDebugProc::UpdateDebugProc()
{
	//�Ȃ�
}

//*****************************************************************************
//DrawDebugProc�֐�
//�f�o�b�O�v���b�N�����̕`�揈���֐�
//�����Fvoid
//�߁Fvoid
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

	// �e�N�X�`���ݒ�����ɖ߂�
	SetColor(1.0f, 0.5f, 0.5f);
	SetAlpha(1.0f);
	SetSpriteUV(0.0f, 0.0f);
	SetSpriteFrameSize(1.0f, 1.0f);
}

//*****************************************************************************
//StartDebugProc�֐�
//�f�o�b�O�v���b�N�̓o�^�J�n�֐�
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void cDebugProc::StartDebugProc(bool hiragana)
{
	SetHiragana(hiragana);
	g_szDebug[0] = '\0';
}

//*****************************************************************************
//PrintDebugProc�֐�
//�f�o�b�O�v���b�N�̓o�^�֐�
//�����Fconst char *fmt, ...
//�߁Fvoid
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
//SetHiragana�֐�
//�Ђ炪�Ȃɂ���֐�
//�����Fbool
//�߁Fbool
//*****************************************************************************
bool cDebugProc::SetHiragana(bool hiragana)
{
	bool prev = g_bHiragana;
	g_bHiragana = hiragana;
	return prev;
}
