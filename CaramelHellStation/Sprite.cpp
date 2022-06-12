//*****************************************************************************
// Sprite.cpp
// �X�v���C�g�����_�����O�̊Ǘ�
// Author : Mane
//*****************************************************************************
#include "Sprite.h"
#include "Shader.h"
#include "TextureManager.h"
#include "UniversalVariables.h"
#include "Camera.h"
#include "Texture.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define FILTER_ONLY_WHEN_ZOOM_CHANGE true

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
ID3D11Device				*pDevice;
HRESULT						*g_hr;
float						fUniversalScale = COMMON_UNIVERSAL_SCALE;
int							nSpriteNum = 0;
D3D11_FILTER				d3d11GeneralFilter = D3D11_FILTER_ANISOTROPIC;

//No Filter -> D3D11_FILTER_MIN_MAG_MIP_POINT
//Linear Filter -> D3D11_FILTER_MIN_MAG_MIP_LINEAR

//*****************************************************************************
//�R���X�g���N�^
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
//InitSprite�֐�
//�����������֐�
//�����Fvoid
//�߁FHRESULT
//*****************************************************************************

HRESULT Sprite::InitSprite()
{
	if (bSpriteInited)
		return S_OK;
	Parent = nullptr;
	HRESULT hr = S_OK;
	bSpriteInited = true;
	// �V�F�[�_������
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

	// �萔�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SHADER_GLOBAL);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pDevice->CreateBuffer(&bd, nullptr, &gpConstantBuffer);
	if (FAILED(hr)) return hr;

	// �e�N�X�`�� �T���v������
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

	// �u�����h �X�e�[�g����
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

	// �ϊ��s�񏉊���
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

	// ���_���̍쐬
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
//InitSprite�֐�
//�����������֐�
//�����FID3D11ShaderResourceView*
//�߁FHRESULT
//*****************************************************************************
HRESULT Sprite::InitSprite(ID3D11ShaderResourceView* pTexture)
{
	HRESULT hr = InitSprite();
	SetTexture(pTexture);
	return hr;
}

//*****************************************************************************
//InitSprite�֐�
//�����������֐�
//�����Fconst char*
//�߁FHRESULT
//*****************************************************************************
HRESULT Sprite::InitSprite(const char* texturepath)
{
#ifdef USE_CONSOLE
	printf("SP: ������e�N�X�`���F %s\n", texturepath);
	strcpy(TextureName, texturepath);
#endif
	HRESULT hr = InitSprite();
	SetTexture(texturepath);
	bTextureNotInTextureManager = true;
	return hr;
}

//*****************************************************************************
//UninitSprite�֐�
//�I�������֐�
//�����Fvoid
//�߁Fvoid
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
	// �u�����h �X�e�[�g���
	SAFE_RELEASE(gpBlendState);
	// �e�N�X�`�� �T���v���̊J��
	SAFE_RELEASE(g_pSamplerState);
	// ���_�o�b�t�@�̉��
	SAFE_RELEASE(g_pVertexBuffer);
	// �萔�o�b�t�@�̉��
	SAFE_RELEASE(gpConstantBuffer);
	// �s�N�Z���V�F�[�_���
	SAFE_RELEASE(g_pPixelShader);
	// ���_�t�H�[�}�b�g���
	SAFE_RELEASE(g_pInputLayout);
	// ���_�V�F�[�_���
	//SAFE_RELEASE(g_pVertexShader);

	SAFE_DELETE(HitBoxSprite);
	if (bTextureNotInTextureManager)
	{
#ifdef USE_CONSOLE
		printf("SP: ������: %s\n", TextureName);
#endif
		SAFE_RELEASE(gpTexture);
	}
	bSpriteInited = false;
}

//*****************************************************************************
//UpdateSprite�֐�
//�X�V����
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Sprite::UpdateSprite()
{
	//�Ȃ�
}

