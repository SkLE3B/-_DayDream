#pragma once
#include "Model.h"
#include "Camera/Camera.h"
#include "FbxLoader.h"
#include "Collision/CollisionInfo.h"
#include "Math/Vector3.h"
#include "Collision/CollisionAttribute.h"
#include<windows.h>
#include<wrl.h>
#include<d3d12.h>
#include<d3dx12.h>
#include<DirectXMath.h>
#include<string>
#include "Math/Matrix4.h"

class BaseCollider;

class Object3d
{
protected://エイリアス
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//Directx::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public://定数
	//ボーンの最大数
	static const int MAX_BONES = 32;
public://サブクラス
	//定数バッファ用データ構造体(座標変換行列用)
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj; //ビュープロジェクション行列
		XMMATRIX world;    //ワールド行列
		XMFLOAT3 cameraPos;//カメラ行列(ワールド行列)
		XMFLOAT4 Color;
	};

	//定数バッファ用データ情報体(スキニング)
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};

public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Object3d();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual bool Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="cmdList"></param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// モデルのセット
	/// </summary>
	/// <param name="model">モデル</param>
	void SetModel(Model* model) { this->model = model; }

	/// <summary>
	/// アニメーション開始
	/// </summary>
	/// <param name="index">アニメーション番号</param>
	void PlayAnimation(const int& index, const int& num);

	/// <summary>
	/// アニメーションリセット
	/// </summary>
	void ResetAnimation();

	//setter
	void SetPosition(const Vector3& position) { this->position = position; }
	void SetRotation(const Vector3& rotation) { this->rotation = rotation; }
	void SetScale(const Vector3& scale) { this->scale = scale; }

	//getter
	Vector3& GetPosition() { return position; }
	Vector3& GetRotation() { return rotation; }
	Vector3& GetScale() { return scale; }

	bool GetIsPlayed() { return isPlayed;}

	/// <summary>
	/// 行列の更新
	/// </summary>
	void UpdateWorldMatrix();

	/// <summary>
	/// ワールド行列の取得
	/// </summary>
	/// <returns>ワールド行列</returns>
	//const XMMATRIX& GetMatWorld() { return matWorld; }	
	const Matrix4& GetMatWorld() { return matWorld; }

	inline Matrix4  Matrix4Identity()
	{
		Matrix4 M;
		M.r[0] = { 1.0f, 0.0f, 0.0f, 0.0f};
		M.r[1] = { 0.0f, 1.0f, 0.0f, 0.0f};
		M.r[2] = { 0.0f, 0.0f, 1.0f, 0.0f};
		M.r[3] = { 0.0f, 0.0f, 0.0f, 1.0f};

		return M;
	}

	/// <summary>
	/// コライダーのセット
	/// </summary>
	/// <param name="collider">コライダー</param>
	void SetCollider(BaseCollider* collider,const unsigned short collisionAttribute);

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	/// <param name="info">衝突情報</param>
	virtual void OnCollision(const CollisionInfo& info) {}

	/// <summary>
    /// モデルを取得
    /// </summary>
    /// <returns>モデル</returns>
	inline Model* GetModel() { return model; }

	///<summary>
    ///alphaの値変更
    /// </summary>
    /// <param name="alpha">α</param>
	void SetAlpha(float alpha) { color.w = alpha; }

public://静的メンバ関数
	/// <summary>
	/// グラフィックスパイプライン生成
	/// </summary>
	static void CreateGraphicsPipeline();
	//setter
	static void SetDevice(ID3D12Device* device) { Object3d::device = device; }
	static void SetCamera(Camera* camera) { Object3d::camera = camera; }
private://静的メンバ変数
	static ID3D12Device* device;
	static Camera* camera;
	//ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	//パイプラインステート
	static ComPtr<ID3D12PipelineState> pipelinestate;
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffTransform;
    //定数バッファ(スキン)
	ComPtr<ID3D12Resource> constBuffSkin;
protected:
	//ローカルスケール
	Vector3 scale = { 1,1,1 };
	//X,Y,Z軸周りのローカル回転角
	Vector3 rotation = { 0,0,0 };
	//ローカル座標
	Vector3 position = { 0,0,0 };
	//色
	XMFLOAT4 color = { 1,1,1,1 };
	//ローカルワールド変換座標
	//XMMATRIX matWorld;
	Matrix4 matWorld;
	//1フレームの時間
	FbxTime frameTime;
	//アニメーション開始時間
	FbxTime startTime;
	//アニメージョン終了時間
	FbxTime endTime;
	//現在時間(アニメーション)
	FbxTime currentTime;
	//アニメーション再生中
	bool isPlay = false;
	bool isPlayBoss = false;
	bool isPlayed = false;
	//モデル
	Model* model = nullptr;
	const char* name = nullptr;
	// コライダー
	BaseCollider* collider = nullptr;
	int animeNum;
};

