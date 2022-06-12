#ifndef	SCENELOGOS_H
#define	SCENELOGOS_H

#include "SceneBase.h"
#include "GameObject.h"

//*****************************************************************************
// �N���X
//*****************************************************************************
class SceneLogos :
	public SceneBase
{
private:
	GameObject* pLogo;
public:
	SceneLogos();
	~SceneLogos();
	void Init();
	void Update();
	void Draw();
	void End();
};

#endif