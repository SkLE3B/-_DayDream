#include "BossState_Rolling.h"
#include "BossState_Wait.h"

void BossState_Rolling::Initialize()
{
	step = RollingStep::RollingStart;
	maxTime = 0.3f;//‘S‘ÌŠÔ[s]
	Ppos = {};
	EaseStart = {};
	collisionFlag = false;
	EaseEnd = {};
	time.SetMaxTime(12.0f);
}

void BossState_Rolling::Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)
{
	time.Update();
	Rolling();
	collider->Update();
}

void BossState_Rolling::Finalize()
{
}

void BossState_Rolling::Rolling()
{
	if (step == RollingStep::RollingStart)
	{
		DecisionDistance(Ppos.x, Ppos.z);
		angle = weak_boss.lock()->GetRotation().y;
		time.Reset();
		step = RollingStep::DuringRolling;
	}

	if (time.IsTimer() && step == RollingStep::DuringRolling)
	{
		const float rollingFlame = 12.0f;
		EasingRolling(rollingFlame,weak_boss.lock()->GetRotation());

		if (time.IsEasingOver())
		{
			weak_boss.lock()->GetRotation().y = weak_boss.lock()->GetRotation().y - 360;
			weak_boss.lock()->ChangeState(std::make_shared<BossState_Wait>());
		}
	}
}