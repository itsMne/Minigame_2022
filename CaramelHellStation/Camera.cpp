//*****************************************************************************
// Camera.cpp
// �Q�[���̃J�����̊Ǘ�
// Author : Mane
//*****************************************************************************
#include "Camera.h"
#include "Player.h"
#include <stdio.h>
#include "UniversalVariables.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CAMERA_SPEED 1.5f

//*****************************************************************************
// �O���[�o���ϐ�
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
//InitCamera�֐�
//�������̊֐�
//�����Fvoid
//�߁Fvoid
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
//UpdateCamera�֐�
//�����̃A�b�v�f�[�g�̊֐�
//�����Fvoid
//�߁Fvoid
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
//EndCamera�֐�
//�����̏I���̊֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void EndCamera()
{
	//�Ȃ�
}

//*****************************************************************************
//GetCameraPos�֐�
//�J�����̋��_��߂�
//�����Fvoid
//�߁FXMFLOAT3
//*****************************************************************************
XMFLOAT3 GetCameraPos()
{
	return { xmfCamPosition.x + xmfAdjustment.x + fTrembleForce_X, 
		xmfCamPosition.y + xmfAdjustment.y + fTrembleForce_Y, 
		xmfCamPosition.z + xmfAdjustment.z };
}

//*****************************************************************************
//GetCameraPos�֐�
//�œ_��ݒ肷��
//�����FSprite*
//�߁Fvoid
//*****************************************************************************
void SetFocalPoint(Sprite* newFocalPoint)
{
	if (sFocalPoint == newFocalPoint)
		return;
	sFocalPoint = newFocalPoint;
	bFocused = false;
}

//*****************************************************************************
//GetFocalPoint�֐�
//�œ_��߂�
//�����Fvoid
//�߁FSprite*
//*****************************************************************************
Sprite* GetFocalPoint()
{
	return sFocalPoint;
}

//*****************************************************************************
//ResetCameraToOrigin�֐�
//�J�����̋��_�����Z�b�g����
//�����Fvoid
//�߁Fvoid
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
//SetCameraAdjustment�֐�
//�J�����̋��_�̃I�t�Z�b�g��ݒ肷��
//�����FXMFLOAT3
//�߁Fvoid
//*****************************************************************************
void SetCameraAdjustment(XMFLOAT3 adj)
{
	xmfAdjustment = adj;
}

//*****************************************************************************
//CameraTremble�֐�
//�J������h�炮���Ԃ��Ǘ�����
//�����Ffloat, float, float
//�߁Fvoid
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
//GetCameraLimitRight�֐�
//�J�����̉E���E��߂�
//�����Fvoid
//�߁Ffloat
//*****************************************************************************
float GetCameraLimitRight()
{
	if(GetUniversalScale()>0)
		return xmfCamPosition.x + ((WINDOW_WIDTH /2) / GetUniversalScale());
	else
		return xmfCamPosition.x + ((WINDOW_WIDTH / 2) / (GetUniversalScale()*-1));
}

//*****************************************************************************
//GetCameraLimitLeft�֐�
//�J�����̍����E��߂�
//�����Fvoid
//�߁Ffloat
//*****************************************************************************
float GetCameraLimitLeft()
{
	if (GetUniversalScale() > 0)
		return xmfCamPosition.x - ((WINDOW_WIDTH / 2) / GetUniversalScale());
	else
		return xmfCamPosition.x - ((WINDOW_WIDTH / 2) / (GetUniversalScale()*-1));
}

//*****************************************************************************
//GetCameraLimitUp�֐�
//�J�����̏㋫�E��߂�
//�����Fvoid
//�߁Ffloat
//*****************************************************************************
float GetCameraLimitUp()
{
	if (GetUniversalScale() > 0)
		return xmfCamPosition.y + ((WINDOW_WIDTH / 2) / (GetUniversalScale() * 1.5f));
	else
		return xmfCamPosition.y + ((WINDOW_WIDTH / 2) / (GetUniversalScale() * -1.5f));
}

//*****************************************************************************
//GetCameraLimitDown�֐�
//�J�����̉����E��߂�
//�����Fvoid
//�߁Ffloat
//*****************************************************************************
float GetCameraLimitDown()
{
	if (GetUniversalScale() > 0)
		return xmfCamPosition.y - ((WINDOW_WIDTH / 2) / (GetUniversalScale() * 1.5f));
	else
		return xmfCamPosition.y - ((WINDOW_WIDTH / 2) / (GetUniversalScale() * -1.5f));
}

//*****************************************************************************
//IsCameraOnFocus�֐�
//�J�����͏œ_���W�����Ă��邱�Ƃ��m�F����
//�����Fvoid
//�߁Fbool
//*****************************************************************************
bool IsCameraOnFocus() 
{
	return bFocused;
}


//*****************************************************************************
//GetZoomOffset�֐�
//�J�����̃X�[���C���̃I�t�Z�b�g��߂�
//�����Fvoid
//�߁Ffloat
//*****************************************************************************
float GetZoomOffset()
{
	return fZoomOffset;
}

//*****************************************************************************
//SetZoomOffset�֐�
//�J�����̃X�[���C���̃I�t�Z�b�g��ݒ肷��
//�����Ffloat, float, int
//�߁Fvoid
//*****************************************************************************
void SetZoomOffset(float ZoomPower, float ZoomSpeed, int Frames)
{
	fZoomPower = ZoomPower;
	fZoomSpeed = ZoomSpeed;
	nZoomOffsetFrames = Frames;
}

//*****************************************************************************
//SetZoomAttackOffset�֐�
//�J�����̍U������X�[���C���̃I�t�Z�b�g��ݒ肷��
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void SetZoomAttackOffset(float SetZoomAttackOffset)
{
	fZoomOffset = SetZoomAttackOffset;
}