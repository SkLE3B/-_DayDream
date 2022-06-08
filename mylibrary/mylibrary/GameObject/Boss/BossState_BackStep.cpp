#include "BossState_BackStep.h"
#include "BossState_Wait.h"

void BossState_BackStep::Initialize()
{
	step = BackStepStep::BackStepStart();
	distance = 100.0f;
	forwardVector = { 0,0,1 };
	maxTime = 14.0f;
	time.SetMaxTime(maxTime);
}

void BossState_BackStep::Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)
{
	time.Update();
	BackStep(player);
	collider->Update();
}

void BossState_BackStep::Finalize()
{
}

void BossState_BackStep::BackStep(Player* player)
{
	if (step == BackStepStep::BackStepStart)
	{
		warldMat = weak_boss.lock()->GetMatWorld();
		walrdPos = warldMat.transformNormal(forwardVector, warldMat);
		walrdPos = { walrdPos.x * distance, 0 ,walrdPos.z * distance };
		
		time.Reset();		
		DecisionDistance(weak_boss.lock()->GetPosition().x - walrdPos.x, weak_boss.lock()->GetPosition().z - walrdPos.z);
		step = BackStepStep::DuringBackStep;
	}

	if (time.IsTimer() && step == BackStepStep::DuringBackStep)
	{
		EasingMove(weak_boss.lock()->GetPosition(), maxTime, player);
		weak_boss.lock()->GetPosition();

		if (time.IsTimeOut(maxTime))
		{
			weak_boss.lock()->ChangeState(std::make_shared<BossState_Wait>());
		}
	}
}