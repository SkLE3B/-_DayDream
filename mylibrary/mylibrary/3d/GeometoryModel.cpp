#include "GeometoryModel.h"
#include <vector>

using namespace DirectX;

const float GeometoryModel::topHeight = 3.0f; //頂点高さ
const float GeometoryModel::R = 5.0f;//半径

void GeometoryModel::Initialize(Direcx12Base* dxBase, TextureManager* textureManager, UINT texnumber)
{
	this->textureManager = textureManager;
	this->texnumber = texnumber;

	HRESULT result = S_FALSE;

	//頂点データ
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
		//ポリゴンのインデックス
		indices[3 * i + 0] = i;//添え字0にインデックス0を代入
		indices[3 * i + 1] = i + 1;//添え字1にインデックス１を代入
		indices[3 * i + 2] = n;
	}
	indices[n * 3 - 2] = 0;

	for (int i = 0; i < n; i++)
	{
		//ポリゴンのインデックス
		indices[3 * i + 9] = i + 1;//添え字0にインデックス0を代入
		indices[3 * i + 10] = i;//添え字1にインデックス１を代入
		indices[3 * i + 11] = n + 1;
	}
	indices[n * 6 - 3] = 0;

	//法線を計算する
	for (int i = 0; i < 18 / 3; i++)
	{//三角形1つごとに計算する
		//三角形のインデックスを取り出して、一時てきな変数に入れる
		unsigned short indices0 = indices[i * 3 + 0];
		unsigned short indices1 = indices[i * 3 + 1];
		unsigned short indices2 = indices[i * 3 + 2];
		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[indices0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[indices1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[indices2].pos);
		//p0→p1ベクトル,p0→p2ベクトルを計算(ベクトル減算)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//外積は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//正規化(長さを１にする)
		normal = XMVector3Normalize(normal);
		//求めた本線を頂点データに代入
		XMStoreFloat3(&vertices[indices0].normal, normal);
		XMStoreFloat3(&vertices[indices1].normal, normal);
		XMStoreFloat3(&vertices[indices2].normal, normal);
	}

	CD3DX12_HEAP_PROPERTIES HEAP_PROPERTIES = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	CD3DX12_RESOURCE_DESC RESOURCE_DESC = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices));

	//頂点バッファの生成
	result = dxBase->GetDevice()->CreateCommittedResource(
		&HEAP_PROPERTIES,//アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&RESOURCE_DESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	//インデックスバッファの生成
	result = dxBase->GetDevice()->CreateCommittedResource(
		&HEAP_PROPERTIES,//アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&RESOURCE_DESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//頂点バッハへのデータ転送
	// GPU 上のバッファに対応した仮想メモリを取得
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	// 全頂点に対して
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i]; // 座標をコピー
	}

	// マップを解除
	vertBuff->Unmap(0, nullptr);

	//GPU上のバッファに対応した仮想メモリを取得
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//全インデックスに対して
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];//インデックスをコピー
	}
	indexBuff->Unmap(0, nullptr);

	//頂点バッハビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);

	//インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);
}

void GeometoryModel::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	//インデックスバッファの設定
	cmdList->IASetIndexBuffer(&ibView);
	//デスクリプタヒープの配列	
	textureManager->SetDescriptorHeaps(cmdList);
	//シェーダーリソースビューをセット
	textureManager->SetShaderResourceView(cmdList, 1, texnumber);
	//描画コマンド
	cmdList->DrawIndexedInstanced(CountIndices(), 1, 0, 0, 0);
}

