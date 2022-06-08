#include "BossState_Rush.h"
#include "BossState_LockOn.h"
#include "BossState_Wait.h"

void BossState_Rush::Initialize()
{
	step = RushStep::RushStart;
	maxTime = 0.3f;//‘S‘ÌŠÔ[s]
	Ppos = {};
	distance = 100.0f;
	forwardVector = { 0,0,1 };
	time.SetMaxTime(6.0f);
}

void BossState_Rush::Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)
{
	time.Update();
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
		walrdPos = {walrdPos.x * distance, 0 ,walrdPos.z * distance};

		Ppos = player->GetPosition();
		DecisionDistance(walrdPos.x + Ppos.x,walrdPos.z  + Ppos.z);
		time.Reset();
		step = RushStep::RushEnd;
	}
		
	if (time.IsTimer() && step == RushStep::RushEnd)
	{
		const float MoveFlame = 6.0f;
		EasingMove(weak_boss.lock()->GetPosition(), MoveFlame, player);
		weak_boss.lock()->GetPosition();
	
		if (time.IsEasingOver())
		{
			distance = 100.0f;
			weak_boss.lock()->ChangeState(std::make_shared<BossState_Wait>());
		}
	}
}