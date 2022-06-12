//*****************************************************************************
// UI_Object.cpp
// UIのオブジェクトの管理
// Author : Mane
//*****************************************************************************
#include "UI_Object.h"
#include "UIManager.h"
#include "SceneInGame.h"
#include "DebugAim.h"
#include "GameInputManager.h"
#include "GameObject.h"
#include "Camera.h"
#include "Player.h"
#include <stdio.h>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define HEART_SIZE				75
#define SCORE_NUM_SIZE			25
#define SCORE_MAX_DIGITS		 7
#define SCORE_MAX_GEMS_DIGITS	 3
#define RESULTS_SCREEN_TIME		60 * 4

//*****************************************************************************
// エナム定義
//*****************************************************************************


//*****************************************************************************
// 構造体
//*****************************************************************************
typedef struct Tutorial_Messages
{
	char TextPath[256];
	XMFLOAT2 PolygonSize;
}_Tutorial_Messages;
//*****************************************************************************
// グローバル変数
//*****************************************************************************
ID3D11ShaderResourceView* gpTutorialMessages[TS_MAX] = { nullptr };
Tutorial_Messages		  t_TutorialMessages[TS_MAX] =
{
	{"data/TEXTURE/UIObjects/TS_MovementControls.tga",{ 685, 63 }},
	{"data/TEXTURE/UIObjects/TS_JumpControls.tga",{ 590, 63 }},
	{"data/TEXTURE/UIObjects/TS_PunchControls.tga",{ 1099, 63 }},
	{"data/TEXTURE/UIObjects/TS_SlideControls.tga",{ 1263, 63 }},
	{"data/TEXTURE/UIObjects/TS_GuitarJump.tga",{ 1276, 63 }},
	{"data/TEXTURE/UIObjects/TS_DoubleJump.tga",{ 1280, 63 }},
	{"data/TEXTURE/UIObjects/TS_SwitchPunch.tga",{ 929, 63 }},
	{"data/TEXTURE/UIObjects/TS_SwitchShoot.tga",{ 966, 63 }},
	{"data/TEXTURE/UIObjects/TS_RankEvent.tga",{ 909, 63 }},
	{"data/TEXTURE/UIObjects/TS_EnemyExplanation.tga",{ 1261, 64 }},
	{"data/TEXTURE/UIObjects/TS_SuperArmor.tga",{ 653, 62 }},
	{"data/TEXTURE/UIObjects/TS_SuperArmor2.tga",{ 938, 64 }},
	{"data/TEXTURE/UIObjects/TS_SuperArmor3.tga",{ 1263, 64 }},
	{"data/TEXTURE/UIObjects/TS_SuperArmor4.tga",{ 532, 63 }},
	{"data/TEXTURE/UIObjects/TS_SuperArmor5.tga",{ 1051, 63 }}
};
//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
UI_Object::UI_Object()
{
	bHeartActive = false;
	bIsTheSelectedOption = false;
	nAnimationMirror = 0;
	nCurrentPauseInstruction = 0;
	nCurrentTutorialMessage = 0;
	nFrame_X = 0;
	nFrame_Y = 0;
	nRank = 0;
	nType = 0;
	pControllerMessage = nullptr;
	uv = { 0,0 };
	x3_OGSize = { 0,0,0 };
	x2PosA = x2PosB = { 0,0 };
	nAnimationTimer = 0;
	n_uvTimer = 0;
	nFullNumber = 0;
	nDirectionUpdiseDown = 0;
	fAlphaTemp = 0;
	bMessageOn = false;
	bIsArrowActive = false;
	PlayerPointer = nullptr;
	MiniAura = nullptr;
	pComboText = nullptr;
	pTimeText = nullptr;
	pDamageText = nullptr;
	pOverralText = nullptr;
	pResultsTextLeft = nullptr;
	pResultsTextRight = nullptr;
	pStageClearBackground = nullptr;
	pTransparentBlackBackground = nullptr;
	pTimeRank = nullptr;
	pDamageRank = nullptr;
	pOverrallRank = nullptr;
	pPauseText = nullptr;
	pComboRank = nullptr;
	pScoreNums = nullptr;
	pGemsNums = nullptr;
	pControlInstructions = nullptr;
	pGemsImage = nullptr;
	pInTutorialText = nullptr;
	pTutorialBorderUp = nullptr;
	pTutorialBorderDown = nullptr;
	pTutorialMessage = nullptr;
	fAcceleration = 0;
	nFrameCountAlternatePos = 0;
	cRankResult = 'D';
	cTime = cCombo = cDamage = cOverrall = 'Z';
	nFrameCountingBeforeDeactivation = 0;
	bResultScreenSettedUp = false;
	bActivateStageClear = false;
	bStageClearSettedUp = false;
	nMaxDigits = SCORE_MAX_DIGITS;
	bSizeUp = false;
	bIsPosA = true;
}

UI_Object::UI_Object(int Type)
{
	nAnimationTimer = 0;
	n_uvTimer = 0;
	nFullNumber = 0;
	nDirectionUpdiseDown = 0;
	bMessageOn = false;
	bIsArrowActive = false;
	PlayerPointer = nullptr;
	MiniAura = nullptr;
	pComboText = nullptr;
	pTimeText = nullptr;
	pDamageText = nullptr;
	pOverralText = nullptr;
	pResultsTextLeft = nullptr;
	pResultsTextRight = nullptr;
	pStageClearBackground = nullptr;
	pTransparentBlackBackground = nullptr;
	pTimeRank = nullptr;
	pDamageRank = nullptr;
	pOverrallRank = nullptr;
	pPauseText = nullptr;
	pComboRank = nullptr;
	pScoreNums = nullptr;
	pGemsNums = nullptr;
	pControlInstructions = nullptr;
	pGemsImage = nullptr;
	pInTutorialText = nullptr;
	pTutorialBorderUp = nullptr;
	pTutorialBorderDown = nullptr;
	pTutorialMessage = nullptr;
	fAcceleration = 0;
	nFrameCountAlternatePos = 0;
	cRankResult = 'D';
	cTime = cCombo = cDamage = cOverrall = 'Z';
	nFrameCountingBeforeDeactivation = 0;
	bResultScreenSettedUp = false;
	bActivateStageClear = false;
	bStageClearSettedUp = false;
	nMaxDigits = SCORE_MAX_DIGITS;
	bSizeUp = false;
	bIsPosA = true;
	Init(Type);
}


