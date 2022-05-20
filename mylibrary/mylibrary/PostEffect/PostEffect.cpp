#include "PostEffect.h"
#include <d3dx12.h>
#include <d3dcompiler.h>
#include "../3d/Object3d.h"
#include "../GameBase.h"
#include "../Camera/DebugCamera.h"
#include "../Base/windowsApi.h"

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;

//�ÓI�����o�ϐ��̎��́@�@�@�@�@�@�@�@�@�@red   green blue alpha
const float PostEffect::clearColor[4] = {1.0f,1.0f,1.0f,1.0f };//�΂��ۂ��F

Camera* PostEffect::camera = nullptr;

PostEffect::PostEffect()
{
}

void PostEffect::Initialize(Camera* camera)
{
	HRESULT result;

	PostEffect::camera = camera;
	fadeOut = 1.0f;
	fadeOutFlag = false;
	//�p�C�v���C������
	CreateGraphicsPipelineState();

	CD3DX12_HEAP_PROPERTIES heapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resource = CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * vertNum);
	// ���_�o�b�t�@����
	result = common->dxBase->GetDevice()->CreateCommittedResource(
		&heapPro,
		D3D12_HEAP_FLAG_NONE,
		&resource,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//���_�f�[�^
	VertexPosUv vertices[vertNum] = {
		{{-1.0f,-1.0f,0.0f},{0.0f,1.0f}},//����
		{{-1.0f,+1.0f,0.0f},{0.0f,0.0f}},//����
		{{+1.0f,-1.0f,0.0f},{1.0f,1.0f}},//�E��
		{{+1.0f,+1.0f,0.0f},{1.0f,0.0f}},//�E��
	};

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	//���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexPosUv) * 4;
	vbView.StrideInBytes = sizeof(VertexPosUv);
	
	CD3DX12_RESOURCE_DESC resourceConstBuf = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBuffurData) + 0xff) & ~0xff);
	// �萔�o�b�t�@�̐���
	result = common->dxBase->GetDevice()->CreateCommittedResource(
		&heapPro, 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&resourceConstBuf,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

	//SRV�p�f�X�N���v�^�q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 3;
	
	//SRV�p�f�X�N���v�^�q�[�v
	result = common->dxBase->GetDevice()->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	assert(SUCCEEDED(result));

	CD3DX12_HEAP_PROPERTIES heapProTex = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
		D3D12_MEMORY_POOL_L0);
	CD3DX12_CLEAR_VALUE ClearTex = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor);

	//basetex
   {
	    //�e�N�X�`�����\�[�X�ݒ�
	    CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
	    	DXGI_FORMAT_R8G8B8A8_UNORM,
	    	windowsApi::window_width,
	    	(UINT)windowsApi::window_height,
	    	1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	    );
	    
	    //�e�N�X�`���o�b�t�@�̐���
	    result = common->dxBase->GetDevice()->CreateCommittedResource(
	    	&heapProTex,
	    	D3D12_HEAP_FLAG_NONE,
	    	&textureDesc,
	    	D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
	    	&ClearTex,
	    	IID_PPV_ARGS(&texBuff));
	    assert(SUCCEEDED(result));
	    
	    
	    //SRV�ݒ�
	    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	    srvDesc.Texture2D.MipLevels = 1;
	    
	    //�f�X�N���v�^�q�[�v(�V�F�[�_�[���\�[�X�r���[�̐���)
	    common->dxBase->GetDevice()->CreateShaderResourceView(texBuff.Get(),
	    	&srvDesc,
	    	descHeapSRV->GetCPUDescriptorHandleForHeapStart());
   }

   //tex0
   {
	   // WIC�e�N�X�`���̃��[�h
	   TexMetadata metadata{};
	   ScratchImage scratchImg{};

	   result = LoadFromWICFile(
		   L"Resources/textures/noize.png", WIC_FLAGS_NONE,
		   &metadata, scratchImg);
	   if (FAILED(result)) {
		   assert(0);
		   return;
	   }

	   const Image* img = scratchImg.GetImage(0, 0, 0); // ���f�[�^���o

	   // �摜�̐ݒ�
	   CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		   metadata.format,
		   metadata.width,
		   (UINT)metadata.height,
		   (UINT16)metadata.arraySize,
		   (UINT16)metadata.mipLevels
	   );

	   // �e�N�X�`���p�o�b�t�@�̐���
	   result = common->dxBase->GetDevice()->CreateCommittedResource(
		   &heapProTex,
		   D3D12_HEAP_FLAG_NONE,
		   &texresDesc,
		   D3D12_RESOURCE_STATE_GENERIC_READ, // �e�N�X�`���p�w��
		   nullptr,
		   IID_PPV_ARGS(&noiseBuff));

	   // �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	   result = noiseBuff->WriteToSubresource(
		   0,
		   nullptr, // �S�̈�փR�s�[
		   img->pixels,    // ���f�[�^�A�h���X
		   (UINT)img->rowPitch,  // 1���C���T�C�Y
		   (UINT)img->slicePitch // 1���T�C�Y
	   );
	   // �V�F�[�_���\�[�X�r���[�쐬
	   D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // �ݒ�\����
	   D3D12_RESOURCE_DESC resDesc = noiseBuff->GetDesc();
	   //�V�F�[�_�[���\�[�X�r���[�̐ݒ�
	   srvDesc.Format = resDesc.Format;
	   srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	   srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	   srvDesc.Texture2D.MipLevels = 1;

	   //�V�F�[�_�[���\�[�X�r���[�̐���
	   common->dxBase->GetDevice()->CreateShaderResourceView(noiseBuff.Get(),
		   &srvDesc,
		   CD3DX12_CPU_DESCRIPTOR_HANDLE(
			   descHeapSRV->GetCPUDescriptorHandleForHeapStart(), 1,
			   common->dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		   )
	   );
   }

   //tex1
   {
	   // WIC�e�N�X�`���̃��[�h
	   TexMetadata metadata{};
	   ScratchImage scratchImg{};

	   result = LoadFromWICFile(
		   L"Resources/textures/noise1.png", WIC_FLAGS_NONE,
		   &metadata, scratchImg);
	   if (FAILED(result)) {
		   assert(0);
		   return;
	   }

	   const Image* img = scratchImg.GetImage(0, 0, 0); // ���f�[�^���o

	   // �摜�̐ݒ�
	   CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		   metadata.format,
		   metadata.width,
		   (UINT)metadata.height,
		   (UINT16)metadata.arraySize,
		   (UINT16)metadata.mipLevels
	   );

	   // �e�N�X�`���p�o�b�t�@�̐���
	   result = common->dxBase->GetDevice()->CreateCommittedResource(
		   &heapProTex,
		   D3D12_HEAP_FLAG_NONE,
		   &texresDesc,
		   D3D12_RESOURCE_STATE_GENERIC_READ, // �e�N�X�`���p�w��
		   nullptr,
		   IID_PPV_ARGS(&noiseBuff1));

	   // �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	   result = noiseBuff1->WriteToSubresource(
		   0,
		   nullptr, // �S�̈�փR�s�[
		   img->pixels,    // ���f�[�^�A�h���X
		   (UINT)img->rowPitch,  // 1���C���T�C�Y
		   (UINT)img->slicePitch // 1���T�C�Y
	   );
	   // �V�F�[�_���\�[�X�r���[�쐬
	   D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // �ݒ�\����
	   D3D12_RESOURCE_DESC resDesc = noiseBuff1->GetDesc();
	   //�V�F�[�_�[���\�[�X�r���[�̐ݒ�
	   srvDesc.Format = resDesc.Format;
	   srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	   srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	   srvDesc.Texture2D.MipLevels = 1;

	   //�V�F�[�_�[���\�[�X�r���[�̐���
	   common->dxBase->GetDevice()->CreateShaderResourceView(noiseBuff1.Get(),
		   &srvDesc,
		   CD3DX12_CPU_DESCRIPTOR_HANDLE(
			   descHeapSRV->GetCPUDescriptorHandleForHeapStart(), 1,
			   common->dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		   )
	   );
   }

	//RTV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = 1;

	//RTV�p�f�X�N���v�^�q�[�v�𐶐�
	result = common->dxBase->GetDevice()->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(&descHeapRTV));
	assert(SUCCEEDED(result));

	//�f�X�N���v�^�q�[�v��RTV�쐬
	common->dxBase->GetDevice()->CreateRenderTargetView(texBuff.Get(),
		nullptr,
		descHeapRTV->GetCPUDescriptorHandleForHeapStart()
	);
	
	//�[�x�o�b�t�@���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC depthResDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			windowsApi::window_width,
			windowsApi::window_height,
			1, 0,
			1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);
	assert(SUCCEEDED(result));

	CD3DX12_HEAP_PROPERTIES depth = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_CLEAR_VALUE depthClear = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0.0f);
	//�[�x�o�b�t�@�̐���
	result = common->dxBase->GetDevice()->CreateCommittedResource(
		&depth,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&depthClear,
		IID_PPV_ARGS(&depthBuff));
	assert(SUCCEEDED(result));

	//DSV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC DescHeapDesc{};
	DescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DescHeapDesc.NumDescriptors = 1;
	//DSV�p�f�X�N���v�^�q�[�v���쐬
	result = common->dxBase->GetDevice()->CreateDescriptorHeap(&DescHeapDesc, IID_PPV_ARGS(&descHeapDSV));
	assert(SUCCEEDED(result));

	//�f�X�N���v�^�q�[�v��DSV�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//�[�x�l�t�H�[�}�b�g
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	common->dxBase->GetDevice()->CreateDepthStencilView(depthBuff.Get(),
		&dsvDesc,
		descHeapDSV->GetCPUDescriptorHandleForHeapStart());
	assert(SUCCEEDED(result));

	//�f�v�X�o�b�t�@���V�F�[�_�[���\�[�X�r���[�Ƃ��ĎQ�Ƃ��A�e�N�X�`���Ƃ��ăs�N�Z���V�F�[�_�[�Ŏg���B
	{
		//DSV�p�V�F�[�_���\�[�X�r���[�쐬
		D3D12_SHADER_RESOURCE_VIEW_DESC dsvSDesc{}; // �ݒ�\����
		D3D12_RESOURCE_DESC resSDesc = depthBuff->GetDesc();
		//DSV�p�V�F�[�_�[���\�[�X�r���[�̐ݒ�
		dsvSDesc.Format = DXGI_FORMAT_R32_FLOAT;
		dsvSDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		dsvSDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
		dsvSDesc.Texture2D.MipLevels = 1;

		////DSV�p�V�F�[�_�[���\�[�X�r���[�̐���
		common->dxBase->GetDevice()->CreateShaderResourceView(depthBuff.Get(),
			&dsvSDesc,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapSRV->GetCPUDescriptorHandleForHeapStart(), 1,
				common->dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			)
		);
		assert(SUCCEEDED(result));
	}
}

