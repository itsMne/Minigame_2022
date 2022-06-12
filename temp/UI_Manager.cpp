//*****************************************************************************
// UI_Manager.cpp
// UIの管理
// Author : Mane
//*****************************************************************************
#include "UI_Manager.h"
#include "UI_Object.h"
#include "Player.h"
#include "RankManager.h"
#include "SceneInGame.h"
#include "EventHandler.h"
#include "GameInputManager.h"
#include "Enemy.h"
#include "Camera.h"
#include <stdio.h>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define VERTICAL_HEARTS			false
#define SCORE_ABOVE_RANK		true

//*****************************************************************************
// グローバル変数
//*****************************************************************************
XMFLOAT2		x2HeartPosition;
Player*			pPlayer =			nullptr;
UI_Object*		Hearts =			nullptr;
UI_Object*		Rank =				nullptr;
UI_Object*		Score =				nullptr;
UI_Object*		StartEventMessage =	nullptr;
UI_Object*		EndEventMessage =	nullptr;
UI_Object*		InEventNotice =		nullptr;
UI_Object*		ResultsScreen =		nullptr;
UI_Object*		StageClearScreen =	nullptr;
UI_Object*		TutorialScreen =	nullptr;
UI_Object*		TutorialArrow =		nullptr;
bool bFightTutorialOn;
Enemy* TutorialEnemy;
//*****************************************************************************
//InitUIManager関数
//初期化の関数
//引数：void
//戻：void
//*****************************************************************************
void InitUIManager()
{
	SceneInGame* pGame = GetCurrentGame();
	pPlayer = GetPlayerPointer();
	x2HeartPosition = { -600,320 };
	bFightTutorialOn = false;
	TutorialEnemy = nullptr;
	if (!Hearts && pGame->GetCurrentLevel()!=TUTORIAL_ROOM)
		Hearts = new UI_Object();
	if (!Rank)
		Rank = new UI_Object();
	if (!Score)
		Score = new UI_Object();
	if (!StartEventMessage)
		StartEventMessage = new UI_Object();
	if (!EndEventMessage)
		EndEventMessage = new UI_Object();
	if (!InEventNotice)
		InEventNotice = new UI_Object();
	if (!ResultsScreen)
		ResultsScreen = new UI_Object();
	if (!StageClearScreen)
		StageClearScreen = new UI_Object();
	if (!TutorialScreen)
	{
		if (pGame->GetCurrentLevel() == TUTORIAL_ROOM)
		{
			TutorialScreen = new UI_Object();
			TutorialScreen->Init(UI_TUTORIAL_SCREEN);
		}
	}
	if (!TutorialArrow)
	{
		if (pGame->GetCurrentLevel() == TUTORIAL_ROOM)
		{
			TutorialArrow = new UI_Object();
			TutorialArrow->Init(UI_ARROW);
			TutorialArrow->SetPositionArrow({90, (WINDOW_HEIGHT/2)-70});
		}
	}
	if(Hearts)
		Hearts->Init(UI_HEART);
	if(Rank) 
		Rank->Init(UI_RANK);
	if(Score)
		Score->Init(UI_SCORE_NUMBERS);
	if(StartEventMessage)
		StartEventMessage->Init(UI_STARTEVENT_MESSAGE);
	if(EndEventMessage)
		EndEventMessage->Init(UI_ENDEVENT_MESSAGE);
	if(InEventNotice)
		InEventNotice->Init(UI_INEVENT_NOTICE);
	if(ResultsScreen)
		ResultsScreen->Init(UI_RESULTS_SCREEN);
	if(StageClearScreen)
		StageClearScreen->Init(UI_STAGECLEAR_RESULTS);
}

//*****************************************************************************
//UpdateUIManager関数
//アップデート処理の関数
//引数：void
//戻：void
//*****************************************************************************
void UpdateUIManager()
{
	SceneInGame* pGame = GetCurrentGame();
	if (!pPlayer) {
		pPlayer = GetPlayerPointer();
		if(!pPlayer)
			return;
	}
	if (Hearts)
		Hearts->Update();
	if (Rank) {
		Rank->Update();
		int nCurrentRank = GetRank();
		int nCurrentRankFrames = GetRankFrames();
		Rank->SetRank(nCurrentRank);
		float fRankColor = (nCurrentRankFrames / (float)(nCurrentRank*(MAX_FRAMES_FOR_RANKINGUP/2)))+0.10f;
		if (fRankColor > 1.15f)
			fRankColor = 1.15f;
#if USE_RANK_COLOR
		Rank->SetColor(fRankColor, fRankColor, fRankColor);
#else
		Rank->SetColor(1, 1, 1);
#endif
	}
	if (Score) {
		Score->Update();
		Score->SetNumberUI(pGame->GetScore());
	}
	if (StartEventMessage)
		StartEventMessage->Update();
	if (EndEventMessage)
		EndEventMessage->Update();
	if (InEventNotice)
		InEventNotice->Update();
	if (ResultsScreen)
		ResultsScreen->Update();
	if (StageClearScreen)
		StageClearScreen->Update();
	TutorialControl();
}

