//*****************************************************************************
// MainMenuControl.cpp
// メニューのかんり
// Author : Mane
//*****************************************************************************
#include "MainMenuControl.h"
#include "GameObject.h"
#include "UI_Object.h"
#include "GameInputManager.h"
#include "SceneInGame.h"
#include "TransitionControl.h"
#include "ObjectManager.h"
#include "C_DXWindow.h"
#include "TextureManager.h"
#include <stdio.h>
#include "sound.h"
#define MAX_RANK_POSITIONS 5
#define SCORE_FILENAMEPATH "data/ScoreData.bin"
//*****************************************************************************
// エナム定義
//*****************************************************************************
enum GAME_MENU_OPTIONS
{
#if DEBUG_ROOM_ACTIVE
	DEBUG_MENU_OPTION,
#endif
	NEW_GAME_START,
	TUTORIAL_START,
	SHOW_RANKING,
	END_GAME,
	MAX_OPTIONS
};

typedef struct RankLevelName 
{
	int Texture;
	XMFLOAT2 Size;
};
//*****************************************************************************
// グローバル変数
//*****************************************************************************
GameObject* p_goMenuBackground = nullptr;
GameObject* p_goRankingBackground = nullptr;
GameObject* goOptions[MAX_OPTIONS] = {nullptr};
GameObject* p_goObjDecorationMenu = nullptr;
UI_Object*	p_UIRankingText = nullptr;
UI_Object*	p_UILevelName = nullptr;
UI_Object*	p_UIRankingScores[MAX_RANK_POSITIONS] = { nullptr };
bool		bBackGroundLoaded;
bool		bRankingScreenOn;
bool		bRankingsPrepared = false;
int			nCurrentSelectedOption;
int			nTopRankingScores[MAX_ROOMS][MAX_RANK_POSITIONS];
int			nRankCurrentLevel;
RankLevelName rln_RankLevelNames[MAX_ROOMS] = 
{
	{ TEXTURE_UI_RANKING_DEBUGROOM_TEXT,{ 309, 54 }},
	{ TEXTURE_UI_RANKING_TUTORIALROOM_TEXT,{ 347, 72 }},
	{ TEXTURE_UI_RANKING_LEVEL1ROOM_TEXT,{ 192, 70 }}
};
void InitMenuControl()
{
	nRankCurrentLevel = LEVEL1_ROOM;
	nCurrentSelectedOption = 0;
	if (!p_goMenuBackground)
		p_goMenuBackground = new GameObject();
	if (!p_goRankingBackground)
		p_goRankingBackground = new GameObject();
	if (!p_UIRankingText)
		p_UIRankingText = new UI_Object();
	if (!p_UILevelName)
		p_UILevelName = new UI_Object();
	for (int i = 0; i < MAX_RANK_POSITIONS; i++)
	{
		if (!bRankingsPrepared) {
			for (int j = 0; j < MAX_ROOMS; j++)
				nTopRankingScores[j][i] = 0;
			FILE *pFile = fopen(SCORE_FILENAMEPATH, "rb");
#ifdef USE_CONSOLE
			printf("スコアデータを読み込みます。\n");
#endif
			if (pFile)
			{
				fread(nTopRankingScores, sizeof(int[MAX_ROOMS][MAX_RANK_POSITIONS]), 1, pFile);
			}
			bRankingsPrepared = true;
		}
		if (!p_UIRankingScores[i])
			 p_UIRankingScores[i] = new UI_Object();
	}

	for (int i = 0; i < MAX_OPTIONS; i++)
		goOptions[i] = new GameObject();
	p_goMenuBackground->Init(GetTexture(TEXTURE_SCROLLING_BACKGROUND), SCROLLING_BACKGROUND_TYPE);
	p_goMenuBackground->SetSpriteUV(0.5f, 1);
	p_goMenuBackground->SetFramesForUV(2, 1);
	p_goMenuBackground->SetAlpha(0);
	p_goMenuBackground->SetUIObject(true);

	p_goRankingBackground->Init(GetTexture(TEXTURE_RANKING_SCROLLING_BACKGROUND), SCROLLING_BACKGROUND_TYPE);
	p_goRankingBackground->SetSpriteUV(0.5f, 1);
	p_goRankingBackground->SetFramesForUV(2, 1);
	p_goRankingBackground->SetAlpha(0);
	p_goRankingBackground->SetUIObject(true);

	p_UIRankingText->Init(UI_RANKINGSCREEN_RANKINGTEXT);
	p_UIRankingText->SetRotation({ 0,90,0 });

	for (int i = 0; i < MAX_RANK_POSITIONS; i++)
	{
		p_UIRankingScores[i]->Init(UI_RANKING_NUMBERS);
		p_UIRankingScores[i]->SetWorldPosition(-217, (WINDOW_HEIGHT / 2) - 235 -(i*SCORE_RANKNUM_SIZE));
		p_UIRankingScores[i]->SetNumberUI(0);
		p_UIRankingScores[i]->SetRotation({ 0,90,0 });
	}

	bBackGroundLoaded = false;
	bRankingScreenOn = false;
#if DEBUG_ROOM_ACTIVE
	goOptions[DEBUG_MENU_OPTION]->Init(GetTexture(TEXTURE_MENU_DEBUG_ROOM), MAX_GOTYPE);
#endif
	goOptions[NEW_GAME_START]->Init(GetTexture(TEXTURE_MENU_STARTGAME), MAX_GOTYPE);
	goOptions[TUTORIAL_START]->Init(GetTexture(TEXTURE_MENU_TUTORIAL_ROOM), MAX_GOTYPE);
	goOptions[SHOW_RANKING]->Init(GetTexture(TEXTURE_MENU_RANKINGS), MAX_GOTYPE);
	goOptions[END_GAME]->Init(GetTexture(TEXTURE_MENU_EXIT), MAX_GOTYPE);

	for (int i = 0; i < MAX_OPTIONS; i++) {
		goOptions[i]->SetUIObject(true);
		goOptions[i]->SetLocalPosition(-960, (float)295 - (i*130));
		goOptions[i]->SetSpriteSize(1571, 106);
	}

	p_goObjDecorationMenu = new GameObject();
	p_goObjDecorationMenu->Init(GetTexture(TEXTURE_MENU_DECORATION_OBJECT), ROTATING_OBJECT_TYPE);
	p_goObjDecorationMenu->SetSpriteSize(533, 605);
	p_goObjDecorationMenu->SetLocalPosition(350, -70);
	p_goObjDecorationMenu->SetUIObject(true);
	p_UILevelName->Init(UI_MAX);
	p_UILevelName->SetSpriteSize(rln_RankLevelNames[nRankCurrentLevel].Size);
	p_UILevelName->gpTexture = GetTexture(rln_RankLevelNames[nRankCurrentLevel].Texture);
	XMFLOAT3 UILevelNamePos = p_UIRankingText->GetWorldPosition();
	UILevelNamePos.y -= 85;
	p_UILevelName->SetWorldPosition(UILevelNamePos);
}