void PostEffect::PreDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	CD3DX12_RESOURCE_BARRIER resourceBarrierShaderToRender = CD3DX12_RESOURCE_BARRIER::Transition(texBuff.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	CD3DX12_RESOURCE_BARRIER resourceBarrierRenderToShader = CD3DX12_RESOURCE_BARRIER::Transition(depthBuff.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);

	//���\�[�X�o���A��ύX
	cmdList->ResourceBarrier(1,
		&resourceBarrierShaderToRender);

	//���\�[�X�o���A��ύX
	cmdList->ResourceBarrier(1,
		&resourceBarrierRenderToShader);

	//�����_�[�^�[�Q�b�g�r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH =
		descHeapRTV->GetCPUDescriptorHandleForHeapStart();
	//�[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		descHeapDSV->GetCPUDescriptorHandleForHeapStart();
	//�����_�[�^�[�Q�b�g���Z�b�g
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	CD3DX12_VIEWPORT viewPort = CD3DX12_VIEWPORT(0.0f, 0.0f,
		windowsApi::window_width, windowsApi::window_height);
	//�r���[�|�[�g�ݒ�
	cmdList->RSSetViewports(1, &viewPort);

	CD3DX12_RECT rect = CD3DX12_RECT(0, 0, windowsApi::window_width,
		windowsApi::window_height);
	//�V�U�����O�Z�`�ݒ�
	cmdList->RSSetScissorRects(1, &rect);

	//�S��ʃN���A
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	//�[�x�o�b�t�@�N���A
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0,
		nullptr);
}

