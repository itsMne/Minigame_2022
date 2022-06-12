//*****************************************************************************
// GameObject_List.cpp
// ゲームの一般的なオブジェクトのリスト
// Author : Mane
//*****************************************************************************
#include "GameObject_List.h"
#include "SceneInGame.h"
#include "PunchStar.h"
#include "Floor.h"
#include "Wall.h"
#include "Gem.h"
#include "Switch.h"
#include "Spike.h"
#include "RecoveryItem.h"
#include "Teleporter.h"

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
Go_List::Go_List()
{
	HeadNode = nullptr;
}

Go_List::~Go_List()
{
	End();
}

//*****************************************************************************
//AddGameObject関数
//リストにゲームオブジェクトのコピーを入れる
//引数：GameObject
//戻：void
//*****************************************************************************
void Go_List::AddGameObject(GameObject newObject)
{
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList != nullptr)
			pPositionList = pPositionList->next;
		pPositionList = new go_node();
		pPositionList->Object = new GameObject();
		*(pPositionList->Object) = newObject;
		pPositionList->next = nullptr;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new GameObject();
		if(pPositionList)
			(pPositionList->Object) = &newObject;
		HeadNode->next = nullptr;
	}
}

//*****************************************************************************
//AddRecoveryPickable関数
//リストに回復アイテムを入れる
//引数：XMFLOAT2
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddRecoveryPickable(XMFLOAT2 newPosition)
{
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		go_node* pWorkList = new go_node();
		pWorkList->Object = new RecoveryItem(newPosition);
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		return pWorkList->Object;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new RecoveryItem(newPosition);
		HeadNode->next = nullptr;
		return HeadNode->Object;
	}
}

//*****************************************************************************
//AddGemPickable関数
//リストにジェムを入れる
//引数：XMFLOAT2
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddGemPickable(XMFLOAT2 newPosition)
{
	SceneInGame* pGame = GetCurrentGame();
	if (!pGame)
		return nullptr;
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		go_node* pWorkList = new go_node();
		pWorkList->Object = new Gem(newPosition);
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		pGame->AddMaxGemNum(1);
		return pWorkList->Object;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new Gem(newPosition);
		HeadNode->next = nullptr;
		pGame->AddMaxGemNum(1);
		return HeadNode->Object;
	}
}

//*****************************************************************************
//AddGemPickable関数
//リストにジェムを入れる(動ける)
//引数：XMFLOAT2, int, XMFLOAT2, XMFLOAT2, XMFLOAT2, int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddGemPickable(XMFLOAT2 newPosition, int nID, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, int DelayFrames)
{
	GameObject* goObj = AddGemPickable(newPosition);
	goObj->SetNewID(nID);
	goObj->SetUpMoveControl(InitialPos, FinalPos, MovingSpeed);
	goObj->SetDelayFrames(DelayFrames);
	return goObj;
}

//*****************************************************************************
//AddRecoveryPickable関数
//リストに回復アイテムを入れる(動ける)
//引数：XMFLOAT2, int, XMFLOAT2, XMFLOAT2, XMFLOAT2, int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddRecoveryPickable(XMFLOAT2 newPosition, int nID, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, int DelayFrames)
{
	GameObject* goObj = AddRecoveryPickable(newPosition);
	goObj->SetNewID(nID);
	goObj->SetUpMoveControl(InitialPos, FinalPos, MovingSpeed);
	goObj->SetDelayFrames(DelayFrames);
	return goObj;
}

//*****************************************************************************
//AddFloor関数
//リストに床を入れる
//引数：int, HitBox, int, XMFLOAT2, XMFLOAT2
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddFloor(int FloorTexture, HitBox hbox, int FloorTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize)
{
	return AddFloor(FloorTexture, hbox, FloorTileSize, newPosition, newSize, STATIC_ID);
}

//*****************************************************************************
//AddFloor関数
//リストに床を入れる
//引数：int, HitBox, int, XMFLOAT2, XMFLOAT2, XMFLOAT2, XMFLOAT2
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddFloor(int FloorTexture, HitBox hbox, int FloorTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed)
{
	GameObject* goAd = AddFloor(FloorTexture, hbox, FloorTileSize, newPosition, newSize);
	goAd->SetUpMoveControl(InitialPos, FinalPos, MovingSpeed);
	return goAd;
}

//*****************************************************************************
//AddFloor関数
//リストに床を入れる
//引数：int, HitBox, int, XMFLOAT2, XMFLOAT2, int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddFloor(int FloorTexture, HitBox hbox, int FloorTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize, int Floor_ID)
{
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		go_node* pWorkList = new go_node();
		pWorkList->Object = new Floor(FloorTexture, hbox, (float)FloorTileSize, Floor_ID);
		Floor* ThisFloor = (Floor*)pWorkList->Object;
		ThisFloor->SetSpriteSize(newSize.x, newSize.y);
		ThisFloor->SetWorldPosition(newPosition.x, newPosition.y);
		ThisFloor->SetObjectInitialPos({ newPosition.x, newPosition.y });
		ThisFloor->SetRepeatableUV_X(true);
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		return pWorkList->Object;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new Floor(FloorTexture, hbox, (float)FloorTileSize, Floor_ID);
		Floor* ThisFloor = (Floor*)HeadNode->Object;
		ThisFloor->SetSpriteSize(newSize.x, newSize.y);
		ThisFloor->SetWorldPosition(newPosition.x, newPosition.y);
		ThisFloor->SetObjectInitialPos({ newPosition.x, newPosition.y });
		ThisFloor->SetRepeatableUV_X(true);
		HeadNode->next = nullptr;
		return HeadNode->Object;
	}
}