//*****************************************************************************
//DrawSprite�֐�
//�`�揈��
//�����FID3D11DeviceContext*
//�߁Fvoid
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
		printf("�t�B���^�[��ς���\n");
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

	// �g�k
	XMMATRIX mWorld = XMMatrixScaling(Size.x*fUniversalScale * Offset, Size.y*fUniversalScale * Offset, Size.z*fUniversalScale * Offset);
	// ��]
	mWorld *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotation.x),
		XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	// �ړ�
	XMFLOAT3 CameraPos = GetCameraPos();
	XMFLOAT3 ObjectPos = GetWorldPosition();
	if (bDontDependOnCamera)
		mWorld *= XMMatrixTranslation(ObjectPos.x*fUniversalScale * Offset, ObjectPos.y*fUniversalScale * Offset, ObjectPos.z*fUniversalScale * Offset);
	else
		mWorld *= XMMatrixTranslation((ObjectPos.x - CameraPos.x + PositionOffset.x)*fUniversalScale * Offset, (ObjectPos.y - CameraPos.y + PositionOffset.y)*fUniversalScale * Offset, (ObjectPos.z - CameraPos.z)*fUniversalScale * Offset);
	// ���[���h �}�g���b�N�X�ɐݒ�
	XMStoreFloat4x4(&g_mWorld, mWorld);

	if (gpTexture) {
		// �g�k
		mWorld = XMMatrixScaling(g_sizTexFrame.x, g_sizTexFrame.y, 1.0f);
		// �ړ�
		mWorld *= XMMatrixTranslation(g_posTexFrame.x, g_posTexFrame.y, 0.0f);
		// �e�N�X�`�� �}�g���b�N�X�ɐݒ�
		XMStoreFloat4x4(&g_mTex, mWorld);
	}
	else {
		// �e�N�X�`������
		g_mTex._44 = 0.0f;
	}

	// ���_�o�b�t�@�X�V
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

	// �v���~�e�B�u�`����Z�b�g
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �|���S���̕`��
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
//DrawSpriteUI�֐�
//UI�̕`�揈��
//�����FID3D11DeviceContext*
//�߁Fvoid
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
	// �g�k
	XMMATRIX mWorld = XMMatrixScaling(Size.x, Size.y, Size.z);
	// ��]
	mWorld *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotation.x),
		XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	// �ړ�
	XMFLOAT3 CameraPos = GetCameraPos();
	XMFLOAT3 ObjectPos = GetWorldPosition();
	if (bDontDependOnCamera)
		mWorld *= XMMatrixTranslation(ObjectPos.x, ObjectPos.y, ObjectPos.z);
	else
		mWorld *= XMMatrixTranslation(ObjectPos.x, ObjectPos.y, ObjectPos.z - ObjectPos.z);
	// ���[���h �}�g���b�N�X�ɐݒ�
	XMStoreFloat4x4(&g_mWorld, mWorld);

	if (gpTexture) {
		// �g�k
		mWorld = XMMatrixScaling(g_sizTexFrame.x, g_sizTexFrame.y, 1.0f);
		// �ړ�
		mWorld *= XMMatrixTranslation(g_posTexFrame.x, g_posTexFrame.y, 0.0f);
		// �e�N�X�`�� �}�g���b�N�X�ɐݒ�
		XMStoreFloat4x4(&g_mTex, mWorld);
	}
	else {
		// �e�N�X�`������
		g_mTex._44 = 0.0f;
	}

	// ���_�o�b�t�@�X�V
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

	// �v���~�e�B�u�`����Z�b�g
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �|���S���̕`��
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
//DrawSpriteHB�֐�
//�q�b�g�{�b�N�X�̕`�揈��
//�����FID3D11DeviceContext*
//�߁Fvoid
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
	// �g�k
	XMMATRIX mWorld = XMMatrixScaling(Size.x*fUniversalScale, Size.y*fUniversalScale, Size.z*fUniversalScale);
	XMFLOAT3 ObjectPos = GetWorldPosition();
	// ��]
	mWorld *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotation.x),
		XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	// �ړ�
	XMFLOAT3 CameraPos = GetCameraPos();
	if (bDontDependOnCamera)
		mWorld *= XMMatrixTranslation(ObjectPos.x*fUniversalScale, ObjectPos.y*fUniversalScale, ObjectPos.z*fUniversalScale);
	else
		mWorld *= XMMatrixTranslation((ObjectPos.x - CameraPos.x + PositionOffset.x)*fUniversalScale, (ObjectPos.y - CameraPos.y + PositionOffset.y)*fUniversalScale, (ObjectPos.z - CameraPos.z)*fUniversalScale);
	// ���[���h �}�g���b�N�X�ɐݒ�
	XMStoreFloat4x4(&g_mWorld, mWorld);

	if (gpTexture) {
		// �g�k
		mWorld = XMMatrixScaling(g_sizTexFrame.x, g_sizTexFrame.y, 1.0f);
		// �ړ�
		mWorld *= XMMatrixTranslation(g_posTexFrame.x, g_posTexFrame.y, 0.0f);
		// �e�N�X�`�� �}�g���b�N�X�ɐݒ�
		XMStoreFloat4x4(&g_mTex, mWorld);
	}
	else {
		// �e�N�X�`������
		g_mTex._44 = 0.0f;
	}

	// ���_�o�b�t�@�X�V
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

	// �v���~�e�B�u�`����Z�b�g
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �|���S���̕`��
	pDeviceContext->Draw(NUM_VERTEX, 0);
}

//*****************************************************************************
//SetTexture�֐�
//�e�N�X�`���̐ݒ�
//�����FID3D11ShaderResourceView*
//�߁Fvoid
//*****************************************************************************
void Sprite::SetTexture(ID3D11ShaderResourceView * pTexture)
{
	gpTexture = pTexture;
	g_mTex._44 = (gpTexture) ? 1.0f : 0.0f;
}

