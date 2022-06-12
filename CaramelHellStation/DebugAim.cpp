//*****************************************************************************
// DebugAim.h
// デバッグアイムとレベルエディターを管理する
// Author : Mane
//*****************************************************************************
#include "DebugAim.h"
#include "GameInputManager.h"
#include "TextureManager.h"
#include "SceneInGame.h"
#include "PunchStar.h"
#include "Player.h"
#include "Teleporter.h"
#include "Enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_SLOTS 100
#define MAX_OBJ_SPEED 999
#define MAX_OBJ_DELAY_STOP 99

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
DebugAim::DebugAim(): GameObject()
{
	InitSprite();
	Init();
}


DebugAim::~DebugAim()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void DebugAim::Init()
{
	nTypeObjectToCreate = 0;
	nSaveSlot = 0;
	nDelaySeconds = 0;
	nMoveableObjectSpeed = 0;
	nFramesCooldownForSave = 0;
	SetType(DEBUG_AIM_TYPE);
	gpTexture = GetTexture(TEXTURE_DEBUG_AIM);
	bIsInUse = false;
	bSetStarTeleportDestination = false;
	goLastAdded = nullptr;
	SetSpriteSize(30, 30);
	SetLocalPosition(0, 0);
	hitbox = { 0,0,30,30 };
	SelectNumChange = 0;
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void DebugAim::Update()
{
	if (GetInput(DEBUG_SWITCH_NUM_OPTION))
	{
		SelectNumChange++;
		if (SelectNumChange >= MAX_SELECT_NUM_OPTIONS)
			SelectNumChange = 0;
	}
	switch (SelectNumChange)
	{
	case DAO_LEVEL_SLOT:
		if (GetInput(DEBUG_SWITCH_SLOTUP))
		{
			if (GetInput(DEBUG_SPEED_CHANGE))
				nSaveSlot += 10;
			else
				nSaveSlot++;
		}
		else if (GetInput(DEBUG_SWITCH_SLOTDOWN))
		{
			if (GetInput(DEBUG_SPEED_CHANGE))
				nSaveSlot -= 10;
			else
				nSaveSlot--;
		}
		break;
	case DAO_OBJECT_SPEED:
		if (GetInput(DEBUG_SWITCH_SLOTUP))
		{
			if (GetInput(DEBUG_SPEED_CHANGE))
				nMoveableObjectSpeed += 10;
			else
				nMoveableObjectSpeed++;
		}
		else if (GetInput(DEBUG_SWITCH_SLOTDOWN))
		{
			if (GetInput(DEBUG_SPEED_CHANGE))
				nMoveableObjectSpeed -= 10;
			else
				nMoveableObjectSpeed--;
		}
		break;
	case DAO_OBJECT_DELAY:
		if (GetInput(DEBUG_SWITCH_SLOTUP))
		{
			if (GetInput(DEBUG_SPEED_CHANGE))
				nDelaySeconds += 10;
			else
				nDelaySeconds++;
		}
		else if (GetInput(DEBUG_SWITCH_SLOTDOWN))
		{
			if (GetInput(DEBUG_SPEED_CHANGE))
				nDelaySeconds -= 10;
			else
				nDelaySeconds--;
		}
		break;
	default:
		break;
	}
	if (nMoveableObjectSpeed > MAX_OBJ_SPEED)
		nMoveableObjectSpeed = MAX_OBJ_SPEED;
	if (nDelaySeconds > MAX_OBJ_DELAY_STOP)
		nDelaySeconds = MAX_OBJ_DELAY_STOP;
	if (nMoveableObjectSpeed < 0)
		nMoveableObjectSpeed = 0;
	if (nDelaySeconds < 0)
		nDelaySeconds = 0;

	if (nSaveSlot < 0)
		nSaveSlot = 0;
	if (nSaveSlot >= MAX_SLOTS)
		nSaveSlot = MAX_SLOTS - 1;
	float Speed = 5;
	if (GetInput(DEBUG_SPEED_CHANGE))
		Speed = 25;
	if (GetInput(MOVE_UP))
		Position.y += Speed;
	if (GetInput(MOVE_DOWN))
		Position.y -= Speed;
	if (GetInput(MOVE_RIGHT))
		Position.x += Speed;
	if (GetInput(MOVE_LEFT))
		Position.x -= Speed;
	if (goLastAdded  && !bSetStarTeleportDestination) {
		if (nMoveableObjectSpeed>0)
		{
			bRepeatableUV_Y = false;
			bRepeatableUV_X = false;
			nFrame_Y = 1;
			nFrame_X = 1;
			uv = { 0,0 };
			gpTexture = GetTexture(TEXTURE_DEBUG_X_GREEN);
			SetSpriteSize(30, 30);
			if (GetInput(MOVE_JUMP_TRIGGER))
			{
				XMFLOAT3 Initial = goLastAdded->GetWorldPosition();
				XMFLOAT3 Final = GetWorldPosition();
				goLastAdded->SetUpMoveControl({ Initial.x, Initial.y }, { Final.x, Final.y }, (float)nMoveableObjectSpeed);
				goLastAdded->SetDelayFrames(nDelaySeconds*30);
				if (goLastAdded->GetType() == TELEPORTER_TYPE) {
					bSetStarTeleportDestination = true;
					return;
				}
				goLastAdded = nullptr;
			}
			return;
		}
		else {
			if(!bSetStarTeleportDestination)
				goLastAdded = nullptr;
		}
	}
	//テレポートの拠点を設定する
	if (bSetStarTeleportDestination)
	{
		bRepeatableUV_Y = false;
		bRepeatableUV_X = false;
		nFrame_Y = 1;
		nFrame_X = 1;
		uv = { 0,0 };
		gpTexture = GetTexture(TEXTURE_DEBUG_X_GREEN);
		SetSpriteSize(30, 30);
		if (GetInput(MOVE_JUMP_TRIGGER))
		{
			if (goLastAdded) {
				if (goLastAdded->GetType() == TELEPORTER_TYPE) {
					Teleporter* pTele = (Teleporter*)goLastAdded;
					printf("a");
					pTele->SetTeleporterDestination({ GetWorldPosition().x, GetWorldPosition().y });
				}
				else if (goLastAdded->GetType() == PUNCHABLE_STAR_TYPE) {
					PunchStar* pStar = (PunchStar*)goLastAdded;
					pStar->SetStarDestination({ GetWorldPosition().x, GetWorldPosition().y });
				}
			}
			bSetStarTeleportDestination = false;
			goLastAdded = nullptr;
		}
		return;
	}
	if (GetInput(DEBUG_CHANGE_DEBUG_AIM_CREATOR_RIGHT)) {
		ResetUniversalScale();
		nTypeObjectToCreate++;
		if (nType == DA_GENERAL_POLYGON)
			SetSpriteSize(10, 10);
		else if(nTypeObjectToCreate == DA_SPIKE)
			Size.x = SPIKE_SIZE;
		else
			SetAlpha(1);
	}
	if (GetInput(DEBUG_CHANGE_DEBUG_AIM_CREATOR_LEFT)) {
		ResetUniversalScale();
		nTypeObjectToCreate--;
		if (nType == DA_GENERAL_POLYGON)
			SetSpriteSize(10, 10);
		else if (nTypeObjectToCreate == DA_SPIKE)
			Size.x=SPIKE_SIZE;
		else
			SetAlpha(1);
	}
	if (nTypeObjectToCreate < 0)
		nTypeObjectToCreate = 0;
	if (nTypeObjectToCreate > DA_MAX - 1)
		nTypeObjectToCreate = DA_MAX - 1;
	SceneInGame* pGame = GetCurrentGame();
	if (!pGame)
		return;
	Go_List* pGems = pGame->GetPickableGems();
	Go_List* LevelObjects = pGame->GetLevelObjects();
	Go_List* Floors = pGame->GetFloors();
	Go_List* Walls = pGame->GetWalls();
	Enemy_List* Enemies = pGame->GetEnemies();
	if (GetInput(DEBUG_SAVELEVEL)) {
		pGame->GetUIManager()->ActivateSavedLevelMessage();
		char szSlot[2];
		_itoa_s(nSaveSlot, szSlot, 10);
		char FileName[256] = "";
		strcat_s(FileName, szSlot);
		strcat_s(FileName, "_Gems_Level");
		printf("%s\n", FileName);

		pGems->SaveGems(FileName);

		strcpy_s(FileName,"");
		strcat_s(FileName, szSlot);
		strcat_s(FileName, "_Floors_Level");

		Floors->SaveFloors(FileName);

		strcpy_s(FileName, "");
		strcat_s(FileName, szSlot);
		strcat_s(FileName, "_Enemies_Level");

		Enemies->Save(FileName);

		strcpy_s(FileName, "");
		strcat_s(FileName, szSlot);
		strcat_s(FileName, "_Wall_Level");

		Walls->SaveWalls(FileName);

		strcpy_s(FileName, "");
		strcat_s(FileName, szSlot);
		strcat_s(FileName, "_Spikes_Level");

		LevelObjects->SaveSpikes(FileName);

		strcpy_s(FileName, "");
		strcat_s(FileName, szSlot);
		strcat_s(FileName, "_Soba_Level");

		LevelObjects->SaveRecoveryPicks(FileName);

		strcpy_s(FileName, "");
		strcat_s(FileName, szSlot);
		strcat_s(FileName, "_Teleporters_Level");

		LevelObjects->SaveTeleporters(FileName);

		strcpy_s(FileName, "");
		strcat_s(FileName, szSlot);
		strcat_s(FileName, "_Stars_Level");

		LevelObjects->SavePunchStars(FileName);
	}
	if (GetInput(DEBUG_LOADLEVEL) && pGame->GetCurrentLevel() == DEBUG_ROOM)
	{
		pGame->GetUIManager()->ActivateLoadedLevelMessage();
		pGems->End();
		Enemies->ResetList();
		Floors->End();
		Walls->End();
		LevelObjects->End();
		GetPlayerPointer()->ResetPlayerLists();

		char szSlot[2];
		_itoa_s(nSaveSlot, szSlot, 10);
		char FileName[256] = "";
		strcat_s(FileName, szSlot);
		strcat_s(FileName, "_Gems_Level");
		printf("%s\n", FileName);

		pGems->Load(FileName, GEM_TYPE);

		strcpy_s(FileName, "");
		strcat_s(FileName, szSlot);
		strcat_s(FileName, "_Floors_Level");

		Floors->Load(FileName, FLOOR_TYPE);

		strcpy_s(FileName, "");
		strcat_s(FileName, szSlot);
		strcat_s(FileName, "_Enemies_Level");

		Enemies->Load(FileName);

		strcpy_s(FileName, "");
		strcat_s(FileName, szSlot);
		strcat_s(FileName, "_Wall_Level");

		Walls->Load(FileName, WALL_TYPE);

		strcpy_s(FileName, "");
		strcat_s(FileName, szSlot);
		strcat_s(FileName, "_Spikes_Level");

		LevelObjects->Load(FileName, SPIKE_TYPE);

		strcpy_s(FileName, "");
		strcat_s(FileName, szSlot);
		strcat_s(FileName, "_Soba_Level");

		LevelObjects->Load(FileName, RECOVER_TYPE);

		strcpy_s(FileName, "");
		strcat_s(FileName, szSlot);
		strcat_s(FileName, "_Teleporters_Level");

		LevelObjects->Load(FileName, TELEPORTER_TYPE);
		strcpy_s(FileName, "");
		strcat_s(FileName, szSlot);
		strcat_s(FileName, "_Stars_Level");

		LevelObjects->Load(FileName, PUNCHABLE_STAR_TYPE);
	}
	float fGrowSpeed = 7;
	if (GetInput(DEBUG_SPEED_CHANGE))
		fGrowSpeed = 15;
	char szFileName[256] = "";
	if (nTypeObjectToCreate != DA_GENERAL_POLYGON)
	{
		if (GetInput(DEBUG_SIZEX_UP))
			IncreaseUniversalScale(0.0005f*fGrowSpeed);
		if (GetInput(DEBUG_SIZEX_DOWN))
			ReduceUniversalScale(0.0005f*fGrowSpeed);
	}
	if (GetInput(DEBUG_TELEPORTPLAYER))
	{
		if (GetPlayerPointer())
			GetPlayerPointer()->SetWorldPosition(GetWorldPosition().x, GetWorldPosition().y);
	}
	//作るオブジェクト
	static int nSpikeGrowCooldown = 0;//大きくなるボタンを押したときに、スパイクのサイズは毎フレームで管理する
	switch (nTypeObjectToCreate)
	{
		case DA_AIM:
			bRepeatableUV_Y = false;
			bRepeatableUV_X = false;
			nFrame_Y = 1;
			nFrame_X = 1;
			uv = { 0,0 };
	#ifdef USE_CONSOLE
			if (GetInput(INPUT_DEBUG_PRINT_POS))
				printf("AIM: {%.2ff, %.2ff}\n", Position.x, Position.y);
	#endif
			gpTexture = GetTexture(TEXTURE_DEBUG_AIM);
			SetSpriteSize(30, 30);
			if (GetInput(DEBUG_DELETE_LAST_OBJECT))
			{
				GameObject* ToDel = nullptr;
				ToDel = Floors->FindWithCollision(GetHitbox());
				if (ToDel) {
					Floors->Delete(ToDel);
					break;
				}
				ToDel = Walls->FindWithCollision(GetHitbox());
				if (ToDel) {
					Walls->Delete(ToDel);
					break;
				}
				ToDel = pGems->FindWithCollision(GetHitbox());
				if (ToDel) {
					pGems->Delete(ToDel);
					break;
				}
				ToDel = LevelObjects->FindWithCollision(GetHitbox());
				if (ToDel) {
					LevelObjects->Delete(ToDel);
					break;
				}
			}
			break;

		case DA_GEM:
			bRepeatableUV_Y = false;
			bRepeatableUV_X = false;
			gpTexture = GetTexture(TEXTURE_CRYSTAL);
			SetSpriteSize(50, 71);
			nFrame_Y = 4;
			nFrame_X = 5;
			if (GetInput(MOVE_JUMP_TRIGGER))
				goLastAdded = pGems->AddGemPickable({ Position.x, Position.y });
			if (GetInput(DEBUG_DELETE_LAST_OBJECT))
				pGems->DeleteLastPosObject();
			break;

		case DA_SOBA:
			bRepeatableUV_Y = false;
			bRepeatableUV_X = false;
			gpTexture = GetTexture(TEXTURE_SOBA);
			SetSpriteSize(125, 82);
			nFrame_Y = 1;
			nFrame_X = 1;
			if (GetInput(MOVE_JUMP_TRIGGER))
				goLastAdded = LevelObjects->AddRecoveryPickable({ Position.x, Position.y });
			if (GetInput(DEBUG_DELETE_LAST_OBJECT))
				LevelObjects->DeleteLastPosObject();
			break;

		case DA_FLOOR_DEBUG:
			bRepeatableUV_Y = false;
			bRepeatableUV_X = true;
			nFrame_Y = 1;
			fXTileSize = 270;
			Size.y = 270;
			gpTexture = GetTexture(DOMINOS_FLOOR_BLACK_TEXTURE);
			if (GetInput(DEBUG_FLOOR_SIZE_UP))
				Size.x += fGrowSpeed;
			if (GetInput(DEBUG_FLOOR_SIZE_DOWN)) {
				Size.x -= fGrowSpeed;
				ResetUniversalScale();
			}
			while (Size.x*(GetUniversalScale() / 2) > WINDOW_WIDTH / 2)
				ReduceUniversalScale(0.0005f*fGrowSpeed);

			if (GetInput(MOVE_JUMP_TRIGGER)) 
				goLastAdded = Floors->AddFloor(DOMINOS_FLOOR_BLACK_TEXTURE, { 0,-80,Size.x,75,false,nullptr }, (int)fXTileSize, { GetWorldPosition().x, GetWorldPosition().y }, { Size.x,Size.y }, LEVEL1_ROOM);
			if (GetInput(DEBUG_DELETE_LAST_OBJECT))
				Floors->DeleteLastPosObject();
			break;

		case DA_FLOOR_REGULAR_PURPLE:
			bRepeatableUV_Y = false;
			bRepeatableUV_X = true;
			nFrame_Y = 1;
			fXTileSize = TILE_SIZE_FLOOR2;
			Size.y = 270;
			gpTexture = GetTexture(REGULAR_FLOOR_PURPLE_TEXTURE);
			if (GetInput(DEBUG_FLOOR_SIZE_UP))
				Size.x += fGrowSpeed;
			if (GetInput(DEBUG_FLOOR_SIZE_DOWN)) {
				Size.x -= fGrowSpeed;
				ResetUniversalScale();
			}
			while (Size.x*(GetUniversalScale() / 2) > WINDOW_WIDTH / 2)
				ReduceUniversalScale(0.0005f*fGrowSpeed);
			if (GetInput(MOVE_JUMP_TRIGGER)) 
				goLastAdded = Floors->AddFloor(REGULAR_FLOOR_PURPLE_TEXTURE, { 0,-80,Size.x,50,false,nullptr }, (int)fXTileSize, { GetWorldPosition().x, GetWorldPosition().y }, { Size.x,Size.y }, LEVEL1_ROOM);
			if (GetInput(DEBUG_DELETE_LAST_OBJECT))
				Floors->DeleteLastPosObject();
			break;

		case DA_WALL_PURPLEFACE:
			bRepeatableUV_Y = true;
			bRepeatableUV_X = false;
			nFrame_X = 1;
			nFrame_Y = 1;
			fXTileSize = 150;
			Size.x = 150;
			gpTexture = GetTexture(EVIL_BLOCK_TEXTURE);
			if (GetInput(DEBUG_FLOOR_SIZE_UP))
				Size.y += fGrowSpeed;
			if (GetInput(DEBUG_FLOOR_SIZE_DOWN)) {
				Size.y -= fGrowSpeed;
				ResetUniversalScale();
			}
			while (Size.y*(GetUniversalScale() / 2) > WINDOW_HEIGHT / 2)
				ReduceUniversalScale(0.0005f*fGrowSpeed);
			if (GetInput(MOVE_JUMP_TRIGGER))
				goLastAdded = Walls->AddWall(EVIL_BLOCK_TEXTURE, { 0,0,150,Size.y,false,nullptr }, (int)fXTileSize, { GetWorldPosition().x, GetWorldPosition().y }, { Size.x,Size.y }, LEVEL1_ROOM);
			if (GetInput(DEBUG_DELETE_LAST_OBJECT))
				Walls->DeleteLastPosObject();
			break;

		case DA_SPIKE:
			bRepeatableUV_Y = false;
			bRepeatableUV_X = true;
			fXTileSize = SPIKE_SIZE;
			nFrame_Y = 1;
			Size.y = SPIKE_SIZE_TALL;
			gpTexture = GetTexture(TEXTURE_SPIKE);
			if (++nSpikeGrowCooldown > 5) {
				if (GetInput(DEBUG_FLOOR_SIZE_UP)) {
					Size.x += SPIKE_SIZE;
					nSpikeGrowCooldown = 0;
				}
				if (GetInput(DEBUG_FLOOR_SIZE_DOWN)) {
					Size.x -= SPIKE_SIZE;
					nSpikeGrowCooldown = 0;
					ResetUniversalScale();
				}
			}
			if (Size.x < 0)
				Size.x = 0;
			while (Size.x*(GetUniversalScale() / 2) > WINDOW_WIDTH / 2)
				ReduceUniversalScale(0.0005f*fGrowSpeed);
			if (GetInput(MOVE_JUMP_TRIGGER))
				goLastAdded = LevelObjects->AddSpike({ Position.x,Position.y }, (int)(Size.x / SPIKE_SIZE));
			if (GetInput(DEBUG_DELETE_LAST_OBJECT))
				LevelObjects->DeleteLastPosObject();
			break;
		case DA_GENERAL_POLYGON:
			gpTexture = GetHitboxTexture();
			SetAlpha(0.5f);
			if (GetInput(DEBUG_SIZEY_UP))
				Size.y += fGrowSpeed;
			if (GetInput(DEBUG_SIZEY_DOWN)) {
				Size.y -= fGrowSpeed;
				ResetUniversalScale();
			}
			if (GetInput(DEBUG_SIZEX_UP))
				Size.x += fGrowSpeed;
			if (GetInput(DEBUG_SIZEX_DOWN)) {
				Size.x -= fGrowSpeed;
				ResetUniversalScale();
			}
			while (Size.y*(GetUniversalScale() / 2) > WINDOW_HEIGHT / 2)
				ReduceUniversalScale(0.0005f*fGrowSpeed);
			while (Size.x*(GetUniversalScale() / 2) > WINDOW_WIDTH / 2)
				ReduceUniversalScale(0.0005f*fGrowSpeed);
			break;

		case DA_ENEMY_BUNBUN:
			nFrame_X = 6;
			nFrame_Y = 15;
			uv = { 2,0 };
			bRepeatableUV_Y = false;
			bRepeatableUV_X = false;
			gpTexture = GetTexture(TEXTURE_BUNBUN_ENEMY);
			SetSpriteSize(BUNBUN_SIZE, BUNBUN_SIZE);
			if (GetInput(DEBUG_DELETE_LAST_OBJECT))
				Enemies->DeleteLastPosObject();
			if (GetInput(MOVE_JUMP_TRIGGER)) {
				Enemies->AddEnemy(BUNBUN_ENEMY, { GetWorldPosition().x, GetWorldPosition().y });
			}
			break;

		case DA_TELEPORT:
			nFrame_X = 1;
			nFrame_Y = 15;
			uv = { 1,1 };
			bRepeatableUV_Y = false;
			bRepeatableUV_X = false;
			gpTexture = GetTexture(TEXTURE_TELEPORTER);
			SetSpriteSize(TELEPORTER_SIZE, TELEPORTER_SIZE);
			if (GetInput(DEBUG_DELETE_LAST_OBJECT))
				LevelObjects->DeleteLastPosObject();
			if (GetInput(MOVE_JUMP_TRIGGER)) 
			{
				if(nMoveableObjectSpeed==0)
					bSetStarTeleportDestination = true;
				goLastAdded = LevelObjects->AddTeleporter({ GetWorldPosition().x, GetWorldPosition().y }, { GetWorldPosition().x, GetWorldPosition().y });
			}
			break;

		case DA_ENDPOINT:
			nFrame_X = 1;
			nFrame_Y = 15;
			uv = { 1,1 };
			bRepeatableUV_Y = false;
			bRepeatableUV_X = false;
			gpTexture = GetTexture(TEXTURE_ENDPOINT_TELEPORTER);
			SetSpriteSize(TELEPORTER_SIZE, TELEPORTER_SIZE);
			if (GetInput(DEBUG_DELETE_LAST_OBJECT))
				LevelObjects->DeleteLastPosObject();
			if (GetInput(MOVE_JUMP_TRIGGER))
				goLastAdded = LevelObjects->AddEndPoint({ GetWorldPosition().x, GetWorldPosition().y });
			break;

		case DA_STAR:
			nFrame_X = 3;
			nFrame_Y = 5;
			uv = { 1,4 };
			bRepeatableUV_Y = false;
			bRepeatableUV_X = false;
			gpTexture = GetTexture(TEXTURE_PUNCHABLE_STAR);
			SetSpriteSize(STAR_SIZE, STAR_SIZE);
			if (GetInput(DEBUG_DELETE_LAST_OBJECT))
				LevelObjects->DeleteLastPosObject();
			if (GetInput(MOVE_JUMP_TRIGGER))
			{
				bSetStarTeleportDestination = true;
				goLastAdded = LevelObjects->AddPunchStar({ GetWorldPosition().x, GetWorldPosition().y }, { GetWorldPosition().x, GetWorldPosition().y });
			}
			break;

		default:
			break;
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void DebugAim::Draw()
{
	GameObject::Draw();
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void DebugAim::End()
{
	GameObject::End();
}

//*****************************************************************************
//GetSaveSlot関数
//現在の保存のスロットを戻す
//引数：void
//戻：int
//*****************************************************************************
int DebugAim::GetSaveSlot()
{
	return nSaveSlot;
}

//*****************************************************************************
//GetMoveableObjectSpeed関数
//動けるオブジェクトの速さを戻す
//引数：void
//戻：int
//*****************************************************************************
int DebugAim::GetMoveableObjectSpeed()
{
	return nMoveableObjectSpeed;
}

//*****************************************************************************
//GetDelayObjects関数
//動けるオブジェクトの止まるディレイを戻す
//引数：void
//戻：int
//*****************************************************************************
int DebugAim::GetDelayObjects()
{
	return nDelaySeconds;
}

//*****************************************************************************
//GetSelectNum関数
//選んだオプションを戻す
//引数：void
//戻：int
//*****************************************************************************
int DebugAim::GetSelectNum()
{
	return SelectNumChange;
}
