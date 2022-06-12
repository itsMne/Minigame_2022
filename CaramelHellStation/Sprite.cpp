//*****************************************************************************
// Sprite.cpp
// スプライトレンダリングの管理
// Author : Mane
//*****************************************************************************
#include "Sprite.h"
#include "Shader.h"
#include "TextureManager.h"
#include "UniversalVariables.h"
#include "Camera.h"
#include "Texture.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define FILTER_ONLY_WHEN_ZOOM_CHANGE true

//*****************************************************************************
// グローバル変数
//*****************************************************************************
ID3D11Device				*pDevice;
HRESULT						*g_hr;
float						fUniversalScale = COMMON_UNIVERSAL_SCALE;
int							nSpriteNum = 0;
D3D11_FILTER				d3d11GeneralFilter = D3D11_FILTER_ANISOTROPIC;

//No Filter -> D3D11_FILTER_MIN_MAG_MIP_POINT
//Linear Filter -> D3D11_FILTER_MIN_MAG_MIP_LINEAR

//*****************************************************************************
//コンストラクタ
//*****************************************************************************
Sprite::Sprite()
{
	bSpriteInited = false;
	g_bInvalidate = false;
	g_mTex = XMFLOAT4X4();
	g_mView = XMFLOAT4X4();
	g_mWorld = XMFLOAT4X4();
	Position = { 0,0,0 };
	Rotation = { 0,0,0 };
	PositionOffset = { 0,0 };
	g_posTexFrame = { 0,0 };
	g_sizTexFrame = { 0,0 };
	color = { 0,0,0,0 };
	d3d11CurrentFilter = d3d11GeneralFilter;
	HitBoxSprite = nullptr;
	bDontDependOnCamera = false;
	bIsHitbox = false;
	bSetUpNoUIFilter = false;
	Parent = nullptr;
	gpTexture = nullptr;
	bTextureNotInTextureManager = false;
	gpConstantBuffer = nullptr;
	g_pVertexBuffer = nullptr;
	g_pSamplerState = nullptr;
	gpBlendState = nullptr;
	g_pVertexShader = nullptr;
	g_pInputLayout = nullptr;
	g_pPixelShader = nullptr;
	nSpriteNum++;
	nPolygonID = nSpriteNum;
	Projection = XMFLOAT4X4();
	Size = { 1,1,1 };
	for (int i = 0; i < MAX_SPRITE_CHILDS; i++)
		Childs[i] = nullptr;
}

Sprite::Sprite(bool bHitbox)
{
	bSpriteInited = false;
	g_bInvalidate = false;
	g_mTex = XMFLOAT4X4();
	g_mView = XMFLOAT4X4();
	g_mWorld = XMFLOAT4X4();
	Position = { 0,0,0 };
	Rotation = { 0,0,0 };
	PositionOffset = { 0,0 };
	g_posTexFrame = { 0,0 };
	g_sizTexFrame = { 0,0 };
	color = { 0,0,0,0 };
	d3d11CurrentFilter = d3d11GeneralFilter;
	bSetUpNoUIFilter = false;
	HitBoxSprite = nullptr;
	g_pVertexShader = nullptr;
	bTextureNotInTextureManager = false;
	bDontDependOnCamera = false;
	bIsHitbox = bHitbox;
	Parent = nullptr;
	gpTexture = nullptr;
	bTextureNotInTextureManager = false;
	gpConstantBuffer = nullptr;
	g_pVertexBuffer = nullptr;
	g_pSamplerState = nullptr;
	gpBlendState = nullptr;
	g_pVertexShader = nullptr;
	g_pInputLayout = nullptr;
	g_pPixelShader = nullptr;
	nSpriteNum++;
	nPolygonID = nSpriteNum;
	Projection = XMFLOAT4X4();
	Size = { 1,1,1 };
	for (int i = 0; i < MAX_SPRITE_CHILDS; i++)
		Childs[i] = nullptr;
}


Sprite::~Sprite()
{
	UninitSprite();
	nSpriteNum--;
}

//*****************************************************************************
//InitSprite関数
//初期化処理関数
//引数：void
//戻：HRESULT
//*****************************************************************************

