#include "BossState_BackStep.h"
#include "BossState_Wait.h"

void BossState_BackStep::Initialize()
{
	step = BackStepStep::BackStepStart();
	maxTime = 0.3f;//‘S‘ÌŠÔ[s]
	timeRate = 0;;
	elapsedTime = 0;
	totalTime = 0;
	angle = 0;
	timerFlag = false;
	Ppos = {};
	EaseStart = {};
	collisionFlag = false;
	EaseEnd = {};
	bai = 100.0f;
	forwardVector = { 0,0,1 };
}

void BossState_BackStep::Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)
{
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
		bai = 100;
		warldMat = weak_boss.lock()->GetMatWorld();
		walrdPos = warldMat.transformNormal(forwardVector, warldMat);
		walrdPos = { walrdPos.x * bai, 0 ,walrdPos.z * bai };
		
		TimerStart(&time, &timerFlag);		
		DecisionDistance(weak_boss.lock()->GetPosition().x - walrdPos.x, weak_boss.lock()->GetPosition().z - walrdPos.z);

		step = BackStepStep::DuringBackStep;
	}

	if (timerFlag && step == BackStepStep::DuringBackStep)
	{
		const float MoveFlame = 6.0f;
		Vector3 post = weak_boss.lock()->GetPosition();
		EasingMove(post, MoveFlame, player);
		weak_boss.lock()->GetPosition();
		weak_boss.lock()->SetPosition(post);
		AdvanceTimer(MoveFlame);

		if (IsEasingOver())
		{
			bai = 100.0f;
			ResetTimer();
			weak_boss.lock()->ChangeState(std::make_shared<BossState_Wait>());
		}
	}
}