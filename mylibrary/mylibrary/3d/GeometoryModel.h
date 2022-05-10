#pragma once

#include<DirectXMath.h>
#include<d3d12.h>
#include "../Base/Direcx12Base.h"
#include<wrl.h>
#include"../2d/TextureManager.h"

class GeometoryModel
{
public://定数
	static const float topHeight;//頂点高さ
	static const float R;//半径
	static const int n = 3;//n角形
	//インナークラス
public:
	//頂点データ構造体
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;//xyz座標
		DirectX::XMFLOAT3 normal;//法線ベクトル
		DirectX::XMFLOAT2 uv; //uv座標
	};
public://メンバ関数

	//インデックス配列の要素数を数える
	int CountIndices() { return _countof(indices); }

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxBase">Directx12べース</param>
	void Initialize(Direcx12Base* dxBase, TextureManager* textureManager, UINT texnumber);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//void Draw(Direcx12Base* dxBase);

	//getter
	const D3D12_VERTEX_BUFFER_VIEW& GetVbView() { return vbView; }

	//getter
	const D3D12_INDEX_BUFFER_VIEW& GetIbView() { return  ibView; }

private://メンバ変数

	//テクスチャマネージャ
	TextureManager* textureManager = nullptr;

	//テクスチャ番号
	UINT texnumber;

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;
	//インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource>indexBuff;

	//頂点バッハビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//インデックスバッファビュ
	D3D12_INDEX_BUFFER_VIEW ibView{};

	//頂点データ配列
	Vertex vertices[n + 1 + 1] = {};
	//インデックスデータ配列
	unsigned short indices[n * n * 2];
};

