#include "BossState_Biting.h"
#include "BossState_Wait.h"

void BossState_Biting::Initialize()
{
	step = BiteStep::BiteStart();
	bai = 0.0f;
	maxTime = 0.3f;//‘S‘ÌŠÔ[s]
	timeRate = 0;;
	elapsedTime = 0;
	totalTime = 0;
	angle = 0;
	timerFlag = false;
	collisionFlag = false;
	Ppos = {};
	EaseStart = {};
	EaseEnd = {};
	forwardVector = { 0,0,1 };
}

void BossState_Biting::Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)
{
	Biting(player, ememyCollision,audio);
	collider->Update();
}

void BossState_Biting::Finalize()
{
}

void BossState_Biting::Biting(Player* player, AttackEnemyCollisionObject* ememyCollision,AudioManager* audio)
{
	Vector3 pos = { 0,2,0 };
	if (step == BiteStep::BiteStart)
	{
		weak_boss.lock()->PlayAnimation(3,1);
		TimerStart(&time, &timerFlag);
		step = BiteStep::DuringBite;
	}

	if (timerFlag && step == BiteStep::DuringBite)
	{
		AdvanceTimer(maxTime);

		if (IsTimeOut(totalTime, 3.6f))
		{
			audio->PlayWave(L"Resources/sounds/Bite.wav");
			ResetTimer();
			TimerStart(&time, &timerFlag);
			collisionFlag = !collisionFlag;
			ememyCollision->SetPosition(weak_boss.lock()->GetPosition());
			step = BiteStep::BiteEnd;
		}
	}

	if (timerFlag && step == BiteStep::BiteEnd)
	{		
		warldMat = weak_boss.lock()->GetMatWorld();
		AdvanceTimer(maxTime);

		bai += 2.5f;
		walrdPos = warldMat.transformNormal(forwardVector, warldMat);
		walrdPos = { walrdPos.x * (bai + 10), 2 ,walrdPos.z * (bai + 10) };
		PositionCorrection(ememyCollision, walrdPos);
		handle = EffekseerManager::PlayEffect(u"Resources/Effects/scroll.efk", { weak_boss.lock()->GetPosition().x + walrdPos.x, weak_boss.lock()->GetPosition().y + walrdPos.y, weak_boss.lock()->GetPosition().z + walrdPos.z });
		EffekseerManager::SetScale(handle, { 3,3,3 });

		if (IsTimeOut(totalTime, 0.00001f))
		{
			handle = EffekseerManager::StopEffect(handle);
		}

		if (IsTimeOut(totalTime, 10.0f))
		{
			ResetTimer();
			collisionFlag = !collisionFlag;
			bai = 0.5f;
			weak_boss.lock()->ChangeState(std::make_shared<BossState_Wait>());
		}
	}
}
