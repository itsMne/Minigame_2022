//*****************************************************************************
// GameInputManager.h
// Author : Mane
//*****************************************************************************
#ifndef GAMEINPUTMANAGER_H
#define GAMEINPUTMANAGER_H

//*****************************************************************************
// エナム定義
//*****************************************************************************
enum bINPUTS
{
	MOVE_LEFT=0,
	GAME_START,
	MOVE_RIGHT,
	MOVE_UP,
	MOVE_DOWN,
	MOVE_JUMP_TRIGGER,
	MOVE_JUMP,
	INPUT_CONFIRM,
	INPUT_ATTACK,
	HOLD_ATTACK,
	INPUT_RIGHT,
	INPUT_LEFT,
	INPUT_DOWN,
	INPUT_UP,
	INPUT_MENU_RIGHT,
	INPUT_MENU_LEFT,
	INPUT_LOCKON,
	INPUT_SHOOT,
	INPUT_CATCHSHOOT,
	INPUT_TAUNT,
	INPUT_DEBUG_PRINT_POS,
	INPUT_PAUSE,
	DEBUG_POINTER_ACTIVATE,
	DEBUG_CHANGE_DEBUG_AIM_CREATOR_RIGHT,
	DEBUG_CHANGE_DEBUG_AIM_CREATOR_LEFT,
	DEBUG_DELETE_LAST_OBJECT,
	DEBUG_SAVE_POSITIONS_LIST,
	DEBUG_FLOOR_SIZE_UP,
	DEBUG_FLOOR_SIZE_DOWN,
	DEBUG_SPEED_CHANGE,
	DEBUG_SIZEY_UP,
	DEBUG_SIZEY_DOWN,
	DEBUG_SIZEX_UP,
	DEBUG_SIZEX_DOWN,
	DEBUG_TELEPORTPLAYER,
	DEBUG_SAVELEVEL,
	DEBUG_LOADLEVEL,
	DEBUG_SWITCH_SLOTUP,
	DEBUG_SWITCH_SLOTDOWN,
	DEBUG_SWITCH_NUM_OPTION,
	MAX_INPUTS
};
enum EAXIS
{
	LEFT_STICK_HORIZONTAL,
	LEFT_STICK_VERTICAL,
	RIGHT_STICK_HORIZONTAL,
	RIGHT_STICK_VERTICAL,
	MAX_AXIS
};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitInputManager();
void UpdateInputManager();
void XinputTriggerControl(bool BeforeInputs);
void EndInputManager();
bool GetInput(int nInput);
bool IsXInputConnected();
void VibrateXinput(float vivbrationL, float VibrationR, int FrameNumbers);

#endif
