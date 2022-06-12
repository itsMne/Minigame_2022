#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include "GameObject.h"
#include "Enemy.h"

//*****************************************************************************
// エナム定義
//*****************************************************************************
enum SubLevelControl
{
	TUTORIAL_R = 0,
	SUBLEVEL1_1,
	SUBLEVEL1_2,
	SUB_MAX
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
namespace InGame
{
	void Init();
	void TutorialObjects();
	void Level1_1Objects();
	void Level1_2Objects();
	void DebugRoomObjects();
	void Update();
	void Draw();
	void End();
}

void  SetLevel(int newLevel);
void  SetSubLevel(int newSubLevel);
void  AddToScore(int nToAdd);
void  AddToScoreWithMultiplier(int nToAdd);
void  AddGem(int newGemNum);
void  AddMaxGemNum(int newGemNum);
void  SetLevelScale(float newScale);
void  PauseGame();
void  UnpauseGame();
float GetLevelScale();
int	  GetScore();
int	  GetGems();
int	  GetCurrentLevel();
int	  GetCurrentSubLevel();
bool  IsGamePaused();
Color GetCurrentBackgroundColor();
Go_List * GetFloors();
Go_List * GetWalls();
Go_List * GetLevelObjects();
Go_List * GetPickableGems();
Enemy_List * GetEnemies();
#endif // !GAMEMANAGER_H