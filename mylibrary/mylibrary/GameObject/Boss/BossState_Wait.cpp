#include "BossState_Wait.h"
#include "BossState_LockOn.h"
#include "BossState_BackStep.h"

void BossState_Wait::Initialize()
{
	changeStateTimer = 0;
	collisionFlag = false;
}

void BossState_Wait::Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)
{
	changeStateTimer++;

	weak_boss.lock()->PlayAnimation(0,1);
	
	collider->Update();
	if (changeStateTimer <= CHENGEMAXTIME)
	{
		weak_boss.lock()->ChangeState(std::make_shared<BossState_LockOn>());
	}
}

void BossState_Wait::Finalize()
{
}