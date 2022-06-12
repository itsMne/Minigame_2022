//*****************************************************************************
// Sprite.h
// Author : Mane
//*****************************************************************************
#ifndef	 SPRITE_H
#define  SPRITE_H

#include "C_DXWindow.h"
#include "UniversalVariables.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define  MAX_SPRITE_CHILDS 100

//*****************************************************************************
// �G�i����`
//*****************************************************************************
enum Direction
{
	RIGHT_DIR = 1,
	LEFT_DIR = -1
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct SHADER_GLOBAL {
	XMMATRIX	mWorld;		// ���[���h�ϊ��s��(�]�u�s��)
	XMMATRIX	mView;		// �r���[�ϊ��s��(�]�u�s��)
	XMMATRIX	mProj;		// �ˉe�ϊ��s��(�]�u�s��)
	XMMATRIX	mTex;		// �e�N�X�`���ϊ��s��(�]�u�s��)
};

struct HitBox
{
	float x=0, y=0, w=0, h=0;
	bool  print = false;
	void* Sprite = nullptr;
};

//*****************************************************************************
// �N���X
//*****************************************************************************
class Sprite
{
private:
	Sprite						*HitBoxSprite;
	bool						bIsHitbox;
protected:
	bool						bSpriteInited;
	bool						g_bInvalidate;			// ���_�f�[�^�X�V�t���O
	VERTEX_2D					g_vertexWk[NUM_VERTEX];	// ���_���i�[���[�N
	XMFLOAT3					Position;				// �|���S���̈ړ���
	XMFLOAT2					PositionOffset;			// ���_�̃I�t�Z�b�g
	XMFLOAT3					Rotation;				// �|���S���̉�]��
	XMFLOAT3					Size;					// �|���S���̃T�C�Y
	XMFLOAT4					color;					// �|���S���̒��_�J���[
	XMFLOAT2					g_posTexFrame;			// UV���W
	XMFLOAT2					g_sizTexFrame;			// �e�N�X�`�����o�T�C�Y
	XMFLOAT4X4					Projection;				// �ˉe�ϊ��s��
	XMFLOAT4X4					g_mView;				// �r���[�ϊ��s��
	XMFLOAT4X4					g_mWorld;				// ���[���h�ϊ��s��
	XMFLOAT4X4					g_mTex;					// �e�N�X�`���ϊ��s��
	ID3D11Buffer*				gpConstantBuffer;		// �萔�o�b�t�@
	ID3D11Buffer*				g_pVertexBuffer;		// ���_�o�b�t�@
	ID3D11SamplerState*			g_pSamplerState;		// �e�N�X�`�� �T���v��
	ID3D11BlendState*			gpBlendState;			// �u�����h �X�e�[�g (���Z�����p)
	ID3D11VertexShader*			g_pVertexShader;		// ���_�V�F�[�_
	ID3D11InputLayout*			g_pInputLayout;			// ���_�t�H�[�}�b�g
	ID3D11PixelShader*			g_pPixelShader;			// �s�N�Z���V�F�[�_]
	HitBox						hitbox;					// �q�b�g�{�b�N�X
	bool						bDontDependOnCamera;	// �J�����ŗ���Bool�^
	bool						bSetUpNoUIFilter;		// �t�B���^�[�̐ݒ�̕ϐ�
	bool						bTextureNotInTextureManager;
	D3D11_FILTER				d3d11CurrentFilter;		// �g���Ă���t�B���^�[
	Sprite*						Parent;					// �e�̐擪�A�h���X
	Sprite*						Childs[MAX_SPRITE_CHILDS];					// �e�̐擪�A�h���X
	int							nPolygonID;
#ifdef USE_CONSOLE
	char TextureName[256];
#endif
public:
	ID3D11ShaderResourceView    *gpTexture;				// �e�N�X�`��

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
// �v���g�^�C�v�錾
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