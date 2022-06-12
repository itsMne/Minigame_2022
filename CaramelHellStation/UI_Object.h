//*****************************************************************************
// UI_Object.h
// Author : Mane
//*****************************************************************************
#ifndef UI_OBJECT_H
#define UI_OBJECT_H

#include "Sprite.h"
#include "GameObject.h"
#include "TextureManager.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCORE_RANKNUM_SIZE		110/1.35f

//*****************************************************************************
// エナム定義
//*****************************************************************************
enum UI_TYPE
{
	UI_HEART=0,
	UI_RANK,
	UI_SCORE_NUMBERS,
	UI_RANKING_NUMBERS,
	UI_STARTEVENT_MESSAGE,
	UI_ENDEVENT_MESSAGE,
	UI_INEVENT_NOTICE,
	UI_RESULTS_SCREEN,
	UI_COMBO_TEXT,
	UI_TIME_TEXT,
	UI_DAMAGE_TEXT,
	UI_OVERRALL_TEXT,
	UI_RESULTS_TEXT,
	UI_RESULT_RANK_TEXT,
	UI_PAUSE_SCREEN,
	UI_PAUSE_TEXT,
	UI_PAUSE_CONTROL_INSTRUCTIONS,
	UI_STAGECLEAR_RESULTS,
	UI_STAGECLEAR_BACKGROUND,
	UI_GEM,
	UI_RANKINGSCREEN_RANKINGTEXT,
	UI_TUTORIAL_SCREEN,
	UI_TUTORIAL_TEXT,
	UI_TUTORIAL_MESSAGE,
	UI_CONTROLLER_MESSAGE,
	UI_PAUSE_MENU_OPTIONS_CONTINUE,
	UI_PAUSE_MENU_OPTIONS_CONTROLS,
	UI_PAUSE_MENU_OPTIONS_COMMANDS,
	UI_PAUSE_MENU_OPTIONS_RETURNTITLE,
	UI_ARROW,
	UI_ATK_EFFECT,
	UI_EDITOR_INSTRUCTIONS_A,
	UI_EDITOR_INSTRUCTIONS_B,
	UI_EDITORSLOT,
	UI_EDITORSPEED,
	UI_EDITORDELAY,
	UI_EDITORSLOT_TEXT,
	UI_EDITORSPEED_TEXT,
	UI_EDITORDELAY_TEXT,
	UI_LEVEL_LOADEDSAVED_TEXT,
	UI_MAX
};

enum TS_TUTORIAL_MESSAGES
{
	TS_MOVEMENT = 0,
	TS_JUMP,
	TS_PUNCH,
	TS_SLIDE,
	TS_GUITARJUMP,
	TS_DOUBLEJUMP,
	TS_SWITCHPUNCH,
	TS_SWITCHSHOOT,
	TS_EVENT,
	TS_ENEMY,
	TS_ENEMYSUPERARMOR,
	TS_ENEMYSUPERARMOR2,
	TS_ENEMYSUPERARMOR3,
	TS_ENEMYSUPERARMOR4,
	TS_ENEMYSUPERARMOR5,
	TS_MAX
};

enum INSTRUCTIONS_TEXTURES
{
	IT_CONTROLS = TEXTURE_PAUSE_CONTROLS,
	IT_COMBOS = TEXTURE_PAUSE_MOVES,
	IT_MAX
};
//*****************************************************************************
// クラス
//*****************************************************************************
class UI_Object :
	public Sprite
{
private:
	int			nType;
	int			nFrame_X, nFrame_Y;
	int			nAnimationTimer;
	tUV			uv;
	int			n_uvTimer;
	float		fAlphaTemp;
	//ハートのUIの変数
	bool		bHeartActive;
	//ランクのUIの変数
	GameObject* MiniAura;
	int			nRank;
	//数字
	int			nFullNumber;
	int			nMaxDigits;
	//イベントメッセージの変数
	bool		bMessageOn;
	char		cTime, cCombo, cDamage, cOverrall;
	//イベント中のメッセージの変数
	int			nAnimationMirror;
	//結果表示の変数
	UI_Object*	pComboText;
	UI_Object*	pTimeText;
	UI_Object*	pDamageText;
	UI_Object*	pOverralText;
	UI_Object*	pResultsTextLeft;
	UI_Object*	pResultsTextRight;
	UI_Object*	pTransparentBlackBackground;
	UI_Object*	pComboRank;
	UI_Object*	pTimeRank;
	UI_Object*	pDamageRank;
	UI_Object*	pOverrallRank;
	UI_Object*	pPauseText;
	UI_Object*	pControlInstructions;
	char		cRankResult;
	int			nDirectionUpdiseDown;
	float		fAcceleration;
	int			nFrameCountingBeforeDeactivation;
	bool		bResultScreenSettedUp;
	//ステージクリアの変数
	UI_Object*	pStageClearBackground;
	UI_Object*	pScoreNums;
	UI_Object*	pGemsNums;
	UI_Object*	pGemsImage;
	//チュートリアルに関して
	UI_Object* pInTutorialText;
	UI_Object* pTutorialMessage;
	UI_Object* pControllerMessage;
	UI_Object* pTutorialBorderUp;
	UI_Object* pTutorialBorderDown;
	int		   nCurrentTutorialMessage;
	bool		bSizeUp;
	//ステージクリアに関して
	bool		bActivateStageClear;
	bool		bStageClearSettedUp;
	int			nCurrentPauseInstruction;
	//ポースメニューのオプションに関して
	bool bIsTheSelectedOption;
	XMFLOAT3 x3_OGSize;
	//やじるしの変数
	XMFLOAT2 x2PosA;
	XMFLOAT2 x2PosB;
	int nFrameCountAlternatePos;
	bool bIsPosA;
	bool bIsArrowActive;
	void* PlayerPointer;
public:
	UI_Object();
	UI_Object(int Type);
	~UI_Object();

	void Init(int Type);
	void Update();
	void ControllerTutorialControl();
	void PauseInstructionsControl();
	void RegularUVAnimationControl(int speed);
	void ResultScreenControl();
	void Draw();
	void End();
	void SetRank(int newRank);
	void SetHeartActive(bool);
	void SetNumberUI(int newNum);
	void SetMessageOn();
	void SetMessageOn(char Time, char Combo, char Damage, char Overrall);
	void SetMessageOff();
	void ActivateFinalResultsScreen();
	void SetUpDownDirection(int ndir);
	void SetRankResult(char result);
	void SetMaxDigits(int newDigits);
	void SetNewTutorialMessage(int nNewMessage);
	bool IsResultScreenSetUp();
	void SetAsSelectedOption(bool isSelectedOption);
	void SetPauseInstructions(int newPauseInstructions);
	void SetPositionArrow(XMFLOAT2 newPos);
	void SetArrowActivation(bool activation);
	void ActivateSavedLevelMessage();
	void ActivateLoadedLevelMessage();
};

#endif
