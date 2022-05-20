#pragma once
#include "3d/Object3d.h"
#include "Input/Input.h"
#include "Camera/DebugCamera.h"
#include "Base/Timer.h"
#include "Collision/AttackCollisionObject.h"
#include "GameObject/Boss/Boss.h"
#include "Math/ExNum.h"
#include "Collision/SphereCollider.h"
#include "Collision/CollisionManager.h"
#include "Collision/CollisionAttribute.h"
#include "Audio/AudioManager.h"
#include "3d/EffekseerManager.h"

class AttackCollisionObject;
class Boss;
class Player :
	public Object3d
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
	//static Player* Create(Model* model = nullptr);
	static std::shared_ptr<Player> Create(Model* model, Camera* camera);

protected:
	// ボスクエリーコールバッククラス
	class BossHIt : public QueryCallback
	{
	public:
		BossHIt(Sphere* sphere) : sphere(sphere) {};

		// 衝突時コールバック関数
		bool OnQueryHit(const QueryHit& info);
		
		Sphere* sphere = nullptr;
		DirectX::XMVECTOR move = {};
		bool flag = false;
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
	/// プレイヤー追従カメラ
	/// </summary>
	void PlayerFollowCamera(Camera* camera);

	/// <summary>
	/// カメラを移動
	/// </summary>
	void MoveCamera(Camera* camera,Boss* boss);

	/// <summary>
	/// 始めのアニメーション再生
	/// </summary>
	/// <param name="camera"></param>
	void StartAnimation(Camera* camera);

	/// <summary>
	/// 歩く
	/// </summary>
	void Walk(Camera* camera);

	/// <summary>
	/// 避ける
	/// </summary>
	void Avoid(AudioManager* audio);

	/// <summary>
    /// イージングの終点の座標を保存する
    /// </summary>
    /// <param name="EashEnd">終点座標</param>
    void DecisionDistance(Vector3 EashEnd);

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	/// <param name="info">衝突情報</param>
	void OnCollision(const CollisionInfo& info) override;

	/// <summary>
	/// 当たり判定を補正する
	/// </summary>
	/// <param name="AttackCol">当たり判定</param>
	/// <param name="AddPos">加える座標</param>
	void PositionCorrection(AttackCollisionObject* AttackCol,const Vector3 AddPos);

	/// <summary>
	/// 攻撃1突進攻撃
	/// </summary>
	/// <param name="AttackCol">当たり判定</param>
	/// <param name="AddPos">加える座標</param>
	void DrillRush(AttackCollisionObject* AttackCol,AudioManager* audio);

	/// <summary>
    /// 攻撃2遠距離攻撃
    /// </summary>
    /// <param name="AttackCol">攻撃判定</param>
	void BlazingEnd(AttackCollisionObject* AttackCol, AudioManager* audio);

	/// <summary>
	/// イージング
	/// </summary>
	/// <param name="position"></param>
	/// <param name="maxTime"></param>
	void Easing(Vector3& position,const float maxTime);

	/// <summary>
	/// タイマーをスタートする
	/// </summary>
	void TimerStart();

	/// <summary>
	/// タイマーを進める
	/// </summary>
	/// <param name="maxTime"></param>
	void AdvanceTimer(const float maxTime);

	/// <summary>
	/// タイマーを止める
	/// </summary>
	void ResetTimer();

	/// <summary>
	/// タイマーがMaxTimeに到達したかどうか
	/// </summary>
	/// <param name="TotalTime">カウントしているタイマー</param>
	/// <param name="Maxtime">終了フレーム</param>
	/// <returns></returns>
	bool IsTimeOut(float TotalTime, const float Maxtime) const;

	/// <summary>
	/// イージングが終わったかどうか
	/// </summary>
	/// <returns></returns>
	bool IsEasingOver() const;

	/// <summary>
	///	ノックバック
	/// </summary>
	/// <param name="boss"></param>
	/// <param name="bossCallback"></param>
	void KnockBack(Boss* boss, BossHIt bossCallback,AudioManager* audio);

	/// <summary>
	/// 咆哮時ノックバック
	/// </summary>
	/// <param name="boss"></param>
	/// <param name="roarFlag"></param>
	void RoarKnockBack(Boss* boss, bool roarFlag,AudioManager* audio);

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

	void ResetCamera(Boss* boss, Camera* camera);
	float& GetTimeRate() { return timeRate;}
	float& GetTime() { return totalTime; }
	float Getstamina() { return SP->get(); }
	float GetPlayerHP() { return HP->get();}
	Vector3& GetDirection() { return direction; }
	float& GetrotY() { return rotY; }
	bool& GetColFlag() { return collisionFlag; }
	bool& GetTimerFlag() { return timerFlag; }
	bool& GetLifeFlag() { return lifeFlag; }

private:
	static Camera* camera;
private:
	//貯める係数
	int charge;
	// マウスの入力を取得
	Input* input = Input::GetInstance();
	Input::MouseMove mouseMove = input->GetMouseMove();
	Timer* time;
	//落下判定
	bool onGround = true;
	//生存フラグ
	bool lifeFlag = true;
	// 落下ベクトル
	DirectX::XMVECTOR fallV;
	//速度
	float velocity = 0.3f;;
	XMVECTOR velocityR = { velocity,0,0,0 };
	XMVECTOR velocityL = { -velocity,0,0,0 };
	XMVECTOR velocityU = { 0,0,velocity,0 };
	XMVECTOR velocityD = { 0,0,-velocity,0 };
	bool isAvoid = false;
	float lapseTime;
	Vector3 prePos;
	//現在向いてる方向
	Vector3 direction;
	Vector3 target;
	Vector3 vec;
	float walkSpeed = 0.55f; //移動速度	
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

	//時間系
	float maxTime;//全体時間[s]
	float timeRate;//何％時間が進んだか(率)
	float elapsedTime;
	float totalTime;

	Vector3 DashStart;
	Vector3 DashEnd;
	Vector3 correctionPos;//攻撃判定補正座標

	float stamina;//スタミナ
	float radius;//半径
	float distance;
	float rotY;

	bool collisionFlag;//攻撃用コリジョンを出現させるかどうか
	bool timerFlag;    //タイマーが起動しているかどうか
	bool actionFlag;   //プレイヤーが行動しているかどうか
	bool dashFlag;     //ダッシュ中かどうか
	bool flag;
	float phi = 0;
	float theta = 0;

	Vector3 AttColPos;//当たり判定の座標

	ExNum* HP;//プレイヤーのHP
	ExNum* SP;//スタミナ

	Effekseer::Handle handle;

	enum
	{
		STARTANIME,
		NONE,//何もしてない
		WALK,
		RUN, //走る
		AVOID,//避ける
		ATTCK1,//攻撃1
		ATTACKEND1,//攻撃終了
		ATTACK2,//攻撃2
		ATTACK3,
		ATTACK4,
		ATTACKEND4,
		KNOCKBACK
	}ActionState;//プレイヤーの状態
};