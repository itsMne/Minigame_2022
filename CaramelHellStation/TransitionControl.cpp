//*****************************************************************************
// TransitionControl.cpp
// ゲームの表示を変える管理
// Author : Mane
//*****************************************************************************
#include "TransitionControl.h"
#include "GameObject.h"
#include "Texture.h" 
#include "TextureManager.h" 
#include "UniversalVariables.h"
#include "SceneManager.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define FADE_SPEED					 0.0525f
#define NOISE_SPEED					 0.0125f
#define COLOR_TRANSITIONTOMENU_SPEED 0.015f
#define MAX_STICKERS_OBJECTS		 110
#define MAX_STICKERS_TEXTURES		 9
#define STICKER_SIZE				 400

//*****************************************************************************
// グローバル変数
//*****************************************************************************
bool						bIsTransitionOver;
bool						bUseFadeTransition;
bool						bUseNoiseTransition;
bool						bMainMenuTransition;
bool						bUseStickerTransition;
bool						bAllStickersOn;
int							nTransitionToMenuAcceleration;
ID3D11ShaderResourceView*	pStickerTextures[MAX_STICKERS_TEXTURES];
ID3D11ShaderResourceView*	pBlackFadeTexture;
ID3D11ShaderResourceView*	pWhiteFadeTexture;
ID3D11ShaderResourceView*	pMenuTransitionTexture;
int							nUseStickerCount;
int							FadeMirrorControl;
int							nNextState;
GameObject*					pBlackFade =			nullptr;
GameObject*					pMainMenuTransition =	nullptr;
GameObject*					pStickers =				nullptr;
GameObject*					pNoiseTransition =		nullptr;

//*****************************************************************************
//InitFade関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void InitFade()
{
	bIsTransitionOver = true;
	bUseFadeTransition = false;
	bAllStickersOn = false;
	nUseStickerCount = 0;
	pStickers = new GameObject[MAX_STICKERS_OBJECTS];
	for (int i = 0; i < MAX_STICKERS_TEXTURES; i++)
	{
		pStickerTextures[i] = nullptr;
	}
	GameObject* pAllStickers = pStickers;
	for (int i = 0, stkr=0; i < MAX_STICKERS_OBJECTS; i++, stkr++, pAllStickers++)
	{
		if (stkr == MAX_STICKERS_TEXTURES)
			stkr = 0;
		if (pStickerTextures[stkr] == nullptr)
		{
			char szTextureName[256] = "data/TEXTURE/Stickers/s";
			char szsteck[2];
			_itoa_s(stkr, szsteck, 10);
			strcat_s(szTextureName, szsteck);
			strcat_s(szTextureName, ".tga");
			CreateTextureFromFile(szTextureName, &pStickerTextures[stkr]);
			pAllStickers->Init(pStickerTextures[stkr], MAX_GOTYPE);
			//pStickerTextures[stkr] = pAllStickers->gpTexture;
			pAllStickers->SetSpriteSize(STICKER_SIZE, STICKER_SIZE);
			
		}else
		{
			pAllStickers->Init(pStickerTextures[stkr], MAX_GOTYPE);
			pAllStickers->SetSpriteSize(STICKER_SIZE, STICKER_SIZE);
		}
		pAllStickers->SetLocalPosition((float)(rand() % WINDOW_WIDTH - (WINDOW_WIDTH / 2)), (float)(rand() % WINDOW_HEIGHT - (WINDOW_HEIGHT / 2)));
		pAllStickers->SetUse(false);
		pAllStickers->SetUIObject(true);
	}
	if (!pBlackFade) {
		pBlackFade = new GameObject();
		CreateTextureFromFile("data/TEXTURE/UIObjects/fade.tga", &pBlackFadeTexture);
		pBlackFade->Init(pBlackFadeTexture, MAX_GOTYPE);
		pBlackFade->SetLocalPosition(0, 0);
		pBlackFade->SetUIObject(true);
		pBlackFade->SetSpriteSize(WINDOW_WIDTH, WINDOW_HEIGHT);
		pBlackFade->SetAlpha(0);
	}
	if (!pNoiseTransition) {
		pNoiseTransition = new GameObject();
		CreateTextureFromFile("data/TEXTURE/UIObjects/WhiteFade.tga", &pWhiteFadeTexture);
		pNoiseTransition->Init(pWhiteFadeTexture, MAX_GOTYPE);
		pNoiseTransition->SetLocalPosition(0, 0);
		pNoiseTransition->SetUIObject(true);
		pNoiseTransition->SetSpriteSize(WINDOW_WIDTH, WINDOW_HEIGHT);
		pNoiseTransition->SetAlpha(0);
	}
	if (!pMainMenuTransition) {
		pMainMenuTransition = new GameObject();
		CreateTextureFromFile("data/TEXTURE/UIObjects/MenuTransition.tga", &pMenuTransitionTexture);
		pMainMenuTransition->Init(pMenuTransitionTexture, MAX_GOTYPE);
		pMainMenuTransition->SetLocalPosition(0, 0);
		pMainMenuTransition->SetSpriteSize(10, 0);
		pMainMenuTransition->SetColor(0, 0, 0);
		pMainMenuTransition->SetUIObject(true);
	}
	FadeMirrorControl = 1;
	nNextState = MAX_GAME_STATE;
}

