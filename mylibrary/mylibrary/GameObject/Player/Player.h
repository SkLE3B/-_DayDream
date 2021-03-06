#pragma once
#include "../mylibrary/3d/Object3d.h"
#include "../mylibrary/Input/Input.h"
#include "../mylibrary/Camera/DebugCamera.h"
#include "../mylibrary/Base/Timer.h"
#include "../mylibrary/Collision/AttackCollisionObject.h"
#include "../mylibrary/GameObject/Boss/Boss.h"
#include "../mylibrary/Math/ExNum.h"
#include "../mylibrary/Collision/SphereCollider.h"
#include "../mylibrary/Collision/CollisionManager.h"
#include "../mylibrary/Collision/CollisionAttribute.h"
#include "../mylibrary/Audio/AudioManager.h"
#include "../mylibrary/3d/EffekseerManager.h"
#include "../mylibrary/GameObject/Player/PlayerBaseState.h"
#include "../mylibrary/Collision/QueryCallback.h"
#include <memory.h>

class PlayerBaseState;
class AttackCollisionObject;
class Boss;
class Player :
	public Object3d, public std::enable_shared_from_this<Player>
{
public://エイリアス
	using XMMATRIX = DirectX::XMMATRIX;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMVECTOR = DirectX::XMVECTOR;
public:
	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	static std::shared_ptr<Player> Create(Model* model, Camera* camera);

public:
	// ボスクエリーコールバッククラス
	class BossHIt : public QueryCallback
	{
	public:
		BossHIt(Sphere* sphere) : sphere(sphere) {};

		// 衝突時コールバック関数
		bool OnQueryHit(const QueryHit& info);
		
		static bool IsFlag() { return flag; }

		static bool FalseFlag() { return flag = !flag; }

		static bool FlagInit() { return flag = false; }

		Sphere* sphere = nullptr;
		DirectX::XMVECTOR move = {};
		static bool flag;
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Initialize(Camera* camera);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update(Camera* camera,AttackCollisionObject* AttackCol,Boss* boss,AudioManager* audio);

	/// <summary>
	/// ステート変更
	/// </summary>
	/// <param name="weak_PlayerState">weakポインタ</param>
	void ChangeState(std::shared_ptr<PlayerBaseState> weak_PlayerState);

	/// <summary>
	/// プレイヤー追従カメラ
	/// </summary>
	void PlayerFollowCamera(Camera* camera);

	/// <summary>
	/// カメラを移動
	/// </summary>
	void MoveCamera(Camera* camera,Boss* boss);

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	/// <param name="info">衝突情報</param>
	void OnCollision(const CollisionInfo& info) override;

	/// <summary>
	/// 移動範囲制限
	/// </summary>
	void MovementRangeLimit();

	/// <summary>
	/// 落下状態
	/// </summary>
	void Fall();

	/// <summary>
	/// 接地しているかどうか
	/// </summary>
	void IsGrounded(SphereCollider* sphereCollider);

    /// <summary>
    /// 補正座標をセットする
    /// </summary>
    /// <param name="distance">距離</param>
    /// <param name="correctionPosY">補正するY座標</param>
    Vector3 GetCorrectionPos(const float& distance, Vector3& correctionPosY) 
	{ 	return ((moveDirection + cameraDirectionZ) * distance) + correctionPosY;}

	inline const XMMATRIX& GetLocalRotation() const
	{
		return rotationMatrix;
	}

	Vector3& GetDirection() { return direction; }
	void ResetCamera(Boss* boss, Camera* camera);
	float GetPlayerHP() { return HP->get();}
	float& GetrotY() { return rotY; }
	bool& GetColFlag() { return collisionFlag; }
	bool& GetLifeFlag() { return lifeFlag; }
	/// <summary>
	/// getter
	/// </summary>
	/// <returns></returns>
	Vector3& GetCameraDirectionX() { return cameraDirectionX; }
	/// <summary>
	/// getter
	/// </summary>
	/// <returns></returns>
	Vector3& GetCameraDirectionZ() { return cameraDirectionZ; }
	/// <summary>
	/// getter
	/// </summary>
	/// <returns></returns>
	Vector3& GetMoveDirection() { return moveDirection; }

	BossHIt& GetBossHit() { return bossCallback; }

	ExNum* GetHp() { return HP; }

	bool& PlayerDead() { return lifeFlag = !lifeFlag; }

private:
	static Camera* camera;
private:
	static BossHIt bossCallback;
	std::shared_ptr<PlayerBaseState> shared_PlayerState;
	// マウスの入力を取得
	Input* input = Input::GetInstance();
	Input::MouseMove mouseMove = input->GetMouseMove();
	//落下判定
	bool onGround = true;
	//生存フラグ
	bool lifeFlag = true;
	// 落下ベクトル
	DirectX::XMVECTOR fallV;
	//現在向いてる方向
	Vector3 direction;
	Vector3 target;
	float rotateSpeed = 3.5f;//回転速度
	//カメラ回転中
	bool rotCamera;
	//カメラの回転度合い
	float radY;
	int cameraRotCount;
	const int rotTime = 10;
	Vector3 moveDirection = {};
	Vector3 cameraDirectionZ;
	Vector3 cameraDirectionX;
	Vector3 cameraPosition;
	Vector3 cameraTarget;
	XMMATRIX camMatWorld;
	XMMATRIX rotationMatrix;
	float radius;//コライダー半径
	float distance;
	float rotY;
	bool collisionFlag;//攻撃用コリジョンを出現させるかどうか
	ExNum* HP;//プレイヤーのHP
	Effekseer::Handle handle;//エフェクトハンドル
};