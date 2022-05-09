﻿#include "Fog.h"
#include "Base/windowsApi.h"
#include <d3dx12.h>
#include <d3dcompiler.h>
#include "3d/Object3d.h"
#include "GameBase.h"
#include "Camera/DebugCamera.h"

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;

//静的メンバ変数の実体　　　　　　　　　　red   green blue alpha
const float Fog::clearColor[4] = {1.0f,1.0f,1.0f,1.0f };//白色

Camera* Fog::camera = nullptr;

Fog::Fog()
{
}

void Fog::Initialize(Camera* camera)
{
	HRESULT result;

	Fog::camera = camera;

	//パイプライン生成
	CreateGraphicsPipelineState();

	fadeOut = 0.5f;
	fadeOutFlag = false;

	// 頂点バッファ生成
	result = common->dxBase->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * vertNum),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//頂点データ
	VertexPosUv vertices[vertNum] = {
		{{-1.0f,-1.0f,0.0f},{0.0f,1.0f}},//左下
		{{-1.0f,+1.0f,0.0f},{0.0f,0.0f}},//左上
		{{+1.0f,-1.0f,0.0f},{1.0f,1.0f}},//右下
		{{+1.0f,+1.0f,0.0f},{1.0f,0.0f}},//右上
	};

	// 頂点バッファへのデータ転送
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	//頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexPosUv) * 4;
	vbView.StrideInBytes = sizeof(VertexPosUv);

	// 定数バッファの生成
	result = common->dxBase->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBuffurData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

	//SRV用デスクリプタヒープ
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 3;

	//SRV用デスクリプタヒープ
	result = common->dxBase->GetDevice()->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	assert(SUCCEEDED(result));

	//basetex
	{
		//テクスチャリソース設定
		CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			windowsApi::window_width,
			(UINT)windowsApi::window_height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		//テクスチャバッファの生成
		result = common->dxBase->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
				D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor),
			IID_PPV_ARGS(&texBuff));
		assert(SUCCEEDED(result));


		//SRV設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		//デスクリプタヒープ(シェーダーリソースビューの生成)
		common->dxBase->GetDevice()->CreateShaderResourceView(texBuff.Get(),
			&srvDesc,
			descHeapSRV->GetCPUDescriptorHandleForHeapStart());
	}

	//tex0
	{
		// WICテクスチャのロード
		TexMetadata metadata{};
		ScratchImage scratchImg{};

		result = LoadFromWICFile(
			L"Resources/textures/noize.png", WIC_FLAGS_NONE,
			&metadata, scratchImg);
		if (FAILED(result)) {
			assert(0);
			return;
		}

		const Image* img = scratchImg.GetImage(0, 0, 0); // 生データ抽出

		// 画像の設定
		CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			metadata.format,
			metadata.width,
			(UINT)metadata.height,
			(UINT16)metadata.arraySize,
			(UINT16)metadata.mipLevels
		);

		// テクスチャ用バッファの生成
		result = common->dxBase->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, // テクスチャ用指定
			nullptr,
			IID_PPV_ARGS(&noiseBuff));

		// テクスチャバッファにデータ転送
		result = noiseBuff->WriteToSubresource(
			0,
			nullptr, // 全領域へコピー
			img->pixels,    // 元データアドレス
			(UINT)img->rowPitch,  // 1ラインサイズ
			(UINT)img->slicePitch // 1枚サイズ
		);
		// シェーダリソースビュー作成
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
		D3D12_RESOURCE_DESC resDesc = noiseBuff->GetDesc();
		//シェーダーリソースビューの設定
		srvDesc.Format = resDesc.Format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
		srvDesc.Texture2D.MipLevels = 1;

		//シェーダーリソースビューの生成
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
		// WICテクスチャのロード
		TexMetadata metadata{};
		ScratchImage scratchImg{};

		result = LoadFromWICFile(
			L"Resources/textures/noise1.png", WIC_FLAGS_NONE,
			&metadata, scratchImg);
		if (FAILED(result)) {
			assert(0);
			return;
		}

		const Image* img = scratchImg.GetImage(0, 0, 0); // 生データ抽出

		// 画像の設定
		CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			metadata.format,
			metadata.width,
			(UINT)metadata.height,
			(UINT16)metadata.arraySize,
			(UINT16)metadata.mipLevels
		);

		// テクスチャ用バッファの生成
		result = common->dxBase->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, // テクスチャ用指定
			nullptr,
			IID_PPV_ARGS(&noiseBuff1));

		// テクスチャバッファにデータ転送
		result = noiseBuff1->WriteToSubresource(
			0,
			nullptr, // 全領域へコピー
			img->pixels,    // 元データアドレス
			(UINT)img->rowPitch,  // 1ラインサイズ
			(UINT)img->slicePitch // 1枚サイズ
		);
		// シェーダリソースビュー作成
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
		D3D12_RESOURCE_DESC resDesc = noiseBuff1->GetDesc();
		//シェーダーリソースビューの設定
		srvDesc.Format = resDesc.Format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
		srvDesc.Texture2D.MipLevels = 1;

		//シェーダーリソースビューの生成
		common->dxBase->GetDevice()->CreateShaderResourceView(noiseBuff1.Get(),
			&srvDesc,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapSRV->GetCPUDescriptorHandleForHeapStart(), 1,
				common->dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			)
		);
	}

	//tex2
	{
		// WICテクスチャのロード
		TexMetadata metadata{};
		ScratchImage scratchImg{};

		result = LoadFromWICFile(
			L"Resources/textures/noise2.png", WIC_FLAGS_NONE,
			&metadata, scratchImg);
		if (FAILED(result)) {
			assert(0);
			return;
		}

		const Image* img = scratchImg.GetImage(0, 0, 0); // 生データ抽出

		// 画像の設定
		CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			metadata.format,
			metadata.width,
			(UINT)metadata.height,
			(UINT16)metadata.arraySize,
			(UINT16)metadata.mipLevels
		);

		// テクスチャ用バッファの生成
		result = common->dxBase->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, // テクスチャ用指定
			nullptr,
			IID_PPV_ARGS(&noiseBuff2));

		// テクスチャバッファにデータ転送
		result = noiseBuff2->WriteToSubresource(
			0,
			nullptr, // 全領域へコピー
			img->pixels,    // 元データアドレス
			(UINT)img->rowPitch,  // 1ラインサイズ
			(UINT)img->slicePitch // 1枚サイズ
		);
		// シェーダリソースビュー作成
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
		D3D12_RESOURCE_DESC resDesc = noiseBuff2->GetDesc();
		//シェーダーリソースビューの設定
		srvDesc.Format = resDesc.Format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
		srvDesc.Texture2D.MipLevels = 1;

		//シェーダーリソースビューの生成
		common->dxBase->GetDevice()->CreateShaderResourceView(noiseBuff2.Get(),
			&srvDesc,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapSRV->GetCPUDescriptorHandleForHeapStart(), 1,
				common->dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			)
		);
	}

	//tex3
	{
		// WICテクスチャのロード
		TexMetadata metadata{};
		ScratchImage scratchImg{};

		result = LoadFromWICFile(
			L"Resources/textures/noise3.png", WIC_FLAGS_NONE,
			&metadata, scratchImg);
		if (FAILED(result)) {
			assert(0);
			return;
		}

		const Image* img = scratchImg.GetImage(0, 0, 0); // 生データ抽出

		// 画像の設定
		CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			metadata.format,
			metadata.width,
			(UINT)metadata.height,
			(UINT16)metadata.arraySize,
			(UINT16)metadata.mipLevels
		);

		// テクスチャ用バッファの生成
		result = common->dxBase->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, // テクスチャ用指定
			nullptr,
			IID_PPV_ARGS(&noiseBuff3));

		// テクスチャバッファにデータ転送
		result = noiseBuff3->WriteToSubresource(
			0,
			nullptr, // 全領域へコピー
			img->pixels,    // 元データアドレス
			(UINT)img->rowPitch,  // 1ラインサイズ
			(UINT)img->slicePitch // 1枚サイズ
		);
		// シェーダリソースビュー作成
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
		D3D12_RESOURCE_DESC resDesc = noiseBuff3->GetDesc();
		//シェーダーリソースビューの設定
		srvDesc.Format = resDesc.Format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
		srvDesc.Texture2D.MipLevels = 1;

		//シェーダーリソースビューの生成
		common->dxBase->GetDevice()->CreateShaderResourceView(noiseBuff3.Get(),
			&srvDesc,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapSRV->GetCPUDescriptorHandleForHeapStart(), 1,
				common->dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			)
		);
	}

	//RTV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = 1;

	//RTV用デスクリプタヒープを生成
	result = common->dxBase->GetDevice()->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(&descHeapRTV));
	assert(SUCCEEDED(result));

	//デスクリプタヒープにRTV作成
	common->dxBase->GetDevice()->CreateRenderTargetView(texBuff.Get(),
		nullptr,
		descHeapRTV->GetCPUDescriptorHandleForHeapStart()
	);

	//深度バッファリソース設定
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

	//深度バッファの生成
	result = common->dxBase->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0.0f),
		IID_PPV_ARGS(&depthBuff));
	assert(SUCCEEDED(result));

	//DSV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC DescHeapDesc{};
	DescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DescHeapDesc.NumDescriptors = 1;
	//DSV用デスクリプタヒープを作成
	result = common->dxBase->GetDevice()->CreateDescriptorHeap(&DescHeapDesc, IID_PPV_ARGS(&descHeapDSV));
	assert(SUCCEEDED(result));

	//デスクリプタヒープにDSV作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	common->dxBase->GetDevice()->CreateDepthStencilView(depthBuff.Get(),
		&dsvDesc,
		descHeapDSV->GetCPUDescriptorHandleForHeapStart());
	assert(SUCCEEDED(result));

	//深度バッファをシェーダーリソースビューとして参照し、テクスチャとしてピクセルシェーダーで使う。
	{
		//DSV用シェーダリソースビュー作成
		D3D12_SHADER_RESOURCE_VIEW_DESC dsvSDesc{}; // 設定構造体
		D3D12_RESOURCE_DESC resSDesc = depthBuff->GetDesc();
		//DSV用シェーダーリソースビューの設定
		dsvSDesc.Format = DXGI_FORMAT_R32_FLOAT;
		dsvSDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		dsvSDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
		dsvSDesc.Texture2D.MipLevels = 1;

		////DSV用シェーダーリソースビューの生成
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

void Fog::PreDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	//リソースバリアを変更
	cmdList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(texBuff.Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET));

	//リソースバリアを変更
	cmdList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(depthBuff.Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_DEPTH_WRITE));

	//レンダーターゲットビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH =
		descHeapRTV->GetCPUDescriptorHandleForHeapStart();
	//深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		descHeapDSV->GetCPUDescriptorHandleForHeapStart();
	//レンダーターゲットをセット
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	//ビューポート設定
	cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f,
		windowsApi::window_width, windowsApi::window_height));
	//シザリング短形設定
	cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, windowsApi::window_width,
		windowsApi::window_height));

	//全画面クリア
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	//深度バッファクリア
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0,
		nullptr);
}