//*****************************************************************************
//AddFloor関数
//リストに床を入れる
//引数：int, HitBox, int, XMFLOAT2, XMFLOAT2, int, XMFLOAT2, 
//		XMFLOAT2, XMFLOAT2, int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddFloor(int FloorTexture, HitBox hbox, int FloorTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize, int Floor_ID, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, int nDelay)
{
	GameObject* goAd = AddFloor(FloorTexture, hbox, FloorTileSize, newPosition, newSize, Floor_ID);
	goAd->SetUpMoveControl(InitialPos, FinalPos, MovingSpeed);
	goAd->SetDelayFrames(nDelay);
	return goAd;
}

//*****************************************************************************
//AddFloor関数
//リストに床を入れる
//引数：int, HitBox, int, XMFLOAT2, XMFLOAT2, int, XMFLOAT2, 
//		XMFLOAT2, XMFLOAT2, XMFLOAT2
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddFloor(int FloorTexture, HitBox hbox, int FloorTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize, int Floor_ID, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, XMFLOAT2 ResetSwitchPosition)
{
	Floor* goAd = (Floor*)AddFloor(FloorTexture, hbox, FloorTileSize, newPosition, newSize, Floor_ID);
	goAd->SetUpMoveFloorControlledBySwitch(InitialPos, FinalPos, MovingSpeed, ResetSwitchPosition);
	return goAd;
}

//*****************************************************************************
//AddWall関数
//リストに壁を入れる
//引数：int, HitBox, int, XMFLOAT2, XMFLOAT2
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddWall(int TextureID, HitBox hbox, int WallTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize)
{
	return AddWall(TextureID, hbox, WallTileSize, newPosition, newSize, STATIC_ID, WALL_NORMAL);
}

//*****************************************************************************
//AddWall関数
//リストに壁を入れる
//引数：int, HitBox, int, XMFLOAT2, XMFLOAT2, int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddWall(int TextureID, HitBox hbox, int WallTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize, int nID)
{
	return AddWall(TextureID, hbox, WallTileSize, newPosition, newSize, nID, WALL_NORMAL);
}

//*****************************************************************************
//AddWall関数
//リストに壁を入れる
//引数：int, HitBox, int, XMFLOAT2, XMFLOAT2, int, int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddWall(int TextureID, HitBox hbox, int WallTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize, int n_WallID, int nWallActivation)
{
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		go_node* pWorkList = new go_node();
		pWorkList->Object = new Wall(TextureID, WALL_TYPE, hbox, WallTileSize, nWallActivation);
		pWorkList->Object->SetSpriteSize(newSize.x, newSize.y);
		pWorkList->Object->SetLocalPosition(newPosition.x, newPosition.y);
		pWorkList->Object->SetObjectInitialPos(newPosition);
		((Wall*)pWorkList->Object)->SetWallID(n_WallID);
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		return pWorkList->Object;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new Wall(TextureID, WALL_TYPE, hbox, WallTileSize, nWallActivation);
		HeadNode->Object->SetSpriteSize(newSize.x, newSize.y);
		HeadNode->Object->SetLocalPosition(newPosition.x, newPosition.y);
		HeadNode->Object->SetObjectInitialPos(newPosition);
		((Wall*)HeadNode->Object)->SetWallID(n_WallID);
		HeadNode->next = nullptr;
		return HeadNode->Object;
	}
}

//*****************************************************************************
//AddWall関数
//リストに壁を入れる
//引数：int, HitBox, int, XMFLOAT2, XMFLOAT2, int, int, 
//		XMFLOAT2, XMFLOAT2, XMFLOAT2, int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddWall(int TextureID, HitBox hbox, int WallTileSize, XMFLOAT2 newPosition, XMFLOAT2 newSize, int n_WallID, int nWallActivation, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, int DelayFrames)
{
	GameObject* goObj = AddWall(TextureID, hbox, WallTileSize, newPosition, newSize, n_WallID, nWallActivation);
	if (goObj) {
		goObj->SetUpMoveControl(InitialPos, FinalPos, MovingSpeed);
		goObj->SetDelayFrames(DelayFrames);
	}
	return goObj;
}

//*****************************************************************************
//GetWall関数
//壁を探し、戻す
//引数：int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::GetWall(int nWall_ID)
{
	if (HeadNode == nullptr)
		return nullptr;
	go_node* pPositionList = HeadNode;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr)
		{
			if (((Wall*)pPositionList->Object)->GetWallID() == nWall_ID)
				return pPositionList->Object;
		}
		pPositionList = pPositionList->next;
	}
	return nullptr;
}

//*****************************************************************************
//GetFloor関数
//床を探し、戻す
//引数：int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::GetFloor(int nFloor_ID)
{
	if (HeadNode == nullptr)
		return nullptr;
	go_node* pPositionList = HeadNode;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr)
		{
			Floor* ThisFloor = (Floor*)pPositionList->Object;
			if (ThisFloor->GetFloorID() == nFloor_ID)
				return pPositionList->Object;
		}
		pPositionList = pPositionList->next;
	}
	return nullptr;
}