//*****************************************************************************
//UpdateFade関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void UpdateFade()
{
	if (bIsTransitionOver)
		return;
	if (bUseFadeTransition)
	{
		float alpha = pBlackFade->GetColor().w;
		alpha += FADE_SPEED*FadeMirrorControl;
		pBlackFade->SetAlpha(alpha);
		if (alpha >= 1)
		{
			pBlackFade->SetAlpha(1);
			if (nNextState != GetCurrentState())
				SetNewState(nNextState);
			FadeMirrorControl *= -1;
		}
		if (FadeMirrorControl < 1 && alpha <= 0)
		{
			pBlackFade->SetAlpha(0);
			FadeMirrorControl = 1;
			bUseFadeTransition = false;
			bIsTransitionOver = true;
			ControlSoundPlayForNextState();
		}
	}
	if (bUseNoiseTransition)
	{
		pNoiseTransition->Update();
		float alpha = pNoiseTransition->GetColor().w;
		alpha += NOISE_SPEED * FadeMirrorControl;
		pNoiseTransition->SetAlpha(alpha);
		if (alpha >= 1)
		{
			pNoiseTransition->SetAlpha(1);
			if (nNextState != GetCurrentState())
				SetNewState(nNextState);
			FadeMirrorControl *= -1;
			StopSound();
		}
		if (FadeMirrorControl < 1 && alpha <= 0)
		{
			pNoiseTransition->SetAlpha(0);
			FadeMirrorControl = 1;
			bUseNoiseTransition = false;
			bIsTransitionOver = true;
			ControlSoundPlayForNextState();
		}
	}
	if (bMainMenuTransition)
	{
		XMFLOAT3 transitionSize = pMainMenuTransition->GetSize();
		nTransitionToMenuAcceleration+=3;
		if (transitionSize.y < WINDOW_HEIGHT)
		{
			transitionSize.y+= nTransitionToMenuAcceleration;
		}
		else {
			if (transitionSize.x < WINDOW_WIDTH)
			{
				transitionSize.x += nTransitionToMenuAcceleration;
			}
		}
		XMFLOAT4 colortoGet = pMainMenuTransition->GetColor();
		if(colortoGet.x<1)
			colortoGet.x+= COLOR_TRANSITIONTOMENU_SPEED;
		if (colortoGet.x > 1)
			colortoGet.x = 1;
		if (colortoGet.y<1)
			colortoGet.y += COLOR_TRANSITIONTOMENU_SPEED;
		if (colortoGet.y > 1)
			colortoGet.y = 1;
		if (colortoGet.z<1)
			colortoGet.z += COLOR_TRANSITIONTOMENU_SPEED;
		if (colortoGet.z > 1)
			colortoGet.z = 1;
		pMainMenuTransition->SetColor(colortoGet.x, colortoGet.y, colortoGet.z);
		pMainMenuTransition->SetSpriteSize(transitionSize);
		if (transitionSize.y >= WINDOW_HEIGHT && transitionSize.x >= WINDOW_WIDTH)
		{
			bMainMenuTransition = false;
			bIsTransitionOver = true;
			pMainMenuTransition->SetLocalPosition(0, 0);
			pMainMenuTransition->SetSpriteSize(10, 0);
			pMainMenuTransition->SetColor(0, 0, 0);
			nTransitionToMenuAcceleration = 0;
			SetNewState(nNextState);
			SetVolume(0.5f);
			PlaySound(SOUND_LABEL_BGM_MENU);

		}
	}
	if (bUseStickerTransition)
	{
		static int StickerSFXCount=0;
		if (!bAllStickersOn) {
			if (nUseStickerCount < MAX_STICKERS_OBJECTS)
			{
				nUseStickerCount++;
				pStickers[nUseStickerCount].SetUse(true);
				if (++StickerSFXCount > 4)
				{
					PlaySound(SOUND_LABEL_SE_PASTE_STICKER);
					StickerSFXCount = 0;
				}
				if (nUseStickerCount == MAX_STICKERS_OBJECTS) 
				{
					SetNewState(nNextState);
					bAllStickersOn = true;
				}
			}
		}
		else {
			if (nUseStickerCount >= 0)
			{
				
				nUseStickerCount--;
				pStickers[nUseStickerCount].SetUse(false);
				if (++StickerSFXCount > 8) 
				{
					PlaySound(SOUND_LABEL_SE_REMOVE_STICKER);
					StickerSFXCount = 0;
				}
				if (nUseStickerCount == 0) 
				{
					bAllStickersOn = false;
					bUseStickerTransition = false;
					bIsTransitionOver = true;
					ControlSoundPlayForNextState();
				}
			}
		}
	}
}

