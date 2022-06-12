//*****************************************************************************
// C_XINPUT.h
// Author : Mane
//*****************************************************************************
#ifndef	C_XINPUT_H
#define C_XINPUT_H

#include <windows.h>
#include <XInput.h>

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define WIN32_LEAN_AND_MEAN

#pragma comment(lib, "XInput.lib")

//*****************************************************************************
// �N���X
//*****************************************************************************
class C_XInput
{
private:
	XINPUT_STATE xiControllerState;
	int nControllerNumber;
	int nVibrationTime;
	int nCurrentVibrationLeft;
	int nCurrentVibrationRight;
	bool UseVibration;
public:
	C_XInput(int playerNumber);
	~C_XInput();
	void UpdateXinput();
	XINPUT_STATE GetState();
	bool IsConnected();
	void Vibrate(int leftVal, int rightVal, int VibrationTime);
};
#endif