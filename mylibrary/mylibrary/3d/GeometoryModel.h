#pragma once

#include<DirectXMath.h>
#include<d3d12.h>
#include "../Base/Direcx12Base.h"
#include<wrl.h>
#include"../2d/TextureManager.h"

class GeometoryModel
{
public://�萔
	static const float topHeight;//���_����
	static const float R;//���a
	static const int n = 3;//n�p�`
	//�C���i�[�N���X
public:
	//���_�f�[�^�\����
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;//xyz���W
		DirectX::XMFLOAT3 normal;//�@���x�N�g��
		DirectX::XMFLOAT2 uv; //uv���W
	};
public://�����o�֐�

	//�C���f�b�N�X�z��̗v�f���𐔂���
	int CountIndices() { return _countof(indices); }

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="dxBase">Directx12�ׁ[�X</param>
	void Initialize(Direcx12Base* dxBase, TextureManager* textureManager, UINT texnumber);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//void Draw(Direcx12Base* dxBase);

	//getter
	const D3D12_VERTEX_BUFFER_VIEW& GetVbView() { return vbView; }

	//getter
	const D3D12_INDEX_BUFFER_VIEW& GetIbView() { return  ibView; }

private://�����o�ϐ�

	//�e�N�X�`���}�l�[�W��
	TextureManager* textureManager = nullptr;

	//�e�N�X�`���ԍ�
	UINT texnumber;

	// ���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;
	//�C���f�b�N�X�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource>indexBuff;

	//���_�o�b�n�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//�C���f�b�N�X�o�b�t�@�r��
	D3D12_INDEX_BUFFER_VIEW ibView{};

	//���_�f�[�^�z��
	Vertex vertices[n + 1 + 1] = {};
	//�C���f�b�N�X�f�[�^�z��
	unsigned short indices[n * n * 2];
};

