//*****************************************************************************
// GameInputManager.cpp
// インプットの管理
// Author : Mane
//*****************************************************************************
#include "GameInputManager.h"
#include "input.h" 
#include <stdio.h>
#include "UniversalVariables.h"
#include "Sprite.h"
#include "C_XInput.h"

//*****************************************************************************
// グローバル変数
//*****************************************************************************
bool bInputs[MAX_INPUTS];
C_XInput* Player1 = nullptr;
bool bXinputConnected;
bool bHoldingXinput[MAX_INPUTS];
float fAxis[MAX_AXIS];

//*****************************************************************************
//InitInputManager関数
//初期化の関数
//引数：void
//戻：void
//*****************************************************************************
void InitInputManager()
{
	for (int i = 0; i < MAX_INPUTS; bInputs[i] = false, i++);
	for (int i = 0; i < MAX_INPUTS; bHoldingXinput[i] = false, i++);
	for (int i = 0; i < MAX_AXIS; fAxis[i] = 0, i++);
	Player1 = new C_XInput(1);
	bXinputConnected = false;
}

//*****************************************************************************
//UpdateInputManager関数
//アップデートの関数
//引数：void
//戻：void
//*****************************************************************************
void UpdateInputManager()
{
	
	if(!Player1)
		Player1 = new C_XInput(1);
	Player1->UpdateXinput();
	bXinputConnected = Player1->IsConnected();
	bool bScaleIsInversed = GetUniversalScale() < 0;

	XinputTriggerControl(true);

	bInputs[GAME_START] = GetKeyTrigger(VK_RETURN) || GetJoyButton(0, JOY_BUTTON3) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START && !bHoldingXinput[GAME_START]);
	bInputs[MOVE_RIGHT] = ((GetKeyPress(VK_RIGHT) || GetKeyPress(VK_D)) && !bScaleIsInversed)
		|| ((GetKeyPress(VK_LEFT) || GetKeyPress(VK_A)) && bScaleIsInversed) ||
		(bXinputConnected && (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) && !bScaleIsInversed) || 
		(bXinputConnected && (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) && bScaleIsInversed);
	bInputs[MOVE_LEFT] = ((GetKeyPress(VK_LEFT) || GetKeyPress(VK_A)) && !bScaleIsInversed)
		|| ((GetKeyPress(VK_RIGHT) || GetKeyPress(VK_D)) && bScaleIsInversed) ||
		(bXinputConnected && (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT )&& !bScaleIsInversed) ||
		(bXinputConnected && (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) && bScaleIsInversed);

	bInputs[MOVE_DOWN] = GetKeyPress(VK_DOWN) || GetKeyPress(VK_S) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
	bInputs[MOVE_UP] = GetKeyPress(VK_UP) || GetKeyPress(VK_W) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
	bInputs[INPUT_UP] = GetKeyTrigger(VK_UP) || GetKeyTrigger(VK_W) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP && !bHoldingXinput[INPUT_UP]);

	bInputs[MOVE_JUMP] = GetKeyPress(VK_K) || GetKeyPress(VK_Z) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A);
	bInputs[INPUT_CONFIRM] = GetKeyTrigger(VK_K) || GetKeyTrigger(VK_Z) || bInputs[GAME_START] ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A && !bHoldingXinput[MOVE_JUMP]);
	bInputs[INPUT_LOCKON] = GetKeyPress(VK_SPACE) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
	bInputs[MOVE_JUMP_TRIGGER] = GetKeyTrigger(VK_K) || GetKeyTrigger(VK_Z) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A && !bHoldingXinput[MOVE_JUMP_TRIGGER]);
	bInputs[INPUT_ATTACK] = GetKeyTrigger(VK_I) || GetKeyTrigger(VK_X) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y && !bHoldingXinput[INPUT_ATTACK]);
	bInputs[HOLD_ATTACK] = GetKeyPress(VK_I) || GetKeyPress(VK_X) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y);
	bInputs[INPUT_SHOOT] = GetKeyPress(VK_C) || GetKeyPress(VK_J) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X);
	bInputs[INPUT_CATCHSHOOT] = GetKeyPress(VK_SHIFT) || GetKeyPress(VK_L) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B);

	bInputs[INPUT_RIGHT] = ((GetKeyTrigger(VK_LEFT) || GetKeyTrigger(VK_A)) && bScaleIsInversed) ||
		((GetKeyTrigger(VK_RIGHT) || GetKeyTrigger(VK_D) && !bScaleIsInversed)) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT && !bScaleIsInversed && !bHoldingXinput[INPUT_RIGHT]) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT && bScaleIsInversed && !bHoldingXinput[INPUT_LEFT]);
	bInputs[INPUT_LEFT] =  ((GetKeyTrigger(VK_LEFT)  || GetKeyTrigger(VK_A)) && !bScaleIsInversed)||
		((GetKeyTrigger(VK_RIGHT) || GetKeyTrigger(VK_D) && bScaleIsInversed)) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT && bScaleIsInversed && !bHoldingXinput[INPUT_RIGHT]) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT && !bScaleIsInversed && !bHoldingXinput[INPUT_LEFT]);

	bInputs[INPUT_DOWN] = GetKeyTrigger(VK_DOWN) || GetKeyTrigger(VK_S) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN && !bHoldingXinput[INPUT_DOWN]);
	bInputs[INPUT_PAUSE] =  GetKeyTrigger(VK_BACK) || GetKeyTrigger(VK_RETURN) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START && !bHoldingXinput[INPUT_PAUSE]);
	bInputs[INPUT_MENU_RIGHT] = GetKeyTrigger(VK_RIGHT) || GetKeyTrigger(VK_D) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT && !bHoldingXinput[INPUT_MENU_RIGHT]);
	bInputs[INPUT_MENU_LEFT] = GetKeyTrigger(VK_LEFT) || GetKeyTrigger(VK_A) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT && !bHoldingXinput[INPUT_MENU_LEFT]);
	bInputs[INPUT_TAUNT] = GetKeyTrigger(VK_M) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK && !bHoldingXinput[INPUT_TAUNT]);

	if (bXinputConnected) {
		fAxis[RIGHT_STICK_HORIZONTAL] = (float)Player1->GetState().Gamepad.sThumbRX / 32876.7f + 0.023543f;
		fAxis[RIGHT_STICK_VERTICAL] = (float)Player1->GetState().Gamepad.sThumbRY / 32876.7f - 0.017848f + 0.010000f;
		fAxis[LEFT_STICK_HORIZONTAL] = (float)Player1->GetState().Gamepad.sThumbLX / 32767;
		fAxis[LEFT_STICK_VERTICAL] = (float)Player1->GetState().Gamepad.sThumbLY / 32767;
	}

	//レベルエディターに関して
	bInputs[INPUT_DEBUG_PRINT_POS] =  GetKeyTrigger(VK_P);
	bInputs[DEBUG_POINTER_ACTIVATE] =  GetKeyTrigger(VK_1) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB && !bHoldingXinput[DEBUG_POINTER_ACTIVATE]);
	bInputs[DEBUG_CHANGE_DEBUG_AIM_CREATOR_RIGHT] =  GetKeyTrigger(VK_E) || 
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER && !bHoldingXinput[DEBUG_CHANGE_DEBUG_AIM_CREATOR_RIGHT]);
	bInputs[DEBUG_CHANGE_DEBUG_AIM_CREATOR_LEFT] =  GetKeyTrigger(VK_Q) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER && !bHoldingXinput[DEBUG_CHANGE_DEBUG_AIM_CREATOR_LEFT]);
	bInputs[DEBUG_DELETE_LAST_OBJECT] =  GetKeyTrigger(VK_BACK) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y && !bHoldingXinput[DEBUG_DELETE_LAST_OBJECT]);
	bInputs[DEBUG_SAVE_POSITIONS_LIST] = GetKeyPress(VK_SHIFT) && GetKeyTrigger(VK_P);
	bInputs[DEBUG_FLOOR_SIZE_UP] = GetKeyPress(VK_O) ||
		fAxis[RIGHT_STICK_HORIZONTAL]>=0.5f;
	bInputs[DEBUG_FLOOR_SIZE_DOWN] = GetKeyPress(VK_U) || 
		fAxis[RIGHT_STICK_HORIZONTAL]<=-0.5f;
	bInputs[DEBUG_SPEED_CHANGE] = GetKeyPress(VK_SHIFT) ||
		(bXinputConnected && Player1->GetState().Gamepad.bRightTrigger > 0);
	bInputs[DEBUG_SIZEY_UP] = GetKeyPress(VK_I);
	bInputs[DEBUG_SIZEY_DOWN] = GetKeyPress(VK_K);
	bInputs[DEBUG_SIZEX_UP] = GetKeyPress(VK_L) ||
		fAxis[LEFT_STICK_VERTICAL] >= 0.5f;
	bInputs[DEBUG_SIZEX_DOWN] = GetKeyPress(VK_J) ||
		fAxis[LEFT_STICK_VERTICAL] <= -0.5f;
	bInputs[DEBUG_TELEPORTPLAYER] = GetKeyPress(VK_R) ||
		(bXinputConnected && Player1->GetState().Gamepad.bLeftTrigger > 0);	
	bInputs[DEBUG_SWITCH_NUM_OPTION] = GetKeyTrigger(VK_TAB) ||
		(bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB && !bHoldingXinput[DEBUG_SWITCH_NUM_OPTION]);
	//保存する
	bInputs[DEBUG_SAVELEVEL] = (GetKeyPress(VK_CONTROL) && GetKeyTrigger(VK_S))
		|| (bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START && !bHoldingXinput[DEBUG_SAVELEVEL]);
	bInputs[DEBUG_LOADLEVEL] = (GetKeyPress(VK_CONTROL) && GetKeyTrigger(VK_L))
		|| (bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK && !bHoldingXinput[DEBUG_LOADLEVEL]);

	bInputs[DEBUG_SWITCH_SLOTUP] = GetKeyTrigger(VK_M)
		|| (bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B && !bHoldingXinput[DEBUG_SWITCH_SLOTUP]);

	bInputs[DEBUG_SWITCH_SLOTDOWN] = GetKeyTrigger(VK_N)
		|| (bXinputConnected && Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X && !bHoldingXinput[DEBUG_SWITCH_SLOTDOWN]);
	XinputTriggerControl(false);
}