void UpdateMenuControl()
{
	p_goMenuBackground->Update();
	//ランキング
	if (bRankingScreenOn) {
		if (p_goRankingBackground) {
			float fCurrentAlpha = p_goRankingBackground->GetAlpha();
			if (fCurrentAlpha < 1)
				p_goRankingBackground->SetAlpha(fCurrentAlpha + 0.025f);
			else if (fCurrentAlpha > 1)
				p_goRankingBackground->SetAlpha(1);
			p_goRankingBackground->Update();

			if (GetInput(INPUT_RIGHT))
				nRankCurrentLevel++;
			if (GetInput(INPUT_LEFT))
				nRankCurrentLevel--;
#if DEBUG_ROOM_ACTIVE
			if (nRankCurrentLevel < 0)
				nRankCurrentLevel = 0;
#else
			if (nRankCurrentLevel < 1)
				nRankCurrentLevel = 1;
#endif
			if (nRankCurrentLevel >= MAX_ROOMS)
				nRankCurrentLevel = MAX_ROOMS-1;
			if (p_UILevelName) {
				p_UILevelName->SetSpriteSize(rln_RankLevelNames[nRankCurrentLevel].Size);
				p_UILevelName->gpTexture = GetTexture(rln_RankLevelNames[nRankCurrentLevel].Texture);
				p_UILevelName->SetAlpha(1);
			}
			if (fCurrentAlpha == 1)
			{
				static int nAccelerator = 0;
				nAccelerator++;
				XMFLOAT3 fRankTextRot = p_UIRankingText->GetRotation();
				if (fRankTextRot.y > 0)
					fRankTextRot.y -= nAccelerator;
				else
				{
					fRankTextRot.y = 0;
					nAccelerator = 0;
				}
				p_UIRankingText->SetRotation(fRankTextRot);
				for (int i = 0; i < MAX_RANK_POSITIONS; i++) {
					p_UIRankingScores[i]->SetNumberUI(nTopRankingScores[nRankCurrentLevel][i]);
					p_UIRankingScores[i]->SetRotation(fRankTextRot);
				}
				if(p_UILevelName)
					p_UILevelName->SetRotation(fRankTextRot);
			}
			if ((GetInput(GAME_START) || GetInput(INPUT_CONFIRM)) && fCurrentAlpha == 1)
				bRankingScreenOn = false;
			return;
		}
	}
	else {
			if (p_goRankingBackground) {
				float fCurrentAlpha = p_goRankingBackground->GetAlpha();
				if (fCurrentAlpha > 0)
					p_goRankingBackground->SetAlpha(fCurrentAlpha - 0.025f);
				else if (fCurrentAlpha < 0)
					p_goRankingBackground->SetAlpha(0);
				p_goRankingBackground->Update();
				if (fCurrentAlpha != 1)
				{
					static int nAccelerator = 0;
					nAccelerator++;
					XMFLOAT3 fRankTextRot = p_UIRankingText->GetRotation();
					if (fRankTextRot.y < 90)
						fRankTextRot.y += nAccelerator;
					else
					{
						fRankTextRot.y = 90;
						nAccelerator = 0;
					}
					p_UIRankingText->SetRotation(fRankTextRot);
					for (int i = 0; i < MAX_RANK_POSITIONS; i++)
						p_UIRankingScores[i]->SetRotation(fRankTextRot);
					if(p_UILevelName)
						p_UILevelName->SetRotation(fRankTextRot);
				}
			}
			if(p_UILevelName)
				p_UILevelName->SetAlpha(0);
	}
	static int nAcceleration=0;
	if (!bBackGroundLoaded) {
		float f_bgAlpha = p_goMenuBackground->GetColor().w;
		if (f_bgAlpha < 1) {
			f_bgAlpha += 0.025f;
			if (f_bgAlpha > 1)
				f_bgAlpha = 1;
			p_goMenuBackground->SetAlpha(f_bgAlpha);
			bBackGroundLoaded = f_bgAlpha == 1;
		}
	}

	p_goObjDecorationMenu->Update();
	nAcceleration+=10;
	for (int i = 0; i < MAX_OPTIONS; i++) {
		if (nCurrentSelectedOption == i) {
			float currentX = goOptions[i]->GetLocalPosition().x;
			if (currentX < -760)
				currentX+= nAcceleration;
			if (currentX > -760)
				currentX = -760;
			goOptions[i]->SetLocalPosition(currentX, (float)295 - (i * 130));
		}
		else {
			float currentX = goOptions[i]->GetLocalPosition().x;
			if (currentX > -980)
				currentX -= nAcceleration;
			if (currentX < -980)
				currentX = -980;
			goOptions[i]->SetLocalPosition(currentX, (float)295 - (i * 130));
		}
	}
	if (GetInput(INPUT_DOWN)) 
	{
		nAcceleration = 0;
		nCurrentSelectedOption++;
		PlaySound(SOUND_LABEL_SE_SELECTOPTION);
	}
	if (GetInput(INPUT_UP)) 
	{
		nAcceleration = 0;
		nCurrentSelectedOption--;
		PlaySound(SOUND_LABEL_SE_SELECTOPTION);
	}
	if (nCurrentSelectedOption < 0)
		nCurrentSelectedOption = MAX_OPTIONS-1;
	if (nCurrentSelectedOption == MAX_OPTIONS)
		nCurrentSelectedOption = 0;
	if (GetInput(GAME_START) || GetInput(INPUT_CONFIRM))
	{
		switch (nCurrentSelectedOption)
		{
#if DEBUG_ROOM_ACTIVE
		case DEBUG_MENU_OPTION:
			SetLevel(DEBUG_ROOM);
			SetStickerTransition(INGAME_STATE);
			break;
#endif
		case NEW_GAME_START:
			SetLevel(LEVEL1_ROOM);
			SetStickerTransition(INGAME_STATE);
			break;
		case TUTORIAL_START:
			SetLevel(TUTORIAL_ROOM);
			SetStickerTransition(INGAME_STATE);
			break;
		case SHOW_RANKING:
			PlaySound(SOUND_LABEL_SE_ON);
			bRankingScreenOn = true;
			break;
		case END_GAME:
			EndCurrentGame();
			break;
		default: 
			break;
		}
	}
}

