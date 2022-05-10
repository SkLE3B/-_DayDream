#pragma once
#include "Camera.h"
#include "../Input/Input.h"
#include "../Player.h"

/// <summary>
/// �f�o�b�O�p�J����
/// </summary>
class window;
class DebugCamera :
    public Camera
{
    using XMMATRIX = DirectX::XMMATRIX;
public:
    DebugCamera();
    /// <summary>
    /// �R���X�g���N�^
    /// </summary>
    /// <param name="window_width">��ʕ�</param>
    /// <param name="window_height">��ʍ���</param>
    /// <param name="input">����</param>
    void Initialize(int window_width, int window_height, Input* input);

    //�X�V
    void Update() override;

    void SetDistance(float distance) {
        this->distance = distance; viewDirty = true;
    }

    void AddPhi(const float angle);
    
    void ResetCamera() override;
private:
    //���̓N���X�̃|�C���^
    Input* input;
    //�v���C���[�̃|�C���^
    //Player* player;
    //�J���������_�܂ł̋���
    float distance = 10;
    //�X�P�[�����O
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    //��]�s��
    XMMATRIX matRot = DirectX::XMMatrixIdentity();

    float cameraMoveSpeed = 21.0f;
    float phi;
    float theta;
};