HRESULT Sprite::InitSprite()
{
	if (bSpriteInited)
		return S_OK;
	Parent = nullptr;
	HRESULT hr = S_OK;
	bSpriteInited = true;
	// シェーダ初期化
	static const D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = LoadShader(MAKEINTRESOURCEW(IDR_VERTEXSHADER), MAKEINTRESOURCEW(IDR_PIXELSHADER),
		&g_pVertexShader, &g_pInputLayout, &g_pPixelShader, layout, _countof(layout));
	if (FAILED(hr)) {
		return hr;
	}

	// 定数バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SHADER_GLOBAL);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pDevice->CreateBuffer(&bd, nullptr, &gpConstantBuffer);
	if (FAILED(hr)) return hr;

	// テクスチャ サンプラ生成
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = d3d11GeneralFilter;
	d3d11CurrentFilter = d3d11GeneralFilter;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = pDevice->CreateSamplerState(&sd, &g_pSamplerState);
	if (FAILED(hr)) {
		return hr;
	}

	// ブレンド ステート生成
	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(BlendDesc));
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	hr = pDevice->CreateBlendState(&BlendDesc, &gpBlendState);
	if (FAILED(hr)) {
		return hr;
	}

	// 変換行列初期化
	XMStoreFloat4x4(&g_mWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&g_mView, XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
	XMStoreFloat4x4(&Projection,
		XMMatrixOrthographicLH(WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f));
	XMStoreFloat4x4(&g_mTex, XMMatrixIdentity());
	g_mTex._44 = 0.0f;

	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Size = XMFLOAT3(1.0f, 1.0f, 1.0f);
	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_bInvalidate = false;

	g_posTexFrame = XMFLOAT2(0.0f, 0.0f);
	g_sizTexFrame = XMFLOAT2(1.0f, 1.0f);

	// 頂点情報の作成
	hr = MakeVertexSprite(pDevice);

	g_hr = &hr;
#if SHOW_HITBOXES
	if (!bIsHitbox) {
		HitBoxSprite = new Sprite(true);
		HitBoxSprite->InitSprite();
		HitBoxSprite->gpTexture = GetTexture(TEXTURE_HITBOX);
		HitBoxSprite->SetAlpha(0.5f);
	}
#endif
	PositionOffset = { 0,0 };
	return hr;
}

//*****************************************************************************
//InitSprite関数
//初期化処理関数
//引数：ID3D11ShaderResourceView*
//戻：HRESULT
//*****************************************************************************
HRESULT Sprite::InitSprite(ID3D11ShaderResourceView* pTexture)
{
	HRESULT hr = InitSprite();
	SetTexture(pTexture);
	return hr;
}

//*****************************************************************************
//InitSprite関数
//初期化処理関数
//引数：const char*
//戻：HRESULT
//*****************************************************************************
HRESULT Sprite::InitSprite(const char* texturepath)
{
#ifdef USE_CONSOLE
	printf("SP: 作ったテクスチャ： %s\n", texturepath);
	strcpy(TextureName, texturepath);
#endif
	HRESULT hr = InitSprite();
	SetTexture(texturepath);
	bTextureNotInTextureManager = true;
	return hr;
}

//*****************************************************************************
//UninitSprite関数
//終了処理関数
//引数：void
//戻：void
//*****************************************************************************
void Sprite::UninitSprite()
{
	for (int i = 0; i < MAX_SPRITE_CHILDS; i++)
	{
		if (!Childs[i])
			continue;
		Childs[i]->RemoveParent();
		Childs[i] = nullptr;
	}
	// ブレンド ステート解放
	SAFE_RELEASE(gpBlendState);
	// テクスチャ サンプラの開放
	SAFE_RELEASE(g_pSamplerState);
	// 頂点バッファの解放
	SAFE_RELEASE(g_pVertexBuffer);
	// 定数バッファの解放
	SAFE_RELEASE(gpConstantBuffer);
	// ピクセルシェーダ解放
	SAFE_RELEASE(g_pPixelShader);
	// 頂点フォーマット解放
	SAFE_RELEASE(g_pInputLayout);
	// 頂点シェーダ解放
	//SAFE_RELEASE(g_pVertexShader);

	SAFE_DELETE(HitBoxSprite);
	if (bTextureNotInTextureManager)
	{
#ifdef USE_CONSOLE
		printf("SP: 消した: %s\n", TextureName);
#endif
		SAFE_RELEASE(gpTexture);
	}
	bSpriteInited = false;
}

//*****************************************************************************
//UpdateSprite関数
//更新処理
//引数：void
//戻：void
//*****************************************************************************
void Sprite::UpdateSprite()
{
	//ない
}

