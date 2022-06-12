#ifndef UI_MANAGER
#define UI_MANAGER

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitUIManager();
void UpdateUIManager();
void TutorialControl();
void DrawUIManager();
void EndUIManager();
void SetStartEventMessageOn();
void SetEndEventMessageOn(char Time, char Combo, char Damage, char Overrall);
void SetOnAirMessageUse(bool use);
void SetResultsScreenOn(char Time, char Combo, char Damage, char Overrall);
void ActivateFinalResults();
bool IsResultScreenUp();
#endif