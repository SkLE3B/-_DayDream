#include "TextureManager.h"

#include<cassert>

using namespace DirectX;

void TextureManager::Initialize(Direcx12Base* dxBase)
{
	HRESULT result = S_FALSE;
	//メンバ変数に記録
	this->dxBase = dxBase;

	//デスクリプタヒープの配列を生成
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

	////WiCのテクスチャロード
	TexMetadata metadata{};
	ScratchImage scratchlmg{};

	result = LoadFromWICFile(
		filename,//画像名
		WIC_FLAGS_NONE,
		&metadata, scratchlmg);
	if (FAILED(result))
	{
		assert(0);
	}

	const Image* img = scratchlmg.GetImage(0, 0, 0);

	//リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	CD3DX12_HEAP_PROPERTIES HEAP_PROPERTIES1 = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

	//テクスチャ用バッファの生成
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

	//テクスチャバッファの生成
	CD3DX12_HEAP_PROPERTIES texHeapProp{};//頂点ヒープ設定
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;//
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//データ転送
	//テクスチャバッファにデータ転送
	result = textureBuffer[texnumber]->WriteToSubresource(
		0,
		nullptr,//全領域へコピー
		img->pixels,//元データアドレス
		(UINT)img->rowPitch,//1ラインサイズ
		(UINT)img->slicePitch//全サイズ
	);
	if (FAILED(result))
	{
		assert(0);
	}

	//シェーダーリソースビュー生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//テクスチャ2D
	srvDesc.Texture2D.MipLevels = 1;

	////ヒープの2番目にシェーダーリソースビュー作成
	dxBase->GetDevice()->CreateShaderResourceView(textureBuffer[texnumber].Get(),//ビューと関連づけるバッファ
		&srvDesc,//テクスチャ設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(DescHeap->GetCPUDescriptorHandleForHeapStart(),
			texnumber, dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV))
	);
}

ID3D12Resource* TextureManager::GetSpriteTexbuff(UINT texnumber)
{
	//配列オーバーフロー防止
	if (texnumber >= maxTextureCount)
	{
		return nullptr;
	}

	return textureBuffer[texnumber].Get();
}

void TextureManager::SetDescriptorHeaps(ID3D12GraphicsCommandList* cmdList)
{
	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = {DescHeap.Get()};
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void TextureManager::SetShaderResourceView(ID3D12GraphicsCommandList* cmdList, UINT RootParameterIndex, UINT texnumber)
{
	//SRVのサイズを取得
	UINT sizeSRV = dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//デスクリプタテーブルの先頭を取得
	D3D12_GPU_DESCRIPTOR_HANDLE start = DescHeap->GetGPUDescriptorHandleForHeapStart();

	//SRVのGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(start,texnumber, sizeSRV);

	//SRVをセット
	cmdList->SetGraphicsRootDescriptorTable(RootParameterIndex,handle);
}
