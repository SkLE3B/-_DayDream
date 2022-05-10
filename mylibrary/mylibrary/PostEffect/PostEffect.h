#pragma once
#include "../Camera/Camera.h"
#include "../2d/Sprite.h"

class PostEffect :
    public Sprite
{
private://�G�C���A�X
    template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    //Directx::���ȗ�
    using XMFLOAT2 = DirectX::XMFLOAT2;
    using XMFLOAT3 = DirectX::XMFLOAT3;
    using XMFLOAT4 = DirectX::XMFLOAT4;
    using XMMATRIX = DirectX::XMMATRIX;

public:
    struct NoiseConstBuffer
    {
        float time;
        float FadeOut;
    };

public://�����o�֐�
    /// <summary>
    /// �R���X�g���N�^
    /// </summary>
    PostEffect();

    /// <summary>
    /// ������
    /// </summary>
    void Initialize(Camera* camera);

    /// <summary>
    /// �V�[���`��O����
    /// </summary>
    /// <param name="cmdList">�R�}���h���X�g</param>
    void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

    /// <summary>
    /// �V�[���`��㏈��
    /// </summary>
    /// <param name="cmdList">�R�}���h���X�g</param>
    void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

    /// <summary>
    /// �`��R�}���h
    /// </summary>
    /// <param name="cmdList"></param>
    void Draw(ID3D12GraphicsCommandList* cmdList);

    /// <summary>
    /// �p�C�v���C������
    /// </summary>
    void CreateGraphicsPipelineState();

    /// <summary>
    /// �^�C�}�[�̃J�E���g��i�߂�
    /// </summary>
    void TimeCount();

    /// <summary>
    /// �t�F�[�h�A�E�g
    /// </summary>
    void FadeOut();

    /// <summary>
    /// �t�F�[�h�A�E�g�t���O�؂�ւ�
    /// </summary>
    /// <returns>�t�F�[�h�A�E�g�t���O</returns>
    bool& ChengeFadeOutFlag() { return fadeOutFlag = !fadeOutFlag; }

    bool& GetFadeOutFlag() { return fadeOutFlag; }

    void ResetFade(const float& fadeout, bool& fadeOutFlag) {
        fadeOut = fadeout;
        fadeOutFlag = false;
    }

private://�ÓI�����o�ϐ�
//��ʃN���A�J���[
    static const float clearColor[4];
    // �J����
    static Camera* camera;
private://�����o�ϐ�
    //�e�N�X�`���o�b�t�@
    ComPtr<ID3D12Resource> texBuff;
    ComPtr<ID3D12Resource> noiseBuff;
    ComPtr<ID3D12Resource> noiseBuff1;
    ComPtr<ID3D12Resource> noiseBuff2;
    ComPtr<ID3D12Resource> noiseBuff3;

    //�f�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap> descHeapSRV;
    //�[�x�o�b�t�@
    ComPtr<ID3D12Resource>       depthBuff;
    //RTV�p�f�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap> descHeapRTV;
    //DSV�p�f�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap> descHeapDSV;
    //�O���t�B�b�N�X�p�C�v���C��
    ComPtr<ID3D12PipelineState>  pipelineState;
    //���[�g�V�O�l�`��
    ComPtr<ID3D12RootSignature>  rootSignature;

    float    time = 0.0f;
    XMFLOAT4 animation = { 0,0,0,0 };
    XMMATRIX viewproj;//�r���[�v���W�F�N�V�����s��
    XMMATRIX world;//���[���h�s��
    XMFLOAT3 cameraPos;//�J�������W(���[���h���W)
    XMFLOAT4 distance;
    float    fadeOut;
    bool     fadeOutFlag;
};