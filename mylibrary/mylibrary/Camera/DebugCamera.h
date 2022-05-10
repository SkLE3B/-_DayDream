#pragma once
#include "Camera.h"
#include "../Input/Input.h"
#include "../Player.h"

/// <summary>
/// デバッグ用カメラ
/// </summary>
class window;
class DebugCamera :
    public Camera
{
    using XMMATRIX = DirectX::XMMATRIX;
public:
    DebugCamera();
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="window_width">画面幅</param>
    /// <param name="window_height">画面高さ</param>
    /// <param name="input">入力</param>
    void Initialize(int window_width, int window_height, Input* input);

    //更新
    void Update() override;

    void SetDistance(float distance) {
        this->distance = distance; viewDirty = true;
    }

    void AddPhi(const float angle);
    
    void ResetCamera() override;
private:
    //入力クラスのポインタ
    Input* input;
    //プレイヤーのポインタ
    //Player* player;
    //カメラ注視点までの距離
    float distance = 10;
    //スケーリング
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    //回転行列
    XMMATRIX matRot = DirectX::XMMatrixIdentity();

    float cameraMoveSpeed = 21.0f;
    float phi;
    float theta;
};