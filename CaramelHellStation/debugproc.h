//*****************************************************************************
// DEBUGPROC.h
// Author : Mane
//*****************************************************************************
#ifndef	 DEBUGPROC_H
#define  DEBUGPROC_H

#include "Sprite.h"

//*****************************************************************************
// ƒNƒ‰ƒX
//*****************************************************************************
class cDebugProc :
	public Sprite
{
private:

public:
	cDebugProc();
	~cDebugProc();
	HRESULT InitDebugProc(void);
	void	UninitDebugProc(void);
	void	UpdateDebugProc(void);
	void	DrawDebugProc(void);
	void	StartDebugProc(bool hiragana = false);
	void	PrintDebugProc(const char *fmt, ...);
	bool	SetHiragana(bool hiragana = true);
};
#endif