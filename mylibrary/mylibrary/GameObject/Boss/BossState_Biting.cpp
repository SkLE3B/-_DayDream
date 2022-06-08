#include "BossState_Biting.h"
#include "BossState_Wait.h"

void BossState_Biting::Initialize()
{
	step = BiteStep::BiteStart();
	distance = 10.0f;
	velocity = 2.5f;
	maxTime = 40.0f;//全体時間[s]
	forwardVector = { 0,0,1 };
	time.SetMaxTime(maxTime);
}

void BossState_Biting::Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)
{
	time.Update();
	Biting(player, ememyCollision,audio);
	collider->Update();
}

void BossState_Biting::Finalize()
{
}

void BossState_Biting::Biting(Player* player, AttackEnemyCollisionObject* ememyCollision,AudioManager* audio)
{
	if (step == BiteStep::BiteStart)
	{
		weak_boss.lock()->PlayAnimation(3,1);
		time.Reset();
		step = BiteStep::DuringBite;
	}

	if (time.IsTimer() && step == BiteStep::DuringBite)
	{
		if (time.IsTimeOut(3.6f))
		{
			audio->PlayWave(L"Resources/sounds/Bite.wav");
			time.Reset();
			ememyCollision->ChangeCollisionFlag();
			ememyCollision->SetPosition(weak_boss.lock()->GetPosition());
			step = BiteStep::BiteEnd;
		}
	}

	if (time.IsTimer() && step == BiteStep::BiteEnd)
	{		
		warldMat = weak_boss.lock()->GetMatWorld();
		distance += velocity;
		walrdPos = warldMat.transformNormal(forwardVector, warldMat);
		walrdPos = { walrdPos.x * distance, 2 ,walrdPos.z * distance};
		PositionCorrection(ememyCollision, walrdPos);
		handle = EffekseerManager::PlayEffect(u"Resources/Effects/scroll.efk", 
			{ weak_boss.lock()->GetPosition().x + walrdPos.x, weak_boss.lock()->GetPosition().y + walrdPos.y, 
			weak_boss.lock()->GetPosition().z + walrdPos.z });
		EffekseerManager::SetScale(handle, { 3,3,3 });

		if (time.IsTimeOut(0.12f))
		{
			handle = EffekseerManager::StopEffect(handle);
		}

		if (time.IsTimeOut(maxTime))
		{
			//敵の球に当たって無い場合フラグをoffにする。
			if (ememyCollision->GetCollisionFlag())
			{
				ememyCollision->ChangeCollisionFlag();
			}
			weak_boss.lock()->ChangeState(std::make_shared<BossState_Wait>());
		}
	}
}
