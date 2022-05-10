#pragma once

#include"../Base/Direcx12Base.h"
#include<d3d12.h>
#include<d3dx12.h>
#include<wrl.h>
#include<DirectXTex.h>

/// <summary>
/// テクスチャマネージャー
/// </summary>
class TextureManager
{
public://エイリアス
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


public://定数
	static const int maxTextureCount = 512;//テクスチャの最大枚数

public://メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxBase"></param>
	void Initialize(Direcx12Base* dxBase);

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	/// <param name="texnumber">テクスチャ番号</param>
	/// <param name="filename">テクスチャファイル名</param>
	void spriteLoadTexture(UINT texnumber, const wchar_t* filename);

	/// <summary>
	/// テクスチャバッファ取得
	/// </summary>
	/// <param name="texnumber"></param>
	/// <returns>テクスチャバッファ</returns>
	ID3D12Resource* GetSpriteTexbuff(UINT texnumber);

	/// <summary>
	/// デスクリプタヒープをセット(グラフィックスコマンド)
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void SetDescriptorHeaps(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// SRVをセット(グラフィックスコマンド)
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	/// <param name="RootParameterIndex">ルートパラメータの番号</param>
	/// <param name="texnumber">テクスチャ番号</param>
	void SetShaderResourceView(ID3D12GraphicsCommandList* cmdList, UINT RootParameterIndex, UINT texnumber);

	//getter
	ID3D12DescriptorHeap* GetDescHeap() {return DescHeap.Get(); }

private://メンバ変数
	ComPtr<ID3D12DescriptorHeap> DescHeap;
	ComPtr<ID3D12Resource> textureBuffer[maxTextureCount];
	Direcx12Base* dxBase = nullptr;
};

