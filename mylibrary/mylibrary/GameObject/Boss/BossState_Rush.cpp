#include "BossState_Rush.h"
#include "BossState_LockOn.h"
#include "BossState_Wait.h"

void BossState_Rush::Initialize()
{
	step = RushStep::RushStart;
	maxTime = 0.3f;//‘S‘ÌŠÔ[s]
	timeRate = 0;;
	elapsedTime = 0;
	totalTime = 0;
	angle = 0;
	timerFlag = false;
	Ppos = {};
	collisionFlag = false;
	EaseStart = {};
	EaseEnd = {};
	bai = 100.0f;
	forwardVector = { 0,0,1 };
}

void BossState_Rush::Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)
{
	Rush(player,audio);
	collider->Update();
}

void BossState_Rush::Finalize()
{
}

void BossState_Rush::Rush(Player* player, AudioManager* audio)
{
	if (step == RushStep::RushStart)
	{
		audio->PlayWave(L"Resources/sounds/Rush.wav");
		warldMat = weak_boss.lock()->GetMatWorld();
		walrdPos = warldMat.transformNormal(forwardVector, warldMat);
		walrdPos = {walrdPos.x * bai, 0 ,walrdPos.z * bai};

		TimerStart(&time, &timerFlag);
		Ppos = player->GetPosition();
		DecisionDistance(walrdPos.x + Ppos.x,walrdPos.z  + Ppos.z);
		step = RushStep::RushEnd;
	}
		
	if (timerFlag && step == RushStep::RushEnd)
	{
		const float MoveFlame = 6.0f;
		AdvanceTimer(MoveFlame);
		EasingMove(weak_boss.lock()->GetPosition(), MoveFlame, player);
		weak_boss.lock()->GetPosition();

		if (IsEasingOver())
		{
			bai = 100.0f;
			ResetTimer();
			weak_boss.lock()->ChangeState(std::make_shared<BossState_Wait>());
		}
	}
}