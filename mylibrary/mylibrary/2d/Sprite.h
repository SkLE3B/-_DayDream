#pragma once
#include<d3d12.h>
#include<d3dx12.h>
#include<wrl.h>
#include<DirectxMath.h>
#include"TextureManager.h"

/// <summary>
/// スプライト
/// </summary>
class Sprite
{
public://エイリアス
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

protected://サブクラス

	class Common {

		friend class Sprite;
		friend class PostEffect;
		friend class Fog;
		friend class Fog;
	public:
		void InitializeGraphicsPipeline();
	protected:
		TextureManager* textureManager = nullptr;
		Direcx12Base* dxBase = nullptr;
		ComPtr<ID3D12RootSignature> RootSignature; //ルートシグネチャ
		ComPtr<ID3D12PipelineState> PipelineState; //パイプラインステート
		XMMATRIX MatProjection{}; //射影行列
	};

	//スプライトデータ構造
	struct VertexPosUv
	{
		DirectX::XMFLOAT3 pos; //xyz座標
		DirectX::XMFLOAT2 uv;  //uv座標
	};

	//定数用バッファに送るデータ構造体
	struct ConstBuffurData
	{
		DirectX::XMFLOAT4 color; //色(RGBA)
		DirectX::XMMATRIX mat;   //3D変換行列
		float Time;              //時間
	};

public://静的メンバ関数

	/// <summary>
	/// 静的メンバの初期化
	/// </summary>
	/// <param name="dxBase"></param>
	static void StaticInitialize(Direcx12Base* dxBase, TextureManager* textureManager);

	/// <summary>
	/// 静的メンバの解放
	/// </summary>
	static void StaticFinalize();

	/// <summary>
	/// グラフィックスパイプラインのセット
	/// </summary>
	/// <param name="cmdList"></param>
	static void SetPipelineState(ID3D12GraphicsCommandList* cmdList);

protected://静的メンバ変数
	/// <summary>
	/// 静的メンバの初期化
	/// </summary>
	static Common* common;
	 
	// 頂点数
	static const int vertNum = 4;

	// デバイス
	static ID3D12Device* device;
public://メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="texnumber">テクスチャ番号</param>
	void Initialize(UINT texnumber);

	/// <summary>
	/// 頂点バッファにデータ転送
	/// </summary>
	void TransferVertices();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="cmdList"></param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// サイズ設定
	/// </summary>
	/// <param name="width">幅</param>
	/// <param name="height">高さ</param>
	void SetSize(float width, float height);

	/// <summary>
	/// 角度設定
	/// </summary>
	/// <param name="rotation">角度</param>
	void SetRotation(float rotation);

	/// <summary>
	/// アンカーポイント設定
	/// </summary>
	/// <param name="anchorpoint">アンカーポイント</param>
	void SetAnchorPoint(XMFLOAT2 anchorpoint);

	/// <summary>
	/// 座標設定
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	void SetPosition(float x, float y);

	/// <summary>
	/// 左右反転の設定
	/// </summary>
	/// <param name="isFlipX">左右反転</param>
	void SetIsFlipX(bool isFlipX);

	/// <summary>
	/// 上下反転の設定
	/// </summary>
	/// <param name="isFlipY">上下反転</param>
	void SetIsFlipY(bool isFlipY);

	/// <summary>
	/// テクスチャ範囲指定
	/// </summary>
	/// <param name="tex_x">テクスチャ座標</param>
	/// <param name="tex_width">テクスチャサイズ</param>
	void SetTextureRange(float tex_x, float tex_y, float tex_width, float tex_height);

	/// <summary>
	/// テクスチャ範囲指定
	/// </summary>
	/// <param name="tex_x">テクスチャX座標</param>
	/// <param name="tex_y">テクスチャY座標</param>
	/// <param name="tex_width">テクスチャ横サイズ</param>
	/// <param name="tex_height">テクスチャ縦サイズ</param>
	void SetTextureRect(float tex_x, float tex_y, float tex_width, float tex_height);

	/// <summary>
	/// 座標取得
	/// </summary>
	/// <returns></returns>
	XMFLOAT2 Getposition();

	///<summary>
    ///alphaの値変更
    /// </summary>
    /// <param name="alpha">α</param>
	void SetAlpha(float alpha) { Color.w = alpha; }

	/// <summary>
	/// フェードイン
	/// </summary>
	void Fadein(const std::string& swich);

	/// <summary>
	/// フェードアウト
	/// </summary>
	void Fadeout(const std::string& swich);

	/// <summary>
	/// フェードアウト
	/// </summary>
	void FadeOut();

	/// <summary>
	/// フェードアウトフラグ切り替え
	/// </summary>
	/// <returns>フェードアウトフラグ</returns>
	bool& ChengeFadeOutFlag() { return fadeOutFlag = !fadeOutFlag; 
	}

	float getAlpha() { return Color.w; }

	std::string getSwich(const std::string& swich) { return swich; }

	bool isMin() {
		return getAlpha() <= 0.0f;
	}

	bool isMax() {
		return getAlpha() >= 1.0f;
	}

protected://メンバ変数
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	//Z軸まわりの回転角
	float rotation = 0.0f;
	//座標
	XMVECTOR position{};
	//ワールド行列
	XMMATRIX spriteMatWorld{};
	//色
	XMFLOAT4 Color = {1,1,1,1};
	//テクスチャ番号
	UINT texnumber = 0;
	//アンカーポイント
	XMFLOAT2 anchorpoint = {};
	const float limit = 1.0f;//上限
	//横幅
	float width = 100.0f;
	float height = 100.0f;//縦幅
	bool isFlipX = false;//左右反転
	bool isFlipY = false;//上下反転
	float tex_x = 0.0f;     //テクスチャ左上　X座標
	float tex_y = 0.0f;     //テクスチャ左上　Y座標
	float tex_width  = 100.0f; //テクスチャ横幅
	float tex_height = 100.0f;//テクスチャ縦幅
	float time;
	bool fadeOutFlag;
};