//*****************************************************************************
//DrawSprite関数
//描画処理
//引数：ID3D11DeviceContext*
//戻：void
//*****************************************************************************
void Sprite::DrawSprite(ID3D11DeviceContext* pDeviceContext)
{
	if (!bSpriteInited)
		return;
	if (d3d11CurrentFilter != d3d11GeneralFilter) {
		if (!FILTER_ONLY_WHEN_ZOOM_CHANGE) {
			D3D11_SAMPLER_DESC sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.Filter = d3d11GeneralFilter;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			pDevice->CreateSamplerState(&sd, &g_pSamplerState);
		}
		d3d11CurrentFilter = d3d11GeneralFilter;
#ifdef USE_CONSOLE
		printf("フィルターを変えた\n");
#endif
	}
	float Offset = GetZoomOffset();
	if (d3d11GeneralFilter != D3D11_FILTER_MIN_MAG_MIP_POINT && FILTER_ONLY_WHEN_ZOOM_CHANGE)
	{
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		if (GetUniversalScale()+ Offset != 1)
			sd.Filter = d3d11GeneralFilter;
		else
			sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		pDevice->CreateSamplerState(&sd, &g_pSamplerState);
	}

	// 拡縮
	XMMATRIX mWorld = XMMatrixScaling(Size.x*fUniversalScale * Offset, Size.y*fUniversalScale * Offset, Size.z*fUniversalScale * Offset);
	// 回転
	mWorld *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotation.x),
		XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	// 移動
	XMFLOAT3 CameraPos = GetCameraPos();
	XMFLOAT3 ObjectPos = GetWorldPosition();
	if (bDontDependOnCamera)
		mWorld *= XMMatrixTranslation(ObjectPos.x*fUniversalScale * Offset, ObjectPos.y*fUniversalScale * Offset, ObjectPos.z*fUniversalScale * Offset);
	else
		mWorld *= XMMatrixTranslation((ObjectPos.x - CameraPos.x + PositionOffset.x)*fUniversalScale * Offset, (ObjectPos.y - CameraPos.y + PositionOffset.y)*fUniversalScale * Offset, (ObjectPos.z - CameraPos.z)*fUniversalScale * Offset);
	// ワールド マトリックスに設定
	XMStoreFloat4x4(&g_mWorld, mWorld);

	if (gpTexture) {
		// 拡縮
		mWorld = XMMatrixScaling(g_sizTexFrame.x, g_sizTexFrame.y, 1.0f);
		// 移動
		mWorld *= XMMatrixTranslation(g_posTexFrame.x, g_posTexFrame.y, 0.0f);
		// テクスチャ マトリックスに設定
		XMStoreFloat4x4(&g_mTex, mWorld);
	}
	else {
		// テクスチャ無し
		g_mTex._44 = 0.0f;
	}

	// 頂点バッファ更新
	SetVertexSprite();

	pDeviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(g_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(g_pInputLayout);

	UINT stride = sizeof(VERTEX_2D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	pDeviceContext->PSSetSamplers(0, 1, &g_pSamplerState);
	pDeviceContext->PSSetShaderResources(0, 1, &gpTexture);


	SHADER_GLOBAL cb;
	cb.mProj = XMMatrixTranspose(XMLoadFloat4x4(&Projection));
	cb.mView = XMMatrixTranspose(XMLoadFloat4x4(&g_mView));
	cb.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&g_mWorld));
	cb.mTex = XMMatrixTranspose(XMLoadFloat4x4(&g_mTex));
	pDeviceContext->UpdateSubresource(gpConstantBuffer, 0, nullptr, &cb, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &gpConstantBuffer);
	pDeviceContext->PSSetConstantBuffers(0, 1, &gpConstantBuffer);

	// プリミティブ形状をセット
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴンの描画
	pDeviceContext->Draw(NUM_VERTEX, 0);
#if SHOW_HITBOXES
	if (hitbox.print && HitBoxSprite != nullptr)
	{

		HitBoxSprite->SetLocalPosition(GetHitbox().x, GetHitbox().y);
		HitBoxSprite->SetSpriteSize(GetHitbox().w, GetHitbox().h);
		HitBoxSprite->DrawSpriteHB(pDeviceContext);
	}
#endif
}

//*****************************************************************************
//DrawSpriteUI関数
//UIの描画処理
//引数：ID3D11DeviceContext*
//戻：void
//*****************************************************************************
void Sprite::DrawSpriteUI(ID3D11DeviceContext * pDeviceContext)
{
	if (!bSpriteInited)
		return;
	if (!bSetUpNoUIFilter) {
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		pDevice->CreateSamplerState(&sd, &g_pSamplerState);
		bSetUpNoUIFilter = true;
	}
	// 拡縮
	XMMATRIX mWorld = XMMatrixScaling(Size.x, Size.y, Size.z);
	// 回転
	mWorld *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotation.x),
		XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	// 移動
	XMFLOAT3 CameraPos = GetCameraPos();
	XMFLOAT3 ObjectPos = GetWorldPosition();
	if (bDontDependOnCamera)
		mWorld *= XMMatrixTranslation(ObjectPos.x, ObjectPos.y, ObjectPos.z);
	else
		mWorld *= XMMatrixTranslation(ObjectPos.x, ObjectPos.y, ObjectPos.z - ObjectPos.z);
	// ワールド マトリックスに設定
	XMStoreFloat4x4(&g_mWorld, mWorld);

	if (gpTexture) {
		// 拡縮
		mWorld = XMMatrixScaling(g_sizTexFrame.x, g_sizTexFrame.y, 1.0f);
		// 移動
		mWorld *= XMMatrixTranslation(g_posTexFrame.x, g_posTexFrame.y, 0.0f);
		// テクスチャ マトリックスに設定
		XMStoreFloat4x4(&g_mTex, mWorld);
	}
	else {
		// テクスチャ無し
		g_mTex._44 = 0.0f;
	}

	// 頂点バッファ更新
	SetVertexSprite();

	pDeviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(g_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(g_pInputLayout);

	UINT stride = sizeof(VERTEX_2D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	pDeviceContext->PSSetSamplers(0, 1, &g_pSamplerState);
	pDeviceContext->PSSetShaderResources(0, 1, &gpTexture);


	SHADER_GLOBAL cb;
	cb.mProj = XMMatrixTranspose(XMLoadFloat4x4(&Projection));
	cb.mView = XMMatrixTranspose(XMLoadFloat4x4(&g_mView));
	cb.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&g_mWorld));
	cb.mTex = XMMatrixTranspose(XMLoadFloat4x4(&g_mTex));
	pDeviceContext->UpdateSubresource(gpConstantBuffer, 0, nullptr, &cb, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &gpConstantBuffer);
	pDeviceContext->PSSetConstantBuffers(0, 1, &gpConstantBuffer);

	// プリミティブ形状をセット
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴンの描画
	pDeviceContext->Draw(NUM_VERTEX, 0);
