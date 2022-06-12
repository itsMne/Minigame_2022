//*****************************************************************************
// SceneTitleScreen.h
// Author : Mane
//*****************************************************************************
#ifndef	SCENETITLESCREEN_H
#define	SCENETITLESCREEN_H

#include "SceneBase.h"
#include "GameObject.h"

//*****************************************************************************
// �N���X
//*****************************************************************************
class SceneTitleScreen :
	public SceneBase
{
private: 
	GameObject* goTitle_Screen;
	bool		bFirstInit;
public:
	SceneTitleScreen();
	~SceneTitleScreen();
	void Init();
	void Update();
	void Draw();
	void End();
};

#endif