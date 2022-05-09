#pragma once
#include<DirectXMath.h>
#include "Math/Vector3.h"

/// <summary>
/// �J����
/// </summary>
class Camera
{
public://�G�C���A�X
	using XMMATRIX = DirectX::XMMATRIX;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMVECTOR = DirectX::XMVECTOR;

public://�����o�֐�
	/// <summary>
	/// ������
	/// </summary>
	/// /// <param name="window_width">��ʕ�</param>
	/// <param name="window_height">��ʍ���</param>
	void Initialize(int window_width, int window_height);

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update();

	/// <summary>
    /// �r���[�s����X�V
    /// </summary>
	void UpdateViewMatrix();

	/// <summary>
    /// �ˉe�s����X�V
    /// </summary>
	void UpdateProjectionMatrix();

	/// <summary>
    /// �r���{�[�h�s��̎擾
    /// </summary>
    /// <returns>�r���{�[�h�s��</returns>
	inline const XMMATRIX& GetBillboardMatrix() {
		return matBillboard;
	}

	virtual void AddPhi(const float angle) = 0;

	virtual void ResetCamera() = 0;

	/// <summary>
	/// �x�N�g���ɂ�鎋�_�ړ�
	/// </summary>
	/// <param name="move">�ړ���</param>
	void MoveEyeVector(const XMFLOAT3& move);
	void MoveEyeVector(const XMVECTOR& move);

	/// <summary>
	/// �x�N�g���ɂ��ړ�
	/// </summary>
	/// <param name="move">�ړ���</param>
	void MoveVector(const XMFLOAT3& move);
	void MoveVector(const XMVECTOR& move);

public://�ÓI�����o�֐�
	//getter
	const XMMATRIX& GetMatView() { return matView; }
	const XMMATRIX& GetMatProjection() { return matProjection; }
	const Vector3& GetEye() { return eye; }
	const Vector3& GetTarget() { return target; }
	const Vector3& GetUp() { return up; }
	const XMMATRIX& GetInverseView() {return InverseMatrix; }
	const bool& GetDirtyFlag() { return viewDirty; }

	inline const XMMATRIX& GetViewProjectionMatrix() {
		return matViewProjection;
	}

public://�����o�֐�
	//setter
	void SetMatView(const XMMATRIX& matView) { this->matView = matView; }
	void SetMatProjeciton(const XMMATRIX& matProjection) { this->matProjection = matProjection; }
	void SetEye(const Vector3& eye) { this->eye = eye; }
	void SetTarget(Vector3 target) { this->target = target; }
	void SetUp(const Vector3& up) { this->up = up; }
	
	/// <summary>���b�N�I���J����</summary>�@
    /// <param name = "targetPos">���b�N�I���Ώێ҈ʒu</param>
    /// <param name = "subPos">���C���L�����̈ʒu</param>
	void LockOn(Vector3 targetPos, Vector3 subPos, Vector3 cameraPosition);

	/// <summary>������x�N�g���v�Z</summary>
	void CalUp(const Vector3 vec);

protected://�����o�ϐ�
	XMMATRIX matView;            //�r���[�s��
	XMMATRIX InverseMatrix;      //�r���[�t�s��
	XMMATRIX matProjection;      //�ˉe�s��
	Vector3 eye;                //���_���W
	Vector3 target;             //���ӓ_���W
	Vector3 up;                 //������x�N�g��
		// �r���{�[�h�s��
	XMMATRIX matBillboard = DirectX::XMMatrixIdentity();
	// Y�����r���{�[�h�s��
	XMMATRIX matBillboardY = DirectX::XMMatrixIdentity();
	//�r���[�ˉe�s��
	XMMATRIX matViewProjection = DirectX::XMMatrixIdentity();
	// �r���[�s��_�[�e�B�t���O
	bool viewDirty = false;
	// �A�X�y�N�g��
	float aspectRatio = 1.0f;
};