UI_Object::~UI_Object()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：int
//戻：void
//*****************************************************************************
void UI_Object::Init(int Type)
{
	InitSprite();
	nType = Type;
	bIsTheSelectedOption = false;
	switch (nType)
	{
	case UI_HEART://プレイヤーのHPを見せる
		gpTexture = GetTexture(TEXTURE_UI_HEARTS);
		SetSpriteSize(HEART_SIZE, HEART_SIZE);
		nFrame_X = 5;
		nFrame_Y = 4;
		uv = { 1,1 };
		bHeartActive = false;
		break;
	case UI_RANK://プレイヤーのスタイルランクを見せる
		gpTexture = GetTexture(TEXTURE_UI_RANK);
		if (!MiniAura)
		{
			MiniAura = new GameObject();
			MiniAura->Init(GetTexture(TEXTURE_UI_MINIAURA), MINI_AURA_EFFECT_TYPE);
			MiniAura->SetUse(false);
			MiniAura->SetUIObject(true);
		}
		SetSpriteSize(100, 100);
		SetWorldPosition(550, 280);
		nFrame_X = 4;
		nFrame_Y = 10;
		uv = { 0,0 };
		nRank = 0;
		bHeartActive = false;
		break;
	case UI_SCORE_NUMBERS://スコアと一般的な数字を見せる
		gpTexture = GetTexture(TEXTURE_UI_NUMBERS);
		SetSpriteSize(SCORE_NUM_SIZE*0.7f, SCORE_NUM_SIZE);
		nFrame_X = 10;
		nFrame_Y = 10;
		uv = { 0,0 };
		nFullNumber = 0;
		break;
	case UI_RANKING_NUMBERS://ランキングメニューの数字を見せる
		gpTexture = GetTexture(TEXTURE_UI_RANKNUMBERS);
		SetSpriteSize(SCORE_RANKNUM_SIZE, SCORE_RANKNUM_SIZE);
		nFrame_X = 10;
		nFrame_Y = 1;
		uv = { 0,0 };
		nFullNumber = 0;
		break;
	case UI_STARTEVENT_MESSAGE://ランクイベントの最初のメッセージを見せる
		gpTexture = GetTexture(TEXTURE_UI_EVENTMESSAGE_START);
		nFrame_X = 6;
		nFrame_Y = 10;
		SetSpriteSize(800, 168);
		SetWorldPosition(0, 200);
		uv = { 0,0 };
		break;
	case UI_ENDEVENT_MESSAGE://ランクイベントの最後のメッセージを見せる
		gpTexture = GetTexture(TEXTURE_UI_EVENTMESSAGE_OVER);
		nFrame_X = 4;
		nFrame_Y = 6;
		SetSpriteSize(400, 269);
		SetWorldPosition(0, 200);
		uv = { 0,0 };
		break;
	case UI_INEVENT_NOTICE://ランクイベント中の符号を見せる
		gpTexture = GetTexture(TEXTURE_UI_EVENTMESSAGE_ONAIR);
		nFrame_X = 1;
		nFrame_Y = 15;
		SetSpriteSize(100, 28);
		SetWorldPosition((WINDOW_WIDTH / 2) - 55, -(WINDOW_HEIGHT/2)+25);
		uv = { 0,0 };
		nAnimationMirror = 1;
		break;
	case UI_RESULTS_SCREEN://ランクイベントの評価画面を見せる
		pTransparentBlackBackground = new UI_Object();
		pTransparentBlackBackground->Init(UI_MAX);
		pTransparentBlackBackground->SetRotation({0,90,0});
		//コンボの評価
		pComboText = new UI_Object();
		pComboText->Init(UI_COMBO_TEXT);
		pComboText->SetWorldPosition(-(WINDOW_WIDTH / 2) + 255, (WINDOW_HEIGHT / 2) - 45);
		pComboText->SetRotation({ 0,90,0 });
		pComboRank = new UI_Object();
		pComboRank->Init(UI_RESULT_RANK_TEXT);
		pComboRank->SetWorldPosition(-(WINDOW_WIDTH / 2) + 255, (WINDOW_HEIGHT / 2) - 175);
		pComboRank->SetUpDownDirection(-1);
		pComboRank->SetRotation({ 0,90,0 });
		//タイムの評価
		pTimeText = new UI_Object();
		pTimeText->Init(UI_TIME_TEXT);
		pTimeText->SetWorldPosition(0, (WINDOW_HEIGHT / 2) - 45);
		pTimeText->SetRotation({ 0,90,0 });
		pTimeRank = new UI_Object();
		pTimeRank->Init(UI_RESULT_RANK_TEXT);
		pTimeRank->SetWorldPosition(0, (WINDOW_HEIGHT / 2) - 175);
		pTimeRank->SetUpDownDirection(-1);
		pTimeRank->SetRotation({ 0,90,0 });
		//ダメージの評価
		pDamageText = new UI_Object();
		pDamageText->Init(UI_DAMAGE_TEXT);
		pDamageText->SetWorldPosition((WINDOW_WIDTH / 2) - 255, (WINDOW_HEIGHT / 2) - 45);
		pDamageText->SetRotation({ 0,90,0 });
		pDamageRank = new UI_Object();
		pDamageRank->Init(UI_RESULT_RANK_TEXT);
		pDamageRank->SetWorldPosition((WINDOW_WIDTH / 2) - 255, (WINDOW_HEIGHT / 2) - 175);
		pDamageRank->SetUpDownDirection(-1);
		pDamageRank->SetRotation({ 0,90,0 });
		//全体の評価
		pOverralText = new UI_Object();
		pOverralText->Init(UI_OVERRALL_TEXT);
		pOverralText->SetWorldPosition((WINDOW_WIDTH / 2) - 180, -150);
		pOverralText->SetRotation({ 0,90,0 });
		pOverrallRank = new UI_Object();
		pOverrallRank->Init(UI_RESULT_RANK_TEXT);
		pOverrallRank->SetWorldPosition((WINDOW_WIDTH / 2) - 170, -260);
		pOverrallRank->SetUpDownDirection(-1);
		pOverrallRank->SetRotation({ 0,90,0 });
		//画面の文字
		pResultsTextLeft = new UI_Object();
		pResultsTextLeft->Init(UI_RESULTS_TEXT);
		pResultsTextLeft->SetWorldPosition(-(WINDOW_WIDTH / 2) + 76 / 2, 0);
		pResultsTextLeft->SetUpDownDirection(1);
		pResultsTextLeft->SetRotation({ 0,90,0 });
		pResultsTextRight = new UI_Object();
		pResultsTextRight->Init(UI_RESULTS_TEXT);
		pResultsTextRight->SetWorldPosition((WINDOW_WIDTH / 2) - 76 / 2, 0);
		pResultsTextRight->SetUpDownDirection(-1);
		pResultsTextRight->SetRotation({ 0,90,0 });
		break;
	case UI_COMBO_TEXT://「コンボ」の文字
		gpTexture = GetTexture(TEXTURE_UI_RANKINGTEXT_COMBO);
		nFrame_X = 1;
		nFrame_Y = 1;
		uv = { 0,0 };
		SetSpriteSize(232, 100); 
		break;
	case UI_TIME_TEXT://「タイム」の文字
		gpTexture = GetTexture(TEXTURE_UI_RANKINGTEXT_TIME);
		nFrame_X = 1;
		nFrame_Y = 1;
		uv = { 0,0 };
		SetSpriteSize(199, 80);
		break;
	case UI_DAMAGE_TEXT://「ダメージ」の文字
		gpTexture = GetTexture(TEXTURE_UI_RANKINGTEXT_DAMAGE);
		nFrame_X = 1;
		nFrame_Y = 1;
		uv = { 0,0 };
		SetSpriteSize(266, 75);
		break;
	case UI_OVERRALL_TEXT://「全体」の文字
		gpTexture = GetTexture(TEXTURE_UI_RANKINGTEXT_OVERRAL);
		nFrame_X = 1;
		nFrame_Y = 1;
		uv = { 0,0 };
		SetSpriteSize(141, 75);
		break;
	case UI_RESULTS_TEXT://「結果」の文字
		gpTexture = GetTexture(TEXTURE_UI_RANKINGTEXT_RESULTSTEXT);
		nFrame_X = 1;
		nFrame_Y = 1;
		uv = { 0,0 };
		SetSpriteSize(75, 155);
		break;
	case UI_RESULT_RANK_TEXT://「ランク」の文字
		gpTexture = GetTexture(TEXTURE_UI_RANKINGTEXT_RANK);
		nFrame_X = 5;
		nFrame_Y = 1;
		uv = { 0,0 };
		SetSpriteSize(137, 130);
		break;
	case UI_PAUSE_SCREEN://ポース画面
		pTransparentBlackBackground = new UI_Object();
		pTransparentBlackBackground->Init(UI_MAX);		
		pPauseText = new UI_Object();
		pPauseText->Init(UI_PAUSE_TEXT);

		pGemsNums = new UI_Object();
		pGemsNums->Init(UI_SCORE_NUMBERS);
		pGemsNums->SetWorldPosition((WINDOW_WIDTH/2) - 90, -(WINDOW_HEIGHT/2) + 20);
		pGemsNums->SetMaxDigits(SCORE_MAX_GEMS_DIGITS);
		
		pGemsImage = new UI_Object();
		pGemsImage->Init(UI_GEM);
		pGemsImage->SetWorldPosition((WINDOW_WIDTH / 2) - 120, -(WINDOW_HEIGHT / 2) + 20);
		pGemsImage->gpTexture = GetTexture(TEXTURE_CRYSTAL);
		pGemsImage->SetNewFilter(D3D11_FILTER_MIN_MAG_MIP_LINEAR);

		pControlInstructions = new UI_Object();
		pControlInstructions->Init(UI_PAUSE_CONTROL_INSTRUCTIONS);
		pControlInstructions->SetWorldPosition(0,0);
		break;
	case UI_PAUSE_TEXT://ポース画面の文字
		gpTexture = GetTexture(TEXTURE_UI_PAUSETEXT);
		nFrame_X = 1;
		nFrame_Y = 1;
		uv = { 0,0 };
		SetSpriteSize(257, 60);
		break;
	case UI_STAGECLEAR_RESULTS://ステージの評価画面
		pStageClearBackground = new UI_Object();
		pStageClearBackground->Init(UI_STAGECLEAR_BACKGROUND);
		pStageClearBackground->SetSpriteSize(0, 10);
		pScoreNums = new UI_Object();
		pScoreNums->Init(UI_SCORE_NUMBERS);
		pScoreNums->SetWorldPosition(40, 5);
		pGemsNums = new UI_Object();
		pGemsNums->Init(UI_SCORE_NUMBERS);
		pGemsNums->SetWorldPosition(40, -55);
		break;
	case UI_STAGECLEAR_BACKGROUND://ステージの評価画面のバックグラウンド
		gpTexture = GetTexture(TEXTURE_UI_STAGECLEAR_BACKGROUND);
		nFrame_X = 1;
		nFrame_Y = 1;
		uv = { 0,0 };
		SetSpriteSize(1280, 720);
		break;
	case UI_GEM://ジェムのUI画像
		SetSpriteSize(50/1.5f, 71/1.5f);
		SetWorldPosition(0, 0);
		hitbox = { 0,0,50,71 };
		uv = { 0,0 };
		nFrame_Y = 4;
		nFrame_X = 5;
		break;
	case UI_PAUSE_CONTROL_INSTRUCTIONS://UIのポース画面の説明書
		gpTexture = GetTexture(IT_CONTROLS);
		GetTexture(IT_COMBOS);
		nCurrentPauseInstruction = IT_MAX;
		nFrame_X = 1;
		nFrame_Y = 1;
		uv = { 0,0 };
		SetSpriteSize(1280, 720);
		break;
	case UI_RANKINGSCREEN_RANKINGTEXT://UIのランクの文字
		gpTexture = GetTexture(TEXTURE_UI_GENERAL_RANKING);
		nFrame_X = 1;
		nFrame_Y = 1;
		uv = { 0,0 };
		SetWorldPosition(0, (WINDOW_HEIGHT/2)-50);
		SetSpriteSize(451, 91);
		break;
	case UI_TUTORIAL_SCREEN://UIのチュートリアル画面
		pTutorialBorderUp = new UI_Object(UI_MAX);
		pTutorialBorderUp->SetSpriteSize(WINDOW_WIDTH, 100);
		pTutorialBorderUp->SetWorldPosition(0, (WINDOW_HEIGHT/2)-50);
		pTutorialBorderDown = new UI_Object(UI_MAX);
		pTutorialBorderDown->SetSpriteSize(WINDOW_WIDTH, 100);
		pTutorialBorderDown->SetWorldPosition(0, (-WINDOW_HEIGHT / 2) + 50);
		pInTutorialText = new UI_Object(UI_TUTORIAL_TEXT);
		pTutorialMessage = new UI_Object(UI_TUTORIAL_MESSAGE);
		pControllerMessage = new UI_Object(UI_CONTROLLER_MESSAGE);
		break;
	case UI_TUTORIAL_TEXT://UIのチュートリアル画面の文字
		gpTexture = GetTexture(TEXTURE_UI_TUTORIAL_TEXT);
		SetSpriteSize(360, 59);
		nFrame_X = 1;
		nFrame_Y = 1;
		SetAlpha(1);
		uv = { 0,0 };
		SetWorldPosition((-WINDOW_WIDTH/2)+ 251, (WINDOW_HEIGHT / 2) - 48);
		break;
	case UI_TUTORIAL_MESSAGE://UIのチュートリアル画面のメッセージ
		for (int i = 0; i < TS_MAX; i++)
		{
			CreateTextureFromFile(t_TutorialMessages[i].TextPath, &gpTutorialMessages[i]);
		}
		nCurrentTutorialMessage = TS_MAX;
		gpTexture = gpTutorialMessages[0];
		SetSpriteSize(360, 59);
		nFrame_X = 1;
		nFrame_Y = 1;
		SetAlpha(1);
		uv = { 0,0 };
		SetWorldPosition(0, (-WINDOW_HEIGHT / 2) + 40);
		SetRotation({0,90,0});
		break;
	case UI_PAUSE_MENU_OPTIONS_CONTINUE://ポースメニューの「続ける」文字
		gpTexture = GetTexture(TEXTURE_UI_PAUSE_OPTIONS_CONTINUE);
		SetSpriteSize(160, 30);
		nFrame_X = 1;
		nFrame_Y = 1;
		SetAlpha(1);
		uv = { 0,0 };
		SetWorldPosition(0, -100);
		SetColor(0.5f, 0.5f, 0.5f);
		break;
	case UI_PAUSE_MENU_OPTIONS_CONTROLS://ポースメニューの「コントロール」文字
		gpTexture = GetTexture(TEXTURE_UI_PAUSE_OPTIONS_CONTROLS);
		SetSpriteSize(158, 27);
		nFrame_X = 1;
		nFrame_Y = 1;
		SetAlpha(1);
		uv = { 0,0 };
		SetWorldPosition(0, -140);
		SetColor(0.5f, 0.5f, 0.5f);
		break;	
	case UI_PAUSE_MENU_OPTIONS_COMMANDS://ポースメニューの「コマンド」文字
		gpTexture = GetTexture(TEXTURE_UI_PAUSE_OPTIONS_COMMANDS);
		SetSpriteSize(158, 28);
		nFrame_X = 1;
		nFrame_Y = 1;
		SetAlpha(1);
		uv = { 0,0 };
		SetWorldPosition(0, -180);
		SetColor(0.5f, 0.5f, 0.5f);
		break;
	case UI_PAUSE_MENU_OPTIONS_RETURNTITLE://ポースメニューの「戻る」文字
		gpTexture = GetTexture(TEXTURE_UI_PAUSE_OPTIONS_RETURNTITLE);
		SetSpriteSize(160, 29);
		nFrame_X = 1;
		nFrame_Y = 1;
		SetAlpha(1);
		uv = { 0,0 };
		SetWorldPosition(0, -220);
		SetColor(0.5f, 0.5f, 0.5f);
		break;
	case UI_ARROW://UIの矢印
		gpTexture = GetTexture(TEXTURE_UI_ARROW);
		SetSpriteSize(125/2, 162/2);
		nFrame_X = 1;
		nFrame_Y = 1;
		SetAlpha(1);
		uv = { 0,0 };
		SetWorldPosition(0, 0);
		Rotation.y = 90;
		break;
	case UI_ATK_EFFECT://UIの攻撃動作のエフェクト
		gpTexture = GetTexture(TEXTURE_UI_ATK_EFFECT);
		SetSpriteSize(1600, 1600);
		nFrame_X = 1;
		nFrame_Y = 1;
		SetAlpha(0);
		uv = { 0,0 };
		SetWorldPosition(0, 0);
		SetColor(1, 1, 1);
		break;
	case UI_EDITOR_INSTRUCTIONS_A://レベルエディターの説明（A）
		gpTexture = GetTexture(TEXTURE_UI_EDITOR_INSTRUCTIONS_1);
		SetSpriteSize(448/2, 667/2);
		nFrame_X = 1;
		nFrame_Y = 1;
		SetAlpha(1);
		uv = { 0,0 };
		SetWorldPosition(500, -180);
		SetColor(1, 1, 1);
		break;
	case UI_EDITOR_INSTRUCTIONS_B://レベルエディターの説明（B）
		gpTexture = GetTexture(TEXTURE_UI_EDITOR_INSTRUCTIONS_2);
		SetSpriteSize(1307/2, 886/2);
		nFrame_X = 1;
		nFrame_Y = 1;
		SetAlpha(0);
		uv = { 0,0 };
		SetWorldPosition(300, 140);
		SetColor(1, 1, 1);
		break;
	case UI_EDITORSLOT://レベルエディターのレベルスロット
		SetAlpha(0);
		gpTexture = GetTexture(TEXTURE_UI_RANKNUMBERS);
		SetSpriteSize(SCORE_RANKNUM_SIZE, SCORE_RANKNUM_SIZE);
		nFrame_X = 10;
		nFrame_Y = 1;
		uv = { 0,0 };
		nFullNumber = 0;
		nMaxDigits = 1;
		SetWorldPosition(-(WINDOW_WIDTH / 2 - (Size.x / 2)), -(WINDOW_HEIGHT / 2 - (Size.y / 2)));
		break;
	case UI_EDITORSPEED://レベルエディターのオブジェクトの速さ
		SetAlpha(0);
		gpTexture = GetTexture(TEXTURE_UI_RANKNUMBERS);
		SetSpriteSize(SCORE_RANKNUM_SIZE, SCORE_RANKNUM_SIZE);
		nFrame_X = 10;
		nFrame_Y = 1;
		uv = { 0,0 };
		nFullNumber = 0;
		nMaxDigits = 2;
		SetWorldPosition(-(WINDOW_WIDTH / 2 - (Size.x / 2)) + 200, -(WINDOW_HEIGHT / 2 - (Size.y / 2)));
		break;
	case UI_EDITORDELAY://レベルエディターの動けるオブジェクトの止まるディレイ
		SetAlpha(0);
		gpTexture = GetTexture(TEXTURE_UI_RANKNUMBERS);
		SetSpriteSize(SCORE_RANKNUM_SIZE, SCORE_RANKNUM_SIZE);
		nFrame_X = 10;
		nFrame_Y = 1;
		uv = { 0,0 };
		nFullNumber = 0;
		nMaxDigits = 1;
		SetWorldPosition(-(WINDOW_WIDTH / 2 - (Size.x / 2)) + 450, -(WINDOW_HEIGHT / 2 - (Size.y / 2)));
		break;
	case UI_EDITORSLOT_TEXT://レベルエディターのレベルスロットの文字
		gpTexture = GetTexture(TEXTURE_UI_EDITOR_SLOT_TEXT);
		SetSpriteSize(148, 54);
		nFrame_X = 1;
		nFrame_Y = 1;
		SetAlpha(0);
		uv = { 0,0 };
		SetWorldPosition(-(WINDOW_WIDTH / 2 - (Size.x / 2))+10, -(WINDOW_HEIGHT / 2 - (Size.y / 2))+75);
		SetColor(1, 1, 1);
		break;
	case UI_EDITORSPEED_TEXT://レベルエディターのオブジェクトの速さの文字
		gpTexture = GetTexture(TEXTURE_UI_EDITOR_SPEED_TEXT);
		SetSpriteSize(133, 54);
		nFrame_X = 1;
		nFrame_Y = 1;
		SetAlpha(0);
		uv = { 0,0 };
		SetWorldPosition(-(WINDOW_WIDTH / 2 - (Size.x / 2)) + 240, -(WINDOW_HEIGHT / 2 - (Size.y / 2)) + 75);
		SetColor(1, 1, 1);
		break;
	case UI_EDITORDELAY_TEXT://レベルエディターの動けるオブジェクトの止まるディレイの文字
		gpTexture = GetTexture(TEXTURE_UI_EDITOR_DELAY_TEXT);
		SetSpriteSize(174, 54);
		nFrame_X = 1;
		nFrame_Y = 1;
		SetAlpha(0);
		uv = { 0,0 };
		SetWorldPosition(-(WINDOW_WIDTH / 2 - (Size.x / 2)) + 430, -(WINDOW_HEIGHT / 2 - (Size.y / 2)) + 75);
		SetColor(1, 1, 1);
		break;
	case UI_CONTROLLER_MESSAGE://チュートリアル中にコントローラーのボタン操作説明
		gpTexture = GetTexture(TEXTURE_UI_CONTROLLER_TUTORIAL_MOVE);
		SetAlpha(1);
		uv = { 0,0 };
		nFrame_X = 2;
		nFrame_Y = 1;
		SetSpriteSize(736/4, 516/4);
		SetWorldPosition(0, 235);
		nCurrentTutorialMessage = 0;
		break;
	case UI_LEVEL_LOADEDSAVED_TEXT:
		gpTexture = GetTexture(TEXTURE_UI_LEVEL_SAVED_TEXT);
		SetAlpha(0);
		fAlphaTemp = 0;
		uv = { 0,0 };
		nFrame_X = 1;
		nFrame_Y = 1;
		SetSpriteSize(500, 51);
		SetWorldPosition(0, -200);
		nCurrentTutorialMessage = 0;
		break;
	default:
		gpTexture = GetTexture(TEXTURE_BLACKSPOT);
		SetAlpha(0.35f);
		nFrame_X = 1;
		nFrame_Y = 1;
		uv = { 0,0 };
		SetSpriteSize(WINDOW_WIDTH, WINDOW_HEIGHT);
		break;
	}
	x3_OGSize = Size;
}

