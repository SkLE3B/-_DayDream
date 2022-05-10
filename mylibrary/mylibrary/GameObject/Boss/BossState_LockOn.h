#pragma once
#include "BossBaseState.h"

enum class LockOnStep
{
	LockOnStart,
	DuringLockOn,
	LockOnEnd,
};

class BossState_LockOn :
    public BossBaseState
{
public:
	BossState_LockOn()
	{
		statePattern = BossStatePattern::LOCKON;
	}

	~BossState_LockOn() {}

	void Initialize() override;
	
	void Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)override;

	void Finalize() override;

	void LockOn(Player* player);

	void SelectState(Player* player);

	int MakeRand(int maxNum, int minNum);

private:
	LockOnStep step;
	Vector3 pos;
	Vector3 playerpos;
	//ボスがプレイヤーを近いと思う距離
	const float NEAR_BORDER = 80.0f * 80.0f;
	//ボスがプレイヤーを遠いと思う距離
	const float FAR_BORDER = 120.0f * 120.0f;
	float distanceBoundary;//ボスとプレイヤーが遠いか近いかの境目
};

