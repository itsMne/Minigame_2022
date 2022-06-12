//*****************************************************************************
// TransitionControl.h
// Author : Mane
//*****************************************************************************
#ifndef TRANSITIONCONTROL_H
#define TRANSITIONCONTROL_H

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitFade();
void UpdateFade();
void ControlSoundPlayForNextState();
void DrawFade();
void EndFade();
bool IsTransitionOver();
void SetFadeTransitionToNewState(int newState);
void SetNoiseTransitionToNewState(int newState);
void SetStickerTransition(int newState);
void SetTransitionToMenu();

#endif // !TRANSITIONCONTROL_H