void PostEffect::PostDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	CD3DX12_RESOURCE_BARRIER resourceBarrierRenderToShader = CD3DX12_RESOURCE_BARRIER::Transition(texBuff.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	CD3DX12_RESOURCE_BARRIER resourceBarrierShaderToRender = CD3DX12_RESOURCE_BARRIER::Transition(depthBuff.Get(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	//���\�[�X�o���A��ύX(�`��\�V�F�[�_�[���\�[�X)
	cmdList->ResourceBarrier(1, &resourceBarrierRenderToShader);

	//���\�[�X�o���A��ύX(�`��\�V�F�[�_�[���\�[�X)
	cmdList->ResourceBarrier(1, &resourceBarrierShaderToRender);
}

void PostEffect::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// �萔�o�b�t�@�Ƀf�[�^�]��
	NoiseConstBuffer* constMap = nullptr;
	HRESULT result = this->constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->time = this->time;
		constMap->FadeOut = fadeOut;
		TimeCount();
		FadeOut();
	    this->constBuff->Unmap(0, nullptr);
    }

	//�p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipelineState.Get());
	//���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(rootSignature.Get());
	//�v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// ���_�o�b�t�@�̐ݒ�
	cmdList->IASetVertexBuffers(0, 1, &this->vbView);
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	// �f�X�N���v�^�q�[�v���Z�b�g
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, this->constBuff->GetGPUVirtualAddress());
	// �V�F�[�_���\�[�X�r���[���Z�b�g
	cmdList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeapSRV->GetGPUDescriptorHandleForHeapStart(),
		0, common->dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
	// �V�F�[�_���\�[�X�r���[���Z�b�g
	cmdList->SetGraphicsRootDescriptorTable(2, CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeapSRV->GetGPUDescriptorHandleForHeapStart(),
		1, common->dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	// �`��R�}���h
	cmdList->DrawInstanced(4, 1, 0, 0);
}

