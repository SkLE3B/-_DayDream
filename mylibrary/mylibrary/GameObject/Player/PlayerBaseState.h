#pragma once
#include "Player.h"
#include "../mylibrary/Camera/Camera.h"
#include "../mylibrary/GameObject/Boss/Boss.h"
#include "../mylibrary/Collision/AttackCollisionObject.h"
#include "../mylibrary/Audio/AudioManager.h"
#include "../mylibrary/Input/Input.h"
#include "../mylibrary/3d/EffekseerManager.h"
#include "../mylibrary/Base/HelperTimer.h"

enum class PlayerStatepattern
{
	NONE,
	MOVE,
	ATTACK,
	AVOID,
	RUSH,
	KNOCKBACK,
	ROARKNOCKBACK
};

class Boss;
class AttackCollisionObject;
class PlayerBaseState
{
public:
	PlayerBaseState() {}
	virtual ~PlayerBaseState() {}
	virtual void Initialize() = 0;
	virtual void Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio) = 0;
	virtual void Finalize() = 0;
	void SetPlayer(std::weak_ptr<Player> weakPlayer) { weak_player = weakPlayer; }

	/// <summary>
    /// イージングの終点の座標を保存する
    /// </summary>
    /// <param name="EashEnd">終点座標</param>
	void DecisionDistance(Vector3 EashEnd);

	/// <summary>
    /// イージング
    /// </summary>
    /// <param name="position"></param>
    /// <param name="maxTime"></param>
	void Easing(Vector3& position, const float maxTime);

	/// <summary>
	/// 当たり判定を補正する
	/// </summary>
	/// <param name="AttackCol">当たり判定</param>
	/// <param name="AddPos">加える座標</param>
	void PositionCorrection(AttackCollisionObject* AttackCol, const Vector3 AddPos);

	/// <summary>
    /// 補正座標をセットする
    /// </summary>
    /// <param name="distance">距離</param>
    /// <param name="correctionPosY">補正するY座標</param>
	Vector3 GetCorrectionPos(const float& distance, Vector3& correctionPosY)
	{
		return ((moveDirection + cameraDirectionZ) * distance) + correctionPosY;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Vector3& GetEasingPosition() { return EasingPosition; }

protected:
	Input* input = Input::GetInstance();
	std::weak_ptr<Player> weak_player;
	PlayerStatepattern statePattern;
	
	Vector3 EasingPosition;
	Vector3 moveDirection ;
	Vector3 cameraDirectionZ;
	Vector3 Start;
	Vector3 End;
	Effekseer::Handle handle;//エフェクトハンドル
};