void TutorialControl()
{
	SceneInGame* pGame = GetCurrentGame();
	if (!pGame)
		return;
	if (!TutorialScreen || !TutorialArrow)
		return;
	Player* pPlayer = GetPlayerPointer();
	if (!pPlayer)
		return;

	static int FramePos;
	static int FrameMessage;
	static float fAccelerationZoom;
	static bool bFirstSuperArmorBroken;
	static bool bEnemyIntroductionOver;
	static bool bEnemyFirstSuperArmorMessage;
	static bool bEnemySuperArmorMessageOver;
	static int FramePosSuperArmorBroke;

	int nMessageToShow = TS_MAX;
	if (!bFightTutorialOn) {
		FramePosSuperArmorBroke = 0;
		FramePos = 0;
		fAccelerationZoom = 0;
		FrameMessage = 0;
		bFirstSuperArmorBroken = false;
		bEnemyIntroductionOver = false;
		bEnemyFirstSuperArmorMessage = false;
		bEnemySuperArmorMessageOver = false;
		if (IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), { -5.000f, -29.000f, 1427.000f, 600.000f }))
			nMessageToShow = TS_MOVEMENT;
		if (IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), { 1622.000f,236.000f,1537.000f,1560.000f }))
			nMessageToShow = TS_JUMP;
		if (IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), { 2834.000f,1488.000f,506.000f,2761.000f }))
			nMessageToShow = TS_PUNCH;
		if (IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), { 3776.190f,547.080f,810.000f,462.000f }))
			nMessageToShow = TS_SLIDE;
		if (IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), { 6146.190f,728.080f,1170.000f,1080.000f }))
			nMessageToShow = TS_GUITARJUMP;
		if (IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), { 10377.189f,3627.870f,3735.000f,1725.000f }))
			nMessageToShow = TS_DOUBLEJUMP;
		if (IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), { 7945.000f,8461.919f,1075.000f,1155.000f }))
			nMessageToShow = TS_SWITCHPUNCH;
		if (IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), { 7935.000f,9961.000f,945.000f,1440.000f }))
			nMessageToShow = TS_SWITCHSHOOT;
		if (IsInCollision(pPlayer->GetPlayerHitbox(HB_BODY), { 3918.000f,3880.000f,1695.000f,2130.000f }) && !TutorialEnemy)
			bFightTutorialOn = true;
		if (GetCurrentActiveEvent() != nullptr)
			nMessageToShow = TS_EVENT;
	}
	else {
		if (!TutorialEnemy) {
			TutorialEnemy = GetEventEnemy(ENEMY_TUTORIAL1);
			return;
		}
		bFightTutorialOn = TutorialEnemy->GetEnemyHP() > 0;
		float fCurrentScale = pGame->GetLevelScale();
		if (!bEnemyIntroductionOver) {
			if (GetFocalPoint() == TutorialEnemy)
			{
				if (fCurrentScale < 1.35f) {
					fAccelerationZoom += 0.005f;
					SetCameraAdjustment({ 60,-15, 0 });
					pGame->SetLevelScale(fCurrentScale + fAccelerationZoom);
					if (fCurrentScale >= 1.35f)
					{
						fCurrentScale = 1.35f;
						fAccelerationZoom = 0;
					}
				}
			}
			else {
				if (fCurrentScale > 0.9f) {
					fAccelerationZoom -= 0.005f;
					SetCameraAdjustment({ 0, 0, 0 });
					fCurrentScale = fCurrentScale + fAccelerationZoom;
					if (fCurrentScale <= 0.9f) {
						fCurrentScale = 0.9f;
						bEnemyIntroductionOver = true;
						fAccelerationZoom = 0;
					}
					pGame->SetLevelScale(fCurrentScale);
				}
			}
			if (TutorialEnemy->GetIntroLogoGameObject()) {
				PausePlayerUpdate(true);
				SetFocalPoint(TutorialEnemy);
			}
			else {
				PausePlayerUpdate(false);
				SetFocalPoint(GetPlayerPointer());
			}
		}

		if(FramePos==0)
			nMessageToShow = TS_ENEMY;
		if (TutorialEnemy->GetEnemyHP() < MAX_BUN_BUN_HP)
		{
			FramePos++;
			if (!bEnemyFirstSuperArmorMessage) {
				if (FrameMessage < 250) {
					if (FramePos > 2)
					{
						PausePlayerUpdate(true);
						pPlayer->SetDrawWeapon(false);
						PauseEnemyUpdate(true);
						SetFocalPoint(TutorialEnemy);
						if (fCurrentScale < 2) {
							fAccelerationZoom += 0.005f;
							pGame->SetLevelScale(fCurrentScale + fAccelerationZoom);
						}
						else {
							fAccelerationZoom = 0;
							nMessageToShow = TS_ENEMYSUPERARMOR;
							FrameMessage++;
							TutorialArrow->SetArrowActivation(true);
							if (FrameMessage > 120) {
								nMessageToShow = TS_ENEMYSUPERARMOR2;
							}
						}
					}
				}
				else
				{
					if (fCurrentScale > 0.90f) {
						fAccelerationZoom += 0.005f;
						pGame->SetLevelScale(fCurrentScale - fAccelerationZoom);
						if (pGame->GetLevelScale() < 0.90f)
							pGame->SetLevelScale(0.90f);
					}
					if (fCurrentScale == 0.90f) {
						PausePlayerUpdate(false);
						pPlayer->SetDrawWeapon(true);
						PauseEnemyUpdate(false);
						TutorialArrow->SetArrowActivation(false);
						SetFocalPoint(GetPlayerPointer());
						fAccelerationZoom = 0;
						nMessageToShow = TS_ENEMYSUPERARMOR3;
					}
				}
			}
			if (!bFirstSuperArmorBroken)
				bFirstSuperArmorBroken = TutorialEnemy->IsSuperArmorBroken();
			if (bFirstSuperArmorBroken) {
				
				FramePosSuperArmorBroke++;
				if (FramePosSuperArmorBroke > 10) {
					bEnemyFirstSuperArmorMessage = true;
					SetFocalPoint(TutorialEnemy);
					PausePlayerUpdate(true);
					pPlayer->SetDrawWeapon(false);
					PauseEnemyUpdate(true);
					nMessageToShow = TS_ENEMYSUPERARMOR4;
					if (!bEnemySuperArmorMessageOver) {
						fAccelerationZoom += 0.005f;
						SetCameraAdjustment({ 0,-30, 0 });
						if (fCurrentScale >= 1.35f)
						{
							fCurrentScale = 1.35f;
							fAccelerationZoom = 0;
						}
						pGame->SetLevelScale(fCurrentScale + fAccelerationZoom);
					}

				}
				if (FramePosSuperArmorBroke > 130) {
					nMessageToShow = TS_ENEMYSUPERARMOR5;
				}
				if (FramePosSuperArmorBroke > 240)
				{
					SetFocalPoint(GetPlayerPointer());
					PausePlayerUpdate(false);
					pPlayer->SetDrawWeapon(true);
					PauseEnemyUpdate(false);
					if (!bEnemySuperArmorMessageOver) {
						fAccelerationZoom -= 0.005f;
						SetCameraAdjustment({ 0, 0, 0 });
						if (fCurrentScale < 0.9f)
						{
							fCurrentScale = 0.9f;
							fAccelerationZoom = 0;
							bEnemySuperArmorMessageOver = true;
						}
						if (fCurrentScale != 0.9f)
							pGame->SetLevelScale(fCurrentScale + fAccelerationZoom);
					}
				}
				
			}
		}
		if (TutorialEnemy->GetEnemyHP() == 0) {
			FramePosSuperArmorBroke = 0;
			FramePos = 0;
			fAccelerationZoom = 0;
			FrameMessage = 0;
			bFirstSuperArmorBroken = false;
		}
	}
	TutorialScreen->SetNewTutorialMessage(nMessageToShow);
	TutorialScreen->Update();
	TutorialArrow->Update();
}