//*****************************************************************************
//AddSpike関数
//リストにスパイクを入れる
//引数：XMFLOAT2, int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddSpike(XMFLOAT2 Pos, int nSpikeNumber)
{
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		go_node* pWorkList = new go_node();
		pWorkList->Object = new Spike(Pos, nSpikeNumber);
		pWorkList->Object->SetObjectInitialPos({ Pos.x, Pos.y });
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		return pWorkList->Object;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new Spike(Pos, nSpikeNumber);
		HeadNode->Object->SetObjectInitialPos({ Pos.x, Pos.y });
		HeadNode->next = nullptr;
		return HeadNode->Object;
	}
}

//*****************************************************************************
//AddSpike関数
//リストにスパイクを入れる
//引数：XMFLOAT2, int, int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddSpike(XMFLOAT2 Pos, int nSpikeNumber, int nID)
{
	GameObject* goObj = AddSpike(Pos, nSpikeNumber);
	goObj->SetNewID(nID);
	return goObj;
}

//*****************************************************************************
//AddSpike関数
//リストにスパイクを入れる
//引数：XMFLOAT2, int, int, XMFLOAT2, XMFLOAT2, XMFLOAT2, int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddSpike(XMFLOAT2 Pos, int nSpikeNumber, int nID, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, int DelayFrames)
{
	GameObject* goObj = AddSpike(Pos, nSpikeNumber, nID);
	goObj->SetUpMoveControl(InitialPos, FinalPos, MovingSpeed);
	goObj->SetDelayFrames(DelayFrames);
	return goObj;
}

//*****************************************************************************
//AddSwitchBulb関数
//リストにスイッチオブジェクトを入れる
//引数：XMFLOAT2, int, bool
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddSwitchBulb(XMFLOAT2 Pos, int nSwitchID, bool bIsPunchable)
{
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		go_node* pWorkList = new go_node();
		pWorkList->Object = new SwitchGO(Pos, nSwitchID, bIsPunchable);
		pWorkList->Object->SetObjectInitialPos({ Pos.x, Pos.y });
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		return pWorkList->Object;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new SwitchGO(Pos, nSwitchID, bIsPunchable);
		HeadNode->Object->SetObjectInitialPos({ Pos.x, Pos.y });
		HeadNode->next = nullptr;
		return HeadNode->Object;
	}
}

//*****************************************************************************
//GetSwitch関数
//スイッチオブジェクトを探し、戻す
//引数：int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::GetSwitch(int nSwitch_ID)
{
	if (HeadNode == nullptr)
		return nullptr;
	go_node* pPositionList = HeadNode;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr)
		{
			if (pPositionList->Object->GetType() == BULB_SWITCH_TYPE) {
				SwitchGO* thisSwitch = (SwitchGO*)pPositionList->Object;
				if (thisSwitch->GetSwitchID() == nSwitch_ID)
					return pPositionList->Object;
			}
		}
		pPositionList = pPositionList->next;
	}
	return nullptr;
}

//*****************************************************************************
//AddTeleporter関数
//リストにテレポートを入れる
//引数：XMFLOAT2, XMFLOAT2
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddTeleporter(XMFLOAT2 TeleporterPos, XMFLOAT2 NewPlayerPos)
{
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		go_node* pWorkList = new go_node();
		pWorkList->Object = new Teleporter(TeleporterPos, NewPlayerPos);
		pWorkList->Object->SetObjectInitialPos({ TeleporterPos.x, TeleporterPos.y });
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		return pWorkList->Object;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new Teleporter(TeleporterPos, NewPlayerPos);
		HeadNode->Object->SetObjectInitialPos({ TeleporterPos.x, TeleporterPos.y });
		HeadNode->next = nullptr;
		return HeadNode->Object;
	}
}

//*****************************************************************************
//AddTeleporter関数
//リストにテレポートを入れる(動ける)
//引数：XMFLOAT2, XMFLOAT2, int, XMFLOAT2, XMFLOAT2, XMFLOAT2, int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddTeleporter(XMFLOAT2 TeleporterPos, XMFLOAT2 NewPlayerPos, int nID, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, int DelayFrames)
{
	Teleporter *obj = (Teleporter*)(AddTeleporter(TeleporterPos, NewPlayerPos));
	obj->SetTeleporterID(nID);
	obj->SetUpMoveControl(InitialPos, FinalPos, MovingSpeed);
	obj->SetDelayFrames(DelayFrames);

	return nullptr;
}

//*****************************************************************************
//AddEndPoint関数
//リストにクリアのテレポートを入れる
//引数：XMFLOAT2
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddEndPoint(XMFLOAT2 TeleporterPos)
{
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		go_node* pWorkList = new go_node();
		pWorkList->Object = new Teleporter(TeleporterPos, { TeleporterPos.x, TeleporterPos.y + 1000000 });
		Teleporter* pTele = (Teleporter*)pWorkList->Object;
		pTele->SetObjectInitialPos({ TeleporterPos.x, TeleporterPos.y });
		pTele->SetTeleporterAsEndpoint();
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		return pWorkList->Object;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new Teleporter(TeleporterPos, { TeleporterPos.x, TeleporterPos.y + 1000000 });
		Teleporter* pTele = (Teleporter*)HeadNode->Object;
		pTele->SetObjectInitialPos({ TeleporterPos.x, TeleporterPos.y });
		pTele->SetTeleporterAsEndpoint();
		HeadNode->next = nullptr;
		return HeadNode->Object;
	}
}

