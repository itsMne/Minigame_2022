//*****************************************************************************
// UIManager.cpp
// UIを管理する
// Author : Mane
//*****************************************************************************
#include "UIManager.h"
#include "RankManager.h"
#include "SceneInGame.h"
#include "EventHandler.h"
#include "UniversalVariables.h"
#include "Camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define VERTICAL_HEARTS			false
#define SCORE_ABOVE_RANK		true

static UI_Object*		InEventNotice;
static UI_Object*		ResultsScreen;
static UI_Object*		StageClearScreen;
static UI_Object*		StartEventMessage;
static UI_Object*		EndEventMessage;

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
UIManager::UIManager()
{
	pPlayer = nullptr;
	Hearts = nullptr;
	Rank = nullptr;
	Score = nullptr;
	TutorialScreen = nullptr;
	TutorialArrow = nullptr;
	AtkZoom = nullptr;
	EditorInstructionsA = nullptr;
	EditorInstructionsB = nullptr;
	StageClearScreen = nullptr;
	StartEventMessage = nullptr;
	EndEventMessage = nullptr;
	ResultsScreen = nullptr;
	InEventNotice = nullptr;
	TextEditorLevelSavedLoadedMessage = nullptr;
	Init();
}

UIManager::~UIManager()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void UIManager::Init()
{
	SceneInGame* pGame = GetCurrentGame();
	pPlayer = GetPlayerPointer();
	x2HeartPosition = { -600,320 };
	bFightTutorialOn = false;
	TutorialEnemy = nullptr;
	Hearts = new UI_Object(UI_HEART);
	Rank = new UI_Object(UI_RANK);
	Score = new UI_Object(UI_SCORE_NUMBERS);
	StartEventMessage = new UI_Object(UI_STARTEVENT_MESSAGE);
	EndEventMessage = new UI_Object(UI_ENDEVENT_MESSAGE);
	InEventNotice = new UI_Object(UI_INEVENT_NOTICE);
	ResultsScreen = new UI_Object(UI_RESULTS_SCREEN);
	StageClearScreen = new UI_Object(UI_STAGECLEAR_RESULTS);
	AtkZoom = new UI_Object(UI_ATK_EFFECT);
	EditorSlot = new UI_Object(UI_EDITORSLOT);
	EditorSpeed = new UI_Object(UI_EDITORSPEED);
	EditorDelay = new UI_Object(UI_EDITORDELAY);
	TextEditorSlot = new UI_Object(UI_EDITORSLOT_TEXT);
	TextEditorSpeed = new UI_Object(UI_EDITORSPEED_TEXT);
	TextEditorDelay = new UI_Object(UI_EDITORDELAY_TEXT);
	TextEditorLevelSavedLoadedMessage = new UI_Object(UI_LEVEL_LOADEDSAVED_TEXT);
	if(pGame->GetCurrentLevel()==DEBUG_ROOM)
		EditorInstructionsA = new UI_Object(UI_EDITOR_INSTRUCTIONS_A);
	EditorInstructionsB = new UI_Object(UI_EDITOR_INSTRUCTIONS_B);
	if (!TutorialScreen)
	{
		if (pGame->GetCurrentLevel() == TUTORIAL_ROOM)
			TutorialScreen = new UI_Object(UI_TUTORIAL_SCREEN);
	}
	if (!TutorialArrow)
	{
		if (pGame->GetCurrentLevel() == TUTORIAL_ROOM)
		{
			TutorialArrow = new UI_Object(UI_ARROW);
			TutorialArrow->SetPositionArrow({ 90, (WINDOW_HEIGHT / 2) - 70 });
		}
	}
#ifdef USE_CONSOLE
	printf("INITED UI_MANAGER\n");
#endif
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void UIManager::Update()
{
	SceneInGame* pGame = GetCurrentGame();
	if (!pPlayer) {
		pPlayer = GetPlayerPointer();
		if (!pPlayer)
			return;
	}
	if (Hearts)
		Hearts->Update();
	if (Rank) {
		Rank->Update();
		int nCurrentRank = GetRank();
		int nCurrentRankFrames = GetRankFrames();
		Rank->SetRank(nCurrentRank);
		float fRankColor = (nCurrentRankFrames / (float)(nCurrentRank*(MAX_FRAMES_FOR_RANKINGUP / 2))) + 0.10f;
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
	if (AtkZoom)
		AtkZoom->Update();
	if (EditorInstructionsA)
		EditorInstructionsA->Update();
	if (EditorInstructionsB)
		EditorInstructionsB->Update();
	if (EditorSlot)
		EditorSlot->Update();	
	if (EditorSpeed)
		EditorSpeed->Update();	
	if (EditorDelay)
		EditorDelay->Update();	
	if (TextEditorSlot)
		TextEditorSlot->Update();	
	if (TextEditorSpeed)
		TextEditorSpeed->Update();	
	if (TextEditorDelay)
		TextEditorDelay->Update();
	if (TextEditorLevelSavedLoadedMessage)
		TextEditorLevelSavedLoadedMessage->Update();
	TutorialControl();
}

//*****************************************************************************
//TutorialControl関数
//チュートリアルのメッセージを管理する
//引数：void
//戻：void
//*****************************************************************************
void UIManager::TutorialControl()
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

		if (FramePos == 0)
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
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void UIManager::Draw()
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
			if (VERTICAL_HEARTS)
				Hearts->SetWorldPosition(x2HeartPosition.x + i * (55), x2HeartPosition.y);
			else
				Hearts->SetWorldPosition(x2HeartPosition.x, x2HeartPosition.y - i * (55));
			Hearts->Draw();
		}
	}
	if (Rank)
		Rank->Draw();
	if (Score) {
		if (SCORE_ABOVE_RANK)
			Score->SetWorldPosition(490, 339);
		else
			Score->SetWorldPosition(490, 209);
		if (GetRank() > 1)
			Score->Draw();
	}
	if (AtkZoom)
		AtkZoom->Draw();
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
	if (EditorInstructionsA)
		EditorInstructionsA->Draw();
	if (EditorInstructionsB)
		EditorInstructionsB->Draw();
	if (ResultsScreen)
		ResultsScreen->Draw();
	if (StageClearScreen)
		StageClearScreen->Draw();
	if (EditorSlot)
		EditorSlot->Draw();	
	if (EditorSpeed)
		EditorSpeed->Draw();	
	if (EditorDelay)
		EditorDelay->Draw();
	if (TextEditorSlot)
		TextEditorSlot->Draw();
	if (TextEditorSpeed)
		TextEditorSpeed->Draw();	
	if (TextEditorDelay)
		TextEditorDelay->Draw();	
	if (TextEditorLevelSavedLoadedMessage)
		TextEditorLevelSavedLoadedMessage->Draw();
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void UIManager::End()
{
	SAFE_DELETE(Rank);
	SAFE_DELETE(Score);
	SAFE_DELETE(Hearts);
	SAFE_DELETE(StartEventMessage);
	SAFE_DELETE(EndEventMessage);
	SAFE_DELETE(InEventNotice);
	SAFE_DELETE(ResultsScreen);
	SAFE_DELETE(StageClearScreen);
	SAFE_DELETE(TutorialScreen);
	SAFE_DELETE(TutorialArrow);
	SAFE_DELETE(AtkZoom);
	SAFE_DELETE(EditorInstructionsA);
	SAFE_DELETE(EditorInstructionsB);
	SAFE_DELETE(EditorSlot);
	SAFE_DELETE(TextEditorSlot);
	SAFE_DELETE(EditorSpeed);
	SAFE_DELETE(TextEditorSpeed);
	SAFE_DELETE(EditorDelay);
	SAFE_DELETE(TextEditorDelay);
	pPlayer = nullptr;
}