//*****************************************************************************
//DrawUIManager関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void DrawUIManager()
{
	if (!pPlayer)
		return;
	int nCurrentHealth, nMaxHealth;
	nCurrentHealth = pPlayer->GetHP();
	nMaxHealth = pPlayer->GetMaxHP();
	if (Hearts) {
		for (int i = 0; i < nMaxHealth; i++)
		{
			if (i >= nCurrentHealth)
				Hearts->SetHeartActive(false);
			else
				Hearts->SetHeartActive(true);
			if(VERTICAL_HEARTS)
				Hearts->SetWorldPosition(x2HeartPosition.x + i * (55), x2HeartPosition.y);
			else
				Hearts->SetWorldPosition(x2HeartPosition.x, x2HeartPosition.y - i * (55));
			Hearts->Draw();
		}
	}
	if(Rank)
		Rank->Draw();
	if (Score) {
		if(SCORE_ABOVE_RANK)
			Score->SetWorldPosition(490, 339);
		else
			Score->SetWorldPosition(490, 209);
		if(GetRank()>1)
			Score->Draw();
	}
	if (TutorialScreen)
		TutorialScreen->Draw();
	if (TutorialArrow)
		TutorialArrow->Draw();
	if (StartEventMessage)
		StartEventMessage->Draw();
	if (EndEventMessage)
		EndEventMessage->Draw();
	if (InEventNotice)
		InEventNotice->Draw();
	if (ResultsScreen)
		ResultsScreen->Draw();
	if (StageClearScreen)
		StageClearScreen->Draw();
}

