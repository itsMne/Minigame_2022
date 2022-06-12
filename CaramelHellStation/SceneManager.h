//*****************************************************************************
// SceneManager.h
// Author : Mane
//*****************************************************************************
#ifndef	SCENEMANAGER_H
#define	SCENEMANAGER_H

#include <windows.h>
#include "Sprite.h"

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InitScenes();
void UpdateScenes();
void SetNewState(int newState);
void DrawScenes();
void EndScenes();
void SetFPS(int newFPS);
int GetCurrentState();
void SetGameLevel(int nLevel);

#endif // !OBJECTMANAGER_H