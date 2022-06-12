//*****************************************************************************
// TextureManager.cpp
// テクスチャーを管理する
// Author : Mane
//*****************************************************************************
#include "TextureManager.h"
#include "C_DXWindow.h"
#include "UniversalVariables.h"

//*****************************************************************************
// グローバル変数
//*****************************************************************************
ID3D11ShaderResourceView* gpTextures[MAX_TEXTURES];
char gpTexturePath[MAX_TEXTURES][256] = 
{
	"data/TEXTURE/Logo.tga",
	"data/TEXTURE/TitleInit.tga",
	"data/TEXTURE/UIObjects/MenuBackground.tga",
	"data/TEXTURE/UIObjects/GRS_RankingBackground.tga",
	"data/TEXTURE/UIObjects/ExtraOptionsMenuBackground.tga",
	"data/TEXTURE/UIObjects/objMenu.tga",
	"data/TEXTURE/UIObjects/MenuOptionDebug.tga",
	"data/TEXTURE/UIObjects/MenuGameStartOption.tga",
	"data/TEXTURE/UIObjects/MenuTutorialOption.tga",
	"data/TEXTURE/UIObjects/MenuRankingOption.tga",
	"data/TEXTURE/UIObjects/MenuExtraOptions.tga",
	"data/TEXTURE/UIObjects/MenuOwariOption.tga",
	"data/TEXTURE/UIObjects/MenuNSNG.tga",
	"data/TEXTURE/UIObjects/MenuNSNGd.tga",
	"data/TEXTURE/UIObjects/Menu_FPS_SHOW.tga",
	"data/TEXTURE/UIObjects/Menu_FPS_HIDE.tga",
	"data/TEXTURE/UIObjects/Menu_HellsTouchActivated.tga",
	"data/TEXTURE/UIObjects/Menu_HellsTouchDeActivated.tga",
	"data/TEXTURE/UIObjects/MenuReturn.tga",
	"data/TEXTURE/UIObjects/Menu_NSNG_Info.tga",
	"data/TEXTURE/UIObjects/Menu_HM_Info.tga",
	//UI
	"data/TEXTURE/UIObjects/PS_Controls.tga",
	"data/TEXTURE/UIObjects/PS_Combos.tga",
	"data/TEXTURE/UIObjects/HeartUI.tga",
	"data/TEXTURE/UIObjects/Rank.tga",
	"data/TEXTURE/MiniAuraC.tga",
	"data/TEXTURE/UIObjects/Numbers.tga",
	"data/TEXTURE/UIObjects/GRS_GeneralRankingNumbers.tga",
	"data/TEXTURE/UIObjects/TSIC.tga",
	"data/TEXTURE/UIObjects/Done.tga",
	"data/TEXTURE/UIObjects/ONAIR.tga",
	"data/TEXTURE/UIObjects/RS_Combo.tga",
	"data/TEXTURE/UIObjects/RS_Time.tga",
	"data/TEXTURE/UIObjects/RS_Damage.tga",
	"data/TEXTURE/UIObjects/RS_Overrall.tga",
	"data/TEXTURE/UIObjects/RS_ResultsText.tga",
	"data/TEXTURE/UIObjects/RS_Rank.tga",
	"data/TEXTURE/UIObjects/PS_PauseText.tga",
	"data/TEXTURE/UIObjects/RS_StageClear.tga",
	"data/TEXTURE/UIObjects/GRS_GeneralRankingText.tga",
	"data/TEXTURE/UIObjects/TS_TutorialText.tga",
	"data/TEXTURE/UIObjects/RS_DEBUGROOM.tga",
	"data/TEXTURE/UIObjects/RS_TUTORIAL_TEXT.tga",
	"data/TEXTURE/UIObjects/RS_LEVEL1_TEXT.tga",
	"data/TEXTURE/UIObjects/PS_PauseOptionsContinue.tga",
	"data/TEXTURE/UIObjects/PS_PauseOptionsControls.tga",
	"data/TEXTURE/UIObjects/PS_PauseOptionsCommands.tga",
	"data/TEXTURE/UIObjects/PS_PauseOptionsBackToTitle.tga",
	"data/TEXTURE/UIObjects/UI_Arrow.tga",
	"data/TEXTURE/UIObjects/ZoomEffect.tga",
	"data/TEXTURE/UIObjects/ZoomEffectAlt.tga",
	"data/TEXTURE/UIObjects/UI_DEBUGROOM_INSTRUCTIONS_1.tga",
	"data/TEXTURE/UIObjects/UI_DEBUGROOM_INSTRUCTIONS_2.tga",
	"data/TEXTURE/UIObjects/EDITOR_LEVEL_TEXT.tga",
	"data/TEXTURE/UIObjects/EDITOR_SPEED_TEXT.tga",
	"data/TEXTURE/UIObjects/EDITOR_DELAY_TEXT.tga",
	"data/TEXTURE/UIObjects/ControllerTutorial/UI_CT1.tga",
	"data/TEXTURE/UIObjects/ControllerTutorial/UI_CT2.tga",
	"data/TEXTURE/UIObjects/ControllerTutorial/UI_CT3.tga",
	"data/TEXTURE/UIObjects/ControllerTutorial/UI_CT4.tga",
	"data/TEXTURE/UIObjects/ControllerTutorial/UI_CT5.tga",
	"data/TEXTURE/UIObjects/ControllerTutorial/UI_CT6.tga",
	"data/TEXTURE/UIObjects/UI_LEVELSAVED_MESSAGE.tga",
	"data/TEXTURE/UIObjects/UI_LEVELLOADED_MESSAGE.tga",
	"",
	"data/TEXTURE/NoiseTextureRed.tga",
	"data/TEXTURE/GameOverMessage.tga",
	"data/TEXTURE/Floor.tga",
	"data/TEXTURE/Floor2.tga",
	"data/TEXTURE/Floor3.tga",
	"data/TEXTURE/Floor4.tga",
	"data/TEXTURE/Floor5.tga",
	"",
	"data/TEXTURE/FlowerMapsheet.tga",
	"data/TEXTURE/FlowerSpeedMapsheet.tga",
	"data/TEXTURE/Block.tga",
	"data/TEXTURE/Block2.tga",
	"data/TEXTURE/Block3.tga",
	"",
	"data/TEXTURE/Spike.tga",
	"data/TEXTURE/Teleporter.tga",
	"data/TEXTURE/PunchableStar.tga",
	"data/TEXTURE/BulbSwitch.tga",
	"data/TEXTURE/PunchBulbSwitch.tga",
	"data/TEXTURE/SobaRecovery.tga",
	"data/TEXTURE/Crystal.tga",
	"data/TEXTURE/TeleporterEndPoint.tga",
	"",
	"data/TEXTURE/Spotlight.tga",
	"data/TEXTURE/DEBUGAIM.tga",
	"data/TEXTURE/DEBUGXGREEN.tga",
	"data/TEXTURE/DEBUGXBLUE.tga",
	"data/TEXTURE/BulletMapSheet.tga",
	"data/TEXTURE/CatchBulletMapSheet.tga",
	"data/TEXTURE/blackDot.tga",
	"data/TEXTURE/ElyseSpriteMapV2.tga",
	"data/TEXTURE/HaloSpriteSheets.tga",
	"data/TEXTURE/AuraEffectPlayer.tga",
	"data/TEXTURE/HandsSpreadsheet.tga",
	"data/TEXTURE/GuitarSpreedsheets.tga",
	"",
	"data/TEXTURE/BuBunEnemy.tga",
	"",
	"data/TEXTURE/EnemyLogos/EL_BUNBUN.tga",
	//MISC
	"data/TEXTURE/bg1.tga",
	"data/TEXTURE/bg2.tga",
	"data/TEXTURE/bg3.tga",
	"data/TEXTURE/bgEvent.tga",
	"data/TEXTURE/hbox.tga"
};
int nTextureCount = 0;