//*****************************************************************************
//AddEndPoint関数
//リストにクリアのテレポートを入れる
//引数：XMFLOAT2, int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddEndPoint(XMFLOAT2 TeleporterPos, int nID)
{
	Teleporter* obj = (Teleporter*)(AddEndPoint(TeleporterPos));
	obj->SetTeleporterID(nID);
	return obj;
}

//*****************************************************************************
//AddEndPoint関数
//リストにクリアのテレポートを入れる(動ける)
//引数：XMFLOAT2, int, XMFLOAT2, XMFLOAT2, XMFLOAT2, int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddEndPoint(XMFLOAT2 TeleporterPos, int nID, XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, XMFLOAT2 MovingSpeed, int DelayFrames)
{
	GameObject* obj = AddEndPoint(TeleporterPos, nID);
	obj->SetUpMoveControl(InitialPos, FinalPos, MovingSpeed);
	obj->SetDelayFrames(DelayFrames);
	return obj;
}

//*****************************************************************************
//AddTeleporter関数
//リストにテレポートを入れる
//引数：XMFLOAT2, XMFLOAT2, int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddTeleporter(XMFLOAT2 TeleporterPos, XMFLOAT2 NewPlayerPos, int nID)
{
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		go_node* pWorkList = new go_node();
		pWorkList->Object = new Teleporter(TeleporterPos, NewPlayerPos);
		pWorkList->Object->SetObjectInitialPos({ TeleporterPos.x, TeleporterPos.y });
		((Teleporter*)(pWorkList->Object))->SetTeleporterID(nID);
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		return pWorkList->Object;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new Teleporter(TeleporterPos, NewPlayerPos);
		HeadNode->Object->SetObjectInitialPos({ TeleporterPos.x, TeleporterPos.y });
		((Teleporter*)(HeadNode->Object))->SetTeleporterID(nID);
		HeadNode->next = nullptr;
		return HeadNode->Object;
	}
}

//*****************************************************************************
//GetTeleporter関数
//テレポートを探し、戻す
//引数：int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::GetTeleporter(int nTeleporter_ID)
{
	if (HeadNode == nullptr)
		return nullptr;
	go_node* pPositionList = HeadNode;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr)
		{
			if (pPositionList->Object->GetType() == TELEPORTER_TYPE) {
				if (((Teleporter*)pPositionList->Object)->GetTeleporterID() == nTeleporter_ID)
					return pPositionList->Object;
			}
		}
		pPositionList = pPositionList->next;
	}
	return nullptr;
}

//*****************************************************************************
//AddPunchStar関数
//リストに星を入れる
//引数：XMFLOAT2, XMFLOAT2
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddPunchStar(XMFLOAT2 InitialPos, XMFLOAT2 FinalPos)
{
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		go_node* pWorkList = new go_node();
		pWorkList->Object = new PunchStar(InitialPos, FinalPos);
		pWorkList->Object->SetObjectInitialPos({ InitialPos.x, InitialPos.y });
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		return pWorkList->Object;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new PunchStar(InitialPos, FinalPos);
		HeadNode->Object->SetObjectInitialPos({ InitialPos.x, InitialPos.y });
		HeadNode->next = nullptr;
		return HeadNode->Object;
	}
}

//*****************************************************************************
//AddPunchStar関数
//リストに星を入れる
//引数：XMFLOAT2, XMFLOAT2, int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::AddPunchStar(XMFLOAT2 InitialPos, XMFLOAT2 FinalPos, int nID)
{
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		go_node* pWorkList = new go_node();
		pWorkList->Object = new PunchStar(InitialPos, FinalPos, nID);
		pWorkList->Object->SetObjectInitialPos({ InitialPos.x, InitialPos.y });
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		return pWorkList->Object;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new PunchStar(InitialPos, FinalPos, nID);
		HeadNode->Object->SetObjectInitialPos({ InitialPos.x, InitialPos.y });
		HeadNode->next = nullptr;
		return HeadNode->Object;
	}
}

//*****************************************************************************
//GetPunchStar関数
//星を探し、戻す
//引数：int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::GetPunchStar(int nStar_ID)
{
	if (HeadNode == nullptr)
		return nullptr;
	go_node* pPositionList = HeadNode;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr)
		{
			if (pPositionList->Object->GetType() == PUNCHABLE_STAR_TYPE) {
				PunchStar* pStar = (PunchStar*)pPositionList->Object;
				if (pStar->GetStarID() == nStar_ID)
					return pPositionList->Object;
			}
		}
		pPositionList = pPositionList->next;
	}
	return nullptr;
}

//*****************************************************************************
//FindWithCollision関数
//リストのオブジェクトとヒットボックスの当たり判定を確認して、
//当たり判定があれば、オブジェクトを戻す
//引数：int
//戻：GameObject*
//*****************************************************************************
GameObject * Go_List::FindWithCollision(HitBox hbox)
{
	if (HeadNode == nullptr)
		return nullptr;
	go_node* pPositionList = HeadNode;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr)
		{
			if (IsInCollision(pPositionList->Object->GetHitbox(), hbox))
				return pPositionList->Object;
		}
		pPositionList = pPositionList->next;
	}
	return nullptr;
}

