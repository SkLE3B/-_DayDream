#include "BossState_Rolling.h"
#include "BossState_Wait.h"

void BossState_Rolling::Initialize()
{
	step = RollingStep::RollingStart;
	maxTime = 0.3f;//‘S‘ÌŠÔ[s]
	timeRate = 0;;
	elapsedTime = 0;
	totalTime = 0;
	angle = 0;
	timerFlag = false;
	roarFlag = false;
	Ppos = {};
	EaseStart = {};
	collisionFlag = false;
	EaseEnd = {};
}

void BossState_Rolling::Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)
{
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
		TimerStart(&time, &timerFlag);
		step = RollingStep::DuringRolling;
	}

	if (timerFlag && step == RollingStep::DuringRolling)
	{
		const float rollingFlame = 12.0f;
		AdvanceTimer(rollingFlame);
		EasingRolling(rollingFlame,weak_boss.lock()->GetRotation());

		if (IsEasingOver())
		{
			weak_boss.lock()->GetRotation().y = weak_boss.lock()->GetRotation().y - 360;
			ResetTimer();
			weak_boss.lock()->ChangeState(std::make_shared<BossState_Wait>());
		}
	}
}