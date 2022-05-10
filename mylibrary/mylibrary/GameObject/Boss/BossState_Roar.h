#pragma once
#include "BossBaseState.h"

enum class RoarStep
{
	RoarStart,
	DuringRoar,
	RoarEnd,
};

class BossState_Roar :
    public BossBaseState
{
public:
	BossState_Roar()
	{
		statePattern = BossStatePattern::ROAR;
	}

	~BossState_Roar() {}

	void Initialize() override;
	void Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)override;
	void Finalize() override;

	void Roar(Player* player, AudioManager* audio);
private:
	RoarStep step;
	float dotPos;//内積ベクトル
};

