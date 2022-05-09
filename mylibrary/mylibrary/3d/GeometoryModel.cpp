#include "GeometoryModel.h"
#include <vector>

using namespace DirectX;

const float GeometoryModel::topHeight = 3.0f; //���_����
const float GeometoryModel::R = 5.0f;//���a

void GeometoryModel::Initialize(Direcx12Base* dxBase, TextureManager* textureManager, UINT texnumber)
{
	this->textureManager = textureManager;
	this->texnumber = texnumber;

	HRESULT result = S_FALSE;

	//���_�f�[�^
	Vertex vertices[n + 1 + 1] = {};

	for (int i = 0; i < n; i++)
	{
		vertices[i].pos.x = R * sinf(XM_2PI / 3 * i);
		vertices[i].pos.y = R * cosf(XM_2PI / 3 * i);
		vertices[i].pos.z = 0.0f;
	}

	vertices[3].pos.x = 0.0f;
	vertices[3].pos.y = 0.0f;
	vertices[3].pos.z = 0.0f;

	vertices[4].pos.x = 0.0f;
	vertices[4].pos.y = 0.0f;
	vertices[4].pos.z = topHeight;

	for (int i = 0; i < n; i++)
	{
		vertices[i].normal = {};
		vertices[i].uv = { 0.0f,0.0f };
	}

	vertices[4].uv = { 1.0f,1.0f };

	for (int i = 0; i < n; i++)
	{
		//�|���S���̃C���f�b�N�X
		indices[3 * i + 0] = i;//�Y����0�ɃC���f�b�N�X0����
		indices[3 * i + 1] = i + 1;//�Y����1�ɃC���f�b�N�X�P����
		indices[3 * i + 2] = n;
	}
	indices[n * 3 - 2] = 0;

	for (int i = 0; i < n; i++)
	{
		//�|���S���̃C���f�b�N�X
		indices[3 * i + 9] = i + 1;//�Y����0�ɃC���f�b�N�X0����
		indices[3 * i + 10] = i;//�Y����1�ɃC���f�b�N�X�P����
		indices[3 * i + 11] = n + 1;
	}
	indices[n * 6 - 3] = 0;

	//�@�����v�Z����
	for (int i = 0; i < 18 / 3; i++)
	{//�O�p�`1���ƂɌv�Z����
		//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�Ă��ȕϐ��ɓ����
		unsigned short indices0 = indices[i * 3 + 0];
		unsigned short indices1 = indices[i * 3 + 1];
		unsigned short indices2 = indices[i * 3 + 2];
		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
		XMVECTOR p0 = XMLoadFloat3(&vertices[indices0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[indices1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[indices2].pos);
		//p0��p1�x�N�g��,p0��p2�x�N�g�����v�Z(�x�N�g�����Z)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//�O�ς͗������琂���ȃx�N�g��
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//���K��(�������P�ɂ���)
		normal = XMVector3Normalize(normal);
		//���߂��{���𒸓_�f�[�^�ɑ��
		XMStoreFloat3(&vertices[indices0].normal, normal);
		XMStoreFloat3(&vertices[indices1].normal, normal);
		XMStoreFloat3(&vertices[indices2].normal, normal);
	}

	CD3DX12_HEAP_PROPERTIES HEAP_PROPERTIES = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	CD3DX12_RESOURCE_DESC RESOURCE_DESC = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices));

	//���_�o�b�t�@�̐���
	result = dxBase->GetDevice()->CreateCommittedResource(
		&HEAP_PROPERTIES,//�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&RESOURCE_DESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	//�C���f�b�N�X�o�b�t�@�̐���
	result = dxBase->GetDevice()->CreateCommittedResource(
		&HEAP_PROPERTIES,//�A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&RESOURCE_DESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//���_�o�b�n�ւ̃f�[�^�]��
	// GPU ��̃o�b�t�@�ɑΉ��������z���������擾
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	// �S���_�ɑ΂���
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i]; // ���W���R�s�[
	}

	// �}�b�v������
	vertBuff->Unmap(0, nullptr);

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//�S�C���f�b�N�X�ɑ΂���
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];//�C���f�b�N�X���R�s�[
	}
	indexBuff->Unmap(0, nullptr);

	//���_�o�b�n�r���[�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);
}

void GeometoryModel::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//���_�o�b�t�@�̐ݒ�
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	//�C���f�b�N�X�o�b�t�@�̐ݒ�
	cmdList->IASetIndexBuffer(&ibView);
	//�f�X�N���v�^�q�[�v�̔z��	
	textureManager->SetDescriptorHeaps(cmdList);
	//�V�F�[�_�[���\�[�X�r���[���Z�b�g
	textureManager->SetShaderResourceView(cmdList, 1, texnumber);
	//�`��R�}���h
	cmdList->DrawIndexedInstanced(CountIndices(), 1, 0, 0, 0);
}

