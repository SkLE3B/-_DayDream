#pragma once

#include"../Base/Direcx12Base.h"
#include<d3d12.h>
#include<d3dx12.h>
#include<wrl.h>
#include<DirectXTex.h>

/// <summary>
/// �e�N�X�`���}�l�[�W���[
/// </summary>
class TextureManager
{
public://�G�C���A�X
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


public://�萔
	static const int maxTextureCount = 512;//�e�N�X�`���̍ő喇��

public://�����o�֐�
	/// <summary>
	/// ������
	/// </summary>
	/// <param name="dxBase"></param>
	void Initialize(Direcx12Base* dxBase);

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	/// <param name="texnumber">�e�N�X�`���ԍ�</param>
	/// <param name="filename">�e�N�X�`���t�@�C����</param>
	void spriteLoadTexture(UINT texnumber, const wchar_t* filename);

	/// <summary>
	/// �e�N�X�`���o�b�t�@�擾
	/// </summary>
	/// <param name="texnumber"></param>
	/// <returns>�e�N�X�`���o�b�t�@</returns>
	ID3D12Resource* GetSpriteTexbuff(UINT texnumber);

	/// <summary>
	/// �f�X�N���v�^�q�[�v���Z�b�g(�O���t�B�b�N�X�R�}���h)
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void SetDescriptorHeaps(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// SRV���Z�b�g(�O���t�B�b�N�X�R�}���h)
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	/// <param name="RootParameterIndex">���[�g�p�����[�^�̔ԍ�</param>
	/// <param name="texnumber">�e�N�X�`���ԍ�</param>
	void SetShaderResourceView(ID3D12GraphicsCommandList* cmdList, UINT RootParameterIndex, UINT texnumber);

	//getter
	ID3D12DescriptorHeap* GetDescHeap() {return DescHeap.Get(); }

private://�����o�ϐ�
	ComPtr<ID3D12DescriptorHeap> DescHeap;
	ComPtr<ID3D12Resource> textureBuffer[maxTextureCount];
	Direcx12Base* dxBase = nullptr;
};