//*****************************************************************************
//ControlSoundPlayForNextState関数
//選んだオプション次第、SFXを管理する
//引数：void
//戻：void
//*****************************************************************************
void ControlSoundPlayForNextState()
{
	int nCState = GetCurrentState();
	switch (nCState)
	{
	case TITLE_SCREEN_STATE:
		SetVolume(1);
		PlaySound(SOUND_LABEL_BGM_TITLESCREEN);
		break;
	case INGAME_STATE:
		break;
	case GAME_OVER_STATE:
		PlaySound(SOUND_LABEL_BGM_GAMEOVER);
		break;
	default:
		break;
	}
}

//*****************************************************************************
//DrawFade関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void DrawFade()
{
	if (bIsTransitionOver)
		return;
	if(bUseFadeTransition)
		pBlackFade->Draw();
	if (bUseNoiseTransition)
		pNoiseTransition->Draw();
	if (bMainMenuTransition)
		pMainMenuTransition->Draw();
	if (bUseStickerTransition)
	{
		GameObject* pAllStickers = pStickers;
		for (int i = 0; i < MAX_STICKERS_OBJECTS; i++, pAllStickers++)
		{
			pAllStickers->Draw();
		}
	}
}

//*****************************************************************************
//EndFade関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void EndFade()
{
	SAFE_DELETE(pBlackFade);
	SAFE_DELETE(pNoiseTransition);
	SAFE_DELETE(pMainMenuTransition); 
	pBlackFade = pNoiseTransition = pMainMenuTransition = nullptr;
	for (int i = 0; i < MAX_STICKERS_OBJECTS; i++)
	{
		pStickers[i].End();
	}
	delete[](pStickers);
}

//*****************************************************************************
//IsTransitionOver関数
//シーンを変えるエフェクトが終わったことを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool IsTransitionOver()
{
	return bIsTransitionOver;
}

//*****************************************************************************
//SetFadeTransitionToNewState関数
//シーンを変えるエフェクトがフェードにして、新しいシーンを設定する
//引数：int
//戻：void
//*****************************************************************************
void SetFadeTransitionToNewState(int newState)
{
	bIsTransitionOver = false;
	bUseFadeTransition = true;
	nNextState = newState;
}

//*****************************************************************************
//SetNoiseTransitionToNewState関数
//シーンを変えるエフェクトがノイズにして、新しいシーンを設定する
//引数：int
//戻：void
//*****************************************************************************
void SetNoiseTransitionToNewState(int newState)
{
	bIsTransitionOver = false;
	bUseNoiseTransition = true;
	nNextState = newState;
}

//*****************************************************************************
//SetStickerTransition関数
//シーンを変えるエフェクトがシールにして、新しいシーンを設定する
//引数：int
//戻：void
//*****************************************************************************
void SetStickerTransition(int newState)
{
	StopSound();
	bIsTransitionOver = false;
	bUseStickerTransition = true;
	nNextState = newState;
	nUseStickerCount = 0;
	bAllStickersOn = false;
}

//*****************************************************************************
//SetTransitionToMenu関数
//次のシーンはメニューにする
//引数：void
//戻：void
//*****************************************************************************
void SetTransitionToMenu()
{
	StopSound();
	bIsTransitionOver = false;
	bMainMenuTransition = true;
	nNextState = MAIN_MENU;
	nTransitionToMenuAcceleration = 0;
}