//*****************************************************************************
//SaveFloors関数
//床のオブジェクトを保存する
//引数：const char*
//戻：void
//*****************************************************************************
void Go_List::SaveFloors(const char* szFilename)
{
	FILE *pFile = nullptr;
	char szFinalfilename[256] = "data/levels/";
	strcat_s(szFinalfilename, szFilename);
	strcat_s(szFinalfilename, ".bin");
	if (strcmp(szFilename, "") == 0)
	{
		strcpy_s(szFinalfilename, "Default.bin");
	}
	fopen_s(&pFile, szFinalfilename, "wb");
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	while (true) {

		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr)
		{
			FloorContainer gameObject;
			Floor* thisObject = (Floor*)pPositionList->Object;
			XMFLOAT3 Size = thisObject->GetSize();

			gameObject.FloorTexture = thisObject->GetFloorTexture();
			gameObject.hbox = thisObject->GetHitBoxWithoutPosition();
			gameObject.FloorTileSize = (int)thisObject->GetTileSizeX();
			gameObject.Size = { Size.x, Size.y };
			gameObject.Floor_ID = thisObject->GetFloorID();
			gameObject.Pos = thisObject->GetObjectInitialPos();
			gameObject.InitialPos = thisObject->GetMovingInitialPos();
			gameObject.FinalPos = thisObject->GetMovingFinalPos();
			gameObject.MovingSpeed = thisObject->GetMovingSpeed();
			gameObject.nObjMoveable = STATIC_OBJECT;
			GameObject* pSwitch = thisObject->GetResetSwitchForMoveablePlatform();
			if (thisObject->IsMoveable()) {
				gameObject.nObjMoveable = MOVEABLE_OBJECT;
				gameObject.nDelayFrames = thisObject->GetMovementDelayFrames();

			}
			if (pSwitch)
			{
				gameObject.nObjMoveable = MOVEABLE_BY_SWITCH_OBJECT;
				XMFLOAT3 switchPos = thisObject->GetResetSwitchForMoveablePlatform()->GetLocalPosition();
				gameObject.ResetSwitchPosition = { switchPos.x, switchPos.y };
			}
			else {
				gameObject.ResetSwitchPosition = { 0,0 };
			}
			if(pFile)
				fwrite(&gameObject, sizeof(FloorContainer), 1, pFile);
		}
		pPositionList = pPositionList->next;
	}
	printf("SAVED OK: %s\n", szFinalfilename);
	if (pFile) 
	{
		fclose(pFile);
	}
}

//*****************************************************************************
//SaveWalls関数
//壁のオブジェクトを保存する
//引数：const char*
//戻：void
//*****************************************************************************
void Go_List::SaveWalls(const char * szFilename)
{
	FILE *pFile = nullptr;
	char szFinalfilename[256] = "data/levels/";
	strcat_s(szFinalfilename, szFilename);
	strcat_s(szFinalfilename, ".bin");
	if (strcmp(szFilename, "") == 0)
	{
		strcpy_s(szFinalfilename, "Default.bin");
	}
	fopen_s(&pFile, szFinalfilename, "wb");
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	while (true) {

		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr)
		{
			WallContainer gameObject;
			Wall* thisObject = (Wall*)pPositionList->Object;
			XMFLOAT3 Size = thisObject->GetSize();

			gameObject.TextureID = thisObject->GetWallTexture();
			gameObject.hbox = thisObject->GetHitBoxWithoutPosition();
			gameObject.WallTileSize = (int)thisObject->GetTileSizeY();
			gameObject.Pos = thisObject->GetObjectInitialPos();
			gameObject.Size = { Size.x, Size.y };
			gameObject.n_WallID = thisObject->GetWallID();
			gameObject.MoveableObject = thisObject->IsMoveable();
			gameObject.InitialPos = thisObject->GetMovingInitialPos();
			gameObject.FinalPos = thisObject->GetMovingFinalPos();
			gameObject.MovingSpeed = thisObject->GetMovingSpeed();
			gameObject.DelayFrames = thisObject->GetMovementDelayFrames();
			gameObject.ActivationTypeWall = thisObject->GetTypeScaleActivation();
			if (pFile)
				fwrite(&gameObject, sizeof(WallContainer), 1, pFile);
		}
		pPositionList = pPositionList->next;
	}
	printf("SAVED OK: %s\n", szFinalfilename);
	if (pFile)
		fclose(pFile);
}

//*****************************************************************************
//SaveSpikes関数
//スパイクのオブジェクトを保存する
//引数：const char*
//戻：void
//*****************************************************************************
void Go_List::SaveSpikes(const char * szFilename)
{
	FILE *pFile;
	char szFinalfilename[256] = "data/levels/";
	strcat_s(szFinalfilename, szFilename);
	strcat_s(szFinalfilename, ".bin");
	if (strcmp(szFilename, "") == 0)
	{
		strcpy_s(szFinalfilename, "Default.bin");
	}
	fopen_s(&pFile, szFinalfilename, "wb");
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	while (true) {

		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr && pPositionList->Object->GetType() == SPIKE_TYPE)
		{
			SpikeContainer gameObject;
			Spike* thisObject = (Spike*)pPositionList->Object;

			gameObject.Pos = thisObject->GetObjectInitialPos();
			gameObject.nSpikeNumber = thisObject->GetSpikeNumber();
			gameObject.Rotation = thisObject->GetRotation();
			gameObject.InitialPos = thisObject->GetMovingInitialPos();
			gameObject.FinalPos = thisObject->GetMovingFinalPos();
			gameObject.MovingSpeed = thisObject->GetMovingSpeed();
			gameObject.DelayFrames = thisObject->GetMovementDelayFrames();
			gameObject.MoveableObject = thisObject->IsMoveable();

			gameObject.nID = thisObject->GetGameObjectID();
			if (pFile)
				fwrite(&gameObject, sizeof(SpikeContainer), 1, pFile);
		}
		pPositionList = pPositionList->next;
	}
	printf("SAVED OK: %s\n", szFinalfilename);
	if (pFile)
		fclose(pFile);
}

