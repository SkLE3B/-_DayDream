#pragma once
#include "Model.h"
#include "Camera/Camera.h"
#include "FbxLoader.h"
#include "Collision/CollisionInfo.h"
#include "Math/Vector3.h"
#include "Collision/CollisionAttribute.h"
#include<windows.h>
#include<wrl.h>
#include<d3d12.h>
#include<d3dx12.h>
#include<DirectXMath.h>
#include<string>
#include "Math/Matrix4.h"

class BaseCollider;

class Object3d
{
protected://�G�C���A�X
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//Directx::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public://�萔
	//�{�[���̍ő吔
	static const int MAX_BONES = 32;
public://�T�u�N���X
	//�萔�o�b�t�@�p�f�[�^�\����(���W�ϊ��s��p)
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj; //�r���[�v���W�F�N�V�����s��
		XMMATRIX world;    //���[���h�s��
		XMFLOAT3 cameraPos;//�J�����s��(���[���h�s��)
		XMFLOAT4 Color;
	};

	//�萔�o�b�t�@�p�f�[�^����(�X�L�j���O)
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};

public://�����o�֐�
	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	virtual ~Object3d();

	/// <summary>
	/// ������
	/// </summary>
	virtual bool Initialize();

	/// <summary>
	/// ���t���[������
	/// </summary>
	virtual void Update();

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="cmdList"></param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="model">���f��</param>
	void SetModel(Model* model) { this->model = model; }

	/// <summary>
	/// �A�j���[�V�����J�n
	/// </summary>
	/// <param name="index">�A�j���[�V�����ԍ�</param>
	void PlayAnimation(const int& index, const int& num);

	/// <summary>
	/// �A�j���[�V�������Z�b�g
	/// </summary>
	void ResetAnimation();

	//setter
	void SetPosition(const Vector3& position) { this->position = position; }
	void SetRotation(const Vector3& rotation) { this->rotation = rotation; }
	void SetScale(const Vector3& scale) { this->scale = scale; }

	//getter
	Vector3& GetPosition() { return position; }
	Vector3& GetRotation() { return rotation; }
	Vector3& GetScale() { return scale; }

	bool GetIsPlayed() { return isPlayed;}

	/// <summary>
	/// �s��̍X�V
	/// </summary>
	void UpdateWorldMatrix();

	/// <summary>
	/// ���[���h�s��̎擾
	/// </summary>
	/// <returns>���[���h�s��</returns>
	//const XMMATRIX& GetMatWorld() { return matWorld; }	
	const Matrix4& GetMatWorld() { return matWorld; }

	inline Matrix4  Matrix4Identity()
	{
		Matrix4 M;
		M.r[0] = { 1.0f, 0.0f, 0.0f, 0.0f};
		M.r[1] = { 0.0f, 1.0f, 0.0f, 0.0f};
		M.r[2] = { 0.0f, 0.0f, 1.0f, 0.0f};
		M.r[3] = { 0.0f, 0.0f, 0.0f, 1.0f};

		return M;
	}

	/// <summary>
	/// �R���C�_�[�̃Z�b�g
	/// </summary>
	/// <param name="collider">�R���C�_�[</param>
	void SetCollider(BaseCollider* collider,const unsigned short collisionAttribute);

	/// <summary>
	/// �Փˎ��R�[���o�b�N�֐�
	/// </summary>
	/// <param name="info">�Փˏ��</param>
	virtual void OnCollision(const CollisionInfo& info) {}

	/// <summary>
    /// ���f�����擾
    /// </summary>
    /// <returns>���f��</returns>
	inline Model* GetModel() { return model; }

	///<summary>
    ///alpha�̒l�ύX
    /// </summary>
    /// <param name="alpha">��</param>
	void SetAlpha(float alpha) { color.w = alpha; }

public://�ÓI�����o�֐�
	/// <summary>
	/// �O���t�B�b�N�X�p�C�v���C������
	/// </summary>
	static void CreateGraphicsPipeline();
	//setter
	static void SetDevice(ID3D12Device* device) { Object3d::device = device; }
	static void SetCamera(Camera* camera) { Object3d::camera = camera; }
private://�ÓI�����o�ϐ�
	static ID3D12Device* device;
	static Camera* camera;
	//���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootsignature;
	//�p�C�v���C���X�e�[�g
	static ComPtr<ID3D12PipelineState> pipelinestate;
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffTransform;
    //�萔�o�b�t�@(�X�L��)
	ComPtr<ID3D12Resource> constBuffSkin;
protected:
	//���[�J���X�P�[��
	Vector3 scale = { 1,1,1 };
	//X,Y,Z������̃��[�J����]�p
	Vector3 rotation = { 0,0,0 };
	//���[�J�����W
	Vector3 position = { 0,0,0 };
	//�F
	XMFLOAT4 color = { 1,1,1,1 };
	//���[�J�����[���h�ϊ����W
	//XMMATRIX matWorld;
	Matrix4 matWorld;
	//1�t���[���̎���
	FbxTime frameTime;
	//�A�j���[�V�����J�n����
	FbxTime startTime;
	//�A�j���[�W�����I������
	FbxTime endTime;
	//���ݎ���(�A�j���[�V����)
	FbxTime currentTime;
	//�A�j���[�V�����Đ���
	bool isPlay = false;
	bool isPlayBoss = false;
	bool isPlayed = false;
	//���f��
	Model* model = nullptr;
	const char* name = nullptr;
	// �R���C�_�[
	BaseCollider* collider = nullptr;
	int animeNum;
};