//*****************************************************************************
//InitTextures関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void InitTextures()
{
	for (int i = 0; i < MAX_TEXTURES; i++)
	{
		gpTextures[i] = nullptr;
	}
	nTextureCount = 0;
}

//*****************************************************************************
//GetTexture関数
//テクスチャを戻す
//引数：int
//戻：ID3D11ShaderResourceView*
//*****************************************************************************
ID3D11ShaderResourceView * GetTexture(int TextureID)
{
	if (TextureID<0 || TextureID>MAX_TEXTURES)
		return nullptr;
	if (!gpTextures[TextureID]) 
	{
		nTextureCount++;
#ifdef USE_CONSOLE
		printf("TM: 作ったテクスチャ：%s\n TM: テクスチャNo: %d\n", gpTexturePath[TextureID], nTextureCount);
#endif
		CreateTextureFromFile(gpTexturePath[TextureID], &gpTextures[TextureID]);
	}
	return gpTextures[TextureID];
}

//*****************************************************************************
//EndTextures関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void EndTextures()
{
	nTextureCount = 0;
	for (int i = 0; i < MAX_TEXTURES; i++)
	{
		if (gpTextures[i]) {
#ifdef USE_CONSOLE
			printf("TM: 消した: %s\n", gpTexturePath[i]);
#endif
			gpTextures[i]->Release();
			gpTextures[i] = nullptr;
			nTextureCount++;
		}
	}
#ifdef USE_CONSOLE
	printf("消したテクスチャ: %d\n", nTextureCount);
#endif
}