//*****************************************************************************
//Update関数
//処理のアップデート関数
//引数：void
//戻：void
//*****************************************************************************
void UI_Object::Update()
{
	SceneInGame* pGame = GetCurrentGame();
	Player* pPlayer;
	DebugAim* pDA;
	switch (nType)
	{
	case UI_HEART:
		if (++nAnimationTimer > 2) {
			nAnimationTimer = 0;
			uv.U++;
			if (uv.U == nFrame_X)
			{
				uv.U = 0;
				uv.V++;
				if (uv.V == nFrame_Y)
					uv.V = 0;
			}
		}
		break;
	case UI_RANK:
		uv.U = (float)(nRank-2);
		if (++nAnimationTimer > 3) {
			nAnimationTimer = 0;
			uv.V++;
		}
		if (MiniAura) {
			if (!(MiniAura->IsInUse()))
			{
				MiniAura->SetSpriteSize(0, 0);
				MiniAura->SetWorldPosition(Position.x, Position.y);
				MiniAura->SetUse(true);
				MiniAura->SetAlpha(1);
			}
			else {
				MiniAura->Update();
			}
		}
		break;
	case UI_SCORE_NUMBERS:
		if (++nAnimationTimer > 11) {
			nAnimationTimer = 0;
			uv.V++;
			if (uv.V == 6)
				nAnimationTimer = -53;
			if (uv.V == nFrame_Y)
				uv.V = 0;
		}
		break;
	case UI_STARTEVENT_MESSAGE:
		if (!bMessageOn)
			return;
		if (++nAnimationTimer > 1) {
			nAnimationTimer = 0;
			uv.U++;
			if (uv.U == nFrame_X) {
				uv.U = 0;
				uv.V++;
				if (uv.V == nFrame_Y)
					bMessageOn = false;
			}
			if (uv.U == 0 && uv.V == 9)
				nAnimationTimer = -40;
		}
		break;
	case UI_ENDEVENT_MESSAGE:
		if (!bMessageOn)
			return;
		if (++nAnimationTimer > 1) {
			nAnimationTimer = 0;
			uv.U++;
			if (uv.U == nFrame_X) {
				uv.U = 0;
				uv.V++;
				if (uv.V == nFrame_Y) {
					uv = { 0,0 };
					bMessageOn = false;
					UIManager::SetResultsScreenOn(cTime, cCombo, cDamage, cOverrall);
				}
			}
			if (uv.U == 0 && uv.V == 2)
				nAnimationTimer = -60;
		}
		break;
	case UI_INEVENT_NOTICE:
		if (!bMessageOn)
			return;
		if (++nAnimationTimer > 2) {
			nAnimationTimer = 0;
			uv.V+= nAnimationMirror;
			if (uv.V == 7)
				nAnimationTimer = -25;
			if (uv.V == nFrame_Y || uv.V == 0)
				nAnimationMirror *= -1;
		}
		break;
	case UI_RESULTS_SCREEN:
		ResultScreenControl();
		break;
	case UI_RESULTS_TEXT:
		Position.y += nDirectionUpdiseDown*5;
		if (Position.y + Size.y <-WINDOW_HEIGHT / 2 || Position.y - Size.y >WINDOW_HEIGHT/2) {
			if (nDirectionUpdiseDown < 0)
				Position.y = WINDOW_HEIGHT / 2 + Size.y;
			else
				Position.y = -(WINDOW_HEIGHT / 2) - Size.y;
		}
		break;
	case UI_RESULT_RANK_TEXT:
		if (cRankResult == 'D')
			uv.U = 0;
		if (cRankResult == 'C')
			uv.U = 1;
		if (cRankResult == 'B')
			uv.U = 2;
		if (cRankResult == 'A')
			uv.U = 3;
		if (cRankResult == 'S')
			uv.U = 4;
		break;
	case UI_PAUSE_SCREEN:
		if (pPauseText)
			pPauseText->Update();
		if (pGemsNums) {
			pGemsNums->SetNumberUI(pGame->GetGems());
			pGemsNums->Update();
		}
		if (pGemsImage)
			pGemsImage->Update();
		if (pControlInstructions)
			pControlInstructions->Update();
		break;
	case UI_STAGECLEAR_RESULTS:
		if (!bActivateStageClear)
			return;
		if (pStageClearBackground) {
			pStageClearBackground->Update();
			if (!bStageClearSettedUp)
			{
				if (pStageClearBackground->GetSize().x == WINDOW_WIDTH && pStageClearBackground->GetSize().y == WINDOW_HEIGHT)
					bStageClearSettedUp = true;
			}
		}
		if (pScoreNums) {
			pScoreNums->SetNumberUI(pGame->GetScore());
			pScoreNums->Update();
		}
		if (pGemsNums) {
			pGemsNums->SetNumberUI(pGame->GetGems());
			pGemsNums->Update();
		}
		break;
	case UI_STAGECLEAR_BACKGROUND:
		if (Size.x != WINDOW_WIDTH)
		{
			fAcceleration += 15;
			Size.x += fAcceleration;
			if (Size.x > WINDOW_WIDTH)
				Size.x = WINDOW_WIDTH;
			break;
		}
		if (Size.y != WINDOW_HEIGHT)
		{
			fAcceleration += 5;
			Size.y += fAcceleration;
			if (Size.y > WINDOW_HEIGHT)
				Size.y = WINDOW_HEIGHT;
			break;
		}
		break;
	case UI_GEM:
		RegularUVAnimationControl(2);
		break;
	case UI_PAUSE_CONTROL_INSTRUCTIONS:
		PauseInstructionsControl();
		break;
	case UI_TUTORIAL_SCREEN:
		if (pTutorialBorderUp)
			pTutorialBorderUp->Update();
		if (pTutorialBorderDown)
			pTutorialBorderDown->Update();
		if (pInTutorialText)
			pInTutorialText->Update();
		if (pTutorialMessage)
			pTutorialMessage->Update();
		if (pControllerMessage)
			pControllerMessage->Update();
		break;
	case UI_TUTORIAL_TEXT:
		if (bSizeUp)
		{
			if (Size.x < 370)
			{
				Size.x+=0.25f;
				Size.y+=0.25f;
			}
			else
			{
				bSizeUp = false;
			}
		}
		else {
			if (Size.x > 360)
			{
				Size.x-=0.25f;
				Size.y-=0.25f;
			}
			else
			{
				bSizeUp = true;
			}
		}
		break;
	case UI_TUTORIAL_MESSAGE:
		if (nCurrentTutorialMessage < TS_MAX && nCurrentTutorialMessage >= 0)
		{
			gpTexture = gpTutorialMessages[nCurrentTutorialMessage];
			SetSpriteSize(t_TutorialMessages[nCurrentTutorialMessage].PolygonSize);
			if (Rotation.y > 0)
			{
				fAcceleration++;
				Rotation.y-=fAcceleration;
			}
			else if (Rotation.y < 0)
			{
				Rotation.y = 0;
				fAcceleration = 0;
			}
		}
		else {
			if (Rotation.y < 90)
			{
				fAcceleration++;
				Rotation.y += fAcceleration;
			}
			else if (Rotation.y > 90)
			{
				Rotation.y = 90;
				fAcceleration = 0;
			}
		}
		break;
	case UI_PAUSE_MENU_OPTIONS_CONTINUE: case UI_PAUSE_MENU_OPTIONS_CONTROLS: case UI_PAUSE_MENU_OPTIONS_COMMANDS: case UI_PAUSE_MENU_OPTIONS_RETURNTITLE:
		if (bIsTheSelectedOption) 
			SetColor(1,1,1);
		else 
			SetColor(0.5f, 0.5f, 0.5f);
		break;
	case UI_ARROW:
		if (++nFrameCountAlternatePos >= 15)
		{
			if (bIsPosA)
			{
				bIsPosA = false;
				SetWorldPosition(x2PosA.x, x2PosA.y);
			}
			else {
				bIsPosA = true;
				SetWorldPosition(x2PosB.x, x2PosB.y);
			}
			nFrameCountAlternatePos = 0;
		}
		if (bIsArrowActive)
		{
			if (Rotation.y > 0)
			{
				fAcceleration++;
				Rotation.y -= fAcceleration;
				if (Rotation.y < 0)
				{
					fAcceleration=0;
					Rotation.y = 0;
				}
			}
		}
		else 
		{
			if (Rotation.y < 90)
			{
				fAcceleration++;
				Rotation.y += fAcceleration;
				if (Rotation.y > 90)
				{
					fAcceleration = 0;
					Rotation.y = 90;
				}
			}
		}
		break;
	case UI_ATK_EFFECT:
		if(GetZoomOffset()!=1)
			SetAlpha(0.75f);
		else
			SetAlpha(0);
		RotateAroundZ(20);
		break;
	case UI_EDITOR_INSTRUCTIONS_A:
		if (!PlayerPointer)
			PlayerPointer = GetPlayerPointer();
		pPlayer = (Player*)PlayerPointer;
		if (!pPlayer)
			return;
		if (pPlayer->IsDebugAimOn())
			SetAlpha(0);
		else
			SetAlpha(1);
		break;
	case UI_EDITOR_INSTRUCTIONS_B:
		if (!PlayerPointer)
			PlayerPointer = GetPlayerPointer();
		pPlayer = (Player*)PlayerPointer;
		if (!pPlayer)
			return;
		if (pPlayer->IsDebugAimOn())
			SetAlpha(1);
		else
			SetAlpha(0);
		break;
	case UI_EDITORSLOT:	case UI_EDITORSLOT_TEXT:
		if (!PlayerPointer)
			PlayerPointer = GetPlayerPointer();
		pPlayer = (Player*)PlayerPointer;
		pDA=((DebugAim*)pPlayer->GetDebugAim());
		if (!pDA)
			return;
		nFullNumber = pDA->GetSaveSlot();
		if (pPlayer->IsDebugAimOn()) {
			if(pDA->GetSelectNum()==DAO_LEVEL_SLOT)
			{
				SetColor(1, 1, 1);
				SetAlpha(1);
			}
			else
			{
				SetColor(0.25f, 0.25f, 0.25f);
				SetAlpha(0.5f);
			}
		}
		else
			SetAlpha(0);
		break;
	case UI_EDITORSPEED: case UI_EDITORSPEED_TEXT:
		if (!PlayerPointer)
			PlayerPointer = GetPlayerPointer();
		pPlayer = (Player*)PlayerPointer;
		pDA = ((DebugAim*)pPlayer->GetDebugAim());
		if (!pDA)
			return;
		nFullNumber = pDA->GetMoveableObjectSpeed();
		if (pPlayer->IsDebugAimOn())
			if (pDA->GetSelectNum() == DAO_OBJECT_SPEED)
			{
				SetColor(1, 1, 1);
				SetAlpha(1);
			}
			else
			{
				SetColor(0.25f, 0.25f, 0.25f);
				SetAlpha(0.5f);
			}
		else
			SetAlpha(0);
		break;
	case UI_EDITORDELAY: case UI_EDITORDELAY_TEXT:
		if (!PlayerPointer)
			PlayerPointer = GetPlayerPointer();
		pPlayer = (Player*)PlayerPointer;
		pDA = ((DebugAim*)pPlayer->GetDebugAim());
		if (!pDA)
			return;
		nFullNumber = pDA->GetDelayObjects();
		if (pPlayer->IsDebugAimOn())
			if (pDA->GetSelectNum() == DAO_OBJECT_DELAY) 
			{
				SetColor(1, 1, 1);
				SetAlpha(1);
			}
			else
			{
				SetColor(0.25f, 0.25f, 0.25f);
				SetAlpha(0.5f);
			}
		else
			SetAlpha(0);
		break;
	case UI_CONTROLLER_MESSAGE:
		ControllerTutorialControl();
		break;
	case UI_LEVEL_LOADEDSAVED_TEXT:
		if (!PlayerPointer)
			PlayerPointer = GetPlayerPointer();
		pPlayer = (Player*)PlayerPointer;
		pDA = ((DebugAim*)pPlayer->GetDebugAim());
		if (!pDA)
			return;
		if (fAlphaTemp > 0) {
			fAlphaTemp -= 0.01f;
			if (fAlphaTemp < 0)
				fAlphaTemp = 0;
		}
		if (pPlayer->IsDebugAimOn()) {
			SetColor(1, 1, 1);
			SetAlpha(fAlphaTemp);
		}
		else
			SetAlpha(0);
		break;
	default:
		break;
	}
	
}