//*****************************************************************************
//ActivateSavedLevelMessage関数
//保存したメッセージを表す
//引数：void
//戻：void
//*****************************************************************************
void UIManager::ActivateSavedLevelMessage()
{
	TextEditorLevelSavedLoadedMessage->ActivateSavedLevelMessage();
}

//*****************************************************************************
//ActivateLoadedLevelMessage関数
//ロードしたメッセージを表す
//引数：void
//戻：void
//*****************************************************************************
void UIManager::ActivateLoadedLevelMessage()
{
	TextEditorLevelSavedLoadedMessage->ActivateLoadedLevelMessage();
}


//*****************************************************************************
//GetAttackZoom関数
//ズームイン攻撃動作のエフェクトを戻す
//引数：void
//戻：UI_Object*
//*****************************************************************************
UI_Object * UIManager::GetAttackZoom()
{
	return AtkZoom;
}

//*****************************************************************************
//SetStartEventMessageOn関数
//ランクイベントの最初のメッセージを有効する
//引数：void
//戻：void
//*****************************************************************************
void UIManager::SetStartEventMessageOn()
{
	if (StartEventMessage)
		StartEventMessage->SetMessageOn();
}

//*****************************************************************************
//SetEndEventMessageOn関数
//ランクイベントの最後のメッセージを有効する
//引数：char, char, char, char
//戻：void
//*****************************************************************************
void UIManager::SetEndEventMessageOn(char Time, char Combo, char Damage, char Overrall)
{
	if (EndEventMessage)
		EndEventMessage->SetMessageOn(Time, Combo, Damage, Overrall);
}

//*****************************************************************************
//SetOnAirMessageUse関数
//イベント中の符号を有効する
//引数：bool
//戻：void
//*****************************************************************************
void UIManager::SetOnAirMessageUse(bool use)
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
//評価画面を有効する
//引数：char, char, char, char
//戻：void
//*****************************************************************************
void UIManager::SetResultsScreenOn(char Time, char Combo, char Damage, char Overrall)
{
	if (ResultsScreen)
		ResultsScreen->SetMessageOn(Time, Combo, Damage, Overrall);
}

//*****************************************************************************
//ActivateFinalResults関数
//レベルクリアの評価画面を有効する
//引数：void
//戻：void
//*****************************************************************************
void UIManager::ActivateFinalResults()
{
	if (StageClearScreen)
		StageClearScreen->ActivateFinalResultsScreen();
}

//*****************************************************************************
//IsResultScreenUp関数
//評価画面は有効している状態を確認する
//引数：void
//戻：bool
//*****************************************************************************
bool UIManager::IsResultScreenUp()
{
	if (StageClearScreen)
		return StageClearScreen->IsResultScreenSetUp();
	else
		return false;
}

