#pragma once
#include "BossBaseState.h"

enum class RushStep
{
	RushStart,
	DuringRush,
	RushEnd,
};

class BossState_Rush :
    public BossBaseState
{
public:
	BossState_Rush()
	{
		statePattern = BossStatePattern::RUSH;
	}

	~BossState_Rush() {}

	void Initialize() override;
	void Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)override;
	void Finalize() override;

	void Rush(Player* player, AudioManager* audio);
private:
	RushStep step;
};