void UI_Object::ControllerTutorialControl()
{
	if (!IsXInputConnected())
	{
		SetAlpha(0);
		return;
	}
	if (++nAnimationTimer > 10) {
		nAnimationTimer = 0;
		uv.U++;
		if (uv.U == 2)
			uv.U = 0;
	}
	switch (nCurrentTutorialMessage)
	{
	case TS_MOVEMENT:
		fAcceleration++;
		RotateAroundY(-fAcceleration);
		if (Rotation.y <= 0)
		{
			Rotation.y = 0;
			fAcceleration = 0;
		}
		SetAlpha(1);
		gpTexture = GetTexture(TEXTURE_UI_CONTROLLER_TUTORIAL_MOVE);
		break;
	case TS_JUMP:
		fAcceleration++;
		RotateAroundY(-fAcceleration);
		if (Rotation.y <= 0)
		{
			Rotation.y = 0;
			fAcceleration = 0;
		}
		SetAlpha(1);
		gpTexture = GetTexture(TEXTURE_UI_CONTROLLER_TUTORIAL_JUMP);
		break;
	case TS_PUNCH:
		fAcceleration++;
		RotateAroundY(-fAcceleration);
		if (Rotation.y <= 0)
		{
			Rotation.y = 0;
			fAcceleration = 0;
		}
		SetAlpha(1);
		gpTexture = GetTexture(TEXTURE_UI_CONTROLLER_TUTORIAL_ATTACK);
		break;
	case TS_SLIDE:
		fAcceleration++;
		RotateAroundY(-fAcceleration);
		if (Rotation.y <= 0)
		{
			Rotation.y = 0;
			fAcceleration = 0;
		}
		SetAlpha(1);
		gpTexture = GetTexture(TEXTURE_UI_CONTROLLER_TUTORIAL_GUITAR_DASH);
		break;
	case TS_GUITARJUMP:
		fAcceleration++;
		RotateAroundY(-fAcceleration);
		if (Rotation.y <= 0)
		{
			Rotation.y = 0;
			fAcceleration = 0;
		}
		SetAlpha(1);
		gpTexture = GetTexture(TEXTURE_UI_CONTROLLER_TUTORIAL_GUITAR_SHOOT);
		break;	
	case TS_DOUBLEJUMP:
		fAcceleration++;
		RotateAroundY(-fAcceleration);
		if (Rotation.y <= 0)
		{
			Rotation.y = 0;
			fAcceleration = 0;
		}
		SetAlpha(1);
		gpTexture = GetTexture(TEXTURE_UI_CONTROLLER_TUTORIAL_CATCH_BULLET);
		break;
	default:
		fAcceleration++;
		RotateAroundY(fAcceleration);
		if (Rotation.y >= 90)
		{
			Rotation.y = 90;
			fAcceleration = 0;
		}
		break;
	}
}

