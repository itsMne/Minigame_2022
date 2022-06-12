//*****************************************************************************
// SceneMainMenu.h
// Author : Mane
//*****************************************************************************
#ifndef	SCENEMAINMENU_H
#define	SCENEMAINMENU_H

#include "SceneBase.h"
#include "UI_Object.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_RANK_POSITIONS 5

//*****************************************************************************
// エナム
//*****************************************************************************
enum GAME_MENU_OPTIONS
{
	NEW_GAME_START,
#if DEBUG_ROOM_ACTIVE
	DEBUG_MENU_OPTION,
#endif
	TUTORIAL_START,
	SHOW_RANKING,
	EXTRA_OPTIONS,
	END_GAME,
	MAX_OPTIONS
};

enum GAME_MENU_EXTRA_OPTIONS
{
	SET_NGNL,
	SET_HELLSTOUCH,
	SET_SHOWFPS,
	RETURN_PREVIOUS,
	MAX_EXTRA_OPTIONS
};

//*****************************************************************************
// 構造体
//*****************************************************************************
struct RankLevelName
{
	int Texture;
	XMFLOAT2 Size;
};

//*****************************************************************************
// クラス
//*****************************************************************************
class SceneMainMenu :
	public SceneBase
{
private:
	GameObject* p_goMenuBackground;
	GameObject* p_goRankingBackground;
	GameObject* p_goExtraBackground;
	GameObject* goOptions[MAX_OPTIONS];
	GameObject* goExtraOptions[MAX_EXTRA_OPTIONS];
	GameObject* p_goObjDecorationMenu;
	GameObject* pSubMenuInfo[MAX_EXTRA_OPTIONS];
	UI_Object*	p_UIRankingText;
	UI_Object*	p_UILevelName;
	UI_Object*	p_UIRankingScores[MAX_RANK_POSITIONS];
	bool		bBackGroundLoaded;
	bool		bRankingScreenOn;
	bool		bExtraOptionsScreenOn;
	int			nCurrentSelectedOption;
	int			nCurrentSelectedExtraOption;
	int			nRankCurrentLevel;
	RankLevelName rln_RankLevelNames[MAX_ROOMS];
public:
	SceneMainMenu();
	~SceneMainMenu();
	void Init();
	void Update();
	void Draw();
	void End();
	
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void AddScoreToTopRankings(int ScoreToAdd, int Level);

#endif