void Fog::PostDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	//リソースバリアを変更(描画可能シェーダーリソース)
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(texBuff.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	//リソースバリアを変更(描画可能シェーダーリソース)
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(depthBuff.Get(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}

void Fog::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// 定数バッファにデータ転送
	FogConstBuffer* constMap = nullptr;
	HRESULT result = this->constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->viewproj = camera->GetViewProjectionMatrix();
		constMap->animation = this->animation;
		constMap->inverse = camera->GetInverseView();
		constMap->matWorld = world;
		constMap->Time = time;
		constMap->FadeOut = this->fadeOut;

		TimeCount();
		FadeOut();

		this->constBuff->Unmap(0, nullptr);
	}

	//パイプラインステートの設定
	cmdList->SetPipelineState(pipelineState.Get());
	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootSignature.Get());
	//プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// 頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &this->vbView);
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	// デスクリプタヒープをセット
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, this->constBuff->GetGPUVirtualAddress());
	// シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeapSRV->GetGPUDescriptorHandleForHeapStart(),
		0, common->dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
	// シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(2, CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeapSRV->GetGPUDescriptorHandleForHeapStart(),
		1, common->dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	// 描画コマンド
	cmdList->DrawInstanced(4, 1, 0, 0);
}

void Fog::CreateGraphicsPipelineState()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;//頂点シェーダーオブジェクト
	ComPtr<ID3DBlob> psBlob;//ピクセルシェーダーオブジェクト
	ComPtr<ID3DBlob> errorBlob;//エラーオブジェクト

	//頂点シェーダー読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/MainFogVS.hlsl",//シェーダーファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		"main", "vs_5_0", //エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		assert(0);
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/MainFogPS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		assert(0);
	}

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv座標(1行で書いたほうが見やすい)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS; // 常に上書きルール

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

	CD3DX12_DESCRIPTOR_RANGE descRangeSRV1;
	descRangeSRV1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1 レジスタ

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[3];
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT); // s0 レジスタ

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));

	// ルートシグネチャの生成
	result = common->dxBase->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));

	gpipeline.pRootSignature = rootSignature.Get();

	// グラフィックスパイプラインの生成
	result = common->dxBase->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));
}

void Fog::TimeCount()
{
	time.x -= 0.005f;
}

void Fog::FadeOut()
{
	if (fadeOutFlag)
	{
		fadeOut -= 0.005f;

		if (fadeOut == 0)
		{
			ChengeFadeOutFlag();
			fadeOut = 1.0f;
			fadeOutFlag = !fadeOutFlag;
		}
	}
}
