#pragma once
#include "../3d/Object3d.h"
#include "../3d/ParticleManager.h"
#include "../player.h"
#include "../Math/ExNum.h"
#include "../Audio/AudioManager.h"
#include "../3d/EffekseerManager.h"

class Player;
class ParticleManager;
class AttackCollisionObject :
    public Object3d
{
public:
	/// <summary>
    /// オブジェクト生成
    /// </summary>
    /// <returns></returns>
	static std::shared_ptr<AttackCollisionObject> Create(Model* model = nullptr, Camera* camera = nullptr);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Initialize(Model* model, Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="player">プレイヤーインスタンス</param>
	void Update(Player* player, ParticleManager* particleMan, AudioManager* audio);

	/// <summary>
    /// パーティクル生成
    /// </summary>
	void CreateParticles(ParticleManager* particleMan);

	/// <summary>
    /// パーティクル生成
    /// </summary>
	void CreateParticlesAttack(ParticleManager* particleMan);

	bool& GetColFlag() { return collisionFlag; }
	float GetHP() { return HP->rate() * 1000; }
	bool& ChangeColFlag() { return collisionFlag = !collisionFlag; }

public:
	static AttackCollisionObject* GetInstance();
private:
	bool collisionFlag;
	bool soundFlag;
	int soundTimer;
	ExNum* HP;
	Effekseer::Handle handle;
};

