#pragma once
#include "../Camera/Camera.h"
#include "../2d/Sprite.h"

class PostEffect :
    public Sprite
{
private://エイリアス
    template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    //Directx::を省略
    using XMFLOAT2 = DirectX::XMFLOAT2;
    using XMFLOAT3 = DirectX::XMFLOAT3;
    using XMFLOAT4 = DirectX::XMFLOAT4;
    using XMMATRIX = DirectX::XMMATRIX;

public:
    struct NoiseConstBuffer
    {
        float time;
        float FadeOut;
    };

public://メンバ関数
    /// <summary>
    /// コンストラクタ
    /// </summary>
    PostEffect();

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(Camera* camera);

    /// <summary>
    /// シーン描画前処理
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

    /// <summary>
    /// シーン描画後処理
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

    /// <summary>
    /// 描画コマンド
    /// </summary>
    /// <param name="cmdList"></param>
    void Draw(ID3D12GraphicsCommandList* cmdList);

    /// <summary>
    /// パイプライン生成
    /// </summary>
    void CreateGraphicsPipelineState();

    /// <summary>
    /// タイマーのカウントを進める
    /// </summary>
    void TimeCount();

    /// <summary>
    /// フェードアウト
    /// </summary>
    void FadeOut();

    /// <summary>
    /// フェードアウトフラグ切り替え
    /// </summary>
    /// <returns>フェードアウトフラグ</returns>
    bool& ChengeFadeOutFlag() { return fadeOutFlag = !fadeOutFlag; }

    bool& GetFadeOutFlag() { return fadeOutFlag; }

    void ResetFade(const float& fadeout, bool& fadeOutFlag) {
        fadeOut = fadeout;
        fadeOutFlag = false;
    }

private://静的メンバ変数
//画面クリアカラー
    static const float clearColor[4];
    // カメラ
    static Camera* camera;
private://メンバ変数
    //テクスチャバッファ
    ComPtr<ID3D12Resource> texBuff;
    ComPtr<ID3D12Resource> noiseBuff;
    ComPtr<ID3D12Resource> noiseBuff1;
    ComPtr<ID3D12Resource> noiseBuff2;
    ComPtr<ID3D12Resource> noiseBuff3;

    //デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> descHeapSRV;
    //深度バッファ
    ComPtr<ID3D12Resource>       depthBuff;
    //RTV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> descHeapRTV;
    //DSV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> descHeapDSV;
    //グラフィックスパイプライン
    ComPtr<ID3D12PipelineState>  pipelineState;
    //ルートシグネチャ
    ComPtr<ID3D12RootSignature>  rootSignature;

    float    time = 0.0f;
    XMFLOAT4 animation = { 0,0,0,0 };
    XMMATRIX viewproj;//ビュープロジェクション行列
    XMMATRIX world;//ワールド行列
    XMFLOAT3 cameraPos;//カメラ座標(ワールド座標)
    XMFLOAT4 distance;
    float    fadeOut;
    bool     fadeOutFlag;
};