//*****************************************************************************
//GetLocalPosition�֐�
//�X�v���C�g�̃��[�J�����_�̖߂�
//�����Fvoid
//�߁FXMFLOAT3
//*****************************************************************************
XMFLOAT3 Sprite::GetLocalPosition()
{
	return Position;
}

//*****************************************************************************
//GetWorldPosition�֐�
//�X�v���C�g�̊����_�̖߂�
//�����Fvoid
//�߁FXMFLOAT3
//*****************************************************************************
XMFLOAT3 Sprite::GetWorldPosition()
{
	if(!Parent)
		return Position;
	XMFLOAT3 ParentPos = Parent->GetWorldPosition();
	return { Position.x + ParentPos.x, Position.y + ParentPos.y, Position.z + ParentPos.z };
}

//*****************************************************************************
//GetSize�֐�
//�X�v���C�g�̃T�C�Y�̖߂�
//�����Fvoid
//�߁FXMFLOAT3
//*****************************************************************************
XMFLOAT3 Sprite::GetSize()
{
	return Size;
}

//*****************************************************************************
//GetRotation�֐�
//�X�v���C�g�̃��e�[�V�����̖߂�
//�����Fvoid
//�߁FXMFLOAT3
//*****************************************************************************
XMFLOAT3 Sprite::GetRotation()
{
	return Rotation;
}

//*****************************************************************************
//GetColor�֐�
//�X�v���C�g�̐F�̖߂�
//�����Fvoid
//�߁FXMFLOAT4
//*****************************************************************************
XMFLOAT4 Sprite::GetColor()
{
	return color;
}

//*****************************************************************************
//GetAlpha�֐�
//�X�v���C�g�̃A���t�@�F�̖߂�
//�����Fvoid
//�߁Ffloat
//*****************************************************************************
float Sprite::GetAlpha()
{
	return color.w;
}

//*****************************************************************************
//Translate�֐�
//�X�v���C�g�̓�������֐�
//�����Ffloat, float
//�߁Fvoid
//*****************************************************************************
void Sprite::Translate(float x, float y)
{
	Position.x += x;
	Position.y += y;
}