//*****************************************************************************
//EndFloorTextures関数
//床テクスチャの終了関数
//引数：void
//戻：void
//*****************************************************************************
void EndFloorTextures()
{
	int nFloorTextureCount = 0;
	for (int i = DOMINOS_FLOOR_BLACK_TEXTURE; i < MAX_TEXTURE_FLOORS; i++)
	{
		if (gpTextures[i]) {
#ifdef USE_CONSOLE
			printf("TM: 消した: %s\n", gpTexturePath[i]);
#endif
			gpTextures[i]->Release();
			gpTextures[i] = nullptr;
			nFloorTextureCount++;
		}
	}
#ifdef USE_CONSOLE
	printf("消したテクスチャ: %d\n", nFloorTextureCount);
#endif
	nTextureCount -= nFloorTextureCount;
}

//*****************************************************************************
//EndWallTextures関数
//壁テクスチャの終了関数
//引数：void
//戻：void
//*****************************************************************************
void EndWallTextures()
{
	int nWallsTexturesCount = 0;
	for (int i = EVIL_BLOCK_TEXTURE; i < MAX_TEXTURES_WALLS; i++)
	{
		if (gpTextures[i]) {
#ifdef USE_CONSOLE
			printf("TM: 消した: %s\n", gpTexturePath[i]);
#endif
			gpTextures[i]->Release();
			gpTextures[i] = nullptr;
			nWallsTexturesCount++;
		}
	}
#ifdef USE_CONSOLE
	printf("消したテクスチャ: %d\n", nWallsTexturesCount);
#endif
	nTextureCount -= nWallsTexturesCount;
}

//*****************************************************************************
//EndLevelObjectsTextures関数
//レベルのオブジェクトのテクスチャの終了関数
//引数：void
//戻：void
//*****************************************************************************
void EndLevelObjectsTextures()
{
	int nLevelObjectsTexturesCount = 0;
	for (int i = TEXTURE_SPIKE; i < MAX_TEXTURES_LEVELOBJECTS; i++)
	{
		if (gpTextures[i]) {
			if (i == TEXTURE_CRYSTAL)
				continue;
#ifdef USE_CONSOLE
			printf("TM: 消した: %s\n", gpTexturePath[i]);
#endif
			gpTextures[i]->Release();
			gpTextures[i] = nullptr;
			nLevelObjectsTexturesCount++;
		}
	}
#ifdef USE_CONSOLE
	printf("消したテクスチャ: %d\n", nLevelObjectsTexturesCount);
#endif
	nTextureCount -= nLevelObjectsTexturesCount;
}

//*****************************************************************************
//EndLevelObjectsTextures関数
//敵のテクスチャの終了関数
//引数：void
//戻：void
//*****************************************************************************
void EndEnemyTextures()
{
	int nEnemiesTexturesCount = 0;
	for (int i = TEXTURE_BUNBUN_ENEMY; i < MAX_TEXTURE_ENEMIES; i++)
	{
		if (gpTextures[i]) {
#ifdef USE_CONSOLE
			printf("TM: 消した: %s\n", gpTexturePath[i]);
#endif
			gpTextures[i]->Release();
			gpTextures[i] = nullptr;
			nEnemiesTexturesCount++;
		}
	}
#ifdef USE_CONSOLE
	printf("消したテクスチャ: %d\n", nEnemiesTexturesCount);
#endif
	nTextureCount -= nEnemiesTexturesCount;
}

//*****************************************************************************
//GetTextureCountInMemory関数
//メモリで使ってるテクスチャ数を戻す
//引数：void
//戻：int
//*****************************************************************************
int GetTextureCountInMemory()
{
	return nTextureCount;
}
