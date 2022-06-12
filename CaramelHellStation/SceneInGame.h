//*****************************************************************************
// SceneInGame.h
// Author : Mane
//*****************************************************************************
#ifndef	SCENEINGAME_H
#define	SCENEINGAME_H

#include "SceneBase.h"
#include "GameObject.h"
#include "Enemy.h"
#include "Player.h"
#include "UIManager.h"
#include "GameObject_List.h"

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
// クラス
//*****************************************************************************
class SceneInGame :
	public SceneBase
{
private:
	bool		bGameIsPaused;
	bool		bSetDeathScore;
	int			nScore;
	int			nGems;
	int			nMaxGems;
	Enemy_List* Enemies;
	Go_List*	PickableGems;
	Go_List*	Floors;
	Go_List*	Walls;
	Go_List*	LevelObjects;
	Player*		g_Player;
	UIManager*  pUIManager;
	int			nCurrentLevel;
	Color		ColorBackground;
	float		fLevelScale;
	int			nCurrent_SubLevel;
	GameObject* pBackground;
	GameObject* pBackgroundEvent;
public:
	SceneInGame(int nLevel);
	~SceneInGame();
	void Init();
	void Update();
	void Draw();
	void End();
	void TutorialObjects();
	void Level1_1Objects();
	void Level1_2Objects();
	void DebugRoomObjects();
	void  SetSubLevel(int newSubLevel);
	void  AddToScore(int nToAdd);
	void  AddToScoreWithMultiplier(int nToAdd);
	void  AddGem(int newGemNum);
	void  AddMaxGemNum(int newGemNum);
	void  PauseGame();
	void  UnpauseGame();
	int	  GetScore();
	int	  GetGems();
	bool  IsGamePaused();

	Go_List * GetFloors();
	Go_List * GetWalls();
	Go_List * GetLevelObjects();
	Go_List * GetPickableGems();
	Enemy_List * GetEnemies();
	UIManager* GetUIManager();
	Color GetCurrentBackgroundColor();
	void  SetLevel(int newLevel);
	int	  GetCurrentLevel();
	int	  GetCurrentSubLevel();
	float GetLevelScale();
	void  SetLevelScale(float newScale);
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
SceneInGame* GetCurrentGame();

#endif