//*****************************************************************************
//PauseInstructionsControl関数
//ポース表示の説明を管理する
//引数：void
//戻：void
//*****************************************************************************
void UI_Object::PauseInstructionsControl()
{
	if (nCurrentPauseInstruction > IT_MAX)
		nCurrentPauseInstruction = IT_CONTROLS;
	if (nCurrentPauseInstruction < IT_CONTROLS)
		nCurrentPauseInstruction = IT_MAX;
	if (nCurrentPauseInstruction == IT_MAX)
		return;
	gpTexture = GetTexture(nCurrentPauseInstruction);
	switch (nCurrentPauseInstruction)
	{
	case IT_CONTROLS:
		nFrame_X = 1;
		nFrame_Y = 1;
		uv = { 0,0 };
		Size.x = 1280;
		
		if (Size.y < 720) {
			fAcceleration+=7;
			Size.y += fAcceleration;
		}
		else if (Size.y > 720) {
			Size.y = 720;
			fAcceleration=0;
		}
		SetWorldPosition(0, 0);
		break;
	case IT_COMBOS:
		nFrame_X = 1;
		nFrame_Y = 1;
		uv = { 0,0 };
		Size.y = 660;
		if (Size.x < 1177) {
			fAcceleration += 8;
			Size.x += fAcceleration;
		}
		else if (Size.x > 1177) {
			Size.x = 1177;
			fAcceleration = 0;
		}
		SetWorldPosition(0, 20);
		break;
	default:
		break;
	}
}

