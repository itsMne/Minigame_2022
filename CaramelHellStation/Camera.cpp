//*****************************************************************************
// Camera.cpp
// ゲームのカメラの管理
// Author : Mane
//*****************************************************************************
#include "Camera.h"
#include "Player.h"
#include <stdio.h>
#include "UniversalVariables.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CAMERA_SPEED 1.5f

//*****************************************************************************
// グローバル変数
//*****************************************************************************
XMFLOAT3 xmfCamPosition;
XMFLOAT3 xmfAdjustment;
Sprite*	 sFocalPoint;
float	 fTrembleForce_X;
float	 fTrembleForce_Y;
int		 nTrembleTime;
float	 fAcceleration;
bool	 bFocused;
float	fZoomOffset;
float	fZoomPower;
int		nZoomOffsetFrames;
float	fZoomOffsetAcceleration;
float   fZoomSpeed;

//*****************************************************************************
//InitCamera関数
//初期化の関数
//引数：void
//戻：void
//*****************************************************************************
void InitCamera()
{
	xmfCamPosition = { 0,0,0 };
	sFocalPoint = nullptr;
	xmfAdjustment = { 0,0,0 };
	fTrembleForce_X = 0;
	fTrembleForce_Y = 0;
	nTrembleTime = 0;
	fAcceleration = 0;
	fZoomOffset = 1;
	bFocused = false;
	nZoomOffsetFrames = 0;
	fZoomOffsetAcceleration = 0;
	fZoomPower = 0;
	fZoomSpeed = 0;
}

//*****************************************************************************
//UpdateCamera関数
//処理のアップデートの関数
//引数：void
//戻：void
//*****************************************************************************
void UpdateCamera()
{
	if (--nZoomOffsetFrames<=0) {
		nZoomOffsetFrames = 0;
		if (fZoomOffset != 1)
		{
			fZoomOffsetAcceleration += fZoomSpeed;
			if (fZoomOffset > 1)
			{
				fZoomOffset -= fZoomOffsetAcceleration;
				if (fZoomOffset < 1)
					fZoomOffset = 1;
			}
			else if (fZoomOffset < 1)
			{
				fZoomOffset += fZoomOffsetAcceleration;
				if (fZoomOffset > 1)
					fZoomOffset = 1;
			}
		}
		else if (fZoomOffset == 1)
		{
			fZoomOffsetAcceleration = 0;
		}
	}
	else
	{
		if (fZoomOffset != fZoomPower)
		{
			fZoomOffsetAcceleration += fZoomSpeed;
			if (fZoomOffset > fZoomPower)
			{
				fZoomOffset -= fZoomOffsetAcceleration;
				if (fZoomOffset < fZoomPower) {
					fZoomOffset = fZoomPower;
					fZoomOffsetAcceleration = 0;
				}
			}
			else if (fZoomOffset < fZoomPower)
			{
				fZoomOffset += fZoomOffsetAcceleration;
				if (fZoomOffset > fZoomPower) {
					fZoomOffset = fZoomPower;
					fZoomOffsetAcceleration = 0;
				}
			}
		}

	}
	if (sFocalPoint == nullptr)
		return;
	if (!bFocused) {
		if (xmfCamPosition.x != sFocalPoint->GetWorldPosition().x || xmfCamPosition.y != sFocalPoint->GetWorldPosition().y)
		{
			fAcceleration += CAMERA_SPEED;
			if (xmfCamPosition.x < sFocalPoint->GetWorldPosition().x)
			{
				xmfCamPosition.x += fAcceleration;
				if (xmfCamPosition.x > sFocalPoint->GetWorldPosition().x)
					xmfCamPosition.x = sFocalPoint->GetWorldPosition().x;
			}
			else if (xmfCamPosition.x > sFocalPoint->GetWorldPosition().x) {
				xmfCamPosition.x -= fAcceleration;
				if (xmfCamPosition.x < sFocalPoint->GetWorldPosition().x)
					xmfCamPosition.x = sFocalPoint->GetWorldPosition().x;
			}

			if (xmfCamPosition.y < sFocalPoint->GetWorldPosition().y)
			{
				xmfCamPosition.y += fAcceleration;
				if (xmfCamPosition.y > sFocalPoint->GetWorldPosition().y)
					xmfCamPosition.y = sFocalPoint->GetWorldPosition().y;
			}
			else if (xmfCamPosition.y > sFocalPoint->GetWorldPosition().y) {
				xmfCamPosition.y -= fAcceleration;
				if (xmfCamPosition.y < sFocalPoint->GetWorldPosition().y)
					xmfCamPosition.y = sFocalPoint->GetWorldPosition().y;
			}
		}
		else {
			bFocused = true;
		}
	}
	else {
		xmfCamPosition = sFocalPoint->GetWorldPosition();
		fAcceleration = 0;
	}
	if (nTrembleTime > 0)
	{
		fTrembleForce_X *= -1;
		fTrembleForce_Y *= -1;
		nTrembleTime--;
	}
	else {
		fTrembleForce_X = 0;
		fTrembleForce_Y = 0;
	}
}

