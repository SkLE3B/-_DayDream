#pragma once
#include<d3d12.h>
#include<d3dx12.h>
#include<wrl.h>
#include<DirectxMath.h>
#include"TextureManager.h"

/// <summary>
/// �X�v���C�g
/// </summary>
class Sprite
{
public://�G�C���A�X
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

protected://�T�u�N���X

	class Common {

		friend class Sprite;
		friend class PostEffect;
		friend class Fog;
		friend class Fog;
	public:
		void InitializeGraphicsPipeline();
	protected:
		TextureManager* textureManager = nullptr;
		Direcx12Base* dxBase = nullptr;
		ComPtr<ID3D12RootSignature> RootSignature; //���[�g�V�O�l�`��
		ComPtr<ID3D12PipelineState> PipelineState; //�p�C�v���C���X�e�[�g
		XMMATRIX MatProjection{}; //�ˉe�s��
	};

	//�X�v���C�g�f�[�^�\��
	struct VertexPosUv
	{
		DirectX::XMFLOAT3 pos; //xyz���W
		DirectX::XMFLOAT2 uv;  //uv���W
	};

	//�萔�p�o�b�t�@�ɑ���f�[�^�\����
	struct ConstBuffurData
	{
		DirectX::XMFLOAT4 color; //�F(RGBA)
		DirectX::XMMATRIX mat;   //3D�ϊ��s��
		float Time;              //����
	};

public://�ÓI�����o�֐�

	/// <summary>
	/// �ÓI�����o�̏�����
	/// </summary>
	/// <param name="dxBase"></param>
	static void StaticInitialize(Direcx12Base* dxBase, TextureManager* textureManager);

	/// <summary>
	/// �ÓI�����o�̉��
	/// </summary>
	static void StaticFinalize();

	/// <summary>
	/// �O���t�B�b�N�X�p�C�v���C���̃Z�b�g
	/// </summary>
	/// <param name="cmdList"></param>
	static void SetPipelineState(ID3D12GraphicsCommandList* cmdList);

protected://�ÓI�����o�ϐ�
	/// <summary>
	/// �ÓI�����o�̏�����
	/// </summary>
	static Common* common;
	 
	// ���_��
	static const int vertNum = 4;

	// �f�o�C�X
	static ID3D12Device* device;
public://�����o�֐�

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="texnumber">�e�N�X�`���ԍ�</param>
	void Initialize(UINT texnumber);

	/// <summary>
	/// ���_�o�b�t�@�Ƀf�[�^�]��
	/// </summary>
	void TransferVertices();

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="cmdList"></param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="cmdList">�`��R�}���h���X�g</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �`��㏈��
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// �T�C�Y�ݒ�
	/// </summary>
	/// <param name="width">��</param>
	/// <param name="height">����</param>
	void SetSize(float width, float height);

	/// <summary>
	/// �p�x�ݒ�
	/// </summary>
	/// <param name="rotation">�p�x</param>
	void SetRotation(float rotation);

	/// <summary>
	/// �A���J�[�|�C���g�ݒ�
	/// </summary>
	/// <param name="anchorpoint">�A���J�[�|�C���g</param>
	void SetAnchorPoint(XMFLOAT2 anchorpoint);

	/// <summary>
	/// ���W�ݒ�
	/// </summary>
	/// <param name="x">X���W</param>
	/// <param name="y">Y���W</param>
	void SetPosition(float x, float y);

	/// <summary>
	/// ���E���]�̐ݒ�
	/// </summary>
	/// <param name="isFlipX">���E���]</param>
	void SetIsFlipX(bool isFlipX);

	/// <summary>
	/// �㉺���]�̐ݒ�
	/// </summary>
	/// <param name="isFlipY">�㉺���]</param>
	void SetIsFlipY(bool isFlipY);

	/// <summary>
	/// �e�N�X�`���͈͎w��
	/// </summary>
	/// <param name="tex_x">�e�N�X�`�����W</param>
	/// <param name="tex_width">�e�N�X�`���T�C�Y</param>
	void SetTextureRange(float tex_x, float tex_y, float tex_width, float tex_height);

	/// <summary>
	/// �e�N�X�`���͈͎w��
	/// </summary>
	/// <param name="tex_x">�e�N�X�`��X���W</param>
	/// <param name="tex_y">�e�N�X�`��Y���W</param>
	/// <param name="tex_width">�e�N�X�`�����T�C�Y</param>
	/// <param name="tex_height">�e�N�X�`���c�T�C�Y</param>
	void SetTextureRect(float tex_x, float tex_y, float tex_width, float tex_height);

	/// <summary>
	/// ���W�擾
	/// </summary>
	/// <returns></returns>
	XMFLOAT2 Getposition();

	///<summary>
    ///alpha�̒l�ύX
    /// </summary>
    /// <param name="alpha">��</param>
	void SetAlpha(float alpha) { Color.w = alpha; }

	/// <summary>
	/// �t�F�[�h�C��
	/// </summary>
	void Fadein(const std::string& swich);

	/// <summary>
	/// �t�F�[�h�A�E�g
	/// </summary>
	void Fadeout(const std::string& swich);

	/// <summary>
	/// �t�F�[�h�A�E�g
	/// </summary>
	void FadeOut();

	/// <summary>
	/// �t�F�[�h�A�E�g�t���O�؂�ւ�
	/// </summary>
	/// <returns>�t�F�[�h�A�E�g�t���O</returns>
	bool& ChengeFadeOutFlag() { return fadeOutFlag = !fadeOutFlag; 
	}

	float getAlpha() { return Color.w; }

	std::string getSwich(const std::string& swich) { return swich; }

	bool isMin() {
		return getAlpha() <= 0.0f;
	}

	bool isMax() {
		return getAlpha() >= 1.0f;
	}

protected://�����o�ϐ�
	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// �R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	//Z���܂��̉�]�p
	float rotation = 0.0f;
	//���W
	XMVECTOR position{};
	//���[���h�s��
	XMMATRIX spriteMatWorld{};
	//�F
	XMFLOAT4 Color = {1,1,1,1};
	//�e�N�X�`���ԍ�
	UINT texnumber = 0;
	//�A���J�[�|�C���g
	XMFLOAT2 anchorpoint = {};
	const float limit = 1.0f;//���
	//����
	float width = 100.0f;
	float height = 100.0f;//�c��
	bool isFlipX = false;//���E���]
	bool isFlipY = false;//�㉺���]
	float tex_x = 0.0f;     //�e�N�X�`������@X���W
	float tex_y = 0.0f;     //�e�N�X�`������@Y���W
	float tex_width  = 100.0f; //�e�N�X�`������
	float tex_height = 100.0f;//�e�N�X�`���c��
	float time;
	bool fadeOutFlag;
};