//*****************************************************************************
//RegularUVAnimationControl関数
//UVのアニメーションをコントロールする
//引数：int
//戻：void
//*****************************************************************************
void UI_Object::RegularUVAnimationControl(int speed)
{
	if (++n_uvTimer > speed)
	{
		n_uvTimer = 0;
		uv.U++;
		if (uv.U == nFrame_X)
		{
			uv.V++;
			uv.U = 0;
			if (uv.V == nFrame_Y)
				uv.V = 0;
		}
	}
}

//*****************************************************************************
//ResultScreenControl関数
//結果表示を管理する
//引数：void
//戻：void
//*****************************************************************************
void UI_Object::ResultScreenControl()
{
	if (pComboRank)
		pComboRank->Update();
	if (pTimeRank)
		pTimeRank->Update();
	if (pDamageRank)
		pDamageRank->Update();
	if (pOverrallRank)
		pOverrallRank->Update();
	if (!bResultScreenSettedUp)
	{
		if (bMessageOn)
		{
			XMFLOAT3 backgroundRot;
			if (pTransparentBlackBackground->GetRotation().y > 0)
			{
				if (pTransparentBlackBackground)
					backgroundRot = pTransparentBlackBackground->GetRotation();
				fAcceleration++;
				backgroundRot.y -= fAcceleration;
				if (backgroundRot.y < 0)
					backgroundRot.y = 0;
				if (backgroundRot.y == 0)
					fAcceleration = 0;
				pTransparentBlackBackground->SetRotation(backgroundRot);
				return;
			}
			if (pTransparentBlackBackground->GetRotation().y == 0 && pResultsTextLeft->GetRotation().y > 0)
			{
				if (pResultsTextLeft)
					backgroundRot = pResultsTextLeft->GetRotation();
				fAcceleration++;
				backgroundRot.y -= fAcceleration;
				if (backgroundRot.y < 0)
					backgroundRot.y = 0;
				if (backgroundRot.y == 0)
					fAcceleration = 0;
				pResultsTextLeft->SetRotation(backgroundRot);
				return;
			}

			if (pResultsTextLeft->GetRotation().y == 0 && pResultsTextRight->GetRotation().y > 0)
			{
				if (pResultsTextRight)
					backgroundRot = pResultsTextRight->GetRotation();
				fAcceleration++;
				backgroundRot.y -= fAcceleration;
				if (backgroundRot.y < 0)
					backgroundRot.y = 0;
				if (backgroundRot.y == 0)
					fAcceleration = 0;
				pResultsTextRight->SetRotation(backgroundRot);
				return;
			}

			if (pResultsTextRight->GetRotation().y == 0 && pComboText->GetRotation().y > 0)
			{
				if (pComboText)
					backgroundRot = pComboText->GetRotation();
				fAcceleration++;
				backgroundRot.y -= fAcceleration;
				if (backgroundRot.y < 0)
					backgroundRot.y = 0;
				if (backgroundRot.y == 0)
					fAcceleration = 0;
				pComboText->SetRotation(backgroundRot);
				return;
			}

			if (pComboText->GetRotation().y == 0 && pTimeText->GetRotation().y > 0)
			{
				if (pTimeText)
					backgroundRot = pTimeText->GetRotation();
				fAcceleration++;
				backgroundRot.y -= fAcceleration;
				if (backgroundRot.y < 0)
					backgroundRot.y = 0;
				if (backgroundRot.y == 0)
					fAcceleration = 0;
				pTimeText->SetRotation(backgroundRot);
				return;
			}

			if (pTimeText->GetRotation().y == 0 && pDamageText->GetRotation().y > 0)
			{
				if (pDamageText)
					backgroundRot = pDamageText->GetRotation();
				fAcceleration++;
				backgroundRot.y -= fAcceleration;
				if (backgroundRot.y < 0)
					backgroundRot.y = 0;
				if (backgroundRot.y == 0)
					fAcceleration = 0;
				pDamageText->SetRotation(backgroundRot);
				return;
			}

			if (pDamageText->GetRotation().y == 0 && pOverralText->GetRotation().y > 0)
			{
				if (pOverralText)
					backgroundRot = pOverralText->GetRotation();
				fAcceleration++;
				backgroundRot.y -= fAcceleration;
				if (backgroundRot.y < 0)
					backgroundRot.y = 0;
				if (backgroundRot.y == 0)
					fAcceleration = 0;
				pOverralText->SetRotation(backgroundRot);
				return;
			}

			if (pOverralText->GetRotation().y == 0 && pComboRank->GetRotation().y > 0)
			{
				if (pComboRank)
					backgroundRot = pComboRank->GetRotation();
				fAcceleration++;
				backgroundRot.y -= fAcceleration;
				if (backgroundRot.y < 0)
					backgroundRot.y = 0;
				if (backgroundRot.y == 0)
					fAcceleration = 0;
				pComboRank->SetRotation(backgroundRot);
				return;
			}
			if (pComboRank->GetRotation().y == 0 && pTimeRank->GetRotation().y > 0)
			{
				if (pTimeRank)
					backgroundRot = pTimeRank->GetRotation();
				fAcceleration++;
				backgroundRot.y -= fAcceleration;
				if (backgroundRot.y < 0)
					backgroundRot.y = 0;
				if (backgroundRot.y == 0)
					fAcceleration = 0;
				pTimeRank->SetRotation(backgroundRot);
				return;
			}

			if (pTimeRank->GetRotation().y == 0 && pDamageRank->GetRotation().y > 0)
			{
				if (pDamageRank)
					backgroundRot = pDamageRank->GetRotation();
				fAcceleration++;
				backgroundRot.y -= fAcceleration;
				if (backgroundRot.y < 0)
					backgroundRot.y = 0;
				if (backgroundRot.y == 0)
					fAcceleration = 0;
				pDamageRank->SetRotation(backgroundRot);
				return;
			}

			if (pDamageRank->GetRotation().y == 0 && pOverrallRank->GetRotation().y > 0)
			{
				if (pOverrallRank)
					backgroundRot = pOverrallRank->GetRotation();
				fAcceleration++;
				backgroundRot.y -= fAcceleration;
				if (backgroundRot.y < 0)
					backgroundRot.y = 0;
				if (backgroundRot.y == 0)
					fAcceleration = 0;
				pOverrallRank->SetRotation(backgroundRot);
				return;
			}
			bResultScreenSettedUp = true;
		}
		else {
			XMFLOAT3 backgroundRot;
			if (pResultsTextLeft->GetRotation().y < 90)
			{
				if (pResultsTextLeft)
					backgroundRot = pResultsTextLeft->GetRotation();
				fAcceleration++;
				backgroundRot.y += fAcceleration;
				if (backgroundRot.y > 90)
					backgroundRot.y = 90;
				if (backgroundRot.y == 90)
					fAcceleration = 0;
				pResultsTextLeft->SetRotation(backgroundRot);
				return;
			}

			if (pResultsTextLeft->GetRotation().y == 90 && pResultsTextRight->GetRotation().y < 90)
			{
				if (pResultsTextRight)
					backgroundRot = pResultsTextRight->GetRotation();
				fAcceleration++;
				backgroundRot.y += fAcceleration;
				if (backgroundRot.y > 90)
					backgroundRot.y = 90;
				if (backgroundRot.y == 90)
					fAcceleration = 0;
				pResultsTextRight->SetRotation(backgroundRot);
				return;
			}

			if (pResultsTextRight->GetRotation().y == 90 && pComboText->GetRotation().y < 90)
			{
				if (pComboText)
					backgroundRot = pComboText->GetRotation();
				fAcceleration++;
				backgroundRot.y += fAcceleration;
				if (backgroundRot.y > 90)
					backgroundRot.y = 90;
				if (backgroundRot.y == 0)
					fAcceleration = 0;
				pComboText->SetRotation(backgroundRot);
				return;
			}

			if (pComboText->GetRotation().y == 90 && pTimeText->GetRotation().y < 90)
			{
				if (pTimeText)
					backgroundRot = pTimeText->GetRotation();
				fAcceleration++;
				backgroundRot.y += fAcceleration;
				if (backgroundRot.y > 90)
					backgroundRot.y = 90;
				if (backgroundRot.y == 90)
					fAcceleration = 0;
				pTimeText->SetRotation(backgroundRot);
				return;
			}

			if (pTimeText->GetRotation().y == 90 && pDamageText->GetRotation().y < 90)
			{
				if (pDamageText)
					backgroundRot = pDamageText->GetRotation();
				fAcceleration++;
				backgroundRot.y += fAcceleration;
				if (backgroundRot.y > 90)
					backgroundRot.y = 90;
				if (backgroundRot.y == 90)
					fAcceleration = 0;
				pDamageText->SetRotation(backgroundRot);
				return;
			}

			if (pDamageText->GetRotation().y == 90 && pOverralText->GetRotation().y < 90)
			{
				if (pOverralText)
					backgroundRot = pOverralText->GetRotation();
				fAcceleration++;
				backgroundRot.y += fAcceleration;
				if (backgroundRot.y > 90)
					backgroundRot.y = 90;
				if (backgroundRot.y == 90)
					fAcceleration = 0;
				pOverralText->SetRotation(backgroundRot);
				return;
			}

			if (pOverralText->GetRotation().y == 90 && pComboRank->GetRotation().y < 90)
			{
				if (pComboRank)
					backgroundRot = pComboRank->GetRotation();
				fAcceleration++;
				backgroundRot.y += fAcceleration;
				if (backgroundRot.y > 90)
					backgroundRot.y = 90;
				if (backgroundRot.y == 90)
					fAcceleration = 0;
				pComboRank->SetRotation(backgroundRot);
				return;
			}
			if (pComboRank->GetRotation().y == 90 && pTimeRank->GetRotation().y < 90)
			{
				if (pTimeRank)
					backgroundRot = pTimeRank->GetRotation();
				fAcceleration++;
				backgroundRot.y += fAcceleration;
				if (backgroundRot.y > 90)
					backgroundRot.y = 90;
				if (backgroundRot.y == 90)
					fAcceleration = 0;
				pTimeRank->SetRotation(backgroundRot);
				return;
			}

			if (pTimeRank->GetRotation().y == 90 && pDamageRank->GetRotation().y < 90)
			{
				if (pDamageRank)
					backgroundRot = pDamageRank->GetRotation();
				fAcceleration++;
				backgroundRot.y += fAcceleration;
				if (backgroundRot.y > 90)
					backgroundRot.y = 90;
				if (backgroundRot.y == 90)
					fAcceleration = 0;
				pDamageRank->SetRotation(backgroundRot);
				return;
			}

			if (pDamageRank->GetRotation().y == 90 && pOverrallRank->GetRotation().y < 90)
			{
				if (pOverrallRank)
					backgroundRot = pOverrallRank->GetRotation();
				fAcceleration++;
				backgroundRot.y += fAcceleration;
				if (backgroundRot.y > 90)
					backgroundRot.y = 90;
				if (backgroundRot.y == 90)
					fAcceleration = 0;
				pOverrallRank->SetRotation(backgroundRot);
				return;
			}
			if (pOverrallRank->GetRotation().y == 90 && pTransparentBlackBackground->GetRotation().y < 90)
			{
				if (pTransparentBlackBackground)
					backgroundRot = pTransparentBlackBackground->GetRotation();
				fAcceleration++;
				backgroundRot.y += fAcceleration;
				if (backgroundRot.y > 90)
					backgroundRot.y = 90;
				if (backgroundRot.y == 90)
					fAcceleration = 0;
				pTransparentBlackBackground->SetRotation(backgroundRot);
				return;
			}
			bResultScreenSettedUp = true;
		}
	}
	else {
		if (bMessageOn) {
			if (++nFrameCountingBeforeDeactivation > RESULTS_SCREEN_TIME)
			{
				bMessageOn = false;
				bResultScreenSettedUp = false;
			}
		}
	}
	if (pResultsTextLeft)
		pResultsTextLeft->Update();
	if (pResultsTextRight)
		pResultsTextRight->Update();
}

