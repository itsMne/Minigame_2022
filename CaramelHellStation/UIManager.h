//*****************************************************************************
// UIManager.h
// Author : Mane
//*****************************************************************************
#ifndef	UIMANAGER_H
#define	UIMANAGER_H

#include "C_DXWindow.h"
#include "Player.h"
#include "UI_Object.h"

//*****************************************************************************
// ƒNƒ‰ƒX
//*****************************************************************************
class UIManager
{
private:
	XMFLOAT2		x2HeartPosition;
	Player*			pPlayer;
	UI_Object*		Hearts;
	UI_Object*		Rank;
	UI_Object*		Score;
	UI_Object*		TutorialScreen;
	UI_Object*		TutorialArrow;
	UI_Object*		AtkZoom;
	UI_Object*		EditorInstructionsA;
	UI_Object*		EditorInstructionsB;
	UI_Object*		EditorSlot;
	UI_Object*		TextEditorSlot;
	UI_Object*		EditorSpeed;
	UI_Object*		TextEditorSpeed;
	UI_Object*		EditorDelay;
	UI_Object*		TextEditorDelay;
	UI_Object*		TextEditorLevelSavedLoadedMessage;
	bool			bFightTutorialOn;
	Enemy*			TutorialEnemy;

public:
	UIManager();
	~UIManager();
	void Init();
	void Update();
	void TutorialControl();
	void Draw();
	void End();
	void ActivateSavedLevelMessage();
	void ActivateLoadedLevelMessage();
	UI_Object* GetAttackZoom();
	static void SetStartEventMessageOn();
	static void SetEndEventMessageOn(char Time, char Combo, char Damage, char Overrall);
	static void SetOnAirMessageUse(bool use);
	static void SetResultsScreenOn(char Time, char Combo, char Damage, char Overrall);
	static void ActivateFinalResults();
	static bool IsResultScreenUp();
};

#endif