void XinputTriggerControl(bool BeforeInputs)
{
	if (BeforeInputs) {
		if (bHoldingXinput[GAME_START] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START)) bHoldingXinput[GAME_START] = false;
		if (bHoldingXinput[INPUT_UP] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)) bHoldingXinput[INPUT_UP] = false;
		if (bHoldingXinput[MOVE_JUMP] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)) bHoldingXinput[MOVE_JUMP] = false;
		if (bHoldingXinput[MOVE_JUMP_TRIGGER] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)) bHoldingXinput[MOVE_JUMP_TRIGGER] = false;
		if (bHoldingXinput[INPUT_ATTACK] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y)) bHoldingXinput[INPUT_ATTACK] = false;
		if (bHoldingXinput[INPUT_RIGHT] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)) bHoldingXinput[INPUT_RIGHT] = false;
		if (bHoldingXinput[INPUT_LEFT] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)) bHoldingXinput[INPUT_LEFT] = false;
		if (bHoldingXinput[INPUT_DOWN] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)) bHoldingXinput[INPUT_DOWN] = false;
		if (bHoldingXinput[INPUT_PAUSE] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START)) bHoldingXinput[INPUT_PAUSE] = false;
		if (bHoldingXinput[INPUT_MENU_RIGHT] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)) bHoldingXinput[INPUT_MENU_RIGHT] = false;
		if (bHoldingXinput[INPUT_MENU_LEFT] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)) bHoldingXinput[INPUT_MENU_LEFT] = false;
		if (bHoldingXinput[INPUT_TAUNT] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK)) bHoldingXinput[INPUT_TAUNT] = false;
		if (bHoldingXinput[DEBUG_POINTER_ACTIVATE] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)) bHoldingXinput[DEBUG_POINTER_ACTIVATE] = false;
		if (bHoldingXinput[DEBUG_CHANGE_DEBUG_AIM_CREATOR_RIGHT] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)) bHoldingXinput[DEBUG_CHANGE_DEBUG_AIM_CREATOR_RIGHT] = false;
		if (bHoldingXinput[DEBUG_CHANGE_DEBUG_AIM_CREATOR_LEFT] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)) bHoldingXinput[DEBUG_CHANGE_DEBUG_AIM_CREATOR_LEFT] = false;
		if (bHoldingXinput[DEBUG_DELETE_LAST_OBJECT] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y)) bHoldingXinput[DEBUG_DELETE_LAST_OBJECT] = false;
		if (bHoldingXinput[DEBUG_SAVELEVEL] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START)) bHoldingXinput[DEBUG_SAVELEVEL] = false;
		if (bHoldingXinput[DEBUG_LOADLEVEL] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK)) bHoldingXinput[DEBUG_LOADLEVEL] = false;
		if (bHoldingXinput[DEBUG_SWITCH_SLOTUP] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B)) bHoldingXinput[DEBUG_SWITCH_SLOTUP] = false;
		if (bHoldingXinput[DEBUG_SWITCH_SLOTDOWN] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X)) bHoldingXinput[DEBUG_SWITCH_SLOTDOWN] = false;
		if (bHoldingXinput[DEBUG_SWITCH_NUM_OPTION] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)) bHoldingXinput[DEBUG_SWITCH_NUM_OPTION] = false;
	}
	else {
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START) bHoldingXinput[GAME_START] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) bHoldingXinput[INPUT_UP] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A) bHoldingXinput[MOVE_JUMP] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A) bHoldingXinput[MOVE_JUMP_TRIGGER] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y) bHoldingXinput[INPUT_ATTACK] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) bHoldingXinput[INPUT_RIGHT] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) bHoldingXinput[INPUT_LEFT] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) bHoldingXinput[INPUT_DOWN] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START) bHoldingXinput[INPUT_PAUSE] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) bHoldingXinput[INPUT_MENU_RIGHT] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) bHoldingXinput[INPUT_MENU_LEFT] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK) bHoldingXinput[INPUT_TAUNT] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) bHoldingXinput[DEBUG_POINTER_ACTIVATE] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y) bHoldingXinput[DEBUG_DELETE_LAST_OBJECT] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) bHoldingXinput[DEBUG_CHANGE_DEBUG_AIM_CREATOR_RIGHT] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) bHoldingXinput[DEBUG_CHANGE_DEBUG_AIM_CREATOR_LEFT] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START) bHoldingXinput[DEBUG_SAVELEVEL] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK) bHoldingXinput[DEBUG_LOADLEVEL] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B) bHoldingXinput[DEBUG_SWITCH_SLOTUP] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X) bHoldingXinput[DEBUG_SWITCH_SLOTDOWN] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) bHoldingXinput[DEBUG_SWITCH_NUM_OPTION] = true;
	}
}

//*****************************************************************************
//EndInputManager関数
//終了の関数
//引数：void
//戻：void
//*****************************************************************************
void EndInputManager()
{
	//ない
}

//*****************************************************************************
//GetInput関数
//インプットの状態を戻す
//引数：int
//戻：bool
//*****************************************************************************
bool GetInput(int nInput)
{
	return bInputs[nInput];
}

//*****************************************************************************
//IsXInputConnected関数
//XBOXのコントローラーを使っていることを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool IsXInputConnected()
{
	if (Player1)
		return Player1->IsConnected();
	return false;
}

//*****************************************************************************
//VibrateXinput関数
//XBOXのコントローラーに揺らせる
//引数：float, float, int
//戻：void
//*****************************************************************************
void VibrateXinput(float vivbrationL, float VibrationR, int FrameNumbers)
{
	if (!bXinputConnected)
		return;
	Player1->Vibrate((int)vivbrationL, (int)VibrationR, FrameNumbers);
}