#if SHOW_HITBOXES
	if (hitbox.print && HitBoxSprite != nullptr)
	{

		HitBoxSprite->SetLocalPosition(GetHitbox().x, GetHitbox().y);
		HitBoxSprite->SetSpriteSize(GetHitbox().w, GetHitbox().h);
		HitBoxSprite->DrawSpriteHB(pDeviceContext);
	}
#endif
}

//*****************************************************************************
//DrawSpriteHB関数
//ヒットボックスの描画処理
//引数：ID3D11DeviceContext*
//戻：void
//*****************************************************************************
void Sprite::DrawSpriteHB(ID3D11DeviceContext* pDeviceContext)
{
	if (!bSpriteInited)
		return;
	if (!bSetUpNoUIFilter) {
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		pDevice->CreateSamplerState(&sd, &g_pSamplerState);
		bSetUpNoUIFilter = true;
	}
	// 拡縮
	XMMATRIX mWorld = XMMatrixScaling(Size.x*fUniversalScale, Size.y*fUniversalScale, Size.z*fUniversalScale);
	XMFLOAT3 ObjectPos = GetWorldPosition();
	// 回転
	mWorld *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotation.x),
		XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	// 移動
	XMFLOAT3 CameraPos = GetCameraPos();
	if (bDontDependOnCamera)
		mWorld *= XMMatrixTranslation(ObjectPos.x*fUniversalScale, ObjectPos.y*fUniversalScale, ObjectPos.z*fUniversalScale);
	else
		mWorld *= XMMatrixTranslation((ObjectPos.x - CameraPos.x + PositionOffset.x)*fUniversalScale, (ObjectPos.y - CameraPos.y + PositionOffset.y)*fUniversalScale, (ObjectPos.z - CameraPos.z)*fUniversalScale);
	// ワールド マトリックスに設定
	XMStoreFloat4x4(&g_mWorld, mWorld);

	if (gpTexture) {
		// 拡縮
		mWorld = XMMatrixScaling(g_sizTexFrame.x, g_sizTexFrame.y, 1.0f);
		// 移動
		mWorld *= XMMatrixTranslation(g_posTexFrame.x, g_posTexFrame.y, 0.0f);
		// テクスチャ マトリックスに設定
		XMStoreFloat4x4(&g_mTex, mWorld);
	}
	else {
		// テクスチャ無し
		g_mTex._44 = 0.0f;
	}

	// 頂点バッファ更新
	SetVertexSprite();

	pDeviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(g_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(g_pInputLayout);

	UINT stride = sizeof(VERTEX_2D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	pDeviceContext->PSSetSamplers(0, 1, &g_pSamplerState);
	pDeviceContext->PSSetShaderResources(0, 1, &gpTexture);

	SHADER_GLOBAL cb;
	cb.mProj = XMMatrixTranspose(XMLoadFloat4x4(&Projection));
	cb.mView = XMMatrixTranspose(XMLoadFloat4x4(&g_mView));
	cb.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&g_mWorld));
	cb.mTex = XMMatrixTranspose(XMLoadFloat4x4(&g_mTex));
	pDeviceContext->UpdateSubresource(gpConstantBuffer, 0, nullptr, &cb, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &gpConstantBuffer);
	pDeviceContext->PSSetConstantBuffers(0, 1, &gpConstantBuffer);

	// プリミティブ形状をセット
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴンの描画
	pDeviceContext->Draw(NUM_VERTEX, 0);
}

//*****************************************************************************
//SetTexture関数
//テクスチャの設定
//引数：ID3D11ShaderResourceView*
//戻：void
//*****************************************************************************
void Sprite::SetTexture(ID3D11ShaderResourceView * pTexture)
{
	gpTexture = pTexture;
	g_mTex._44 = (gpTexture) ? 1.0f : 0.0f;
}

