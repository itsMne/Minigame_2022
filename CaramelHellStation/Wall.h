//*****************************************************************************
// Wall.h
// Author : Mane
//*****************************************************************************
#ifndef WALL_H
#define WALL_H
#include "GameObject.h"

//*****************************************************************************
// エナム定義
//*****************************************************************************
enum eWallActivationDeactivation
{
	WALL_NORMAL,
	WALL_SCALE_POSITIVE,
	WALL_SCALE_NEGATIVE,
};

//*****************************************************************************
// クラス
//*****************************************************************************
class Wall :
	public GameObject
{
private:
	//壁の変数
	int			nWallID;
	int			nWallScaleActivation;
	int			nWallTexture;
	bool		bDeactivateWhenScaleIsNegative;
	bool		bDeactivateWhenScaleIsPositive;
	HitBox		hbOriginalHitbox;
public:
	Wall(int TextureID, int n_Type, HitBox hbox, int WallTileSize, int nActivation);
	~Wall();
	void Init(int TextureID, int n_Type, HitBox hbox, int WallTileSize);
	void Update();
	void Draw();
	void End();
	void SetWallID(int newID);
	void SetWallDeactivateableWhenScaleIsNegative(bool deact);
	void SetWallDeactivateableWhenScaleIsPositive(bool act);
	int GetWallID();
	int GetWallTexture();
	int GetTypeScaleActivation();
};

#endif