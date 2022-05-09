#pragma once
#include "3d/Object3d.h"
#include "Math/Easing.h"
#include "Input/Input.h"
#include "Player.h"
#include "Collision/SphereCollider.h"
#include "3d/AttackEnemyCollisionObject.h"
#include "Math/Matrix4.h"
#include "BossBaseState.h"
#include "Audio/AudioManager.h"

class BossBaseState;
class AttackEnemyCollisionObject;
class Player;
class Boss :
    public Object3d, public std::enable_shared_from_this<Boss>
{
public://エイリアス
	using XMMATRIX = DirectX::XMMATRIX;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMVECTOR = DirectX::XMVECTOR;
	using shared_ptrState = std::shared_ptr<BossBaseState>;

public:
	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	//static Boss* Create(Model* model = nullptr);
	static std::shared_ptr<Boss> Create(Model* model = nullptr);
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update(Player* player, AttackEnemyCollisionObject* AttackCollision, AudioManager* audio);

	/// <summary>
    /// 補正座標をセットする
    /// </summary>
    /// <param name="distance">距離</param>
    /// <param name="correctionPosY">補正するY座標</param>
	Vector3 GetCorrectionPos(const float& distance, Vector3& correctionPosY)
	{
		return ((atan2f(player_position.x - position.x, player_position.z - position.z) * 60) * distance) + correctionPosY;
	}

	//SetterGetter
	float& GetTimeRate() { return timeRate; }
	float& GetTime();
	float& GetAngle() { return angle; }
	Vector3& Getresult() { return moveVector_position; }

	float& GetDotVector() { return dotPosition; }
	float& GetradiusCos() { return radiusCos; }
	float GetAngle(int mx, int my, int sx, int sy) {
		return short(atan2f(-float(sy - my), float(sx - mx)));
	}

	SphereCollider& GetSphereCollider() { return *sphereCollider;}

	bool& GetRoarFlag();
	bool& GetColFlag();
	void ChangeState(std::shared_ptr<BossBaseState> weak_bossState);
	bool ChangeLifeFlag() { return lifeFlag = !lifeFlag; }
	bool GetLifeFlag() { return lifeFlag;}

	shared_ptrState GetState() { return shared_bossState; }
private:
	std::shared_ptr<BossBaseState> shared_bossState;
	Input*          input = Input::GetInstance();   // マウスの入力を取得
	Vector3         moveVector_position;            //移動量のベクトル座標
	Vector3         player_position;                //プレイヤーの座標
	Vector3         boss_position;                  //ボスの座標
	Vector3         easing_start;                   //イージング開始座標
	Vector3         rotationEasing_start;           //回転イージング開始座標
	Vector3         easing_end;                     //イージング終了座標
	Vector3         attackObject_position;          //攻撃オブジェクト座標
	SphereCollider* sphereCollider;                 //球コライダー
	Vector3         forwardVector;                  //前方向ベクトル
	Matrix4         warldMatrix;                    //ワールド座標行列
	Vector3         walrdPosition;                  //ワールド座標
	Vector3         differencePlayerToBoss_position;//プレイヤー座標からボス座標の差
	float           dotPosition;                    //内積
	float           radiusCos;                      //半径のコサイン
	float           maxTime;                        //全体時間[s]
	float           timeRate;                       //何％時間が進んだか(率)
	float           elapsedTime;                    //経過時間
	float           totalTime;                      //進めた時間
	bool            timerFlag;                      //タイマーが起動しているかどうか
	float           angle;                          //角度
	float           distanceMagnification;          //距離の倍率
	bool            lifeFlag;                       //ライフフラグ
	float           radius;                         //半径
	Vector3         angleVector;                    //角度ベクトル
};