//*****************************************************************************
//GetLocalPosition関数
//スプライトのローカル拠点の戻し
//引数：void
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 Sprite::GetLocalPosition()
{
	return Position;
}

//*****************************************************************************
//GetWorldPosition関数
//スプライトの環境拠点の戻し
//引数：void
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 Sprite::GetWorldPosition()
{
	if(!Parent)
		return Position;
	XMFLOAT3 ParentPos = Parent->GetWorldPosition();
	return { Position.x + ParentPos.x, Position.y + ParentPos.y, Position.z + ParentPos.z };
}

//*****************************************************************************
//GetSize関数
//スプライトのサイズの戻し
//引数：void
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 Sprite::GetSize()
{
	return Size;
}

//*****************************************************************************
//GetRotation関数
//スプライトのロテーションの戻し
//引数：void
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 Sprite::GetRotation()
{
	return Rotation;
}

//*****************************************************************************
//GetColor関数
//スプライトの色の戻し
//引数：void
//戻：XMFLOAT4
//*****************************************************************************
XMFLOAT4 Sprite::GetColor()
{
	return color;
}

//*****************************************************************************
//GetAlpha関数
//スプライトのアルファ色の戻し
//引数：void
//戻：float
//*****************************************************************************
float Sprite::GetAlpha()
{
	return color.w;
}

//*****************************************************************************
//Translate関数
//スプライトの動かせる関数
//引数：float, float
//戻：void
//*****************************************************************************
void Sprite::Translate(float x, float y)
{
	Position.x += x;
	Position.y += y;
}

//*****************************************************************************
//MakeVertexSprite関数
//頂点の作成関数
//引数：D3D11Device*
//戻：HRESULT
//*****************************************************************************
HRESULT Sprite::MakeVertexSprite(ID3D11Device* pDevice)
{
	// 頂点座標の設定
	g_vertexWk[0].vtx = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	g_vertexWk[1].vtx = XMFLOAT3(0.5f, 0.5f, 0.0f);
	g_vertexWk[2].vtx = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	g_vertexWk[3].vtx = XMFLOAT3(0.5f, -0.5f, 0.0f);

	// 拡散反射光の設定
	g_vertexWk[0].diffuse = color;
	g_vertexWk[1].diffuse = color;
	g_vertexWk[2].diffuse = color;
	g_vertexWk[3].diffuse = color;

	// テクスチャ座標の設定
	g_vertexWk[0].tex = XMFLOAT2(0.0f, 0.0f);
	g_vertexWk[1].tex = XMFLOAT2(1.0f, 0.0f);
	g_vertexWk[2].tex = XMFLOAT2(0.0f, 1.0f);
	g_vertexWk[3].tex = XMFLOAT2(1.0f, 1.0f);

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(g_vertexWk);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &g_vertexWk[0];

	HRESULT hr = pDevice->CreateBuffer(&vbd, &initData, &g_pVertexBuffer);

	return hr;
}


//*****************************************************************************
//SetVertexSprite関数
//頂点座標の設定
//引数：void
//戻：void
//*****************************************************************************
void Sprite::SetVertexSprite()
{
	if (g_bInvalidate) {
		//頂点バッファの中身を埋める
		ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
		HRESULT hr = S_OK;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		hr = pDeviceContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		if (SUCCEEDED(hr)) {
			// 拡散反射光の設定
			g_vertexWk[0].diffuse = color;
			g_vertexWk[1].diffuse = color;
			g_vertexWk[2].diffuse = color;
			g_vertexWk[3].diffuse = color;
			rsize_t len = msr.RowPitch * msr.DepthPitch;
			if (len == 0)
				len = msr.RowPitch + msr.DepthPitch;
			// 頂点データを上書き
			memcpy_s(msr.pData, len, g_vertexWk, sizeof(g_vertexWk));
			// 頂点データをアンロックする
			pDeviceContext->Unmap(g_pVertexBuffer, 0);
			// フラグをクリア
			g_bInvalidate = false;
		}
	}
}

//*****************************************************************************
//GetHitbox関数
//ヒットボックスをゲットする
//引数：void
//戻：HitBox
//*****************************************************************************
HitBox Sprite::GetHitbox()
{
	XMFLOAT3 CurrentPos = GetWorldPosition();
	return{ hitbox.x + CurrentPos.x, hitbox.y + CurrentPos.y, hitbox.w, hitbox.h, hitbox.print, hitbox.Sprite };
}

//*****************************************************************************
//GetDimensionHitbox関数
//ポリゴンの高さと長さでヒットボックスを作り戻す
//引数：void
//戻：HitBox
//*****************************************************************************
HitBox Sprite::GetDimensionHitbox()
{
	XMFLOAT3 CurrentPos = GetWorldPosition();
	return{ CurrentPos.x, CurrentPos.y, Size.x, Size.y, false, nullptr };
}