void PostEffect::CreateGraphicsPipelineState()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;//���_�V�F�[�_�[�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob;//�G���[�I�u�W�F�N�g

	//���_�V�F�[�_�[�ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/Shaders/PostEffectVS.hlsl",//�V�F�[�_�[�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
		"main", "vs_5_0", //�G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		assert(0);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/Shaders/PostEffectPS.hlsl",	// �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0",	// �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		assert(0);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy���W(1�s�ŏ������ق������₷��)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv���W(1�s�ŏ������ق������₷��)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// �T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	// ���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// �f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS; // ��ɏ㏑�����[��

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// �u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	// �`��Ώۂ�1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

	CD3DX12_DESCRIPTOR_RANGE descRangeSRV1;
	descRangeSRV1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1 ���W�X�^

	// ���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparams[3];
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);

	// �X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT); // s0 ���W�X�^

	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));

	// ���[�g�V�O�l�`���̐���
	result = common->dxBase->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));

	gpipeline.pRootSignature = rootSignature.Get();

	// �O���t�B�b�N�X�p�C�v���C���̐���
	result = common->dxBase->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));
}

void PostEffect::TimeCount()
{
	time += 0.5f;

	if (time >= 60)
	{
		time = 0;
	}
}

void PostEffect::FadeOut()
{
	if (fadeOutFlag)
	{
		fadeOut -= 0.005f;

		if (fadeOut == 0)
		{
			ChengeFadeOutFlag();
			fadeOut = 1.0f;
			time = 0;
		}
	}
}