//*****************************************************************************
//EndUIManager関数
//処理の終了関数
//引数：void
//戻：void
//*****************************************************************************
void EndUIManager()
{
	if (Hearts) {
		Hearts->End();
		delete(Hearts);
		Hearts = nullptr;
	}
	if (Rank) {
		Rank->End();
		delete(Rank);
		Rank = nullptr;
	}
	if (Score)
	{
		Score->End();
		delete(Score);
		Score = nullptr;
	}
	if (StartEventMessage)
	{
		StartEventMessage->End();
		delete(StartEventMessage);
		StartEventMessage = nullptr;
	}
	if (EndEventMessage)
	{
		EndEventMessage->End();
		delete(EndEventMessage);
		EndEventMessage = nullptr;
	}
	if (InEventNotice)
	{
		InEventNotice->End();
		delete(InEventNotice);
		InEventNotice = nullptr;
	}
	if (ResultsScreen)
	{
		ResultsScreen->End();
		delete(ResultsScreen);
		ResultsScreen = nullptr;
	}
	if (StageClearScreen)
	{
		StageClearScreen->End();
		delete(StageClearScreen);
		StageClearScreen = nullptr;
	}
	if (TutorialScreen)
	{
		TutorialScreen->End();
		delete(TutorialScreen);
		TutorialScreen = nullptr;
	}	
	if (TutorialArrow)
	{
		TutorialArrow->End();
		delete(TutorialArrow);
		TutorialArrow = nullptr;
	}
	pPlayer = nullptr;
}

//*****************************************************************************
//SetStartEventMessageOn関数
//イベントが始まった時に、メッセージを表す
//引数：void
//戻：void
//*****************************************************************************
void SetStartEventMessageOn()
{
	if (StartEventMessage)
		StartEventMessage->SetMessageOn();
}

//*****************************************************************************
//SetEndEventMessageOn関数
//イベントが終わった時に、結果を出す
//引数：char, char, char, char
//戻：void
//*****************************************************************************
void SetEndEventMessageOn(char Time, char Combo, char Damage, char Overrall)
{
	if (EndEventMessage)
		EndEventMessage->SetMessageOn(Time, Combo, Damage, Overrall);
}

//*****************************************************************************
//SetOnAirMessageUse関数
//イベントの間に、小さいメッセージを表す
//引数：bool
//戻：void
//*****************************************************************************
void SetOnAirMessageUse(bool use)
{
	if (!InEventNotice)
		return;
	if (use)
		InEventNotice->SetMessageOn();
	else
		InEventNotice->SetMessageOff();
}

//*****************************************************************************
//SetResultsScreenOn関数
//結果表示を表す
//引数：char, char, char, char
//戻：void
//*****************************************************************************
void SetResultsScreenOn(char Time, char Combo, char Damage, char Overrall)
{
	if (ResultsScreen)
		ResultsScreen->SetMessageOn(Time, Combo, Damage, Overrall);
}

//*****************************************************************************
//ActivateFinalResults関数
//ステージが終わった時に、結果を表す
//引数：void
//戻：void
//*****************************************************************************
void ActivateFinalResults()
{
	if (StageClearScreen)
		StageClearScreen->ActivateFinalResultsScreen();
}

//*****************************************************************************
//IsResultScreenUp関数
//結果表示が見えることを確認する
//引数：void
//戻：void
//*****************************************************************************
bool IsResultScreenUp()
{
	if (StageClearScreen)
		return StageClearScreen->IsResultScreenSetUp();
	else
		return false;
}