//*****************************************************************************
//Draw関数
//レンダリングの関数
//引数：void
//戻：void
//*****************************************************************************
void UI_Object::Draw()
{
	SceneInGame* pGame = GetCurrentGame();
	if ((nType == UI_RANK && nRank <= 1) || ((nType == UI_STARTEVENT_MESSAGE || nType == UI_ENDEVENT_MESSAGE || nType == UI_INEVENT_NOTICE) && !bMessageOn)
		|| (nType == UI_PAUSE_CONTROL_INSTRUCTIONS && nCurrentPauseInstruction == IT_MAX))
		return;
	if (nType == UI_GEM && gpTexture == nullptr)
		gpTexture = GetTexture(TEXTURE_CRYSTAL);
	XMFLOAT3 x2OriginalPosition;
	int nNumberToPrint;
	int nDividerByTen;
	int nDigits = 0;
	switch (nType)
	{
	case UI_HEART:
		if (bHeartActive) 
		{
			SetSpriteSize(HEART_SIZE, HEART_SIZE);
			SetColor(1, 1, 1);
		}
		else 
		{
			SetSpriteSize(HEART_SIZE*0.75f, HEART_SIZE*0.75f);
			SetColor(0.25, 0.25, 0.25);
		}
		SetSpriteFrameSize(1.0f / nFrame_X, 1.0f / nFrame_Y);
		SetSpriteUV(uv.U / (float)nFrame_X, uv.V / (float)nFrame_Y);
		DrawSpriteUI(GetDeviceContext());
		break;
	case UI_RANK:
		MiniAura->Draw();
		SetSpriteFrameSize(1.0f / nFrame_X, 1.0f / nFrame_Y);
		SetSpriteUV(uv.U / (float)nFrame_X, uv.V / (float)nFrame_Y);
		DrawSpriteUI(GetDeviceContext());
		break;
	case UI_SCORE_NUMBERS:
		x2OriginalPosition = GetWorldPosition();
		nNumberToPrint = nFullNumber;
		for (nDividerByTen = 1; nNumberToPrint > 0; nNumberToPrint /= 10, nDividerByTen *= 10, nDigits++);
		nDividerByTen /= 10;
		if (nFullNumber < 10)
		{
			for (int i = 0; i < nMaxDigits; i++)
			{
				uv.U = 0;
				SetSpriteFrameSize(1.0f / nFrame_X, 1.0f / nFrame_Y);
				SetSpriteUV(uv.U / (float)nFrame_X, uv.V / (float)nFrame_Y);
				DrawSpriteUI(GetDeviceContext());
				Position.x += SCORE_NUM_SIZE * 0.75f;
			}
			uv.U = (float)nFullNumber;
			SetSpriteFrameSize(1.0f / nFrame_X, 1.0f / nFrame_Y);
			SetSpriteUV(uv.U / (float)nFrame_X, uv.V / (float)nFrame_Y);
			DrawSpriteUI(GetDeviceContext());
		}
		else {
			for (int i = 0; i < nMaxDigits - nDigits + 1; i++)
			{
				uv.U = 0;
				SetSpriteFrameSize(1.0f / nFrame_X, 1.0f / nFrame_Y);
				SetSpriteUV(uv.U / (float)nFrame_X, uv.V / (float)nFrame_Y);
				DrawSpriteUI(GetDeviceContext());
				Position.x += SCORE_NUM_SIZE * 0.75f;
			}
			while (nDividerByTen > 0)
			{
				nNumberToPrint = nFullNumber / nDividerByTen % 10;
				SetSpriteUV(nNumberToPrint / (float)nFrame_X, uv.V / (float)nFrame_Y);
				DrawSpriteUI(GetDeviceContext());
				Position.x += SCORE_NUM_SIZE * 0.75f;
				nDividerByTen /= 10;
			}
		}
		SetWorldPosition(x2OriginalPosition.x, x2OriginalPosition.y);
		break;
	case UI_RANKING_NUMBERS: case UI_EDITORSLOT: case UI_EDITORSPEED: case UI_EDITORDELAY:
		x2OriginalPosition = GetWorldPosition();
		nNumberToPrint = nFullNumber;
		for (nDividerByTen = 1; nNumberToPrint > 0; nNumberToPrint /= 10, nDividerByTen *= 10, nDigits++);
		nDividerByTen /= 10;
		if (nFullNumber < 10)
		{
			for (int i = 0; i < nMaxDigits; i++)
			{
				uv.U = 0;
				SetSpriteFrameSize(1.0f / nFrame_X, 1.0f / nFrame_Y);
				SetSpriteUV(uv.U / (float)nFrame_X, uv.V / (float)nFrame_Y);
				DrawSpriteUI(GetDeviceContext());
				Position.x += SCORE_RANKNUM_SIZE * 0.75f;
			}
			uv.U = (float)nFullNumber;
			SetSpriteFrameSize(1.0f / nFrame_X, 1.0f / nFrame_Y);
			SetSpriteUV(uv.U / (float)nFrame_X, uv.V / (float)nFrame_Y);
			DrawSpriteUI(GetDeviceContext());
		}
		else {
			for (int i = 0; i < nMaxDigits - nDigits + 1; i++)
			{
				uv.U = 0;
				SetSpriteFrameSize(1.0f / nFrame_X, 1.0f / nFrame_Y);
				SetSpriteUV(uv.U / (float)nFrame_X, uv.V / (float)nFrame_Y);
				DrawSpriteUI(GetDeviceContext());
				Position.x += SCORE_RANKNUM_SIZE * 0.75f;
			}
			while (nDividerByTen > 0)
			{
				nNumberToPrint = nFullNumber / nDividerByTen % 10;
				SetSpriteUV(nNumberToPrint / (float)nFrame_X, uv.V / (float)nFrame_Y);
				DrawSpriteUI(GetDeviceContext());
				Position.x += SCORE_RANKNUM_SIZE * 0.75f;
				nDividerByTen /= 10;
			}
		}
		SetWorldPosition(x2OriginalPosition.x, x2OriginalPosition.y);
		break;
	case UI_RESULTS_SCREEN:
		if (pTransparentBlackBackground->GetRotation().y == 90)
			return;
		if (pTransparentBlackBackground)
			pTransparentBlackBackground->Draw();
		if (pComboText)
			pComboText->Draw();
		if (pTimeText)
			pTimeText->Draw();		
		if (pDamageText)
			pDamageText->Draw();		
		if (pOverralText)
			pOverralText->Draw();		
		if (pResultsTextLeft)
			pResultsTextLeft->Draw();
		if (pResultsTextRight)
			pResultsTextRight->Draw();
		if (pComboRank)
			pComboRank->Draw();
		if (pTimeRank)
			pTimeRank->Draw();
		if (pDamageRank)
			pDamageRank->Draw();
		if (pOverrallRank)
			pOverrallRank->Draw();
		break;
	case UI_PAUSE_SCREEN:
		if (!(pGame->IsGamePaused()))
			return;
		if (pTransparentBlackBackground)
			pTransparentBlackBackground->Draw();
		if (pPauseText)
			pPauseText->Draw();
		if (pGemsNums)
			pGemsNums->Draw();
		if (pGemsImage)
			pGemsImage->Draw();
		if (pControlInstructions)
			pControlInstructions->Draw();
		break;
	case UI_STAGECLEAR_RESULTS:
		if (!bActivateStageClear)
			return;
		if (pStageClearBackground)
			pStageClearBackground->Draw();
		if (pStageClearBackground->GetSize().x < 1280 && pStageClearBackground->GetSize().x < 720)
			return;
		if (pScoreNums)
			pScoreNums->Draw();
		if (pGemsNums)
			pGemsNums->Draw();
		break;
	case UI_TUTORIAL_SCREEN:
		if (pTutorialBorderUp)
			pTutorialBorderUp->Draw();
		if (pTutorialBorderDown)
			pTutorialBorderDown->Draw();
		if (pInTutorialText)
			pInTutorialText->Draw();
		if (pTutorialMessage)
			pTutorialMessage->Draw();
		if (pControllerMessage)
			pControllerMessage->Draw();
		break;
	case UI_TUTORIAL_TEXT:
		SetSpriteFrameSize(1.0f / nFrame_X, 1.0f / nFrame_Y);
		SetSpriteUV(uv.U / (float)nFrame_X, uv.V / (float)nFrame_Y);
		DrawSpriteUI(GetDeviceContext());
		break;
	default:
		SetSpriteFrameSize(1.0f / nFrame_X, 1.0f / nFrame_Y);
		SetSpriteUV(uv.U / (float)nFrame_X, uv.V / (float)nFrame_Y);
		DrawSpriteUI(GetDeviceContext());
		break;
	}

}