//*****************************************************************************
//IsSpriteInited関数
//スプライトは初期化があるか確認する
//引数：void
//戻：bool
//*****************************************************************************
bool Sprite::IsSpriteInited()
{
	return bSpriteInited;
}

//*****************************************************************************
//SetDependOnCamera関数
//拠点はカメラで頼っていることをセットする
//引数：bool
//戻：bool
//*****************************************************************************
void Sprite::SetDependOnCamera(bool depend)
{
	bDontDependOnCamera = depend;
}

//*****************************************************************************
//IncreaseSizeWithHitbox関数
//スプライトのサイズとヒットボックスのサイズの大きさ平行で変える
//引数：float, float
//戻：void
//*****************************************************************************
void Sprite::IncreaseSizeWithHitbox(float incX, float incY)
{
	Size.x += incX;
	if (Size.x < 0)
		Size.x = 0;
	Size.y += incY;
	if (Size.y < 0)
		Size.y = 0;
	hitbox.w += incX;
	if (hitbox.w < 0)
		hitbox.w = 0;
	hitbox.h += incY;
	if (hitbox.h < 0)
		hitbox.h = 0;
}

void SetDevice(ID3D11Device * n_pDevice)
{
	pDevice = n_pDevice;
}

void SetHR(HRESULT * nhr)
{
	g_hr = nhr;
}

//*****************************************************************************
//IsInCollision関数
//当たり判定を確認する
//引数：HitBox, HitBox
//戻：bool
//*****************************************************************************
bool IsInCollision(HitBox hbA, HitBox hbB)
{
	if (hbA.h == 0 || hbA.w == 0 || hbB.w == 0 || hbB.h == 0)
		return false;
	HitBox a = hbA;
	HitBox b = hbB;
	a.x = a.x - (a.w*0.5f);
	b.x = b.x - (b.w*0.5f);
	a.y = a.y - (a.h*0.5f);
	b.y = b.y - (b.h*0.5f);
	return (((a.x + a.w) >= b.x) && (a.x) <= (b.x + b.w) && (a.y + a.h) >= b.y && a.y <= (b.y + b.h));
}

//*****************************************************************************
//SetUniversalScale関数
//環境の大きさをセットする
//引数：void
//戻：float
//*****************************************************************************
void SetUniversalScale(float newScale)
{
	fUniversalScale = newScale;
}

//*****************************************************************************
//ReduceUniversalScale関数
//環境の大きさを減らす
//引数：float
//戻：void
//*****************************************************************************
void ReduceUniversalScale(float redScale)
{
	fUniversalScale -= redScale;
}

//*****************************************************************************
//IncreaseUniversalScale関数
//環境の大きさを増やす
//引数：float
//戻：void
//*****************************************************************************
void IncreaseUniversalScale(float upScale)
{
	fUniversalScale += upScale;
}

//*****************************************************************************
//ResetUniversalScale関数
//環境の大きさをリセットする
//引数：void
//戻：void
//*****************************************************************************
void ResetUniversalScale()
{
	fUniversalScale = COMMON_UNIVERSAL_SCALE;
}

//*****************************************************************************
//SetNewGeneralFilter関数
//環境のフィルターを変える
//引数：D3D11_FILTER
//戻：void
//*****************************************************************************
void SetNewGeneralFilter(D3D11_FILTER newFilter)
{
	d3d11GeneralFilter = newFilter;
}

//*****************************************************************************
//GetHitboxTexture関数
//ヒットボックスのテクスチャを戻す
//引数：void
//戻：ID3D11ShaderResourceView*
//*****************************************************************************
ID3D11ShaderResourceView* GetHitboxTexture()
{
	return GetTexture(TEXTURE_HITBOX);
}

//*****************************************************************************
//GetUniversalScale関数
//環境のスケールを戻す
//引数：void
//戻：float
//*****************************************************************************
float GetUniversalScale()
{
	return fUniversalScale;
}

//*****************************************************************************
//CreateTexture関数
//テクスチャーの設定
//引数：void
//戻：ID3D11ShaderResourceView*
//*****************************************************************************
ID3D11ShaderResourceView * CreateTexture(const char * texturepath, ID3D11ShaderResourceView * ngpTexture)
{
	CreateTextureFromFile(texturepath, &ngpTexture);
	return ngpTexture;
}

//*****************************************************************************
//GetSpriteNumInMemory関数
//オブジェクトの数字を戻す
//引数：void
//戻：int
//*****************************************************************************
int GetSpriteNumInMemory()
{
	return nSpriteNum;
}

//*****************************************************************************
//ReduceDebugPolygonCounter関数
//オブジェクトの数字を変える
//引数：int
//戻：void
//*****************************************************************************
void ReduceDebugPolygonCounter(int red)
{
	nSpriteNum -= red;
}