//*****************************************************************************
//EndCamera関数
//処理の終了の関数
//引数：void
//戻：void
//*****************************************************************************
void EndCamera()
{
	//ない
}

//*****************************************************************************
//GetCameraPos関数
//カメラの拠点を戻す
//引数：void
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 GetCameraPos()
{
	return { xmfCamPosition.x + xmfAdjustment.x + fTrembleForce_X, 
		xmfCamPosition.y + xmfAdjustment.y + fTrembleForce_Y, 
		xmfCamPosition.z + xmfAdjustment.z };
}

//*****************************************************************************
//GetCameraPos関数
//焦点を設定する
//引数：Sprite*
//戻：void
//*****************************************************************************
void SetFocalPoint(Sprite* newFocalPoint)
{
	if (sFocalPoint == newFocalPoint)
		return;
	sFocalPoint = newFocalPoint;
	bFocused = false;
}

//*****************************************************************************
//GetFocalPoint関数
//焦点を戻す
//引数：void
//戻：Sprite*
//*****************************************************************************
Sprite* GetFocalPoint()
{
	return sFocalPoint;
}

//*****************************************************************************
//ResetCameraToOrigin関数
//カメラの拠点をリセットする
//引数：void
//戻：void
//*****************************************************************************
void ResetCameraToOrigin()
{
	xmfCamPosition = { 0,0,0 };
	xmfAdjustment = { 0,0,0 };
	fTrembleForce_X = 0;
	fTrembleForce_Y = 0;
	nTrembleTime = 0;
}

//*****************************************************************************
//SetCameraAdjustment関数
//カメラの拠点のオフセットを設定する
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void SetCameraAdjustment(XMFLOAT3 adj)
{
	xmfAdjustment = adj;
}

//*****************************************************************************
//CameraTremble関数
//カメラを揺らぐ時間を管理する
//引数：float, float, float
//戻：void
//*****************************************************************************
void CameraTremble(float fTremblePowerX, float fTremblePowerY, int n_TrembleTime)
{
	if (nTrembleTime > 0)
		return;
	fTrembleForce_X = fTremblePowerX;
	fTrembleForce_Y = fTremblePowerY;
	nTrembleTime = n_TrembleTime;
}

//*****************************************************************************
//GetCameraLimitRight関数
//カメラの右境界を戻す
//引数：void
//戻：float
//*****************************************************************************
float GetCameraLimitRight()
{
	if(GetUniversalScale()>0)
		return xmfCamPosition.x + ((WINDOW_WIDTH /2) / GetUniversalScale());
	else
		return xmfCamPosition.x + ((WINDOW_WIDTH / 2) / (GetUniversalScale()*-1));
}

//*****************************************************************************
//GetCameraLimitLeft関数
//カメラの左境界を戻す
//引数：void
//戻：float
//*****************************************************************************
float GetCameraLimitLeft()
{
	if (GetUniversalScale() > 0)
		return xmfCamPosition.x - ((WINDOW_WIDTH / 2) / GetUniversalScale());
	else
		return xmfCamPosition.x - ((WINDOW_WIDTH / 2) / (GetUniversalScale()*-1));
}

//*****************************************************************************
//GetCameraLimitUp関数
//カメラの上境界を戻す
//引数：void
//戻：float
//*****************************************************************************
float GetCameraLimitUp()
{
	if (GetUniversalScale() > 0)
		return xmfCamPosition.y + ((WINDOW_WIDTH / 2) / (GetUniversalScale() * 1.5f));
	else
		return xmfCamPosition.y + ((WINDOW_WIDTH / 2) / (GetUniversalScale() * -1.5f));
}

//*****************************************************************************
//GetCameraLimitDown関数
//カメラの下境界を戻す
//引数：void
//戻：float
//*****************************************************************************
float GetCameraLimitDown()
{
	if (GetUniversalScale() > 0)
		return xmfCamPosition.y - ((WINDOW_WIDTH / 2) / (GetUniversalScale() * 1.5f));
	else
		return xmfCamPosition.y - ((WINDOW_WIDTH / 2) / (GetUniversalScale() * -1.5f));
}

//*****************************************************************************
//IsCameraOnFocus関数
//カメラは焦点を集中していることを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool IsCameraOnFocus() 
{
	return bFocused;
}


//*****************************************************************************
//GetZoomOffset関数
//カメラのスームインのオフセットを戻す
//引数：void
//戻：float
//*****************************************************************************
float GetZoomOffset()
{
	return fZoomOffset;
}

//*****************************************************************************
//SetZoomOffset関数
//カメラのスームインのオフセットを設定する
//引数：float, float, int
//戻：void
//*****************************************************************************
void SetZoomOffset(float ZoomPower, float ZoomSpeed, int Frames)
{
	fZoomPower = ZoomPower;
	fZoomSpeed = ZoomSpeed;
	nZoomOffsetFrames = Frames;
}

//*****************************************************************************
//SetZoomAttackOffset関数
//カメラの攻撃動作スームインのオフセットを設定する
//引数：float
//戻：void
//*****************************************************************************
void SetZoomAttackOffset(float SetZoomAttackOffset)
{
	fZoomOffset = SetZoomAttackOffset;
}