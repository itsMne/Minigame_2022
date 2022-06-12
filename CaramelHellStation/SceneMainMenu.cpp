//*****************************************************************************
// SceneMainMenu.cpp
// メインメニューを管理する
// Author : Mane
//*****************************************************************************
#include "SceneMainMenu.h"
#include "SceneInGame.h"
#include "TransitionControl.h"
#include "SceneManager.h"
#include "C_DXWindow.h"
#include "GameInputManager.h"
#include "TextureManager.h"
#include <stdio.h>
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCORE_FILENAMEPATH "data/ScoreData.bin"
#define SEPARATION_Y_MENU_OPTIONS 115

int	nTopRankingScores[MAX_ROOMS][MAX_RANK_POSITIONS];
bool bRankingsPrepared = false;

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
SceneMainMenu::SceneMainMenu()
{
	rln_RankLevelNames[0] = { TEXTURE_UI_RANKING_DEBUGROOM_TEXT,{ 309, 54 } };
	rln_RankLevelNames[1] = { TEXTURE_UI_RANKING_TUTORIALROOM_TEXT,{ 347, 72 } };
	rln_RankLevelNames[2] = { TEXTURE_UI_RANKING_LEVEL1ROOM_TEXT,{ 192, 70 } };
	Init();
}

SceneMainMenu::~SceneMainMenu()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void SceneMainMenu::Init()
{
	nRankCurrentLevel = LEVEL1_ROOM;
	nCurrentSelectedOption = 0;
	nCurrentSelectedExtraOption = 0;
	p_goMenuBackground = new GameObject();
	p_goRankingBackground = new GameObject();
	p_goExtraBackground = new GameObject();
	p_UIRankingText = new UI_Object();
	p_UILevelName = new UI_Object();
	for (int i = 0; i < MAX_RANK_POSITIONS; i++)
	{
		if (!bRankingsPrepared) {
			for (int j = 0; j < MAX_ROOMS; j++)
				nTopRankingScores[j][i] = 0;
			FILE* pFile = nullptr; 
			fopen_s(&pFile, SCORE_FILENAMEPATH, "rb");
#ifdef USE_CONSOLE
			printf("スコアデータを読み込みます。\n");
#endif
			if (pFile)
			{
				fread(nTopRankingScores, sizeof(int[MAX_ROOMS][MAX_RANK_POSITIONS]), 1, pFile);
			}
			bRankingsPrepared = true;
		}
		p_UIRankingScores[i] = new UI_Object();
	}

	for (int i = 0; i < MAX_OPTIONS; i++)
		goOptions[i] = new GameObject();	
	for (int i = 0; i < MAX_EXTRA_OPTIONS; i++) {
		goExtraOptions[i] = new GameObject();
		pSubMenuInfo[i] = new GameObject();
		pSubMenuInfo[i]->SetSpriteSize(0, 0);
		pSubMenuInfo[i]->SetAlpha(0);
		pSubMenuInfo[i]->SetUIObject(true);;
	}
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

	p_goExtraBackground->Init(GetTexture(TEXTURE_EXTRA_SCROLLING_BACKGROUND), SCROLLING_BACKGROUND_TYPE);
	p_goExtraBackground->SetSpriteUV(0.5f, 1);
	p_goExtraBackground->SetFramesForUV(2, 1);
	p_goExtraBackground->SetAlpha(0);
	p_goExtraBackground->SetUIObject(true);

	p_UIRankingText->Init(UI_RANKINGSCREEN_RANKINGTEXT);
	p_UIRankingText->SetRotation({ 0,90,0 });

	for (int i = 0; i < MAX_RANK_POSITIONS; i++)
	{
		p_UIRankingScores[i]->Init(UI_RANKING_NUMBERS);
		p_UIRankingScores[i]->SetWorldPosition(-217, (WINDOW_HEIGHT / 2) - 235 - (i*SCORE_RANKNUM_SIZE));
		p_UIRankingScores[i]->SetNumberUI(0);
		p_UIRankingScores[i]->SetRotation({ 0,90,0 });
	}

	bBackGroundLoaded = false;
	bRankingScreenOn = false;
	//オプションの設定
#if DEBUG_ROOM_ACTIVE
	goOptions[DEBUG_MENU_OPTION]->Init(GetTexture(TEXTURE_MENU_DEBUG_ROOM), MAX_GOTYPE);
#endif
	goOptions[NEW_GAME_START]->Init(GetTexture(TEXTURE_MENU_STARTGAME), MAX_GOTYPE);
	goOptions[TUTORIAL_START]->Init(GetTexture(TEXTURE_MENU_TUTORIAL_ROOM), MAX_GOTYPE);
	goOptions[SHOW_RANKING]->Init(GetTexture(TEXTURE_MENU_RANKINGS), MAX_GOTYPE);
	goOptions[EXTRA_OPTIONS]->Init(GetTexture(TEXTURE_MENU_EXTRA_OPTIONS), MAX_GOTYPE);
	goOptions[END_GAME]->Init(GetTexture(TEXTURE_MENU_EXIT), MAX_GOTYPE);

	//他のオプション
	goExtraOptions[SET_NGNL]->Init(GetTexture(TEXTURE_MENU_NSNG_DEACTIVATED), MAX_GOTYPE);
	goExtraOptions[SET_HELLSTOUCH]->Init(GetTexture(TEXTURE_MENU_NSNG_DEACTIVATED), MAX_GOTYPE);
	if (!IsNoStyleNoGameOn()) 
		goExtraOptions[SET_NGNL]->SetTexture(GetTexture(TEXTURE_MENU_NSNG_DEACTIVATED));
	else 
		goExtraOptions[SET_NGNL]->SetTexture(GetTexture(TEXTURE_MENU_NSNG_ACTIVATED));
	if (!ShowFPS())
		goExtraOptions[SET_SHOWFPS]->Init(GetTexture(TEXTURE_MENU_FPS_ACTIVATE), MAX_GOTYPE);
	else
		goExtraOptions[SET_SHOWFPS]->Init(GetTexture(TEXTURE_MENU_FPS_DEACTIVATE), MAX_GOTYPE);

	if (!IsHellsTouchMode())
		goExtraOptions[SET_HELLSTOUCH]->SetTexture(GetTexture(TEXTURE_MENU_HELLSTOUCH_DEACTIVATED));
	else
		goExtraOptions[SET_HELLSTOUCH]->SetTexture(GetTexture(TEXTURE_MENU_HELLSTOUCH_ACTIVATED));

	goExtraOptions[RETURN_PREVIOUS]->Init(GetTexture(TEXTURE_MENU_RETURN), MAX_GOTYPE);

	pSubMenuInfo[SET_NGNL]->Init(GetTexture(TEXTURE_MENU_NSNG_INFO), MAX_GOTYPE);
	pSubMenuInfo[SET_NGNL]->SetSpriteSize(531, 188);
	pSubMenuInfo[SET_NGNL]->SetWorldPosition(350, -250);
	pSubMenuInfo[SET_NGNL]->SetAlpha(0);

	pSubMenuInfo[SET_HELLSTOUCH]->Init(GetTexture(TEXTURE_MENU_HM_INFO), MAX_GOTYPE);
	pSubMenuInfo[SET_HELLSTOUCH]->SetSpriteSize(310, 41);
	pSubMenuInfo[SET_HELLSTOUCH]->SetWorldPosition(350, -250);
	pSubMenuInfo[SET_HELLSTOUCH]->SetAlpha(0);

	pSubMenuInfo[SET_SHOWFPS]->Init(nullptr, MAX_GOTYPE);
	pSubMenuInfo[SET_SHOWFPS]->SetSpriteSize(0, 0);
	for (int i = 0; i < MAX_OPTIONS; i++) {
		goOptions[i]->SetUIObject(true);
		goOptions[i]->SetLocalPosition(-960, (float)295 - (i * SEPARATION_Y_MENU_OPTIONS));
		goOptions[i]->SetSpriteSize(1571, 106);
	}

	for (int i = 0; i < MAX_EXTRA_OPTIONS; i++) {
		goExtraOptions[i]->SetUIObject(true);
		goExtraOptions[i]->SetLocalPosition(-960, (float)295 - (i * SEPARATION_Y_MENU_OPTIONS));
		goExtraOptions[i]->SetSpriteSize(1571, 106);
		goExtraOptions[i]->SetAlpha(0);
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
	bExtraOptionsScreenOn = false;
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void SceneMainMenu::Update()
{
	p_goMenuBackground->Update();
	//エクストラオプション
	if (bExtraOptionsScreenOn)
	{
		float fCurrentAlpha = p_goExtraBackground->GetAlpha();
		if (fCurrentAlpha < 1) {
			p_goExtraBackground->RiseAlpha(0.025f);
			for (int i = 0; i < MAX_EXTRA_OPTIONS; i++) {
				goExtraOptions[i]->RiseAlpha(0.025f);
				pSubMenuInfo[i]->RiseAlpha(0.025f);
			}
		}
		else {
			static int nAcceleration = 0;
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
			nAcceleration += 10;
			for (int i = 0; i < MAX_EXTRA_OPTIONS; i++) {
				if (nCurrentSelectedExtraOption == i) {
					float currentX = goExtraOptions[i]->GetLocalPosition().x;
					if (currentX < -750)
						currentX += nAcceleration;
					if (currentX > -750)
						currentX = -750;
					goExtraOptions[i]->SetLocalPosition(currentX, (float)295 - (i * SEPARATION_Y_MENU_OPTIONS));
				}
				else {
					float currentX = goExtraOptions[i]->GetLocalPosition().x;
					if (currentX > -980)
						currentX -= nAcceleration;
					if (currentX < -980)
						currentX = -980;
					goExtraOptions[i]->SetLocalPosition(currentX, (float)295 - (i * SEPARATION_Y_MENU_OPTIONS));
				}
			}
			if (GetInput(INPUT_DOWN))
			{
				nAcceleration = 0;
				nCurrentSelectedExtraOption++;
				PlaySound(SOUND_LABEL_SE_SELECTOPTION);
			}
			if (GetInput(INPUT_UP))
			{
				nAcceleration = 0;
				nCurrentSelectedExtraOption--;
				PlaySound(SOUND_LABEL_SE_SELECTOPTION);
			}
			if (nCurrentSelectedExtraOption < 0)
				nCurrentSelectedExtraOption = MAX_EXTRA_OPTIONS - 1;
			if (nCurrentSelectedExtraOption == MAX_EXTRA_OPTIONS)
				nCurrentSelectedExtraOption = 0;
			if (GetInput(GAME_START) || GetInput(INPUT_CONFIRM))
			{
				switch (nCurrentSelectedExtraOption)
				{
				case SET_NGNL:
					PlaySound(SOUND_LABEL_SE_ON);
					if (IsNoStyleNoGameOn()) {
						SetNoStyleNoGame(false);
						goExtraOptions[SET_NGNL]->SetTexture(GetTexture(TEXTURE_MENU_NSNG_DEACTIVATED));
					}
					else {
						SetNoStyleNoGame(true);
						goExtraOptions[SET_NGNL]->SetTexture(GetTexture(TEXTURE_MENU_NSNG_ACTIVATED));
					}
					break;
				case SET_SHOWFPS:
					PlaySound(SOUND_LABEL_SE_ON);
					if (ShowFPS()) {
						SetShowFPS(false);
						goExtraOptions[SET_SHOWFPS]->SetTexture(GetTexture(TEXTURE_MENU_FPS_ACTIVATE));
					}
					else {
						SetShowFPS(true);
						goExtraOptions[SET_SHOWFPS]->SetTexture(GetTexture(TEXTURE_MENU_FPS_DEACTIVATE));
					}
					break;
				case SET_HELLSTOUCH:
					PlaySound(SOUND_LABEL_SE_ON);
					if (IsHellsTouchMode()) {
						SetHellsMode(false);
						goExtraOptions[SET_HELLSTOUCH]->SetTexture(GetTexture(TEXTURE_MENU_HELLSTOUCH_DEACTIVATED));
					}
					else {
						SetHellsMode(true);
						goExtraOptions[SET_HELLSTOUCH]->SetTexture(GetTexture(TEXTURE_MENU_HELLSTOUCH_ACTIVATED));
					}
					break;
				case RETURN_PREVIOUS:
					bExtraOptionsScreenOn = false;
					break;
				default:
					break;
				}
			}
		}
		p_goExtraBackground->Update();
		return;
	}
	else {
		float fCurrentAlpha = p_goExtraBackground->GetAlpha();
		if (fCurrentAlpha > 0) {
			p_goExtraBackground->RiseAlpha(-0.025f);
			for (int i = 0; i < MAX_EXTRA_OPTIONS; i++) 
			{
				pSubMenuInfo[i]->RiseAlpha(-0.025f);
				goExtraOptions[i]->RiseAlpha(-0.025f);
			}
		}
		if (p_goExtraBackground->GetAlpha() == 0) {
			nCurrentSelectedExtraOption = 0;
			for (int i = 0; i < MAX_EXTRA_OPTIONS; i++) {
				if (nCurrentSelectedExtraOption == i) {
					float currentX = goExtraOptions[i]->GetLocalPosition().x;
					currentX = -750;
					goExtraOptions[i]->SetLocalPosition(currentX, (float)295 - (i * SEPARATION_Y_MENU_OPTIONS));
				}
				else {
					float currentX = goExtraOptions[i]->GetLocalPosition().x;
					currentX = -980;
					goExtraOptions[i]->SetLocalPosition(currentX, (float)295 - (i * SEPARATION_Y_MENU_OPTIONS));
				}
			}
		}

	}

	//ランキング
	if (bRankingScreenOn) {
		if (p_goRankingBackground) {
			float fCurrentAlpha = p_goRankingBackground->GetAlpha();
			if (fCurrentAlpha < 1)
				p_goRankingBackground->RiseAlpha(0.025f);
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
				nRankCurrentLevel = MAX_ROOMS - 1;
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
				if (p_UILevelName)
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
				p_goRankingBackground->RiseAlpha(-0.025f);
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
				if (p_UILevelName)
					p_UILevelName->SetRotation(fRankTextRot);
			}
		}
		if (p_UILevelName)
			p_UILevelName->SetAlpha(0);
	}
	static int nAcceleration = 0;
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
	nAcceleration += 10;
	for (int i = 0; i < MAX_OPTIONS; i++) {
		if (nCurrentSelectedOption == i) {
			float currentX = goOptions[i]->GetLocalPosition().x;
			if (currentX < -760)
				currentX += nAcceleration;
			if (currentX > -760)
				currentX = -760;
			goOptions[i]->SetLocalPosition(currentX, (float)295 - (i * SEPARATION_Y_MENU_OPTIONS));
		}
		else {
			float currentX = goOptions[i]->GetLocalPosition().x;
			if (currentX > -980)
				currentX -= nAcceleration;
			if (currentX < -980)
				currentX = -980;
			goOptions[i]->SetLocalPosition(currentX, (float)295 - (i * SEPARATION_Y_MENU_OPTIONS));
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
		nCurrentSelectedOption = MAX_OPTIONS - 1;
	if (nCurrentSelectedOption == MAX_OPTIONS)
		nCurrentSelectedOption = 0;
	if (GetInput(GAME_START) || GetInput(INPUT_CONFIRM))
	{
		switch (nCurrentSelectedOption)
		{
#if DEBUG_ROOM_ACTIVE
		case DEBUG_MENU_OPTION:
			SetGameLevel(DEBUG_ROOM);
			SetStickerTransition(INGAME_STATE);
			break;
#endif
		case NEW_GAME_START:
			SetGameLevel(LEVEL1_ROOM);
			SetStickerTransition(INGAME_STATE);
			break;
		case TUTORIAL_START:
			SetGameLevel(TUTORIAL_ROOM);
			SetStickerTransition(INGAME_STATE);
			break;
		case SHOW_RANKING:
			PlaySound(SOUND_LABEL_SE_ON);
			bRankingScreenOn = true;
			break;
		case EXTRA_OPTIONS:
			PlaySound(SOUND_LABEL_SE_ON);
			bExtraOptionsScreenOn = true;
			break;
		case END_GAME:
			EndCurrentGame();
			break;
		default:
			break;
		}
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void SceneMainMenu::Draw()
{
	if (p_goMenuBackground)
		p_goMenuBackground->Draw();
	for (int i = 0; i < MAX_OPTIONS; i++) {
		if(goOptions[i])
			goOptions[i]->Draw();
	}
	if(p_goObjDecorationMenu)
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
	
	if (p_goExtraBackground)
		p_goExtraBackground->Draw();

	for (int i = 0; i < MAX_EXTRA_OPTIONS; i++) {
		if(goExtraOptions[i])
			goExtraOptions[i]->Draw();
	}
	if(pSubMenuInfo[nCurrentSelectedExtraOption])
		pSubMenuInfo[nCurrentSelectedExtraOption]->Draw();
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void SceneMainMenu::End()
{
	if (p_goMenuBackground) {
		p_goMenuBackground->End();
		SAFE_DELETE(p_goMenuBackground);
	}
	if (p_UILevelName) {
		p_UILevelName->End();
		SAFE_DELETE(p_UILevelName);
	}
	if (p_goObjDecorationMenu) {
		p_goObjDecorationMenu->End();
		SAFE_DELETE(p_goObjDecorationMenu);
	}
	for (int i = 0; i < MAX_OPTIONS; i++) {
		if (!goOptions[i])
			continue;
		goOptions[i]->End();
		SAFE_DELETE(goOptions[i]);
	}
	if (p_UIRankingText)
	{
		p_UIRankingText->End();
		SAFE_DELETE(p_UIRankingText);
	}
	for (int i = 0; i < MAX_RANK_POSITIONS; i++)
	{
		for (int j = 0; j < MAX_ROOMS; j++)
		{
			if (p_UIRankingScores[i])
			{
				p_UIRankingScores[i]->End();
				SAFE_DELETE(p_UIRankingScores[i]);
			}
		}
	}
	if (p_goRankingBackground)
	{
		p_goRankingBackground->End();
		SAFE_DELETE(p_goRankingBackground);
	}	
	
	if (p_goExtraBackground)
	{
		p_goExtraBackground->End();
		SAFE_DELETE(p_goExtraBackground);
	}

	for (int i = 0; i < MAX_EXTRA_OPTIONS; i++) {
		goExtraOptions[i]->End();
		pSubMenuInfo[i]->End();
		SAFE_DELETE(goOptions[i]);
		SAFE_DELETE(pSubMenuInfo[i]);
	}
}

//*****************************************************************************
//AddScoreToTopRankings関数
//ランキングにスコアを入れる
//引数：int, int
//戻：void
//*****************************************************************************
void AddScoreToTopRankings(int ScoreToAdd, int Level)
{
	for (int i = 0; i < MAX_RANK_POSITIONS; i++)
	{
		if (nTopRankingScores[Level][i] < ScoreToAdd) {
			for (int j = MAX_RANK_POSITIONS - 1; j > i; j--)
			{
				nTopRankingScores[Level][j] = nTopRankingScores[Level][j - 1];
			}
			nTopRankingScores[Level][i] = ScoreToAdd;
			FILE *pFile = NULL;
			fopen_s(&pFile, SCORE_FILENAMEPATH, "wb");
			if (pFile) {
				fwrite(nTopRankingScores, sizeof(int[MAX_ROOMS][MAX_RANK_POSITIONS]), 1, pFile);
#ifdef USE_CONSOLE
				printf("データを書きだします: SCORE: %d LEVEL: %d。\n", nTopRankingScores[Level][i], Level);
#endif
			}
			return;
		}
	}
}

