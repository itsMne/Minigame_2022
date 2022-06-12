//*****************************************************************************
// Sprite.h
// Author : Mane
//*****************************************************************************
#ifndef	 SPRITE_H
#define  SPRITE_H

#include "C_DXWindow.h"
#include "UniversalVariables.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define  MAX_SPRITE_CHILDS 100

//*****************************************************************************
// エナム定義
//*****************************************************************************
enum Direction
{
	RIGHT_DIR = 1,
	LEFT_DIR = -1
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct SHADER_GLOBAL {
	XMMATRIX	mWorld;		// ワールド変換行列(転置行列)
	XMMATRIX	mView;		// ビュー変換行列(転置行列)
	XMMATRIX	mProj;		// 射影変換行列(転置行列)
	XMMATRIX	mTex;		// テクスチャ変換行列(転置行列)
};

struct HitBox
{
	float x=0, y=0, w=0, h=0;
	bool  print = false;
	void* Sprite = nullptr;
};

//*****************************************************************************
// クラス
//*****************************************************************************
class Sprite
{
private:
	Sprite						*HitBoxSprite;
	bool						bIsHitbox;
protected:
	bool						bSpriteInited;
	bool						g_bInvalidate;			// 頂点データ更新フラグ
	VERTEX_2D					g_vertexWk[NUM_VERTEX];	// 頂点情報格納ワーク
	XMFLOAT3					Position;				// ポリゴンの移動量
	XMFLOAT2					PositionOffset;			// 拠点のオフセット
	XMFLOAT3					Rotation;				// ポリゴンの回転量
	XMFLOAT3					Size;					// ポリゴンのサイズ
	XMFLOAT4					color;					// ポリゴンの頂点カラー
	XMFLOAT2					g_posTexFrame;			// UV座標
	XMFLOAT2					g_sizTexFrame;			// テクスチャ抽出サイズ
	XMFLOAT4X4					Projection;				// 射影変換行列
	XMFLOAT4X4					g_mView;				// ビュー変換行列
	XMFLOAT4X4					g_mWorld;				// ワールド変換行列
	XMFLOAT4X4					g_mTex;					// テクスチャ変換行列
	ID3D11Buffer*				gpConstantBuffer;		// 定数バッファ
	ID3D11Buffer*				g_pVertexBuffer;		// 頂点バッファ
	ID3D11SamplerState*			g_pSamplerState;		// テクスチャ サンプラ
	ID3D11BlendState*			gpBlendState;			// ブレンド ステート (加算合成用)
	ID3D11VertexShader*			g_pVertexShader;		// 頂点シェーダ
	ID3D11InputLayout*			g_pInputLayout;			// 頂点フォーマット
	ID3D11PixelShader*			g_pPixelShader;			// ピクセルシェーダ]
	HitBox						hitbox;					// ヒットボックス
	bool						bDontDependOnCamera;	// カメラで頼るBool型
	bool						bSetUpNoUIFilter;		// フィルターの設定の変数
	bool						bTextureNotInTextureManager;
	D3D11_FILTER				d3d11CurrentFilter;		// 使っているフィルター
	Sprite*						Parent;					// 親の先頭アドレス
	Sprite*						Childs[MAX_SPRITE_CHILDS];					// 親の先頭アドレス
	int							nPolygonID;
#ifdef USE_CONSOLE
	char TextureName[256];
#endif
public:
	ID3D11ShaderResourceView    *gpTexture;				// テクスチャ

	Sprite();
	Sprite(bool bHitbox);
	virtual ~Sprite();
	HRESULT  InitSprite();
	HRESULT  InitSprite(ID3D11ShaderResourceView* pTexture);
	HRESULT  InitSprite(const char * texturepath);
	void	 UninitSprite();
	void	 UpdateSprite();
	void	 DrawSprite(ID3D11DeviceContext* pDeviceContext);
	void	 DrawSpriteUI(ID3D11DeviceContext* pDeviceContext);
	void	 DrawSpriteHB(ID3D11DeviceContext* pDeviceContext);
	void	 SetTexture(ID3D11ShaderResourceView* pTexture);
	void	 SetLocalPosition(float fX, float fY);
	void	 SetWorldPosition(float fX, float fY);
	void	 SetWorldPosition(XMFLOAT3 newSize);
	void	 SetSpriteSize(float fScaleX, float fScaleY);
	void	 SetSpriteSize(XMFLOAT3 newSize);
	void	 SetSpriteSize(XMFLOAT2 newSize);
	void	 SizeUp(XMFLOAT2 newSize);
	void	 SetSpriteAngle(float fAngle);
	void	 SetSpriteUV(float fU, float fV);
	void	 SetSpriteFrameSize(float fWidth, float fHeight);
	void	 SetColor(float fRed, float fGreen, float fBlue);
	void	 SetColorB(float fRed);
	void	 SetAlpha(float fAlpha);
	void	 RiseAlpha(float fAlpha);
	void	 SetTexture(const char* texturepath);
	void	 SetRotation(XMFLOAT3 newRot);
	void	 RotateAroundZ(float rot);
	void	 RotateAroundY(float rot);
	void	 SetParent(Sprite* pParent);
	void	 SetParent(Sprite* pParent, bool KeepWorldPosition);
	bool	 SetChild(Sprite* Child);
	void	 SetNewFilter(D3D11_FILTER newFilter);
	Sprite*  GetParent();
	void	 RemoveParent();
	void	 RemoveChild(Sprite* Child);
	void	 RemoveParent(bool KeepWorldPosition);
	XMFLOAT3 GetLocalPosition();
	XMFLOAT3 GetWorldPosition();
	XMFLOAT3 GetSize();
	XMFLOAT3 GetRotation();
	XMFLOAT4 GetColor();
	float	 GetAlpha();
	void	 Translate(float x, float y);
	HRESULT	 MakeVertexSprite(ID3D11Device* pDevice);
	void	 SetVertexSprite(void);
	HitBox	 GetHitbox();
	HitBox	 GetDimensionHitbox();
	bool	 IsSpriteInited();
	void	 SetDependOnCamera(bool depend);
	void	 IncreaseSizeWithHitbox(float incX, float incY);
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void  SetDevice(ID3D11Device * n_pDevice);
void  SetHR(HRESULT* nhr);
bool  IsInCollision(HitBox hbA, HitBox hbB);
void  SetUniversalScale(float newScale);
void  ReduceUniversalScale(float redScale);
void  IncreaseUniversalScale(float upScale);
void  ResetUniversalScale();
void  SetNewGeneralFilter(D3D11_FILTER newFilter);
ID3D11ShaderResourceView* GetHitboxTexture();
float GetUniversalScale();
ID3D11ShaderResourceView *CreateTexture(const char* texturepath, ID3D11ShaderResourceView * ngpTexture);
int   GetSpriteNumInMemory();
void ReduceDebugPolygonCounter(int);
#endif