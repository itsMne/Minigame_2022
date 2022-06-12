//*****************************************************************************
// SceneGameOver.h
// Author : Mane
//*****************************************************************************
#ifndef	SCENEGAMEOVER_H
#define	SCENEGAMEOVER_H

#include "SceneBase.h"
#include "GameObject.h"

//*****************************************************************************
// ƒNƒ‰ƒX
//*****************************************************************************
class SceneGameOver :
	public SceneBase
{
private:
	GameObject* GameOverScreen;
	GameObject* GameOverMessage;
public:
	SceneGameOver();
	~SceneGameOver();
	void Init();
	void Update();
	void Draw();
	void End();
};

#endif