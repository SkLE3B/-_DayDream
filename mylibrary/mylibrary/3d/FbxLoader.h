#pragma once

#include "fbxsdk.h"
#include "Model.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <string>

class FbxLoader
{
private://エイリアス
	//std省略
	using string = std::string;
public://定数
	//モデル格納ルートパス
	static const string baseDirectory;

public://メンバ関数
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();

	/// <summary>
	/// FBXの行列をXMMatrixに変換
	/// </summary>
	/// <param name="dst">書き込み先</param>
	/// <param name="src">元となるFBX行列</param>
	static void ComvertMatrixFromFbx(DirectX::XMMATRIX* dst,const FbxAMatrix& src);
	
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// ファイルからFBXモデル読み込み
	/// </summary>
	/// <param name="modelname">ファイル名</param>
	Model* LoadModelFromFile(const string& modelname);

	/// <summary>
	/// 再帰的にノート構成を解析
	/// </summary>
	/// <param name="model">読み込み先モデルオブジェクト</param>
	/// <param name="fbxNode">解析対象のノード</param>
	void ParseNodeRecursive(Model* model, FbxNode* fbxNode, Node* parent = nullptr);

	/// <summary>
	/// メッシュの読み取り
	/// </summary>
	/// <param name="model">読み取り先のモデルオブジェクト</param>
	/// <param name="fbxNode">解析対象のノード/param>
	void ParseMesh(Model* model, FbxNode* fbxNode);
	//頂点座標読み取り
	void ParseMeshVertices(Model* model, FbxMesh* fbxMesh);
	//面情報読み取り
	void ParseMeshFaces(Model* model, FbxMesh* fbxMesh);
	//マテリアル読み取り
	void ParseMaterial(Model* model, FbxNode* fbxNode);
	//テクスチャ読み取り
	void LoadTexture(Model* model, const std::string& fullpath);
	//ディレクトリを含んだファイルパスからファイル名を抽出する
	std::string ExtractFileName(const std::string& path);
    //スキニング情報の読み取り
	void ParseSkin(Model* model, FbxMesh* fbxMesh);

	/// <summary>
	/// 後始末
	/// </summary>
	void Finalize();

private://メンバ変数
	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;
	//テクスチャがない場合の標準テクスチャファイル名
	static const string defaultTextureFileName;
	//D3D12デバイス
	ID3D12Device* device = nullptr;
	//FBXマネージャー
	FbxManager* fbxManager = nullptr;
	//FBXインポーター
	FbxImporter* fbxImporter = nullptr;
};