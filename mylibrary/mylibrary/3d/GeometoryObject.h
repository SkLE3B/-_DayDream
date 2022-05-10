#pragma once
#include "../Base/Direcx12Base.h"
#include "GeometoryModel.h"
#include "../Camera/Camera.h"
#include "../Camera/BackCamera.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>

//数式で生成したモデル用3Dオブジェクト
class GeometoryObject
{
public://エイリアス
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public://定数
	static const int maxObjectCount = 512;//3Dオブジェクトの最大数
public:
	//共通
	class Common
	{
		friend class GeometoryObject;

	public:
		/// <summary>
		/// グラフィックスパイプライン初期化
		/// </summary>
		/// <param name="dxBase">DirectX基盤</param>
		void InitializeGraphicsPipeline(Direcx12Base* dxBase);

		/// <summary>
		/// デスクリプタヒープ初期化
		/// </summary>
		/// <param name="dxBase">DirectX基盤</param>
		void InitializeDescriptorHeap(Direcx12Base* dxBase);

		void InitializeCamera();

	private:
		//DirectX12基盤
		Direcx12Base* dxBase = nullptr;
		//ルートシグネチャ
		ComPtr<ID3D12RootSignature>rootsignature;
		//デスクリプタヒープ(定数バッファビュー用)
		ComPtr<ID3D12DescriptorHeap>basicDescHeap;
		//パイプラインステート
		ComPtr<ID3D12PipelineState>pipelinestate;
		//次に使うデスクリプタヒープ
		int descHeapIndex = 0;
		//カメラ
		Camera* camera = nullptr;
		//モデルデータ
		GeometoryModel* model = nullptr;
	};

	//定数用バッファに送るデータ構造体
	struct ConstBuffurData
	{
		XMFLOAT4 color; //色(RGBA)
		XMMATRIX mat;   //3D変換行列
	};

public://静的メンバ関数
	/// <summary>
	/// 静的メンバの初期化
	/// </summary>
	static void StaticInitialize(Direcx12Base* dxBase, GeometoryModel* model,Camera* camera);

	/// <summary>
	/// 静的メンバ変数の初期化
	/// </summary>
	static void StaticFinalize();

	/// <summary>
	/// デスクリプタヒープリセット
	/// </summary>
	static void ResetDescriptorHeap();

private://静的メンバ変数
	static Common* common;

public://メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	//setter(値の設定)
	void SetPosition(const XMFLOAT3& position) { this->position = position; }
	void SetRotation(const XMFLOAT3& rotation) { this->rotation = rotation; }
	void SetScale(const XMFLOAT3& scale) { this->scale = scale;}

	//getter(値の取得)
	XMFLOAT3& GetScale() { return scale; }
	XMFLOAT3& GetPosition() { return position; }
	XMFLOAT3& GetRotation() { return rotation; }

private://メンバ変数
	//定数バッファ
	ComPtr<ID3D12Resource>constBuff;
	//定数バッファビューのハンドル(CPU)
	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleCBV;
	//定数バッファビューのハンドル(GPU)
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV;
	//トランスフォーム
	XMFLOAT3 position = {0,0,0};//座標
	XMFLOAT3 scale = {1,1,1};   //スケーリング倍率
	XMFLOAT3 rotation = {0,0,0};//回転角
	XMMATRIX matWorld;          //ローカルワールド変換行列
	GeometoryObject* parent = nullptr; //親オブジェクト
};