//*****************************************************************************
//GetNumberOfObjects関数
//オブジェクトのカウンターを戻す
//引数：void
//戻：int
//*****************************************************************************
int Go_List::GetNumberOfObjects()
{
	if (HeadNode == nullptr)
		return 0;
	int nCount = 0;
	go_node* pPositionList = HeadNode;
	while (true) {
		if (pPositionList->next == nullptr)
			break;
		pPositionList = pPositionList->next;
		nCount++;
	}
	return nCount;
}

//*****************************************************************************
//DeleteLastPosObject関数
//最後の入れたオブジェクトを消す
//引数：void
//戻：void
//*****************************************************************************
void Go_List::DeleteLastPosObject()
{
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode->next;
	go_node* PreviousPos = HeadNode;
	if (pPositionList == nullptr)
	{
		if (PreviousPos->Object) {
			delete(PreviousPos->Object);
			PreviousPos->Object = nullptr;
			delete(PreviousPos);
			HeadNode = PreviousPos = nullptr;
			return;
		}
	}
	while (true) {
		if (pPositionList && pPositionList->next == nullptr)
			break;
		if(pPositionList)
			pPositionList = pPositionList->next;
		if(PreviousPos)
			PreviousPos = PreviousPos->next;
	}
	if (pPositionList->Object) {
		delete(pPositionList->Object);
		pPositionList->Object = nullptr;
		delete(pPositionList);
		pPositionList = nullptr;
		PreviousPos->next = nullptr;
	}
	return;
}

//*****************************************************************************
//SaveGems関数
//ジェムのオブジェクトを保存する
//引数：const char*
//戻：void
//*****************************************************************************
void Go_List::SaveGems(const char* szFilename)
{
	FILE *pFile = nullptr;
	char szFinalfilename[256] = "data/levels/";
	strcat_s(szFinalfilename, szFilename);
	strcat_s(szFinalfilename, ".bin");
	if (strcmp(szFilename, "") == 0)
	{
		strcpy_s(szFinalfilename, "Default.bin");
	}
	fopen_s(&pFile, szFinalfilename, "wb");
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	while (true) {

		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr && pPositionList->Object->GetType() == GEM_TYPE)
		{
			PickItemContainer gameObject;
			GameObject* thisObject = pPositionList->Object;

			gameObject.Position = thisObject->GetObjectInitialPos();
			gameObject.MoveableObject = thisObject->IsMoveable();
			gameObject.InitialPos = thisObject->GetMovingInitialPos();
			gameObject.FinalPos = thisObject->GetMovingFinalPos();
			gameObject.MovingSpeed = thisObject->GetMovingSpeed();
			gameObject.DelayFrames = thisObject->GetMovementDelayFrames();
			gameObject.nID = thisObject->GetGameObjectID();

			if (pFile)
				fwrite(&gameObject, sizeof(PickItemContainer), 1, pFile);
		}
		pPositionList = pPositionList->next;
	}
	printf("SAVED OK: %s\n", szFinalfilename);
	if (pFile)
		fclose(pFile);
}

//*****************************************************************************
//SaveTeleporters関数
//テレポートのオブジェクトを保存する
//引数：const char*
//戻：void
//*****************************************************************************
void Go_List::SaveTeleporters(const char * szFilename)
{
	FILE *pFile = nullptr;
	char szFinalfilename[256] = "data/levels/";
	strcat_s(szFinalfilename, szFilename);
	strcat_s(szFinalfilename, ".bin");
	if (strcmp(szFilename, "") == 0)
	{
		strcpy_s(szFinalfilename, "Default.bin");
	}
	fopen_s(&pFile, szFinalfilename, "wb");
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	while (true) {

		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr && pPositionList->Object->GetType() == TELEPORTER_TYPE)
		{
			TeleporterContainer gameObject;
			Teleporter* thisObject = ((Teleporter*)(pPositionList->Object));

			gameObject.Position = thisObject->GetObjectInitialPos();
			gameObject.Destination = thisObject->GetTeleporterDestination();
			gameObject.bIsEndPoint = thisObject->IsEndPoint();
			gameObject.MoveableObject = thisObject->IsMoveable();
			gameObject.InitialPos = thisObject->GetMovingInitialPos();
			gameObject.FinalPos = thisObject->GetMovingFinalPos();
			gameObject.MovingSpeed = thisObject->GetMovingSpeed();
			gameObject.DelayFrames = thisObject->GetMovementDelayFrames();
			gameObject.nID = thisObject->GetGameObjectID();

			if (pFile)
				fwrite(&gameObject, sizeof(TeleporterContainer), 1, pFile);
		}
		pPositionList = pPositionList->next;
	}
	printf("SAVED OK: %s\n", szFinalfilename);
	if (pFile)
		fclose(pFile);
}

