#include "BossState_Breth.h"
#include "BossState_LockOn.h"
#include "BossState_Wait.h"

void BossState_Breth::Initialize()
{
	step = BrethStep::BrethStart();
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

void BossState_Breth::Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)
{
	Breth(player, ememyCollision,audio);
	collider->Update();
}

void BossState_Breth::Finalize()
{
}

void BossState_Breth::Breth(Player* player, AttackEnemyCollisionObject* ememyCollision, AudioManager* audio)
{
	Vector3 pos = { 0,2,0 };
	if (step == BrethStep::BrethStart)
	{
		audio->PlayWave(L"Resources/sounds/Breth.wav");
		weak_boss.lock()->PlayAnimation(1,1);
		TimerStart(&time, &timerFlag);
		step = BrethStep::DuringBreth;
	}
	
	if (timerFlag && step == BrethStep::DuringBreth)
	{
		AdvanceTimer(maxTime);
					
		if (IsTimeOut(totalTime, 10.0f))
		{
			ResetTimer();
			ememyCollision->SetPosition(weak_boss.lock()->GetPosition());
			TimerStart(&time, &timerFlag);
			collisionFlag = !collisionFlag;
			step = BrethStep::BrethEnd;
		}
	}
	
	if (timerFlag && step == BrethStep::BrethEnd)
	{
		warldMat = weak_boss.lock()->GetMatWorld();
		AdvanceTimer(maxTime);
	
		bai += 5.5f;
		walrdPos = warldMat.transformNormal(forwardVector, warldMat);
		walrdPos = { walrdPos.x * (bai + 10), 2 ,walrdPos.z * (bai + 10) };
		PositionCorrection(ememyCollision, walrdPos);
		handle = EffekseerManager::PlayEffect(u"Resources/Effects/Bite.efk", { weak_boss.lock()->GetPosition().x + walrdPos.x, weak_boss.lock()->GetPosition().y + walrdPos.y, weak_boss.lock()->GetPosition().z + walrdPos.z });
		
		if (IsTimeOut(totalTime, 0.01f))
		{
			handle = EffekseerManager::StopEffect(handle);
		}

		if (IsTimeOut(totalTime, 15.0f))
		{
			ResetTimer();
			collisionFlag = !collisionFlag;
			bai = 1.0f;
			step = BrethStep::BrethStart;
			weak_boss.lock()->ChangeState(std::make_shared<BossState_Wait>());
		}
	}
}