//*****************************************************************************
//SetLocalPosition関数
//ローカル拠点の設定、これは親のオブジェクトで頼る
//引数：float, float
//戻：void
//*****************************************************************************
void Sprite::SetLocalPosition(float fX, float fY)
{
	Position.x = fX;
	Position.y = fY;
}

//*****************************************************************************
//SetWorldPosition関数
//環境拠点の設定、これは親のオブジェクトで頼ってない
//引数：float, float
//戻：void
//*****************************************************************************
void Sprite::SetWorldPosition(float fX, float fY)
{
	if (Parent == nullptr) {
		SetLocalPosition(fX, fY);
		return;
	}
	Position.x = fX-Parent->GetWorldPosition().x;
	Position.y = fY-Parent->GetWorldPosition().y;
}

//*****************************************************************************
//SetWorldPosition関数
//環境拠点の設定、これは親のオブジェクトで頼ってない
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void Sprite::SetWorldPosition(XMFLOAT3 newSize)
{
	SetWorldPosition(newSize.x, newSize.y);
}

//*****************************************************************************
//SetSpriteSize関数
//表示サイズの設定
//引数：float, float
//戻：void
//*****************************************************************************
void Sprite::SetSpriteSize(float fScaleX, float fScaleY)
{
	Size.x = fScaleX;
	Size.y = fScaleY;
}

//*****************************************************************************
//SetSpriteSize関数
//表示サイズの設定
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void Sprite::SetSpriteSize(XMFLOAT3 newSize)
{
	Size = newSize;
}

//*****************************************************************************
//SetSpriteSize関数
//表示サイズの設定
//引数：XMFLOAT2
//戻：void
//*****************************************************************************
void Sprite::SetSpriteSize(XMFLOAT2 newSize)
{
	Size.x = newSize.x;
	Size.y = newSize.y;
}

//*****************************************************************************
//SizeUp関数
//表示サイズを変える
//引数：XMFLOAT2
//戻：void
//*****************************************************************************
void Sprite::SizeUp(XMFLOAT2 newSize)
{
	Size.x += newSize.x;
	Size.y += newSize.y;
	if (Size.x < 0)
		Size.x = 0;
	if (Size.y < 0)
		Size.y = 0;
}

//*****************************************************************************
//SetSpriteAngle関数
//表示角度の設定(単位:度)
//引数：float
//戻：void
//*****************************************************************************
void Sprite::SetSpriteAngle(float fAngle)
{
	Rotation.z = fAngle;
}

//*****************************************************************************
//SetSpriteUV関数
//左上テクスチャ座標の設定 (0.0≦fU＜1.0, 0.0≦fV＜1.0)
//引数：float, float
//戻：void
//*****************************************************************************
void Sprite::SetSpriteUV(float fU, float fV)
{
	g_posTexFrame.x = fU;
	g_posTexFrame.y = fV;
}

//*****************************************************************************
//SetSpriteFrameSize関数
//テクスチャフレームサイズの設定 (0.0＜fWidth≦1.0, 0.0＜fHeight≦1.0)
//引数：float, float
//戻：void
//*****************************************************************************
void Sprite::SetSpriteFrameSize(float fWidth, float fHeight)
{
	g_sizTexFrame.x = fWidth;
	g_sizTexFrame.y = fHeight;
}

//*****************************************************************************
//SetColor関数
//頂点色の設定
//引数：float, float, float
//戻：void
//*****************************************************************************
void Sprite::SetColor(float fRed, float fGreen, float fBlue)
{
	if (fRed != color.x || fGreen != color.y || fBlue != color.z) {
		color.x = fRed;
		color.y = fGreen;
		color.z = fBlue;
		g_bInvalidate = true;
	}
}
//*****************************************************************************
//SetColorB関数
//頂点色の設定(青だけ)
//引数：float
//戻：void
//*****************************************************************************
void Sprite::SetColorB(float fB)
{
	if (fB != color.z) {
		color.z = fB;
		g_bInvalidate = true;
	}
}

//*****************************************************************************
//SetAlpha関数
//不透明度の設定
//引数：float
//戻：void
//*****************************************************************************
void Sprite::SetAlpha(float fAlpha)
{
	if (fAlpha != color.w) {
		color.w = fAlpha;
		g_bInvalidate = true;
	}
}

//*****************************************************************************
//RiseAlpha関数
//オブジェクトのアルファブレンドを変える
//引数：float
//戻：void
//*****************************************************************************
void Sprite::RiseAlpha(float fAlpha)
{
	color.w += fAlpha;
	if (color.w < 0)
		color.w = 0;
	if (color.w > 1)
		color.w = 1;
	g_bInvalidate = true;
}

