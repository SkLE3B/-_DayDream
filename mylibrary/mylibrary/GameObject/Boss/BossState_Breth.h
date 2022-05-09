#pragma once
#include "BossBaseState.h"

enum class BrethStep
{
	BrethStart,
	DuringBreth,
	BrethEnd,
};

class BossState_Breth :public BossBaseState
{
public:
	BossState_Breth()
	{
		statePattern = BossStatePattern::BRETH;
	}

	~BossState_Breth() {}

	void Initialize() override;
	void Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio) override;
	void Finalize() override;

	void Breth(Player* player, AttackEnemyCollisionObject* ememyCollision, AudioManager* audio);
private:
	BrethStep step;
};