//*****************************************************************************
//SaveRecoveryPicks関数
//回復アイテムのオブジェクトを保存する
//引数：const char*
//戻：void
//*****************************************************************************
void Go_List::SaveRecoveryPicks(const char * szFilename)
{
	FILE *pFile = nullptr;
	char szFinalfilename[256] = "data/levels/";
	strcat_s(szFinalfilename, szFilename);
	strcat_s(szFinalfilename, ".bin");
	if (strcmp(szFilename, "") == 0)
	{
		strcpy_s(szFinalfilename, "Default.bin");
	}
	fopen_s(&pFile, szFinalfilename, "wb");
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	while (true) {

		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr && pPositionList->Object->GetType() == RECOVER_TYPE)
		{
			PickItemContainer gameObject;
			GameObject* thisObject = pPositionList->Object;

			gameObject.Position = thisObject->GetObjectInitialPos();
			gameObject.MoveableObject = thisObject->IsMoveable();
			gameObject.InitialPos = thisObject->GetMovingInitialPos();
			gameObject.FinalPos = thisObject->GetMovingFinalPos();
			gameObject.MovingSpeed = thisObject->GetMovingSpeed();
			gameObject.DelayFrames = thisObject->GetMovementDelayFrames();
			gameObject.nID = thisObject->GetGameObjectID();

			if(pFile)
				fwrite(&gameObject, sizeof(PickItemContainer), 1, pFile);
		}
		pPositionList = pPositionList->next;
	}
	printf("SAVED OK: %s\n", szFinalfilename);
	if (pFile)
		fclose(pFile);
}

//*****************************************************************************
//SavePunchStars関数
//星のオブジェクトを保存する
//引数：const char*
//戻：void
//*****************************************************************************
void Go_List::SavePunchStars(const char * szFilename)
{
	FILE *pFile;
	char szFinalfilename[256] = "data/levels/";
	strcat_s(szFinalfilename, szFilename);
	strcat_s(szFinalfilename, ".bin");
	if (strcmp(szFilename, "") == 0)
	{
		strcpy_s(szFinalfilename, "Default.bin");
	}
	fopen_s(&pFile, szFinalfilename, "wb");
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	while (true) {

		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr && pPositionList->Object->GetType() == PUNCHABLE_STAR_TYPE)
		{
			StarContainer gameObject;
			PunchStar* thisObject = (PunchStar*)pPositionList->Object;

			gameObject.Position = thisObject->GetStarInitialPos();
			gameObject.Destination = thisObject->GetStarFinalPos();
			gameObject.nID = thisObject->GetStarID();

			if(pFile)
				fwrite(&gameObject, sizeof(StarContainer), 1, pFile);
		}
		pPositionList = pPositionList->next;
	}
	printf("SAVED OK: %s\n", szFinalfilename);
	if(pFile)
		fclose(pFile);
}

