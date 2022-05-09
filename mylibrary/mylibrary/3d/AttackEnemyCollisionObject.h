#pragma once
#include "Object3d.h"
#include "GameObject/Boss/Boss.h"
#include "Input/Input.h"
#include "player.h"
#include "Audio/AudioManager.h"

class Player;
class Boss;
class AttackEnemyCollisionObject :
    public Object3d
{
public:
	/// <summary>
	/// オブジェクト生成
	/// </summary>
	/// <returns></returns>
	static std::shared_ptr<AttackEnemyCollisionObject> Create(Model* model = nullptr);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Initialize(Model* model);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="player">プレイヤーインスタンス</param>
	void Update(Boss* boss,Player* player, AudioManager* audio);

private:
	Input* input = Input::GetInstance();
	bool collisionFlag;
	bool roarFlag;
};

