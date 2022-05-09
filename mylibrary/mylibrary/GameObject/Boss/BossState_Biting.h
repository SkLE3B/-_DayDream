#pragma once
#include "BossBaseState.h"

enum class BiteStep
{
	BiteStart,
	DuringBite,
	BiteEnd,
};

class BossState_Biting : public BossBaseState
{
public:
	BossState_Biting()
	{
		statePattern = BossStatePattern::BRETH;
	}

	~BossState_Biting() {}
	void Initialize() override;
	void Update(Player* player, AttackEnemyCollisionObject* ememyCollision,SphereCollider* collider, AudioManager* audio)override;
	void Finalize() override;
	void Biting(Player* player, AttackEnemyCollisionObject* ememyCollision, AudioManager* audio);
private:
	BiteStep step;
};