//*****************************************************************************
//Load関数
//ファイルから保存したオブジェクトをロードして、リストに入れる。
//引数：const char*, int
//戻：void
//*****************************************************************************
void Go_List::Load(const char * szFilename, int nType)
{
	FILE *pFile;
	char szFinalfilename[256] = "data/Levels/";
	strcat_s(szFinalfilename, szFilename);
	strcat_s(szFinalfilename, ".bin");
	fopen_s(&pFile, szFinalfilename, "rb");
	if (pFile == NULL)
	{
#ifdef USE_CONSOLE
		printf_s("%sのファイルはない…\n", szFinalfilename);
#endif
		return;
	}
	FloorContainer* floor_container = new FloorContainer();
	WallContainer*  wall_container = new WallContainer();
	SpikeContainer* spike_container = new SpikeContainer();
	PickItemContainer* pick_container = new PickItemContainer();
	TeleporterContainer* tel_container = new TeleporterContainer();
	StarContainer* star_container = new StarContainer();

	GameObject* go = nullptr;
	switch (nType)
	{
	case FLOOR_TYPE:
		while ((fread(floor_container, sizeof(FloorContainer), 1, pFile)))
		{
			if (floor_container->nObjMoveable == STATIC_OBJECT)
				AddFloor(floor_container->FloorTexture, floor_container->hbox, floor_container->FloorTileSize, floor_container->Pos, floor_container->Size, floor_container->Floor_ID);
			else if (floor_container->nObjMoveable == MOVEABLE_OBJECT)
				AddFloor(floor_container->FloorTexture, floor_container->hbox, floor_container->FloorTileSize, floor_container->Pos, floor_container->Size, floor_container->Floor_ID, floor_container->InitialPos, floor_container->FinalPos, floor_container->MovingSpeed, floor_container->nDelayFrames);
			else if (floor_container->nObjMoveable == MOVEABLE_BY_SWITCH_OBJECT)
				AddFloor(floor_container->FloorTexture, floor_container->hbox, floor_container->FloorTileSize, floor_container->Pos, floor_container->Size, floor_container->Floor_ID, floor_container->InitialPos, floor_container->FinalPos, floor_container->MovingSpeed, floor_container->ResetSwitchPosition);
		}
		break;
	case WALL_TYPE:
		while ((fread(wall_container, sizeof(WallContainer), 1, pFile))) {
			if (!wall_container->MoveableObject)
				AddWall(wall_container->TextureID, wall_container->hbox, wall_container->WallTileSize, wall_container->Pos, wall_container->Size, wall_container->n_WallID, wall_container->ActivationTypeWall);
			else
				AddWall(wall_container->TextureID, wall_container->hbox, wall_container->WallTileSize, wall_container->Pos, wall_container->Size, wall_container->n_WallID, wall_container->ActivationTypeWall, wall_container->InitialPos, wall_container->FinalPos, wall_container->MovingSpeed, wall_container->DelayFrames);
		}
		break;
	case SPIKE_TYPE:
		while ((fread(spike_container, sizeof(SpikeContainer), 1, pFile)))
		{
			if (!spike_container->MoveableObject)
				AddSpike(spike_container->Pos, spike_container->nSpikeNumber, spike_container->nID);
			else
				AddSpike(spike_container->Pos, spike_container->nSpikeNumber, spike_container->nID, spike_container->InitialPos, spike_container->FinalPos, spike_container->MovingSpeed, spike_container->DelayFrames);
		}
		break;
	case RECOVER_TYPE:
		while ((fread(pick_container, sizeof(PickItemContainer), 1, pFile)))
		{
			if (!pick_container->MoveableObject) {
				go = AddRecoveryPickable(pick_container->Position);
				go->SetNewID(pick_container->nID);
			}
			else
				AddRecoveryPickable(pick_container->Position, pick_container->nID, pick_container->InitialPos, pick_container->FinalPos, pick_container->MovingSpeed, pick_container->DelayFrames);
		}
		break;
	case GEM_TYPE:
		while ((fread(pick_container, sizeof(PickItemContainer), 1, pFile)))
		{
			if (!pick_container->MoveableObject) {
				go = AddGemPickable(pick_container->Position);
				go->SetNewID(pick_container->nID);
			}
			else
				AddGemPickable(pick_container->Position, pick_container->nID, pick_container->InitialPos, pick_container->FinalPos, pick_container->MovingSpeed, pick_container->DelayFrames);
		}
		break;
	case TELEPORTER_TYPE:
		while ((fread(tel_container, sizeof(TeleporterContainer), 1, pFile)))
		{
			if (tel_container->bIsEndPoint)
			{
				if (!tel_container->MoveableObject) {
					go = AddEndPoint(tel_container->Position);
					((Teleporter*)go)->SetTeleporterID(tel_container->nID);
				}
				else
					AddEndPoint(tel_container->Position, tel_container->nID, tel_container->InitialPos, tel_container->FinalPos, tel_container->MovingSpeed, tel_container->DelayFrames);
			}
			else {
				if (!tel_container->MoveableObject) {
					go = AddTeleporter(tel_container->Position, tel_container->Destination);
					((Teleporter*)go)->SetTeleporterID(tel_container->nID);
				}
				else
					AddTeleporter(tel_container->Position, tel_container->Destination, tel_container->nID, tel_container->InitialPos, tel_container->FinalPos, tel_container->MovingSpeed, tel_container->DelayFrames);
			}
		}
		break;
	case PUNCHABLE_STAR_TYPE:
		while ((fread(star_container, sizeof(StarContainer), 1, pFile)))
			AddPunchStar(star_container->Position, star_container->Destination, star_container->nID);
		break;
	default:
		break;
	}
#ifdef USE_CONSOLE
	printf("LOADED: %s\n", szFinalfilename);
#endif
	SAFE_DELETE(floor_container);
	SAFE_DELETE(wall_container);
	SAFE_DELETE(spike_container);
	SAFE_DELETE(pick_container);
	SAFE_DELETE(tel_container);
	fclose(pFile);
}

//*****************************************************************************
//Delete関数
//オブジェクトのアドレスから、リストに探し、消します
//引数：GameObject*
//戻：void
//*****************************************************************************
void Go_List::Delete(GameObject * pSearch)
{
	if (HeadNode == nullptr)
		return;
	if (HeadNode->Object == pSearch)
	{
		go_node* pPositionList = HeadNode;
		HeadNode = HeadNode->next;
		delete(pPositionList->Object);
		pPositionList->Object = nullptr;
		delete(pPositionList);
		pPositionList = nullptr;
		return;
	}
	if (HeadNode->next->Object == pSearch)
	{
		go_node* pPositionList = HeadNode->next;
		HeadNode->next = HeadNode->next->next;
		delete(pPositionList->Object);
		pPositionList->Object = nullptr;
		delete(pPositionList);
		pPositionList = nullptr;
		return;
	}
	go_node* pPositionList = HeadNode->next;
	go_node* PreviousPos = HeadNode;
	if (pPositionList == nullptr)
		return;
	while (true) {
		if (pPositionList->next == nullptr)
			return;
		if (pPositionList->Object == pSearch)
			break;
		pPositionList = pPositionList->next;
		PreviousPos = PreviousPos->next;
	}
	if (pPositionList->Object) {
		PreviousPos->next = pPositionList->next;
		delete(pPositionList->Object);
		pPositionList->Object = nullptr;
		delete(pPositionList);
		pPositionList = nullptr;
	}
	return;
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void Go_List::Update()
{
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr)
			pPositionList->Object->Update();
		pPositionList = pPositionList->next;
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Go_List::Draw()
{
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	while (true) {

		if (pPositionList == nullptr)
			break;

		if (pPositionList->Object != nullptr)
			pPositionList->Object->Draw();
		pPositionList = pPositionList->next;
	}
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Go_List::End()
{
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	go_node* pWork = nullptr;
	int Count = 0;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr) {
			Count++;
			SAFE_DELETE(pPositionList->Object);
		}
		pWork = pPositionList;
		pPositionList = pPositionList->next;
		if (pWork)
			SAFE_DELETE(pWork);
	}
	printf("GameObjects Deleted: %d\n", Count);
	HeadNode = nullptr;
}