//*****************************************************************************
//End関数
//処理の終了関数
//引数：void
//戻：void
//*****************************************************************************
void UI_Object::End()
{
	UninitSprite();
	SAFE_DELETE(MiniAura);
	SAFE_DELETE(pComboText);
	SAFE_DELETE(pTimeText);
	SAFE_DELETE(pDamageText);
	SAFE_DELETE(pOverralText);
	SAFE_DELETE(pResultsTextLeft);
	SAFE_DELETE(pResultsTextRight);
	SAFE_DELETE(pStageClearBackground);
	SAFE_DELETE(pTransparentBlackBackground);
	SAFE_DELETE(pComboRank);
	SAFE_DELETE(pTimeRank);
	SAFE_DELETE(pDamageRank);
	SAFE_DELETE(pOverrallRank);
	SAFE_DELETE(pPauseText);
	SAFE_DELETE(pControlInstructions);
	SAFE_DELETE(pScoreNums);
	SAFE_DELETE(pGemsNums);
	SAFE_DELETE(pGemsImage);
	SAFE_DELETE(pInTutorialText);
	SAFE_DELETE(pTutorialBorderUp);
	SAFE_DELETE(pTutorialBorderDown);
	SAFE_DELETE(pTutorialMessage);

	for (int i = 0; i < TS_MAX; i++)
		SAFE_RELEASE(gpTutorialMessages[i]);
}

//*****************************************************************************
//SetRank関数
//ランクを設定する
//引数：int
//戻：void
//*****************************************************************************
void UI_Object::SetRank(int newRank)
{
	nRank = newRank;
}

//*****************************************************************************
//SetHeartActive関数
//心のイメージを活性化する
//引数：bool
//戻：void
//*****************************************************************************
void UI_Object::SetHeartActive(bool heartactive)
{
	bHeartActive = heartactive;
}

//*****************************************************************************
//SetNumberUI関数
//UIの数字を設定する
//引数：int
//戻：void
//*****************************************************************************
void UI_Object::SetNumberUI(int newNum)
{
	nFullNumber = newNum;
}

//*****************************************************************************
//SetMessageOn関数
//UIのメッセージかイメージを活性化する
//引数：void
//戻：void
//*****************************************************************************
void UI_Object::SetMessageOn()
{
	uv = { 0,0 };
	bMessageOn = true;
}

//*****************************************************************************
//SetMessageOn関数
//UIのメッセージかイメージを活性化する
//引数：char, char, char, char
//戻：void
//*****************************************************************************
void UI_Object::SetMessageOn(char Time, char Combo, char Damage, char Overrall)
{
	bMessageOn = true;
	if (nType == UI_RESULTS_SCREEN) {
		bResultScreenSettedUp = false;
		nFrameCountingBeforeDeactivation = 0;
		if (pComboRank)
			pComboRank->SetRankResult(Combo);
		if (pTimeRank)
			pTimeRank->SetRankResult(Time);
		if (pDamageRank)
			pDamageRank->SetRankResult(Damage);
		if (pOverrallRank)
			pOverrallRank->SetRankResult(Overrall);
	}else{
		cTime = Time;
		cCombo = Combo;
		cDamage = Damage;
		cOverrall = Overrall;
	}
}

//*****************************************************************************
//SetMessageOn関数
//UIのメッセージかイメージを無効にする
//引数：void
//戻：void
//*****************************************************************************
void UI_Object::SetMessageOff()
{
	bMessageOn = false;
}

//*****************************************************************************
//ActivateFinalResultsScreen関数
//ステージの結果を活性化する
//引数：void
//戻：void
//*****************************************************************************
void UI_Object::ActivateFinalResultsScreen()
{
	bActivateStageClear = true;
}

//*****************************************************************************
//SetUpDownDirection関数
//方向を設定する
//引数：int
//戻：void
//*****************************************************************************
void UI_Object::SetUpDownDirection(int ndir)
{
	nDirectionUpdiseDown = ndir;
}

//*****************************************************************************
//SetRankResult関数
//結果を設定する
//引数：char
//戻：void
//*****************************************************************************
void UI_Object::SetRankResult(char result)
{
	cRankResult = result;
}

//*****************************************************************************
//SetMaxDigits関数
//桁の範囲を設定する
//引数：int
//戻：void
//*****************************************************************************
void UI_Object::SetMaxDigits(int newDigits)
{
	nMaxDigits = newDigits;
}

void UI_Object::SetNewTutorialMessage(int nNewMessage)
{
	if (nType == UI_TUTORIAL_SCREEN)
	{
		if (pTutorialMessage)
			pTutorialMessage->SetNewTutorialMessage(nNewMessage);
		if (pControllerMessage)
			pControllerMessage->SetNewTutorialMessage(nNewMessage);
		return;
	}
	else {
		nCurrentTutorialMessage = nNewMessage;
	}
}

//*****************************************************************************
//IsResultScreenSetUp関数
//結果表示は設定したことを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool UI_Object::IsResultScreenSetUp()
{
	return bStageClearSettedUp;
}

//*****************************************************************************
//SetAsSelectedOption関数
//選んだオプションを設定する
//引数：bool
//戻：void
//*****************************************************************************
void UI_Object::SetAsSelectedOption(bool isSelectedOption)
{
	bIsTheSelectedOption = isSelectedOption;
}

//*****************************************************************************
//SetPauseInstructions関数
//ポースの操作説明を設定する
//引数：int
//戻：void
//*****************************************************************************
void UI_Object::SetPauseInstructions(int newPauseInstructions)
{
	if (nType == UI_PAUSE_SCREEN) 
	{
		pControlInstructions->SetPauseInstructions(newPauseInstructions);
		pControlInstructions->SetSpriteSize(0, 0);
		return;
	}
	if (nCurrentPauseInstruction == newPauseInstructions)
		return;
	nCurrentPauseInstruction = newPauseInstructions;
}

//*****************************************************************************
//SetPositionArrow関数
//矢印のUIの経典を設定する
//引数：XMFLOAT2
//戻：void
//*****************************************************************************
void UI_Object::SetPositionArrow(XMFLOAT2 newPos)
{
	Position = { newPos.x, newPos.y, 0 };
	x2PosA = newPos;
	x2PosB = newPos;
	x2PosB.y += 5;
}

//*****************************************************************************
//SetArrowActivation関数
//矢印のUIを設定する
//引数：bool
//戻：void
//*****************************************************************************
void UI_Object::SetArrowActivation(bool activation)
{
	bIsArrowActive = activation;
}

//*****************************************************************************
//ActivateSavedLevelMessage関数
//保存したメッセージを表す
//引数：void
//戻：void
//*****************************************************************************
void UI_Object::ActivateSavedLevelMessage()
{
	gpTexture = GetTexture(TEXTURE_UI_LEVEL_SAVED_TEXT);
	SetAlpha(1);
	fAlphaTemp = 1;
}

//*****************************************************************************
//ActivateLoadedLevelMessage関数
//ロードしたメッセージを表す
//引数：void
//戻：void
//*****************************************************************************
void UI_Object::ActivateLoadedLevelMessage()
{
	gpTexture = GetTexture(TEXTURE_UI_LEVEL_LOADED_TEXT);
	SetAlpha(1);
	fAlphaTemp = 1;
}