//*****************************************************************************
//MakeVertexSprite�֐�
//���_�̍쐬�֐�
//�����FD3D11Device*
//�߁FHRESULT
//*****************************************************************************
HRESULT Sprite::MakeVertexSprite(ID3D11Device* pDevice)
{
	// ���_���W�̐ݒ�
	g_vertexWk[0].vtx = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	g_vertexWk[1].vtx = XMFLOAT3(0.5f, 0.5f, 0.0f);
	g_vertexWk[2].vtx = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	g_vertexWk[3].vtx = XMFLOAT3(0.5f, -0.5f, 0.0f);

	// �g�U���ˌ��̐ݒ�
	g_vertexWk[0].diffuse = color;
	g_vertexWk[1].diffuse = color;
	g_vertexWk[2].diffuse = color;
	g_vertexWk[3].diffuse = color;

	// �e�N�X�`�����W�̐ݒ�
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
//SetVertexSprite�֐�
//���_���W�̐ݒ�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Sprite::SetVertexSprite()
{
	if (g_bInvalidate) {
		//���_�o�b�t�@�̒��g�𖄂߂�
		ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
		HRESULT hr = S_OK;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		D3D11_MAPPED_SUBRESOURCE msr;
		hr = pDeviceContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		if (SUCCEEDED(hr)) {
			// �g�U���ˌ��̐ݒ�
			g_vertexWk[0].diffuse = color;
			g_vertexWk[1].diffuse = color;
			g_vertexWk[2].diffuse = color;
			g_vertexWk[3].diffuse = color;
			rsize_t len = msr.RowPitch * msr.DepthPitch;
			if (len == 0)
				len = msr.RowPitch + msr.DepthPitch;
			// ���_�f�[�^���㏑��
			memcpy_s(msr.pData, len, g_vertexWk, sizeof(g_vertexWk));
			// ���_�f�[�^���A�����b�N����
			pDeviceContext->Unmap(g_pVertexBuffer, 0);
			// �t���O���N���A
			g_bInvalidate = false;
		}
	}
}

//*****************************************************************************
//GetHitbox�֐�
//�q�b�g�{�b�N�X���Q�b�g����
//�����Fvoid
//�߁FHitBox
//*****************************************************************************
HitBox Sprite::GetHitbox()
{
	XMFLOAT3 CurrentPos = GetWorldPosition();
	return{ hitbox.x + CurrentPos.x, hitbox.y + CurrentPos.y, hitbox.w, hitbox.h, hitbox.print, hitbox.Sprite };
}

//*****************************************************************************
//GetDimensionHitbox�֐�
//�|���S���̍����ƒ����Ńq�b�g�{�b�N�X�����߂�
//�����Fvoid
//�߁FHitBox
//*****************************************************************************
HitBox Sprite::GetDimensionHitbox()
{
	XMFLOAT3 CurrentPos = GetWorldPosition();
	return{ CurrentPos.x, CurrentPos.y, Size.x, Size.y, false, nullptr };
}

//*****************************************************************************
//IsSpriteInited�֐�
//�X�v���C�g�͏����������邩�m�F����
//�����Fvoid
//�߁Fbool
//*****************************************************************************
bool Sprite::IsSpriteInited()
{
	return bSpriteInited;
}

//*****************************************************************************
//SetDependOnCamera�֐�
//���_�̓J�����ŗ����Ă��邱�Ƃ��Z�b�g����
//�����Fbool
//�߁Fbool
//*****************************************************************************
void Sprite::SetDependOnCamera(bool depend)
{
	bDontDependOnCamera = depend;
}

//*****************************************************************************
//IncreaseSizeWithHitbox�֐�
//�X�v���C�g�̃T�C�Y�ƃq�b�g�{�b�N�X�̃T�C�Y�̑傫�����s�ŕς���
//�����Ffloat, float
//�߁Fvoid
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
//IsInCollision�֐�
//�����蔻����m�F����
//�����FHitBox, HitBox
//�߁Fbool
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
//SetUniversalScale�֐�
//���̑傫�����Z�b�g����
//�����Fvoid
//�߁Ffloat
//*****************************************************************************
void SetUniversalScale(float newScale)
{
	fUniversalScale = newScale;
}

//*****************************************************************************
//ReduceUniversalScale�֐�
//���̑傫�������炷
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void ReduceUniversalScale(float redScale)
{
	fUniversalScale -= redScale;
}

//*****************************************************************************
//IncreaseUniversalScale�֐�
//���̑傫���𑝂₷
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void IncreaseUniversalScale(float upScale)
{
	fUniversalScale += upScale;
}

//*****************************************************************************
//ResetUniversalScale�֐�
//���̑傫�������Z�b�g����
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void ResetUniversalScale()
{
	fUniversalScale = COMMON_UNIVERSAL_SCALE;
}

//*****************************************************************************
//SetNewGeneralFilter�֐�
//���̃t�B���^�[��ς���
//�����FD3D11_FILTER
//�߁Fvoid
//*****************************************************************************
void SetNewGeneralFilter(D3D11_FILTER newFilter)
{
	d3d11GeneralFilter = newFilter;
}

//*****************************************************************************
//GetHitboxTexture�֐�
//�q�b�g�{�b�N�X�̃e�N�X�`����߂�
//�����Fvoid
//�߁FID3D11ShaderResourceView*
//*****************************************************************************
ID3D11ShaderResourceView* GetHitboxTexture()
{
	return GetTexture(TEXTURE_HITBOX);
}

//*****************************************************************************
//GetUniversalScale�֐�
//���̃X�P�[����߂�
//�����Fvoid
//�߁Ffloat
//*****************************************************************************
float GetUniversalScale()
{
	return fUniversalScale;
}

//*****************************************************************************
//CreateTexture�֐�
//�e�N�X�`���[�̐ݒ�
//�����Fvoid
//�߁FID3D11ShaderResourceView*
//*****************************************************************************
ID3D11ShaderResourceView * CreateTexture(const char * texturepath, ID3D11ShaderResourceView * ngpTexture)
{
	CreateTextureFromFile(texturepath, &ngpTexture);
	return ngpTexture;
}

//*****************************************************************************
//GetSpriteNumInMemory�֐�
//�I�u�W�F�N�g�̐�����߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int GetSpriteNumInMemory()
{
	return nSpriteNum;
}

//*****************************************************************************
//ReduceDebugPolygonCounter�֐�
//�I�u�W�F�N�g�̐�����ς���
//�����Fint
//�߁Fvoid
//*****************************************************************************
void ReduceDebugPolygonCounter(int red)
{
	nSpriteNum -= red;
}

//*****************************************************************************
//SetLocalPosition�֐�
//���[�J�����_�̐ݒ�A����͐e�̃I�u�W�F�N�g�ŗ���
//�����Ffloat, float
//�߁Fvoid
//*****************************************************************************
void Sprite::SetLocalPosition(float fX, float fY)
{
	Position.x = fX;
	Position.y = fY;
}

//*****************************************************************************
//SetWorldPosition�֐�
//�����_�̐ݒ�A����͐e�̃I�u�W�F�N�g�ŗ����ĂȂ�
//�����Ffloat, float
//�߁Fvoid
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
//SetWorldPosition�֐�
//�����_�̐ݒ�A����͐e�̃I�u�W�F�N�g�ŗ����ĂȂ�
//�����FXMFLOAT3
//�߁Fvoid
//*****************************************************************************
void Sprite::SetWorldPosition(XMFLOAT3 newSize)
{
	SetWorldPosition(newSize.x, newSize.y);
}

//*****************************************************************************
//SetSpriteSize�֐�
//�\���T�C�Y�̐ݒ�
//�����Ffloat, float
//�߁Fvoid
//*****************************************************************************
void Sprite::SetSpriteSize(float fScaleX, float fScaleY)
{
	Size.x = fScaleX;
	Size.y = fScaleY;
}

//*****************************************************************************
//SetSpriteSize�֐�
//�\���T�C�Y�̐ݒ�
//�����FXMFLOAT3
//�߁Fvoid
//*****************************************************************************
void Sprite::SetSpriteSize(XMFLOAT3 newSize)
{
	Size = newSize;
}

//*****************************************************************************
//SetSpriteSize�֐�
//�\���T�C�Y�̐ݒ�
//�����FXMFLOAT2
//�߁Fvoid
//*****************************************************************************
void Sprite::SetSpriteSize(XMFLOAT2 newSize)
{
	Size.x = newSize.x;
	Size.y = newSize.y;
}

//*****************************************************************************
//SizeUp�֐�
//�\���T�C�Y��ς���
//�����FXMFLOAT2
//�߁Fvoid
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
//SetSpriteAngle�֐�
//�\���p�x�̐ݒ�(�P��:�x)
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void Sprite::SetSpriteAngle(float fAngle)
{
	Rotation.z = fAngle;
}

//*****************************************************************************
//SetSpriteUV�֐�
//����e�N�X�`�����W�̐ݒ� (0.0��fU��1.0, 0.0��fV��1.0)
//�����Ffloat, float
//�߁Fvoid
//*****************************************************************************
void Sprite::SetSpriteUV(float fU, float fV)
{
	g_posTexFrame.x = fU;
	g_posTexFrame.y = fV;
}

//*****************************************************************************
//SetSpriteFrameSize�֐�
//�e�N�X�`���t���[���T�C�Y�̐ݒ� (0.0��fWidth��1.0, 0.0��fHeight��1.0)
//�����Ffloat, float
//�߁Fvoid
//*****************************************************************************
void Sprite::SetSpriteFrameSize(float fWidth, float fHeight)
{
	g_sizTexFrame.x = fWidth;
	g_sizTexFrame.y = fHeight;
}

//*****************************************************************************
//SetColor�֐�
//���_�F�̐ݒ�
//�����Ffloat, float, float
//�߁Fvoid
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
//SetColorB�֐�
//���_�F�̐ݒ�(����)
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void Sprite::SetColorB(float fB)
{
	if (fB != color.z) {
		color.z = fB;
		g_bInvalidate = true;
	}
}

//*****************************************************************************
//SetAlpha�֐�
//�s�����x�̐ݒ�
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void Sprite::SetAlpha(float fAlpha)
{
	if (fAlpha != color.w) {
		color.w = fAlpha;
		g_bInvalidate = true;
	}
}

//*****************************************************************************
//RiseAlpha�֐�
//�I�u�W�F�N�g�̃A���t�@�u�����h��ς���
//�����Ffloat
//�߁Fvoid
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
//SetTexture�֐�
//�s�����x�̐ݒ�
//�����Fconst char* 
//�߁Fvoid
//*****************************************************************************
void Sprite::SetTexture(const char* texturepath)
{
	if (gpTexture) {
#ifdef USE_CONSOLE
		printf("WARNING: TEXTURE POINTER ALREADY CREATED, SAFE RELEASED TO CREATE �u%s�v\n", texturepath);
#endif
		SAFE_RELEASE(gpTexture);
	}
#ifdef USE_CONSOLE
	printf("SP: ������e�N�X�`���F %s\n", texturepath);
	strcpy(TextureName, texturepath);
#endif
	bTextureNotInTextureManager = true;
	CreateTextureFromFile(texturepath, &gpTexture);
}

//*****************************************************************************
//SetRotation�֐�
//�X�v���C�g�̉�]�̐ݒ�
//�����FXMFLOAT3
//�߁Fvoid
//*****************************************************************************
void Sprite::SetRotation(XMFLOAT3 newRot)
{
	Rotation = newRot;
}

//*****************************************************************************
//RotateAroundZ�֐�
//�I�u�W�F�N�g����]����iZ���W�Łj
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void Sprite::RotateAroundZ(float rot)
{
	Rotation.z += rot;
	if (Rotation.z >= 360)
		Rotation.z -= 360;
}

//*****************************************************************************
//RotateAroundY�֐�
//�I�u�W�F�N�g����]����iY���W�Łj
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void Sprite::RotateAroundY(float rot)
{
	Rotation.y += rot;
	if (Rotation.y >= 360)
		Rotation.y -= 360;
}

//*****************************************************************************
//SetParent�֐�
//�X�v���C�g�̐e��ݒ肷��
//�����FSprite
//�߁Fvoid
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
//SetParent�֐�
//�X�v���C�g�̐e��ݒ肷��B�I�u�W�F�N�g�̊����_�𗊂邱�Ƃ����߂���
//�����FSprite, bool
//�߁Fvoid
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
//SetChild�֐�
//�I�u�W�F�N�g�̎q��ݒ肷��
//�����FSprite*
//�߁Fbool
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
//SetNewFilter�֐�
//�I�u�W�F�N�g�̃t�B���^�[��ݒ肷��
//�����FD3D11_FILTER
//�߁Fvoid
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
//GetParent�֐�
//�e��߂�
//�����Fvoid
//�߁FSprite*
//*****************************************************************************
Sprite* Sprite::GetParent()
{
	return Parent;
}

//*****************************************************************************
//RemoveParent�֐�
//�e������
//�����Fvoid
//�߁Fvoid
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
//RemoveChild�֐�
//�I�u�W�F�N�g�̎q������
//�����FSprite*
//�߁Fvoid
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
//RemoveParent�֐�
//�e�������B���̋��_��ς��Ȃ����Ƃ����߂���
//�����Fbool
//�߁Fvoid
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