void DrawMenuControl()
{
	if(p_goMenuBackground)
		p_goMenuBackground->Draw();
	for (int i = 0; i < MAX_OPTIONS; i++) {
		goOptions[i]->Draw();
	}
	p_goObjDecorationMenu->Draw();

	//ランキングの表示
	if (p_goRankingBackground)
		p_goRankingBackground->Draw();
	if (p_UIRankingText)
		p_UIRankingText->Draw();
	for (int i = 0; i < MAX_RANK_POSITIONS; i++)
	{
		if (p_UIRankingScores[i])
			p_UIRankingScores[i]->Draw();
	}
	if (p_UILevelName)
		p_UILevelName->Draw();
}

void EndMenuControl()
{
	if (p_goMenuBackground) {
		p_goMenuBackground->End();
		delete(p_goMenuBackground);
		p_goMenuBackground = nullptr;
	}	
	if (p_UILevelName) {
		p_UILevelName->End();
		delete(p_UILevelName);
		p_UILevelName = nullptr;
	}
	if (p_goObjDecorationMenu) {
		p_goObjDecorationMenu->End();
		delete(p_goObjDecorationMenu);
		p_goObjDecorationMenu = nullptr;
	}
	for (int i = 0; i < MAX_OPTIONS; i++) {
		if (!goOptions[i]) 
			continue;
		goOptions[i]->End();
		delete(goOptions[i]);
		goOptions[i] = nullptr;
	}
	if (p_UIRankingText)
	{
		p_UIRankingText->End();
		delete(p_UIRankingText);
		p_UIRankingText = nullptr;
	}
	for (int i = 0; i < MAX_RANK_POSITIONS; i++)
	{
		for (int j = 0; j < MAX_ROOMS; j++)
		{
			if (p_UIRankingScores[i])
			{
				p_UIRankingScores[i]->End();
				delete(p_UIRankingScores[i]);
				p_UIRankingScores[i] = nullptr;
			}
		}
	}
	if (p_goRankingBackground)
	{
		p_goRankingBackground->End();
		delete(p_goRankingBackground);
		p_goRankingBackground = nullptr;
	}
}

void AddScoreToTopRankings(int ScoreToAdd, int Level)
{
	
	for (int i = 0; i < MAX_RANK_POSITIONS; i++)
	{
		if (nTopRankingScores[Level][i] < ScoreToAdd) {
			for (int j = MAX_RANK_POSITIONS-1; j > i; j--)
			{
				nTopRankingScores[Level][j] = nTopRankingScores[Level][j - 1];
			}
			nTopRankingScores[Level][i] = ScoreToAdd;
			FILE *pFile = NULL;
#ifdef USE_CONSOLE
			printf("データを書きだします。\n");
#endif
			pFile = fopen(SCORE_FILENAMEPATH, "wb");
			if (pFile)
				fwrite(nTopRankingScores, sizeof(int[MAX_ROOMS][MAX_RANK_POSITIONS]), 1, pFile);
			return;
		}
	}
}
