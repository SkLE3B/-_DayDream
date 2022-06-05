#pragma once
#include "BossBaseState.h"

enum class WaitStep
{
	WaitStart,
	DuringWait
};

class BossState_Wait : public BossBaseState
{
public:
	BossState_Wait()
	{
		statePattern = BossStatePattern::WAIT;
	}

	~BossState_Wait() {}

	void Initialize() override;
	void Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)override;
	void Finalize() override;

private:
	WaitStep step;
	int changeStateTimer;
};

