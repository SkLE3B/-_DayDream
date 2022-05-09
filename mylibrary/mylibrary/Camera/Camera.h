#pragma once
#include<DirectXMath.h>
#include "Math/Vector3.h"

/// <summary>
/// カメラ
/// </summary>
class Camera
{
public://エイリアス
	using XMMATRIX = DirectX::XMMATRIX;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMVECTOR = DirectX::XMVECTOR;

public://メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	/// /// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	void Initialize(int window_width, int window_height);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
    /// ビュー行列を更新
    /// </summary>
	void UpdateViewMatrix();

	/// <summary>
    /// 射影行列を更新
    /// </summary>
	void UpdateProjectionMatrix();

	/// <summary>
    /// ビルボード行列の取得
    /// </summary>
    /// <returns>ビルボード行列</returns>
	inline const XMMATRIX& GetBillboardMatrix() {
		return matBillboard;
	}

	virtual void AddPhi(const float angle) = 0;

	virtual void ResetCamera() = 0;

	/// <summary>
	/// ベクトルによる視点移動
	/// </summary>
	/// <param name="move">移動量</param>
	void MoveEyeVector(const XMFLOAT3& move);
	void MoveEyeVector(const XMVECTOR& move);

	/// <summary>
	/// ベクトルによる移動
	/// </summary>
	/// <param name="move">移動量</param>
	void MoveVector(const XMFLOAT3& move);
	void MoveVector(const XMVECTOR& move);

public://静的メンバ関数
	//getter
	const XMMATRIX& GetMatView() { return matView; }
	const XMMATRIX& GetMatProjection() { return matProjection; }
	const Vector3& GetEye() { return eye; }
	const Vector3& GetTarget() { return target; }
	const Vector3& GetUp() { return up; }
	const XMMATRIX& GetInverseView() {return InverseMatrix; }
	const bool& GetDirtyFlag() { return viewDirty; }

	inline const XMMATRIX& GetViewProjectionMatrix() {
		return matViewProjection;
	}

public://メンバ関数
	//setter
	void SetMatView(const XMMATRIX& matView) { this->matView = matView; }
	void SetMatProjeciton(const XMMATRIX& matProjection) { this->matProjection = matProjection; }
	void SetEye(const Vector3& eye) { this->eye = eye; }
	void SetTarget(Vector3 target) { this->target = target; }
	void SetUp(const Vector3& up) { this->up = up; }
	
	/// <summary>ロックオンカメラ</summary>　
    /// <param name = "targetPos">ロックオン対象者位置</param>
    /// <param name = "subPos">メインキャラの位置</param>
	void LockOn(Vector3 targetPos, Vector3 subPos, Vector3 cameraPosition);

	/// <summary>上方向ベクトル計算</summary>
	void CalUp(const Vector3 vec);

protected://メンバ変数
	XMMATRIX matView;            //ビュー行列
	XMMATRIX InverseMatrix;      //ビュー逆行列
	XMMATRIX matProjection;      //射影行列
	Vector3 eye;                //視点座標
	Vector3 target;             //注意点座標
	Vector3 up;                 //上方向ベクトル
		// ビルボード行列
	XMMATRIX matBillboard = DirectX::XMMatrixIdentity();
	// Y軸回りビルボード行列
	XMMATRIX matBillboardY = DirectX::XMMatrixIdentity();
	//ビュー射影行列
	XMMATRIX matViewProjection = DirectX::XMMatrixIdentity();
	// ビュー行列ダーティフラグ
	bool viewDirty = false;
	// アスペクト比
	float aspectRatio = 1.0f;
};

