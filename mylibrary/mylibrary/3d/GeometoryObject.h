#pragma once
#include "../Base/Direcx12Base.h"
#include "GeometoryModel.h"
#include "../Camera/Camera.h"
#include "../Camera/BackCamera.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>

//�����Ő����������f���p3D�I�u�W�F�N�g
class GeometoryObject
{
public://�G�C���A�X
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public://�萔
	static const int maxObjectCount = 512;//3D�I�u�W�F�N�g�̍ő吔
public:
	//����
	class Common
	{
		friend class GeometoryObject;

	public:
		/// <summary>
		/// �O���t�B�b�N�X�p�C�v���C��������
		/// </summary>
		/// <param name="dxBase">DirectX���</param>
		void InitializeGraphicsPipeline(Direcx12Base* dxBase);

		/// <summary>
		/// �f�X�N���v�^�q�[�v������
		/// </summary>
		/// <param name="dxBase">DirectX���</param>
		void InitializeDescriptorHeap(Direcx12Base* dxBase);

		void InitializeCamera();

	private:
		//DirectX12���
		Direcx12Base* dxBase = nullptr;
		//���[�g�V�O�l�`��
		ComPtr<ID3D12RootSignature>rootsignature;
		//�f�X�N���v�^�q�[�v(�萔�o�b�t�@�r���[�p)
		ComPtr<ID3D12DescriptorHeap>basicDescHeap;
		//�p�C�v���C���X�e�[�g
		ComPtr<ID3D12PipelineState>pipelinestate;
		//���Ɏg���f�X�N���v�^�q�[�v
		int descHeapIndex = 0;
		//�J����
		Camera* camera = nullptr;
		//���f���f�[�^
		GeometoryModel* model = nullptr;
	};

	//�萔�p�o�b�t�@�ɑ���f�[�^�\����
	struct ConstBuffurData
	{
		XMFLOAT4 color; //�F(RGBA)
		XMMATRIX mat;   //3D�ϊ��s��
	};

public://�ÓI�����o�֐�
	/// <summary>
	/// �ÓI�����o�̏�����
	/// </summary>
	static void StaticInitialize(Direcx12Base* dxBase, GeometoryModel* model,Camera* camera);

	/// <summary>
	/// �ÓI�����o�ϐ��̏�����
	/// </summary>
	static void StaticFinalize();

	/// <summary>
	/// �f�X�N���v�^�q�[�v���Z�b�g
	/// </summary>
	static void ResetDescriptorHeap();

private://�ÓI�����o�ϐ�
	static Common* common;

public://�����o�֐�
	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//setter(�l�̐ݒ�)
	void SetPosition(const XMFLOAT3& position) { this->position = position; }
	void SetRotation(const XMFLOAT3& rotation) { this->rotation = rotation; }
	void SetScale(const XMFLOAT3& scale) { this->scale = scale;}

	//getter(�l�̎擾)
	XMFLOAT3& GetScale() { return scale; }
	XMFLOAT3& GetPosition() { return position; }
	XMFLOAT3& GetRotation() { return rotation; }

private://�����o�ϐ�
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource>constBuff;
	//�萔�o�b�t�@�r���[�̃n���h��(CPU)
	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleCBV;
	//�萔�o�b�t�@�r���[�̃n���h��(GPU)
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV;
	//�g�����X�t�H�[��
	XMFLOAT3 position = {0,0,0};//���W
	XMFLOAT3 scale = {1,1,1};   //�X�P�[�����O�{��
	XMFLOAT3 rotation = {0,0,0};//��]�p
	XMMATRIX matWorld;          //���[�J�����[���h�ϊ��s��
	GeometoryObject* parent = nullptr; //�e�I�u�W�F�N�g
};