//*****************************************************************************
//SetTexture関数
//不透明度の設定
//引数：const char* 
//戻：void
//*****************************************************************************
void Sprite::SetTexture(const char* texturepath)
{
	if (gpTexture) {
#ifdef USE_CONSOLE
		printf("WARNING: TEXTURE POINTER ALREADY CREATED, SAFE RELEASED TO CREATE 「%s」\n", texturepath);
#endif
		SAFE_RELEASE(gpTexture);
	}
#ifdef USE_CONSOLE
	printf("SP: 作ったテクスチャ： %s\n", texturepath);
	strcpy(TextureName, texturepath);
#endif
	bTextureNotInTextureManager = true;
	CreateTextureFromFile(texturepath, &gpTexture);
}

//*****************************************************************************
//SetRotation関数
//スプライトの回転の設定
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void Sprite::SetRotation(XMFLOAT3 newRot)
{
	Rotation = newRot;
}

//*****************************************************************************
//RotateAroundZ関数
//オブジェクトを回転する（Z座標で）
//引数：float
//戻：void
//*****************************************************************************
void Sprite::RotateAroundZ(float rot)
{
	Rotation.z += rot;
	if (Rotation.z >= 360)
		Rotation.z -= 360;
}

//*****************************************************************************
//RotateAroundY関数
//オブジェクトを回転する（Y座標で）
//引数：float
//戻：void
//*****************************************************************************
void Sprite::RotateAroundY(float rot)
{
	Rotation.y += rot;
	if (Rotation.y >= 360)
		Rotation.y -= 360;
}

//*****************************************************************************
//SetParent関数
//スプライトの親を設定する
//引数：Sprite
//戻：void
//*****************************************************************************
void Sprite::SetParent(Sprite* pParent)
{
	if (!pParent)
		return;
	XMFLOAT3 ParentPos = pParent->GetWorldPosition();
	if (pParent->SetChild(this)) {
		Position.x -= ParentPos.x;
		Position.y -= ParentPos.y;
		Position.z -= ParentPos.z;
		Parent = pParent;
	}
}

//*****************************************************************************
//SetParent関数
//スプライトの親を設定する。オブジェクトの環境拠点を頼ることが決められる
//引数：Sprite, bool
//戻：void
//*****************************************************************************
void Sprite::SetParent(Sprite * pParent, bool KeepWorldPosition)
{
	if (!pParent)
		return;
	XMFLOAT3 ParentPos = pParent->GetWorldPosition();
	if (pParent->SetChild(this)) {
		if (KeepWorldPosition) {
			Position.x -= ParentPos.x;
			Position.y -= ParentPos.y;
			Position.z -= ParentPos.z;
		}
		Parent = pParent;
	}
}

//*****************************************************************************
//SetChild関数
//オブジェクトの子を設定する
//引数：Sprite*
//戻：bool
//*****************************************************************************
bool Sprite::SetChild(Sprite * Child)
{
	for (int i = 0; i < MAX_SPRITE_CHILDS; i++)
	{
		if (Childs[i])
			continue;
		Childs[i] = Child;
		return true;
	}
	return false;
}

//*****************************************************************************
//SetNewFilter関数
//オブジェクトのフィルターを設定する
//引数：D3D11_FILTER
//戻：void
//*****************************************************************************
void Sprite::SetNewFilter(D3D11_FILTER newFilter)
{
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = newFilter;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pDevice->CreateSamplerState(&sd, &g_pSamplerState);
	bSetUpNoUIFilter = true;
}

//*****************************************************************************
//GetParent関数
//親を戻す
//引数：void
//戻：Sprite*
//*****************************************************************************
Sprite* Sprite::GetParent()
{
	return Parent;
}

//*****************************************************************************
//RemoveParent関数
//親を消す
//引数：void
//戻：void
//*****************************************************************************
void Sprite::RemoveParent()
{
	if (Parent)
	{
		XMFLOAT3 ParentPos = Parent->GetWorldPosition();
		Position.x += ParentPos.x;
		Position.y += ParentPos.y;
		Position.z += ParentPos.z;
		Parent->RemoveChild(this);
	}
	Parent = nullptr;
}

//*****************************************************************************
//RemoveChild関数
//オブジェクトの子を消す
//引数：Sprite*
//戻：void
//*****************************************************************************
void Sprite::RemoveChild(Sprite * Child)
{
	for (int i = 0; i < MAX_SPRITE_CHILDS; i++)
	{
		if (Childs[i] == Child)
		{
			Childs[i] = nullptr;
		}
	}
}

//*****************************************************************************
//RemoveParent関数
//親を消す。環境の拠点を変えないことが決められる
//引数：bool
//戻：void
//*****************************************************************************
void Sprite::RemoveParent(bool KeepWorldPosition)
{
	if (Parent)
	{
		XMFLOAT3 ParentPos = Parent->GetWorldPosition();
		if (KeepWorldPosition) {
			Position.x += ParentPos.x;
			Position.y += ParentPos.y;
			Position.z += ParentPos.z;
		}
		Parent->RemoveChild(this);
		Parent = nullptr;
	}
}
