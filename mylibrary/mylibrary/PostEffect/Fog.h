#pragma once
#include "../2d/Sprite.h"
#include "../Camera/Camera.h"
#include "../Math/Fader.h"

class Fog :
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
    struct FogConstBuffer
    {
        XMMATRIX inverse;
        XMMATRIX viewproj;//ビュープロジェクション行列
        XMMATRIX matWorld;
        XMFLOAT4 animation;
        XMFLOAT4 Time;
        float FadeOut;
    };

public://メンバ関数
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Fog();

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
    void FadeOut(const float value);

    /// <summary>
    /// フェードイン
    /// </summary>
    void FadeIn(const float value);

    /// <summary>
    /// フェードアウトフラグ切り替え
    /// </summary>
    /// <returns>フェードアウトフラグ</returns>
    bool& ChengeFadeOutFlag() { return fadeStart = !fadeStart;}

    //値が最大値かどうか？
    bool IsMax() { return fadeFog->isMax(); }

    //以上なら
    bool Morethan(float value) { return fadeFog->Morethan(value); }

    /// <summary>
    /// セッター
    /// </summary>
    void SetFadeOut(const float& fadeout) { fadeOut = fadeout; }

    bool& GetFadeOutFlag() { return fadeStart; }

    void ResetFade(const float& fadeout, bool& fadeOutFlag) { 
        fadeOut = fadeout;
        fadeOutFlag = false;
    }

    bool& StopFade() { return fadeStart = false; };

    ///<summary>
    ///alphaの値変更
    /// </summary>
    /// <param name="alpha">α</param>
    void SetAlpha(float alpha);

    float GetAlpha() { return fadeOut;}

private://静的メンバ変数
//画面クリアカラー
    static const float clearColor[4];
    // カメラ
    static Camera* camera;
private://メンバ変数
    Fader* fadeFog;
    //テクスチャバッファ
    ComPtr<ID3D12Resource> texBuff;
    ComPtr<ID3D12Resource> noiseBuff;
    ComPtr<ID3D12Resource> noiseBuff1;
    ComPtr<ID3D12Resource> noiseBuff2;
    ComPtr<ID3D12Resource> noiseBuff3;

    //デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> descHeapSRV;
    //深度バッファ
    ComPtr<ID3D12Resource> depthBuff;
    //RTV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> descHeapRTV;
    //DSV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> descHeapDSV;
    //グラフィックスパイプライン
    ComPtr<ID3D12PipelineState> pipelineState;
    //ルートシグネチャ
    ComPtr<ID3D12RootSignature> rootSignature;
    XMFLOAT4 time = { 0,0,0,0 };
    XMFLOAT4 animation = { 0,0,0,0 };
    XMMATRIX viewproj;     //ビュープロジェクション行列
    XMMATRIX world;        //ワールド行列
    XMFLOAT3 cameraPos;    //カメラ座標(ワールド座標)
    XMFLOAT4 distance;     //距離
    float    fadeOut;      //フェードアウト
    bool     fadeStart;  
    bool operation;        //操作中
};