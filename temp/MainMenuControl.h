#ifndef MAINMENUCONTROL_H
#define MAINMENUCONTROL_H

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitMenuControl();
void UpdateMenuControl();
void DrawMenuControl();
void EndMenuControl();

void AddScoreToTopRankings(int ScoreToAdd, int Level);
#endif // !MAINMENUCONTROL_H

