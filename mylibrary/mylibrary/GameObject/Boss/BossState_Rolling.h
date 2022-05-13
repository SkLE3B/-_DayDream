#pragma once
#include "BossBaseState.h"
#include "../mylibrary/3d/EffekseerManager.h"

enum class RollingStep
{
	RollingStart,
	DuringRolling,
	RollingEnd,
};

class BossState_Rolling :
    public BossBaseState
{
public:
	BossState_Rolling()
	{
		statePattern = BossStatePattern::ROLLING;
	}

	~BossState_Rolling() {}

	void Initialize() override;
	void Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)override;
	void Finalize() override;

	void Rolling();
private:
	RollingStep step;
	Effekseer::Handle handle;
};

