//*****************************************************************************
// GameObject_List.h
// Author : Mane
//*****************************************************************************
#ifndef	GAMEOBJECT_LIST_H
#define	GAMEOBJECT_LIST_H

#include "GameObject.h" 

//*****************************************************************************
// 構造体
//*****************************************************************************
struct FloorContainer {
	int FloorTexture = 0;
	HitBox hbox = HitBox();
	int FloorTileSize = 0;
	XMFLOAT2 Size = {0,0};
	int Floor_ID = 0;
	XMFLOAT2 Pos = {0,0};
	XMFLOAT2 InitialPos = {0,0};
	XMFLOAT2 FinalPos = {0,0};
	XMFLOAT2 MovingSpeed = {0,0};
	XMFLOAT2 ResetSwitchPosition = {0,0};
	int nObjMoveable = 0;
	int nDelayFrames = 0;
};
struct WallContainer
{
	int TextureID=0;
	HitBox hbox = HitBox();
	int WallTileSize = 0;
	XMFLOAT2 Pos = {0,0};
	XMFLOAT2 Size = {0,0};
	int n_WallID = 0;
	bool MoveableObject = false;
	XMFLOAT2 InitialPos = {0,0};
	XMFLOAT2 FinalPos = {0,0};
	XMFLOAT2 MovingSpeed = {0,0};
	int DelayFrames = 0;
	int ActivationTypeWall = 0;
};

struct SpikeContainer
{
	XMFLOAT2 Pos = {0,0};
	int nSpikeNumber = 0;
	int nID = 0;
	XMFLOAT3 Rotation = {0,0,0};
	XMFLOAT2 InitialPos = {0,0};
	XMFLOAT2 FinalPos = {0,0};
	XMFLOAT2 MovingSpeed = {0,0};
	int DelayFrames = 0;
	bool MoveableObject = false;
};
struct PickItemContainer
{
	XMFLOAT2 Position = {0,0};
	bool MoveableObject = false;
	XMFLOAT2 InitialPos = {0,0};
	XMFLOAT2 FinalPos = {0,0};
	XMFLOAT2 MovingSpeed = {0,0};
	int DelayFrames = 0;
	int nID = 0;
};
struct TeleporterContainer
{
	XMFLOAT2 Position = {0,0};
	XMFLOAT2 Destination = {0,0};
	bool bIsEndPoint = false;
	bool MoveableObject = false;
	XMFLOAT2 InitialPos = {0,0};
	XMFLOAT2 FinalPos = {0,0};
	XMFLOAT2 MovingSpeed = {0,0};
	int DelayFrames = 0;
	int nID = 0;
};

struct StarContainer
{
	XMFLOAT2 Position = { 0,0 };
	XMFLOAT2 Destination = { 0,0 };
	int nID = 0;
};
//*****************************************************************************
// クラス-リスト
//*****************************************************************************
struct go_node {//リストの管理の為に、この構造体が要る
	GameObject* Object = nullptr;
	go_node *next = nullptr;
};

class Go_List
{
private:

public:
	go_node * HeadNode;
	Go_List();
	~Go_List();
	void AddGameObject(GameObject newObject);
	GameObject* AddRecoveryPickable(XMFLOAT2 newPosition);
	GameObject* AddRecoveryPickable(XMFLOAT2 newPosition, int nID, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, int DelayFrames);
	GameObject* AddGemPickable(XMFLOAT2 newPosition);
	GameObject* AddGemPickable(XMFLOAT2 newPosition, int nID, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, int DelayFrames);
	GameObject* AddFloor(int FloorTexture, HitBox hbox, int FloorTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize);
	GameObject* AddFloor(int FloorTexture, HitBox hbox, int FloorTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed);
	GameObject* AddFloor(int FloorTexture, HitBox hbox, int FloorTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize, int Floor_ID);
	GameObject* AddFloor(int FloorTexture, HitBox hbox, int FloorTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize, int Floor_ID, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, int DelayFrames);
	GameObject* AddFloor(int FloorTexture, HitBox hbox, int FloorTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize, int Floor_ID, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, XMFLOAT2 ResetSwitchPosition);
	GameObject* AddWall(int TextureID, HitBox hbox, int WallTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize);
	GameObject* AddWall(int TextureID, HitBox hbox, int WallTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize, int nID);
	GameObject* AddWall(int TextureID, HitBox hbox, int WallTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize, int nID, int nWallActivation);
	GameObject* AddWall(int TextureID, HitBox hbox, int WallTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize, int n_WallID, int nWallActivation, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, int DelayFrames);
	GameObject* GetWall(int nWall_ID);
	GameObject* GetFloor(int nFloor_ID);
	GameObject* AddSpike(XMFLOAT2 Pos, int nSpikeNumber);
	GameObject* AddSpike(XMFLOAT2 Pos, int nSpikeNumber, int nID);
	GameObject* AddSpike(XMFLOAT2 Pos, int nSpikeNumber, int nID, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, int DelayFrames);
	GameObject* AddSwitchBulb(XMFLOAT2 Pos, int nSwitchID, bool bIsPunchable);
	GameObject* GetSwitch(int nSwitch_ID);
	GameObject* AddTeleporter(XMFLOAT2 TeleporterPos, XMFLOAT2 NewPlayerPos);
	GameObject* AddTeleporter(XMFLOAT2 TeleporterPos, XMFLOAT2 NewPlayerPos, int nID, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, int DelayFrames);
	GameObject* AddEndPoint(XMFLOAT2 TeleporterPos);
	GameObject* AddEndPoint(XMFLOAT2 TeleporterPos, int nID);
	GameObject* AddEndPoint(XMFLOAT2 TeleporterPos, int nID, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, int DelayFrames);
	GameObject* AddTeleporter(XMFLOAT2 TeleporterPos, XMFLOAT2 NewPlayerPos, int nID);
	GameObject* GetTeleporter(int nTeleporter_ID);
	GameObject* AddPunchStar(XMFLOAT2 InitialPos, XMFLOAT2 FinalPos);
	GameObject* AddPunchStar(XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, int nID);
	GameObject* GetPunchStar(int nStar_ID);
	GameObject* FindWithCollision(HitBox hbox);
	int GetNumberOfObjects();
	void DeleteLastPosObject();
	void SaveFloors(const char* szFilename);
	void SaveWalls(const char* szFilename);
	void SaveSpikes(const char* szFilename);
	void SaveGems(const char* szFilename);
	void SaveTeleporters(const char* szFilename);
	void SaveRecoveryPicks(const char* szFilename);
	void SavePunchStars(const char* szFilename);
	void Load(const char * szFilename, int nType);
	void Delete(GameObject*);
	void Update();
	void Draw();
	void End();
};

#endif