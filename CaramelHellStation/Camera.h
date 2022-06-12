//*****************************************************************************
// Camera.h
// Author : Mane
//*****************************************************************************
#ifndef CAMERA_H
#define CAMERA_H

#include "Sprite.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void		InitCamera();
void		UpdateCamera();
void		EndCamera();
XMFLOAT3	GetCameraPos();
void		SetFocalPoint(Sprite* newFocalPoint);
Sprite*		GetFocalPoint();
void		ResetCameraToOrigin();
void		SetCameraAdjustment(XMFLOAT3 adj);
void		CameraTremble(float fTremblePowerX, float fTremblePowerY, int n_TrembleTime);
float		GetCameraLimitRight();
float		GetCameraLimitLeft();
float		GetCameraLimitUp();
float		GetCameraLimitDown();
bool		IsCameraOnFocus();
float		GetZoomOffset();
void		SetZoomOffset(float ZoomPower, float ZoomSpeed, int Frames);
#endif // !CAMERA_H
