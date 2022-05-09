#pragma once
#include "BossBaseState.h"

enum class BackStepStep
{
	BackStepStart,
	DuringBackStep,
	BackStepEnd,
};

class BossState_BackStep :
    public BossBaseState
{
public:
	BossState_BackStep()
	{
		statePattern = BossStatePattern::BACKSTEP;
	}

	~BossState_BackStep() {}

	void Initialize() override;
	void Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)override;
	void Finalize() override;

	void BackStep(Player* player);
private:
	BackStepStep step;
};

