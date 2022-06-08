#include "BossState_Wait.h"
#include "BossState_LockOn.h"
#include "BossState_Rolling.h"
#include "BossState_Breth.h"

void BossState_Wait::Initialize()
{
	changeStateTimer = 0;
	step = WaitStep::WaitStart;
}

void BossState_Wait::Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)
{
	changeStateTimer++;

	if (step == WaitStep::WaitStart)
	{
		weak_boss.lock()->PlayAnimation(0, 1);
		step = WaitStep::DuringWait;
	}
	else if (step == WaitStep::DuringWait)
	{
		collider->Update();
		if (changeStateTimer <= CHENGEMAXTIME)
		{
			weak_boss.lock()->ChangeState(std::make_shared<BossState_LockOn>());
		}
	}
}

void BossState_Wait::Finalize()
{
}