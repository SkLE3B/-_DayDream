#include "TextureManager.h"

#include<cassert>

using namespace DirectX;

void TextureManager::Initialize(Direcx12Base* dxBase)
{
	HRESULT result = S_FALSE;
	//�����o�ϐ��ɋL�^
	this->dxBase = dxBase;

	//�f�X�N���v�^�q�[�v�̔z��𐶐�
	D3D12_DESCRIPTOR_HEAP_DESC descheapDesc = {};
	descheapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descheapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descheapDesc.NumDescriptors = maxTextureCount;
	result = dxBase->GetDevice()->CreateDescriptorHeap(&descheapDesc, IID_PPV_ARGS(&DescHeap));

	if (FAILED(result))
	{
		assert(0);
	}
}

void TextureManager::spriteLoadTexture(UINT texnumber, const wchar_t* filename)
{

	HRESULT result;

	////WiC�̃e�N�X�`�����[�h
	TexMetadata metadata{};
	ScratchImage scratchlmg{};

	result = LoadFromWICFile(
		filename,//�摜��
		WIC_FLAGS_NONE,
		&metadata, scratchlmg);
	if (FAILED(result))
	{
		assert(0);
	}

	const Image* img = scratchlmg.GetImage(0, 0, 0);

	//���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	CD3DX12_HEAP_PROPERTIES HEAP_PROPERTIES1 = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

	//�e�N�X�`���p�o�b�t�@�̐���
	result = dxBase->GetDevice()->CreateCommittedResource(
		&HEAP_PROPERTIES1,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureBuffer[texnumber]));
	if (FAILED(result))
	{
		assert(0);
	}

	//�e�N�X�`���o�b�t�@�̐���
	CD3DX12_HEAP_PROPERTIES texHeapProp{};//���_�q�[�v�ݒ�
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;//
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//�f�[�^�]��
	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = textureBuffer[texnumber]->WriteToSubresource(
		0,
		nullptr,//�S�̈�փR�s�[
		img->pixels,//���f�[�^�A�h���X
		(UINT)img->rowPitch,//1���C���T�C�Y
		(UINT)img->slicePitch//�S�T�C�Y
	);
	if (FAILED(result))
	{
		assert(0);
	}

	//�V�F�[�_�[���\�[�X�r���[����
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//�e�N�X�`��2D
	srvDesc.Texture2D.MipLevels = 1;

	////�q�[�v��2�ԖڂɃV�F�[�_�[���\�[�X�r���[�쐬
	dxBase->GetDevice()->CreateShaderResourceView(textureBuffer[texnumber].Get(),//�r���[�Ɗ֘A�Â���o�b�t�@
		&srvDesc,//�e�N�X�`���ݒ���
		CD3DX12_CPU_DESCRIPTOR_HANDLE(DescHeap->GetCPUDescriptorHandleForHeapStart(),
			texnumber, dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV))
	);
}

ID3D12Resource* TextureManager::GetSpriteTexbuff(UINT texnumber)
{
	//�z��I�[�o�[�t���[�h�~
	if (texnumber >= maxTextureCount)
	{
		return nullptr;
	}

	return textureBuffer[texnumber].Get();
}

void TextureManager::SetDescriptorHeaps(ID3D12GraphicsCommandList* cmdList)
{
	//�f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap* ppHeaps[] = {DescHeap.Get()};
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void TextureManager::SetShaderResourceView(ID3D12GraphicsCommandList* cmdList, UINT RootParameterIndex, UINT texnumber)
{
	//SRV�̃T�C�Y���擾
	UINT sizeSRV = dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//�f�X�N���v�^�e�[�u���̐擪���擾
	D3D12_GPU_DESCRIPTOR_HANDLE start = DescHeap->GetGPUDescriptorHandleForHeapStart();

	//SRV��GPU�n���h�����擾
	D3D12_GPU_DESCRIPTOR_HANDLE handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(start,texnumber, sizeSRV);

	//SRV���Z�b�g
	cmdList->SetGraphicsRootDescriptorTable(RootParameterIndex,handle);
}
