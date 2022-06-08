#pragma once
#include "Boss.h"
#include "../mylibrary/GameObject/Player/Player.h"
#include "../mylibrary/Base/Timer.h"
#include "../mylibrary/3d/AttackEnemyCollisionObject.h"
#include "../mylibrary/Audio/AudioManager.h"
#include "../mylibrary/Base/HelperTimer.h"

enum class BossStatePattern
{
	WAIT,
	LOCKON,
	BACKSTEP,
	ROLLING, 
	BITING,  
	ROAR,    
	BRETH,   
	RUSH,    
	END
};

class AttackEnemyCollisionObject;
class Player;
class BossBaseState
{
public://エイリアス
	using weak_ptrBoss = std::weak_ptr<Boss>;
	using weak_ptrPlayer = std::weak_ptr<Player>;
	using XMVECTOR = DirectX::XMVECTOR;
public:
	BossBaseState() {}
	virtual ~BossBaseState() {}

	virtual void Initialize() = 0;
	virtual void Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio) = 0;
	virtual void Finalize() = 0;

	/// <summary>
    /// 始点と終点の座標を保存する
    /// </summary>
    /// <param name="DashX">終点X座標</param>
    /// <param name="DashZ">終点Z座標</param>
    /// <param name="Flag"> 変えたいフラグ</param>
	void DecisionDistance(const float EaseEndX, const float EaseEndZ);

	/// <summary>
	/// 当たり判定を補正する
	/// </summary>
	/// <param name="AttackCol">当たり判定</param>
	/// <param name="AddPos">加える座標</param>
	void PositionCorrection(AttackEnemyCollisionObject* AttackCol, const Vector3 AddPos);

	/// <summary>
	/// 移動イージング
	/// </summary>
	/// <param name="position">動かす座標</param>
	/// <param name="maxTime">移動時間</param>
	void EasingMove(Vector3& position, const float maxTime, Player* player);

	 //<summary>
	 //回転イージング
	 //</summary>
	 //<param name="position"></param>
	 //<param name="maxTime"></param>
	 //<param name="player"></param>
	void EasingRot(Vector3& position, const float maxTime, Player* player,Vector3 rot);

	/// <summary>
	/// 回転イージング(回転攻撃用)
	/// </summary>
	/// <param name="position"></param>
	/// <param name="maxTime"></param>
	/// <param name="player"></param>
	void EasingRolling(const float maxTime,Vector3 rot);

	BossStatePattern GetStatePattern() { return statePattern; }
	bool& GetCollisionFlag() { return collisionFlag; }
	void SetBoss(std::weak_ptr<Boss> weakBoss) { weak_boss = weakBoss; }
protected:
	weak_ptrBoss weak_boss;
	weak_ptrPlayer player;
	BossStatePattern statePattern;
	Timer time;

	Vector3 EaseStart;//イージング開始座標
	Vector3 REaseStart;//イージング開始座標
	Vector3 EaseEnd;  //イージング終了座標
	Vector3 Ppos;     //プレイヤーの座標
	Vector3 rotboss;  //回転
	Vector3 AttackPos;//攻撃当たり判定座標

	Vector3 forwardVector;//前方向ベクトル
	Matrix4 warldMat;//ワールド座標行列
	Vector3 walrdPos;//ワールド座標ベクトル
	Vector3 walrdPos2;//ワールド座標ベクトル
	Vector3 CalculatedPos;//プレイヤーの座標からボスの座標を引いた座標

	const unsigned int STATE_COUNT = (unsigned int)BossStatePattern::END;
	const unsigned int NEAR_BEGIN = (unsigned int)BossStatePattern::BITING;
	const unsigned int ANYFIRST = (unsigned int)BossStatePattern::LOCKON;
	const unsigned int FAR_BEGIN = (unsigned int)BossStatePattern::RUSH;
	const unsigned int CHENGEMAXTIME = 600.0f;

	float angle;
	float velocity;//速度
	float maxTime;   //全体時間[s]
	float distance; //距離
	bool collisionFlag;
};